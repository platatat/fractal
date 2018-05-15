`timescale 1ns/1ns

`include "multi_tile_solver.sv"

module top();
    localparam NUM_SOLVERS          = 2;
    localparam TILE_SIZE_BITS       = 4;
    localparam LIMB_INDEX_BITS      = 6;
    localparam LIMB_SIZE_BITS       = 8;
    localparam DIVERGENCE_RADIUS    = 4;

    integer i;

    reg clock;
    reg reset;

    reg fifo_valid;
    reg fifo_end_of_stream;

    reg [2:0]   fifo_data_type;
    reg [31:0]  fifo_data;

    wire fifo_ready;

    initial begin
        $dumpfile("build/multi_tile_solver_test.vcd");
        $dumpvars(0, top);

        clock <= 0;
        reset <= 1;
        fifo_valid <= 0;
        fifo_end_of_stream <= 0;
        out_ack <= 0;

        #20

        reset <= 0;
        #20

        // send output address
        fifo_data_type <= 3'd0;
        fifo_data <= 32'd0;
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
        fifo_data <= 4;

        // signal end of stream
        fifo_end_of_stream <= 1;
        #20

        fifo_valid <= 0;
        fifo_end_of_stream <= 0;
        #20



        while(!fifo_ready) begin
            #20;
        end


        // send output address
        fifo_data_type <= 3'd0;
        fifo_data <= 1 << (2 * TILE_SIZE_BITS + 1);
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
        fifo_data <= 32'd1;
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
        fifo_data <= 4;

        // signal end of stream
        fifo_end_of_stream <= 1;
        #20

        fifo_valid <= 0;
        fifo_end_of_stream <= 0;
        #20;
    end

    integer num_writes = 0;

    always begin
        #20;
        if (multi_solver.out_write_en) begin
            $display("!,%d,%d", multi_solver.out_addr, multi_solver.out_data);
            #40;
            out_ack <= 1;
            #20;
            out_ack <= 0;

            num_writes <= num_writes + 1;
        end

        if (num_writes == NUM_SOLVERS * (1 << (2 * TILE_SIZE_BITS))) begin
            $finish;
        end
    end

    always begin
        #10
        clock <= !clock;
    end

    logic out_ack;

    multi_tile_solver #(
        NUM_SOLVERS,
        TILE_SIZE_BITS,
        LIMB_INDEX_BITS,
        LIMB_SIZE_BITS,
        DIVERGENCE_RADIUS
    ) multi_solver (
        .clock(clock),
        .reset(reset),

        .in_valid(fifo_valid),
        .in_data(fifo_data | (fifo_data_type << 29)),
        .in_ready(fifo_ready),
        .in_end_of_stream(fifo_end_of_stream),

        .out_ack
    );

endmodule
