`timescale 1ns/1ns

`include "tile_solver_legit.v"

module top();
    localparam LIMB_INDEX_BITS      = 6;
    localparam LIMB_SIZE_BITS       = 8;
    localparam DIVERGENCE_RADIUS    = 4;

    reg clock;
    reg reset;
    reg [15:0] cycle_counter;

    reg fifo_valid;
    reg fifo_end_of_stream;

    reg [2:0]   fifo_data_type;
    reg [31:0]  fifo_data;

    wire fifo_ready;
    

    initial begin
        $dumpfile("build/solver_manager_test.vcd");
        $dumpvars(0, top);

        cycle_counter <= 0;

        clock <= 0;
        reset <= 1;
        fifo_valid <= 0;
        fifo_end_of_stream <= 0;
        #20

        reset <= 0;
        #20

        // send output address
        fifo_data_type <= 3'd0;
        fifo_data <= 32'd1;
        fifo_valid <= 1;
        #20

        // send zoom level
        fifo_data_type <= 3'd1;
        fifo_data <= 32'd2;
        #20

        // start sending c_real
        fifo_data_type <= 3'd2;
        fifo_data <= 32'd3;
        #20

        fifo_data_type <= 3'd2;
        fifo_data <= 32'd4;
        #20

        fifo_data_type <= 3'd2;
        fifo_data <= 32'd5;
        #20

        // start sending c_imag
        fifo_data_type <= 3'd3;
        fifo_data <= 32'd6;
        #20

        fifo_data_type <= 3'd3;
        fifo_data <= 32'd7;
        #20

        // delay for a bit
        fifo_valid <= 0;
        #20
        #20
        #20

        // finish sending c_imag
        fifo_data_type <= 3'd3;
        fifo_data <= 32'd8;
        fifo_valid <= 1;

        // signal end of stream
        fifo_end_of_stream <= 1;
        #20

        fifo_valid <= 0;
        fifo_end_of_stream <= 0;
        #20

        while (!solver.out_valid) begin
            #20;
        end

        $finish;
    end

    always begin
        #10
        clock <= !clock;

        if (!clock) begin
            cycle_counter <= cycle_counter + 1;
            $display("CYCLE: %d", cycle_counter);
        end
    end

    tile_solver_legit #(
        LIMB_INDEX_BITS,
        LIMB_SIZE_BITS,
        DIVERGENCE_RADIUS
    ) solver (
        .clock(clock),
        .reset(reset),

        .in_valid(fifo_valid),
        .in_data(fifo_data | (fifo_data_type << 29)),
        .in_ready(fifo_ready),
        .in_end_of_stream(fifo_end_of_stream)
    );

endmodule
