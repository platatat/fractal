`timescale 1ns/1ns

`include "solver.v"

module top();
    integer i, j;

    localparam LIMB_INDEX_BITS = 6;
    localparam LIMB_SIZE_BITS = 8;
    localparam DIVERGENCE_RADIUS = 4;

    reg clock;
    reg reset;

    localparam NUM_LIMBS = 2;

    reg [LIMB_SIZE_BITS-1:0] c_re_data [NUM_LIMBS-1:0];
    reg [LIMB_SIZE_BITS-1:0] c_im_data [NUM_LIMBS-1:0];

    initial begin
        $dumpfile("build/solver_repeatability_test.vcd");
        $dumpvars(0, top);

        // Limb definitions
        c_re_data[0] <= 0;
        c_re_data[1] <= 8'h04;

        c_im_data[0] <= 0;
        c_im_data[1] <= 8'hbc;

        // Run the solver
        wr_real_en <= 0;
        wr_imag_en <= 0;
        wr_index <= 0;
        real_data <= 0;
        imag_data <= 0;

        wr_num_limbs_en <= 0;
        wr_iter_lim_en <= 0;

        num_limbs_data <= NUM_LIMBS;
        iter_lim_data <= 10;

        start <= 0;

        clock <= 0;
        reset <= 1;
        #20
        reset <= 0;
        #20

        for (i = 0; i < 4; i = i + 1) begin
            wr_num_limbs_en <= 1;
            wr_iter_lim_en <= 1;

            #20

            wr_num_limbs_en <= 0;
            wr_iter_lim_en <= 0;

            wr_real_en <= 1;
            wr_imag_en <= 1;

            for (j = 0; j < NUM_LIMBS; j = j + 1) begin
                real_data <= c_re_data[j];
                imag_data <= c_im_data[j];
                wr_index <= j;
                #20;
            end

            wr_real_en <= 0;
            wr_imag_en <= 0;

            start <= 1;
            #20

            start <= 0;
            #20

            while (!out_ready) begin
                #20;
            end

            $display("Num iterations: ", iterations);
        end

        $finish;
    end

    always begin
        #10
        clock <= !clock;
    end

    // Solver instance
    reg                       wr_real_en;
    reg                       wr_imag_en;
    reg [LIMB_INDEX_BITS-1:0] wr_index;
    reg [LIMB_SIZE_BITS-1:0]  real_data;
    reg [LIMB_SIZE_BITS-1:0]  imag_data;

    reg                       wr_num_limbs_en;
    reg [LIMB_INDEX_BITS-1:0] num_limbs_data;

    reg                       wr_iter_lim_en;
    reg [15:0]                iter_lim_data;

    reg                       start;

    wire        out_ready;
    wire [15:0] iterations;

    solver #(
        LIMB_INDEX_BITS,
        LIMB_SIZE_BITS,
        DIVERGENCE_RADIUS
    ) solv (
        .clock(clock),
        .reset(reset),

        .wr_real_en(wr_real_en),
        .wr_imag_en(wr_imag_en),
        .wr_index(wr_index),
        .real_data(real_data),
        .imag_data(imag_data),
        .wr_num_limbs_en(wr_num_limbs_en),
        .num_limbs_data(num_limbs_data),
        .wr_iter_lim_en(wr_iter_lim_en),
        .iter_lim_data(iter_lim_data),

        .start(start),

        .out_ready(out_ready),
        .iterations(iterations)
    );

endmodule
