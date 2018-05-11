`ifndef _solver_v_
`define _sovler_v_

`include "solver_control.v"
`include "solver_datapath.v"

module solver #(
    parameter LIMB_INDEX_BITS   = 6,
    parameter LIMB_SIZE_BITS    = 27,
    parameter DIVERGENCE_RADIUS = 4
) (
    input  clock, reset,

    input                       wr_real_en,
    input                       wr_imag_en,
    input [LIMB_INDEX_BITS-1:0] wr_index,
    input [LIMB_SIZE_BITS-1:0]  real_data,
    input [LIMB_SIZE_BITS-1:0]  imag_data,
    input                       wr_num_limbs_en,
    input [LIMB_INDEX_BITS-1:0] num_limbs_data,
    input                       wr_iter_lim_en,
    input [15:0]                iter_lim_data,

    input start,

    output                      out_ready,
    output [15:0]               iterations
);

wire                        cre_wr_en;
wire                        cim_wr_en;
wire [LIMB_INDEX_BITS-1:0]  c_limb_ind;
wire [LIMB_INDEX_BITS-1:0]  zre_rd_ind;
wire [LIMB_INDEX_BITS-1:0]  zim_rd_ind;
wire [1:0]                  zre_reg_sel;
wire [1:0]                  zim_reg_sel;
wire [1:0]                  m1_a_sel;
wire [1:0]                  m1_b_sel;
wire [1:0]                  m2_a_sel;
wire [1:0]                  m2_b_sel;
wire                        op_sel;
wire [2:0]                  zre_partial_sel;
wire [1:0]                  zim_partial_sel;
wire [1:0]                  zre_acc_sel;
wire [1:0]                  zim_acc_sel;
wire                        zre_wr_en;
wire                        zim_wr_en;
wire [LIMB_INDEX_BITS-1:0]  zre_wr_ind;
wire [LIMB_INDEX_BITS-1:0]  zim_wr_ind;

wire zre_sign;
wire zim_sign;
wire diverged;

solver_control #(LIMB_INDEX_BITS) control(
    .clock(clock),
    .reset(reset),
    .wr_real_en(wr_real_en),
    .wr_imag_en(wr_imag_en),
    .wr_ind(wr_index),
    .wr_num_limbs_en(wr_num_limbs_en),
    .num_limbs_data(num_limbs_data),
    .wr_iter_lim_en(wr_iter_lim_en),
    .iter_lim_data(iter_lim_data),
    .start(start),

    .cre_wr_en(cre_wr_en),
    .cim_wr_en(cim_wr_en),
    .c_limb_ind(c_limb_ind),
    .zre_rd_ind(zre_rd_ind),
    .zim_rd_ind(zim_rd_ind),
    .zre_reg_sel(zre_reg_sel),
    .zim_reg_sel(zim_reg_sel),
    .m1_a_sel(m1_a_sel),
    .m1_b_sel(m1_b_sel),
    .m2_a_sel(m2_a_sel),
    .m2_b_sel(m2_b_sel),
    .op_sel(op_sel),
    .zre_partial_sel(zre_partial_sel),
    .zim_partial_sel(zim_partial_sel),
    .zre_acc_sel(zre_acc_sel),
    .zim_acc_sel(zim_acc_sel),
    .zre_wr_en(zre_wr_en),
    .zim_wr_en(zim_wr_en),
    .zre_wr_ind(zre_wr_ind),
    .zim_wr_ind(zim_wr_ind),

    .zre_sign(zre_sign),
    .zim_sign(zim_sign),
    .diverged(diverged),

    .out_ready(out_ready),
    .iteration_count(iterations)
);

solver_datapath #(LIMB_INDEX_BITS, LIMB_SIZE_BITS, DIVERGENCE_RADIUS) data_path (
    .clock              (clock),
    .reset              (reset),
    .C_cre_limb         (real_data),
    .C_cim_limb         (imag_data),
    .C_cre_wr_en        (cre_wr_en),
    .C_cim_wr_en        (cim_wr_en),
    .C_c_limb_ind       (c_limb_ind),
    .C_zre_rd_ind       (zre_rd_ind),
    .C_zim_rd_ind       (zim_rd_ind),
    .C_zre_reg_sel      (zre_reg_sel),
    .C_zim_reg_sel      (zim_reg_sel),
    .C_m1_a_sel         (m1_a_sel),
    .C_m1_b_sel         (m1_b_sel),
    .C_m2_a_sel         (m2_a_sel),
    .C_m2_b_sel         (m2_b_sel),
    .C_op_sel           (op_sel),
    .C_zre_partial_sel  (zre_partial_sel),
    .C_zim_partial_sel  (zim_partial_sel),
    .C_zre_acc_sel      (zre_acc_sel),
    .C_zim_acc_sel      (zim_acc_sel),
    .C_zre_wr_en        (zre_wr_en),
    .C_zim_wr_en        (zim_wr_en),
    .C_zre_wr_ind       (zre_wr_ind),
    .C_zim_wr_ind       (zim_wr_ind),
    .W_zre_sign         (zre_sign),
    .W_zim_sign         (zim_sign),
    .W_diverged         (diverged)
);

endmodule

`endif
