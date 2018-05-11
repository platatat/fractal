`timescale 1ns/1ns

`include "solver_control.v"

module top();
    localparam LIMB_INDEX_BITS = 6;

    reg clock;
    reg reset;

    reg start;
    reg diverged;
    reg wr_num_limbs_en;
    reg [LIMB_INDEX_BITS-1:0] num_limbs_data;
    reg wr_iter_lim_en;
    reg [15:0] iter_lim_data;

    initial begin
        $dumpfile("build/solver_control_test.vcd");
        $dumpvars(0, top);

        start <= 0;
        diverged <= 0;
        wr_num_limbs_en <= 0;
        num_limbs_data <= 5;
        wr_iter_lim_en <= 0;
        iter_lim_data <= 3;

        clock <= 1;
        reset <= 1;
        #10
        reset <= 0;

        #100
        wr_num_limbs_en <= 1;
        wr_iter_lim_en  <= 1;
        #20
        wr_num_limbs_en <= 0;
        wr_iter_lim_en  <= 0;

        #40
        start <= 1;
        #20
        start <= 0;

        #1000
        //diverged <= 1;
        #2000

        $finish;
    end

    always begin
        #10
        clock <= !clock;
    end

    solver_control #(LIMB_INDEX_BITS) control(
        .clock(clock),
        .reset(reset),
        .wr_num_limbs_en(wr_num_limbs_en),
        .num_limbs_data(num_limbs_data),
        .wr_iter_lim_en(wr_iter_lim_en),
        .iter_lim_data(iter_lim_data),
        .start(start),
        .diverged(diverged)
    );

endmodule
