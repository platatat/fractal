`timescale 1ns/1ns

`include "tile_solver_legit.sv"

module top();
    localparam TILE_SIZE_BITS       = 6;
    localparam LIMB_INDEX_BITS      = 6;
    localparam LIMB_SIZE_BITS       = 8;
    localparam DIVERGENCE_RADIUS    = 4;

    integer i;

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

        // send number of limbs
        fifo_data_type <= 3'd1;
        fifo_data <= 32'd2;
        #20

        // start sending c_real
        fifo_data_type <= 3'd2;
        fifo_data <= 32'd0;
        #20

        fifo_data_type <= 3'd2;
        fifo_data <= 32'd0;
        #20

        // start sending c_imag
        fifo_data_type <= 3'd3;
        fifo_data <= 32'd0;
        #20

        // delay for a bit
        fifo_valid <= 0;
        #20
        #20
        #20

        // finish sending c_imag
        fifo_data_type <= 3'd3;
        fifo_data <= 32'd0;
        fifo_valid <= 1;
        #20

        // Set max iterations
        fifo_data_type <= 3'd4;
        fifo_data <= 10;
        #20

        // Set bit shift
        fifo_data_type <= 3'd5;
        fifo_data <= 2;

        // signal end of stream
        fifo_end_of_stream <= 1;
        #20

        fifo_valid <= 0;
        fifo_end_of_stream <= 0;
        #20

        for (i = 0; i < 64 * 64; i++) begin
            while (!solver.out_valid) begin
                #20;
            end
            $display("!,%d,%d", solver.out_addr, solver.out_data);
            #20;
        end

        $finish;
    end

    always begin
        #10
        clock <= !clock;
    end

    tile_solver_legit #(
        TILE_SIZE_BITS,
        LIMB_INDEX_BITS,
        LIMB_SIZE_BITS,
        DIVERGENCE_RADIUS
    ) solver (
        .clock(clock),
        .reset(reset),

        .in_valid(fifo_valid),
        .in_data(fifo_data | (fifo_data_type << 29)),
        .in_ready(fifo_ready),
        .in_end_of_stream(fifo_end_of_stream),

        .out_ready(1)
    );

endmodule
