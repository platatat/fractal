`ifndef _solver_datapath_v_
`define _solver_datapath_v_

`include "ram.v"

// Main Stages:
//   F - Fetch
//   R - Read
//   M - Multiply
//   X - Excexute: add and accumulate
//   W - Write

// Other Stages:
//   L - Load cre, cim from input

module solver_datapath #(
    parameter LIMB_INDEX_BITS = 6,
    parameter LIMB_SIZE_BITS = 27
) (
    input clock, reset,

    input [LIMB_INDEX_BITS-1:0] in_num_limbs,
    input [LIMB_SIZE_BITS-1:0] in_cre_limb,
    input [LIMB_SIZE_BITS-1:0] in_cim_limb,

    //Fetch (F) control signals
    input       F_num_limbs_sel,
    input [1:0] F_limb_ind_sel,
    input [1:0] F_zre_ind_sel,

    //Execute (X) control signals
    input [1:0] X_zre_acc_sel,
    input [1:0] X_zim_acc_sel,

    //Write (W) control signals
    input W_zre_wr_en,
    input W_zim_wr_en,

    //Load (L) control signals
    input L_cre_wr_en,
    input L_cim_wr_en
);


// ---------- Fetch Stage (F) -------------------------------------------------

reg  [LIMB_INDEX_BITS-1:0] F_num_limbs;
reg  [LIMB_INDEX_BITS-1:0] F_limb_ind;
reg  [LIMB_INDEX_BITS-1:0] F_zre_ind;

wire [LIMB_INDEX_BITS-1:0] F_zim_ind;

assign F_zim_ind = F_limb_ind - F_zre_ind;

always @(posedge clock) begin
    if (reset) begin
        F_num_limbs <= 0;
        F_limb_ind <= 0;
        F_zre_ind <= 0;
    end else begin
        case (F_num_limbs_sel)
            0: F_num_limbs <= F_num_limbs;      //Do not change F_num_limbs
            1: F_num_limbs <= in_num_limbs;     //Load F_num_limbs from input
            default: $display("[ERROR] F_num_limbs_sel has illegal value: %b", F_num_limbs_sel);
        endcase

        case (F_limb_ind_sel)
            0: F_limb_ind <= F_limb_ind;        //Do not change F_limb_ind
            1: F_limb_ind <= F_limb_ind + 1;    //Increment F_limb_ind
            2: F_limb_ind <= F_num_limbs - 1;   //Reset to LSB
            default: $display("[ERROR] F_limb_ind_sel has illegal value: %b", F_limb_ind_sel);
        endcase

        case (F_zre_ind_sel)
            0: F_zre_ind <= F_zre_ind;          //Do not change F_zre_ind
            1: F_zre_ind <= F_zre_ind + 1;      //Increment F_zre_ind
            2: F_zre_ind <= 0;                  //Reset to MSB
            default: $display("[ERROR] F_zre_ind_sel has illegal value: %b", F_zre_ind_sel);
        endcase
    end
end


// ---------- Read Stage (R) / Load Stage (L) ---------------------------------

reg  [LIMB_INDEX_BITS-1:0] R_limb_ind;
reg  [LIMB_INDEX_BITS-1:0] R_zre_ind;
reg  [LIMB_INDEX_BITS-1:0] R_zim_ind;

reg  [LIMB_SIZE_BITS-1:0] L_in_cre_limb;
reg  [LIMB_SIZE_BITS-1:0] L_in_cim_limb;

reg  [LIMB_SIZE_BITS-1:0] zre_ram [1<<LIMB_INDEX_BITS:0];
reg  [LIMB_SIZE_BITS-1:0] zim_ram [1<<LIMB_INDEX_BITS:0];
reg  [LIMB_SIZE_BITS-1:0] cre_ram [1<<LIMB_INDEX_BITS:0];
reg  [LIMB_SIZE_BITS-1:0] cim_ram [1<<LIMB_INDEX_BITS:0];

wire [LIMB_SIZE_BITS-1:0] R_zre_limb;
wire [LIMB_SIZE_BITS-1:0] R_zim_limb;
wire [LIMB_SIZE_BITS-1:0] R_cre_limb;
wire [LIMB_SIZE_BITS-1:0] R_cim_limb;

assign R_zre_limb = zre_ram[R_zre_ind];
assign R_zim_limb = zim_ram[R_zim_ind];
assign R_cre_limb = cre_ram[R_limb_ind];
assign R_cim_limb = cim_ram[R_limb_ind];

always @(posedge clock) begin
    if (reset) begin
        R_limb_ind <= 0;
        R_zre_ind <= 0;
        R_zim_ind <= 0;

        L_in_cre_limb <= 0;
        L_in_cim_limb <= 0;
    end else begin
        R_limb_ind <= F_limb_ind;
        R_zre_ind <= F_zre_ind;
        R_zim_ind <= F_zim_ind;

        L_in_cre_limb <= in_cre_limb;
        L_in_cim_limb <= in_cim_limb;

        if (L_cre_wr_en) cre_ram[R_limb_ind] <= L_in_cre_limb;
        if (L_cim_wr_en) cim_ram[R_limb_ind] <= L_in_cim_limb;
    end
end


// ---------- Multiply Stage (M) ----------------------------------------------

reg  [LIMB_INDEX_BITS-1:0] M_limb_ind;

reg  [LIMB_SIZE_BITS-1:0] M_zre_limb;
reg  [LIMB_SIZE_BITS-1:0] M_zim_limb;
reg  [LIMB_SIZE_BITS-1:0] M_cre_limb;
reg  [LIMB_SIZE_BITS-1:0] M_cim_limb;

wire [(LIMB_SIZE_BITS<<1)-1:0] M_zre_zim;
wire [(LIMB_SIZE_BITS<<1)-1:0] M_zre_sq;
wire [(LIMB_SIZE_BITS<<1)-1:0] M_zim_sq;

assign M_zre_zim = M_zre_limb * M_zim_limb;
assign M_zre_sq  = M_zre_limb * M_zre_limb;
assign M_zre_sq  = M_zim_limb * M_zim_limb;

always @(posedge clock) begin
    if (reset) begin
        M_limb_ind <= 0;
        M_zre_limb <= 0;
        M_zim_limb <= 0;
        M_cre_limb <= 0;
        M_cim_limb <= 0;
    end else begin
        M_limb_ind <= R_limb_ind;
        M_zre_limb <= R_zre_limb;
        M_zim_limb <= R_zim_limb;
        M_cre_limb <= R_cre_limb;
        M_cim_limb <= R_cim_limb;
    end
end


// ---------- Execute Stage (X) -----------------------------------------------

reg  [LIMB_INDEX_BITS-1:0] X_limb_ind;

reg  [LIMB_SIZE_BITS-1:0] X_cre_limb;
reg  [LIMB_SIZE_BITS-1:0] X_cim_limb;

reg  [(LIMB_SIZE_BITS<<1)-1:0] X_zre_zim;
reg  [(LIMB_SIZE_BITS<<1)-1:0] X_zre_sq;
reg  [(LIMB_SIZE_BITS<<1)-1:0] X_zim_sq;

localparam ACCUMULATOR_BITS = (LIMB_SIZE_BITS << 1) + LIMB_INDEX_BITS;
reg  [ACCUMULATOR_BITS-1:0] X_zre_acc; //zre accumulator
reg  [ACCUMULATOR_BITS-1:0] X_zim_acc; //zim accumulator

wire [ACCUMULATOR_BITS-1:0] X_zre_partial;
wire [ACCUMULATOR_BITS-1:0] X_zim_partial;

wire [LIMB_SIZE_BITS-1:0] X_zre_limb_out;
wire [LIMB_SIZE_BITS-1:0] X_zim_limb_out;

assign X_zre_partial = X_zre_sq - X_zim_sq + X_cre_limb;
assign X_zim_partial = (X_zre_zim << 1) + X_cim_limb;

assign X_zre_limb_out = X_zre_acc[LIMB_SIZE_BITS-1:0];
assign X_zim_limb_out = X_zim_acc[LIMB_SIZE_BITS-1:0];

always @(posedge clock) begin
    if (reset) begin
        X_limb_ind <= 0;
        X_cre_limb <= 0;
        X_cim_limb <= 0;
        X_zre_zim  <= 0;
        X_zre_sq   <= 0;
        X_zim_sq   <= 0;
        X_zre_acc  <= 0;
        X_zim_acc  <= 0;
    end else begin
        X_limb_ind <= M_limb_ind;
        X_cre_limb <= M_cre_limb;
        X_cim_limb <= M_cim_limb;
        X_zre_zim  <= M_zre_zim;
        X_zre_sq   <= M_zre_sq;
        X_zim_sq   <= M_zim_sq;

        case (X_zre_acc_sel)
            0: X_zre_acc <= X_zre_acc + X_zre_partial;      //Add partial into the accumulator
            1: X_zre_acc <= X_zre_acc >> LIMB_SIZE_BITS;    //Shift accumulator to only store the carry
            2: X_zre_acc <= 0;                              //Reset accumulator to zero
            default: $display("[ERROR] X_zre_acc_sel has illegal value: %b", X_zre_acc_sel);
        endcase

        case (X_zim_acc_sel)
            0: X_zim_acc <= X_zim_acc + X_zim_partial;      //Add partial into the accumulator
            1: X_zim_acc <= X_zim_acc >> LIMB_SIZE_BITS;    //Shift accumulator to only store the carry
            2: X_zim_acc <= 0;                              //Reset accumulator to zero
            default: $display("[ERROR] X_zim_acc_sel has illegal value: %b", X_zim_acc_sel);
        endcase
    end
end


// ---------- Write Stage (W) -------------------------------------------------

reg  [LIMB_INDEX_BITS-1:0] W_limb_ind;

reg  [LIMB_SIZE_BITS-1:0] W_zre_limb;
reg  [LIMB_SIZE_BITS-1:0] W_zim_limb;

always @(posedge clock) begin
    if (reset) begin
        W_limb_ind <= 0;
        W_zre_limb <= 0;
        W_zim_limb <= 0;
    end else begin
        W_limb_ind <= X_limb_ind;
        W_zre_limb <= X_zre_limb_out;
        W_zim_limb <= X_zim_limb_out;

        if (W_zre_wr_en) zre_ram[X_limb_ind] <= W_zre_limb;
        if (W_zim_wr_en) zim_ram[X_limb_ind] <= W_zim_limb;
    end
end

endmodule

`endif
