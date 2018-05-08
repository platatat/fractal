`ifndef _solver_control_v_
`define _sovler_control_v_


module solver #(
    parameter LIMB_INDEX_BITS   = 6,
    parameter LIMB_SIZE_BITS    = 27,
    parameter DIVERGENCE_RADIUS = 4
) (
    input                       clock,
    input                       reset,

    input                       wr_real_en,
    input                       wr_imag_en,
    input [LIMB_INDEX_BITS]     wr_limb,    // Which limb to write
    input [LIMB_SIZE_BITS]      wr_data,    // Data to write to the limb

    input                       start,

    output                      out_ready,
    output [LIMB_SIZE_BITS-1:0] iterations
);
    wire        cre_wr_en;
    wire        cim_wr_en;

    wire [1:0]  zre_reg_sel;
    wire [1:0]  zim_reg_sel;

    wire [1:0]  m1_a_sel;
    wire [1:0]  m1_b_sel;
    wire [1:0]  m2_a_sel;
    wire [1:0]  m2_b_sel;

    wire        op_sel;
    wire [1:0]  zre_partial_sel;
    wire        zim_partial_sel;
    wire [1:0]  zre_acc_sel;
    wire [1:0]  zim_acc_sel;

    wire zre_wr_en;
    wire zim_wr_en;

    wire diverged;

    solver_control #(LIMB_INDEX_BITS) control_path (
        .clock              (clock),
        .reset              (reset),
        .wr_en              (wr_en),
        .wr_limb            (wr_limbm),
        .cre_limb           (cre_limb),
        .cim_limb           (cim_limb),
        .limb_ind           (limb_ind),
        .zre_ind            (zre_ind),
        .zim_ind            (zim_ind),
        .cre_wr_en          (cre_wr_en),
        .cim_wr_en          (cim_wr_en),
        .zre_reg_sel        (zre_reg_sel),
        .zim_reg_sel        (zim_reg_sel),
        .m1_a_sel           (m1_a_sel),
        .m1_b_sel           (m1_b_sel),
        .m2_a_sel           (m2_a_sel),
        .m2_b_sel           (m2_b_sel),
        .op_sel             (op_sel),
        .zre_partial_sel    (zre_partial_sel),
        .zim_partial_sel    (zim_partial_sel),
        .zre_acc_sel        (zre_acc_sel),
        .zim_acc_sel        (zim_acc_sel),
        .zre_wr_en          (zre_wr_en),
        .zim_wr_en          (zim_wr_en),
        .diverged           (diverged)
    );

    solver_datapath #(LIMB_INDEX_BITS, LIMB_SIZE_BITS, DIVERGENCE_RADIUS) data_path (
        .clock              (clock),
        .reset              (reset),
        .C_cre_limb         (cre_limb),
        .C_cim_limb         (cim_limb),
        .C_limb_ind         (limb_ind),
        .C_zre_ind          (zre_ind),
        .C_zim_ind          (zim_ind),
        .C_cre_wr_en        (cre_wr_en),
        .C_cim_wr_en        (cim_wr_en),
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
        .W_diverged         (diverged)
    );

endmodule

`endif
