`timescale 1ns/1ns

`include "solver.v"

module top();
    localparam LIMB_INDEX_BITS = 6;
    localparam LIMB_SIZE_BITS = 8;
    localparam DIVERGENCE_RADIUS = 4;

    reg clock;
    reg reset;

    reg [LIMB_SIZE_BITS-1:0] c_re_data [3:0];
    reg [LIMB_SIZE_BITS-1:0] c_im_data [3:0];

    initial begin
        $dumpfile("build/solver_test.vcd");
        $dumpvars(0, top);


        wr_en <= 0;
        wr_limb <= 0;
        wr_data_re <= 0;
        wr_data_im <= 0;
        start <= 0;

        clock   <= 0;
        reset   <= 1;
        #20

        reset <= 0;
        wr_en <= 1;
        wr_limb <= 0;
        wr_data_re <= 8'h00;
        wr_data_im <= 8'h00;
        #20

        wr_limb <= 1;
        wr_data_re <= 8'h80;
        wr_data_im <= 8'h00;
        #20

        wr_limb <= 2;
        wr_data_re <= 8'h00;
        wr_data_im <= 8'h00;
        #20

        wr_en <= 0;
        start <= 1;

        #50000
        $finish;
    end

    always begin
        #10
        clock <= !clock;

        if (solver_done) begin
            $display("iterations: %d", iterations);
            $finish;
        end
    end

    reg                         wr_en;
    reg [LIMB_INDEX_BITS-1:0]   wr_limb;
    reg [LIMB_SIZE_BITS-1:0]    wr_data_re;
    reg [LIMB_SIZE_BITS-1:0]    wr_data_im;
    reg [LIMB_INDEX_BITS-1:0]   num_limbs = 2;
    reg [15:0]                  iter_lim = 10;
    reg                         start;

    wire                        solver_done;
    wire [15:0]                 iterations;

    solver #(LIMB_INDEX_BITS, LIMB_SIZE_BITS, DIVERGENCE_RADIUS) solver (
        .clock              (clock),
        .reset              (reset),
        .wr_real_en         (wr_en),
        .wr_imag_en         (wr_en),
        .wr_index           (wr_limb),
        .real_data          (wr_data_re),
        .imag_data          (wr_data_im),
        .wr_num_limbs_en    (wr_en),
        .num_limbs_data     (num_limbs),
        .wr_iter_lim_en     (wr_en),
        .iter_lim_data      (iter_lim),
        .start              (start),
        .out_ready          (solver_done),
        .iterations         (iterations)
    );

endmodule
