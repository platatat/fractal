`timescale 1ns/1ns

`include "solver_datapath.v"

module top();
    localparam LIMB_INDEX_BITS = 6;
    localparam LIMB_SIZE_BITS = 8;
    localparam DIVERGENCE_RADIUS = 4;

    reg clock;
    reg reset;
    reg [16:0] cycle_num;

    initial begin
        $dumpfile("build/solver_datapath_test.vcd");
        $dumpvars(0, top);

        C_cre_limb <= 0;
        C_cim_limb <= 0;
        C_limb_ind <= 0;
        C_zre_ind <= 0;
        C_zim_ind <= 0;
        C_cre_wr_en <= 0;
        C_cim_wr_en <= 0;
        C_zre_reg_sel <= 0;
        C_zim_reg_sel <= 0;
        C_m1_a_sel <= 0;
        C_m1_b_sel <= 0;
        C_m2_a_sel <= 0;
        C_m2_b_sel <= 0;
        C_zre_partial_sel <= 0;
        C_zim_partial_sel <= 0;
        C_zre_acc_sel <= 0;
        C_zim_acc_sel <= 0;
        C_zre_wr_en <= 0;
        C_zim_wr_en <= 0;

        cycle_num <= 0;
        clock <= 0;
        reset <= 1;
        #20
        reset <= 0;

        //Load 1.5 into cre and 0.5 into cim
        C_cre_wr_en <= 1;
        C_cim_wr_en <= 1;
        C_limb_ind <= 0;
        C_cre_limb <= 0;
        C_cim_limb <= 1;
        #20

        C_limb_ind <= 1;
        C_cre_limb <= 128;
        C_cim_limb <= 128;
        #20

        C_limb_ind <= 2;
        C_cre_limb <= 0;
        C_cim_limb <= 0;
        #20

        C_cre_wr_en <= 0;
        C_cim_wr_en <= 0;
        C_cre_limb <= 0;
        C_cim_limb <= 0;

        //Compute the z series
        //iter 0
        C_limb_ind <= 2;
        C_zre_ind <= 1;
        C_zim_ind <= 1;
        C_zre_reg_sel <= 0;
        C_zim_reg_sel <= 1;
        C_m1_a_sel <= 0;
        C_m1_b_sel <= 0;
        C_m2_a_sel <= 0;
        C_m2_b_sel <= 1;
        C_zre_partial_sel <= 2;
        C_zim_partial_sel <= 0;
        C_zre_acc_sel <= 2;
        C_zim_acc_sel <= 2;
        C_zre_wr_en <= 0;
        C_zim_wr_en <= 0;
        #20

        C_limb_ind <= 2;
        C_zre_ind <= 1;
        C_zim_ind <= 1;
        C_zre_reg_sel <= 0;
        C_zim_reg_sel <= 1;
        C_m1_a_sel <= 0;
        C_m1_b_sel <= 0;
        C_m2_a_sel <= 0;
        C_m2_b_sel <= 1;
        C_zre_partial_sel <= 3;
        C_zim_partial_sel <= 0;
        C_zre_acc_sel <= 0;
        C_zim_acc_sel <= 3;
        C_zre_wr_en <= 0;
        C_zim_wr_en <= 0;
        #20

        C_limb_ind <= 1;
        C_zre_ind <= 0;
        C_zim_ind <= 1;
        C_zre_reg_sel <= 0;
        C_zim_reg_sel <= 1;
        C_m1_a_sel <= 0;
        C_m1_b_sel <= 0;
        C_m2_a_sel <= 0;
        C_m2_b_sel <= 1;
        C_zre_partial_sel <= 0;
        C_zim_partial_sel <= 0;
        C_zre_acc_sel <= 3;
        C_zim_acc_sel <= 1;
        C_zre_wr_en <= 0;
        C_zim_wr_en <= 0;
        #20

        C_limb_ind <= 1;
        C_zre_ind <= 1;
        C_zim_ind <= 0;
        C_zre_reg_sel <= 2;
        C_zim_reg_sel <= 3;
        C_m1_a_sel <= 0;
        C_m1_b_sel <= 2;
        C_m2_a_sel <= 2;
        C_m2_b_sel <= 3;
        C_zre_partial_sel <= 0;
        C_zim_partial_sel <= 0;
        C_zre_acc_sel <= 1;
        C_zim_acc_sel <= 0;
        C_zre_wr_en <= 0;
        C_zim_wr_en <= 1;
        #20

        C_limb_ind <= 1;
        C_zre_ind <= 1;
        C_zim_ind <= 0;
        C_zre_reg_sel <= 2;
        C_zim_reg_sel <= 3;
        C_m1_a_sel <= 1;
        C_m1_b_sel <= 3;
        C_m2_a_sel <= 2;
        C_m2_b_sel <= 3;
        C_zre_partial_sel <= 1;
        C_zim_partial_sel <= 0;
        C_zre_acc_sel <= 0;
        C_zim_acc_sel <= 3;
        C_zre_wr_en <= 1;
        C_zim_wr_en <= 0;
        #20

        C_limb_ind <= 0;
        C_zre_ind <= 0;
        C_zim_ind <= 0;
        C_zre_reg_sel <= 0;
        C_zim_reg_sel <= 1;
        C_m1_a_sel <= 0;
        C_m1_b_sel <= 0;
        C_m2_a_sel <= 0;
        C_m2_b_sel <= 1;
        C_zre_partial_sel <= 2;
        C_zim_partial_sel <= 0;
        C_zre_acc_sel <= 1;
        C_zim_acc_sel <= 1;
        C_zre_wr_en <= 0;
        C_zim_wr_en <= 1;
        #20

        C_limb_ind <= 0;
        C_zre_ind <= 0;
        C_zim_ind <= 0;
        C_zre_reg_sel <= 0;
        C_zim_reg_sel <= 1;
        C_m1_a_sel <= 1;
        C_m1_b_sel <= 1;
        C_m2_a_sel <= 0;
        C_m2_b_sel <= 1;
        C_zre_partial_sel <= 3;
        C_zim_partial_sel <= 0;
        C_zre_acc_sel <= 0;
        C_zim_acc_sel <= 3;
        C_zre_wr_en <= 1;
        C_zim_wr_en <= 0;
        #20

        //Wait for pipeline to flush
        C_zre_wr_en   <= 0;
        C_zim_wr_en   <= 0;
        C_zre_ind     <= 0;
        C_zim_ind     <= 0;
        C_zre_acc_sel <= 0;
        C_zim_acc_sel <= 0;
        #20
        #20
        #20
        #20
        #20

        $finish;
    end

    always begin
        #10
        clock <= !clock;
        $display("cycle %d", cycle_num);
        cycle_num <= cycle_num + 1;
    end

    reg [LIMB_SIZE_BITS-1:0] C_cre_limb;
    reg [LIMB_SIZE_BITS-1:0] C_cim_limb;

    reg [LIMB_INDEX_BITS-1:0] C_limb_ind;
    reg [LIMB_INDEX_BITS-1:0] C_zre_ind;
    reg [LIMB_INDEX_BITS-1:0] C_zim_ind;

    //Load (L) control signals
    reg C_cre_wr_en;
    reg C_cim_wr_en;

    //Read (R) control signals
    reg [1:0] C_zre_reg_sel;
    reg [1:0] C_zim_reg_sel;

    //Multiply (M) control signals
    reg [1:0] C_m1_a_sel;
    reg [1:0] C_m1_b_sel;
    reg [1:0] C_m2_a_sel;
    reg [1:0] C_m2_b_sel;

    //Execute (X) control signals
    reg [1:0] C_zre_partial_sel;
    reg       C_zim_partial_sel;
    reg [1:0] C_zre_acc_sel;
    reg [1:0] C_zim_acc_sel;

    //Write (W) control signals
    reg C_zre_wr_en;
    reg C_zim_wr_en;

    //Output
    wire W_diverged;

    solver_datapath #(LIMB_INDEX_BITS, LIMB_SIZE_BITS, DIVERGENCE_RADIUS) datapath(
        .clock(clock),
        .reset(reset),
        .C_cre_limb(C_cre_limb),
        .C_cim_limb(C_cim_limb),
        .C_limb_ind(C_limb_ind),
        .C_zre_ind(C_zre_ind),
        .C_zim_ind(C_zim_ind),
        .C_cre_wr_en(C_cre_wr_en),
        .C_cim_wr_en(C_cim_wr_en),
        .C_zre_reg_sel(C_zre_reg_sel),
        .C_zim_reg_sel(C_zim_reg_sel),
        .C_m1_a_sel(C_m1_a_sel),
        .C_m1_b_sel(C_m1_b_sel),
        .C_m2_a_sel(C_m2_a_sel),
        .C_m2_b_sel(C_m2_b_sel),
        .C_zre_partial_sel(C_zre_partial_sel),
        .C_zim_partial_sel(C_zim_partial_sel),
        .C_zre_acc_sel(C_zre_acc_sel),
        .C_zim_acc_sel(C_zim_acc_sel),
        .C_zre_wr_en(C_zre_wr_en),
        .C_zim_wr_en(C_zim_wr_en),
        .W_diverged(W_diverged)
    );

endmodule
