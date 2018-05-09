`ifndef _solver_datapath_v_
`define _solver_datapath_v_

`include "ram.v"

// Stages:
//   C - Control (Not part of the datapath)
//   R - Read / L - Load cre, cim from input
//   M - Multiply
//   X - Excexute: add and accumulate
//   W - Write

module solver_datapath #(
    parameter LIMB_INDEX_BITS = 6,
    parameter LIMB_SIZE_BITS = 27,
    parameter DIVERGENCE_RADIUS = 4
) (
    input clock, reset,

    input [LIMB_SIZE_BITS-1:0] C_cre_limb,
    input [LIMB_SIZE_BITS-1:0] C_cim_limb,

    input [LIMB_INDEX_BITS-1:0] C_limb_ind,
    input [LIMB_INDEX_BITS-1:0] C_zre_ind,
    input [LIMB_INDEX_BITS-1:0] C_zim_ind,

    //Load (L) control signals
    input C_cre_wr_en,
    input C_cim_wr_en,

    //Read (R) control signals
    input [1:0] C_zre_reg_sel,
    input [1:0] C_zim_reg_sel,

    //Multiply (M) control signals
    input [1:0] C_m1_a_sel,
    input [1:0] C_m1_b_sel,
    input [1:0] C_m2_a_sel,
    input [1:0] C_m2_b_sel,

    //Execute (X) control signals
    input       C_op_sel,
    input [1:0] C_zre_partial_sel,
    input       C_zim_partial_sel,
    input [1:0] C_zre_acc_sel,
    input [1:0] C_zim_acc_sel,

    //Write (W) control signals
    input C_zre_wr_en,
    input C_zim_wr_en,

    //Output
    output     W_zre_sign,
    output     W_zim_sign,
    output reg W_diverged
);


// ---------- Read Stage (R) / Load Stage (L) ---------------------------------

//Control signals
reg        L_cre_wr_en;
reg        L_cim_wr_en;
reg  [1:0] R_zre_reg_sel;
reg  [1:0] R_zim_reg_sel;
reg  [1:0] R_m1_a_sel;
reg  [1:0] R_m1_b_sel;
reg  [1:0] R_m2_a_sel;
reg  [1:0] R_m2_b_sel;
reg        R_op_sel;
reg  [1:0] R_zre_partial_sel;
reg        R_zim_partial_sel;
reg  [1:0] R_zre_acc_sel;
reg  [1:0] R_zim_acc_sel;
reg        R_zre_wr_en;
reg        R_zim_wr_en;

//Datapath signals
reg  [LIMB_INDEX_BITS-1:0] R_limb_ind;
reg  [LIMB_INDEX_BITS-1:0] R_zre_ind;
reg  [LIMB_INDEX_BITS-1:0] R_zim_ind;

reg  [LIMB_SIZE_BITS-1:0] L_cre_limb;
reg  [LIMB_SIZE_BITS-1:0] L_cim_limb;

reg  [LIMB_SIZE_BITS-1:0] zre_ram [1<<LIMB_INDEX_BITS:0];
reg  [LIMB_SIZE_BITS-1:0] zim_ram [1<<LIMB_INDEX_BITS:0];
reg  [LIMB_SIZE_BITS-1:0] cre_ram [1<<LIMB_INDEX_BITS:0];
reg  [LIMB_SIZE_BITS-1:0] cim_ram [1<<LIMB_INDEX_BITS:0];

reg  [LIMB_SIZE_BITS-1:0] R_regA;
reg  [LIMB_SIZE_BITS-1:0] R_regB;
reg  [LIMB_SIZE_BITS-1:0] R_regC;
reg  [LIMB_SIZE_BITS-1:0] R_regD;

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
        //Control
        L_cre_wr_en       <= 0;
        L_cim_wr_en       <= 0;
        R_zre_reg_sel     <= 0;
        R_zim_reg_sel     <= 0;
        R_m1_a_sel        <= 0;
        R_m1_b_sel        <= 0;
        R_m2_a_sel        <= 0;
        R_m2_b_sel        <= 0;
        R_op_sel          <= 0;
        R_zre_partial_sel <= 0;
        R_zim_partial_sel <= 0;
        R_zre_acc_sel     <= 0;
        R_zim_acc_sel     <= 0;
        R_zre_wr_en       <= 0;
        R_zim_wr_en       <= 0;

        //Datapath
        L_cre_limb <= 0;
        L_cim_limb <= 0;
        R_limb_ind <= 0;
        R_zre_ind  <= 0;
        R_zim_ind  <= 0;
        R_regA     <= 0;
        R_regB     <= 0;
        R_regC     <= 0;
        R_regD     <= 0;
    end else begin
        //Control
        L_cre_wr_en       <= C_cre_wr_en;
        L_cim_wr_en       <= C_cim_wr_en;
        R_zre_reg_sel     <= C_zre_reg_sel;
        R_zim_reg_sel     <= C_zim_reg_sel;
        R_m1_a_sel        <= C_m1_a_sel;
        R_m1_b_sel        <= C_m1_b_sel;
        R_m2_a_sel        <= C_m2_a_sel;
        R_m2_b_sel        <= C_m2_b_sel;
        R_op_sel          <= C_op_sel;
        R_zre_partial_sel <= C_zre_partial_sel;
        R_zim_partial_sel <= C_zim_partial_sel;
        R_zre_acc_sel     <= C_zre_acc_sel;
        R_zim_acc_sel     <= C_zim_acc_sel;
        R_zre_wr_en       <= C_zre_wr_en;
        R_zim_wr_en       <= C_zim_wr_en;

        //Datapath
        L_cre_limb <= C_cre_limb;
        L_cim_limb <= C_cim_limb;
        R_limb_ind <= C_limb_ind;
        R_zre_ind  <= C_zre_ind;
        R_zim_ind  <= C_zim_ind;

        if (L_cre_wr_en) cre_ram[R_limb_ind] <= L_cre_limb;
        if (L_cim_wr_en) cim_ram[R_limb_ind] <= L_cim_limb;
        if (L_cre_wr_en) zre_ram[R_limb_ind] <= L_cre_limb;
        if (L_cim_wr_en) zim_ram[R_limb_ind] <= L_cim_limb;

        if      (R_zre_reg_sel == 0) R_regA <= R_zre_limb;
        else if (R_zim_reg_sel == 0) R_regA <= R_zim_limb;
        if      (R_zre_reg_sel == 1) R_regB <= R_zre_limb;
        else if (R_zim_reg_sel == 1) R_regB <= R_zim_limb;
        if      (R_zre_reg_sel == 2) R_regC <= R_zre_limb;
        else if (R_zim_reg_sel == 2) R_regC <= R_zim_limb;
        if      (R_zre_reg_sel == 3) R_regD <= R_zre_limb;
        else if (R_zim_reg_sel == 3) R_regD <= R_zim_limb;
    end
end


// ---------- Multiply Stage (M) ----------------------------------------------

//Control signals
reg  [1:0] M_m1_a_sel;
reg  [1:0] M_m1_b_sel;
reg  [1:0] M_m2_a_sel;
reg  [1:0] M_m2_b_sel;
reg        M_op_sel;
reg  [1:0] M_zre_partial_sel;
reg        M_zim_partial_sel;
reg  [1:0] M_zre_acc_sel;
reg  [1:0] M_zim_acc_sel;
reg        M_zre_wr_en;
reg        M_zim_wr_en;

//Datapath signal
reg  [LIMB_INDEX_BITS-1:0] M_limb_ind;

reg  [LIMB_SIZE_BITS-1:0] M_cre_limb;
reg  [LIMB_SIZE_BITS-1:0] M_cim_limb;
reg  [LIMB_SIZE_BITS-1:0] M_zre_limb;
reg  [LIMB_SIZE_BITS-1:0] M_zim_limb;

reg  [LIMB_SIZE_BITS-1:0] M_m1_a;
reg  [LIMB_SIZE_BITS-1:0] M_m1_b;
reg  [LIMB_SIZE_BITS-1:0] M_m2_a;
reg  [LIMB_SIZE_BITS-1:0] M_m2_b;

wire [(LIMB_SIZE_BITS<<1)-1:0] M_m1_out;
wire [(LIMB_SIZE_BITS<<1)-1:0] M_m2_out;

assign M_m1_out = M_m1_a * M_m1_b;
assign M_m2_out = M_m2_a * M_m2_b;

always @* begin
    case (M_m1_a_sel)
        0: M_m1_a = R_regA;
        1: M_m1_a = R_regB;
        2: M_m1_a = R_regC;
        3: M_m1_a = R_regD;
    endcase
    case (M_m1_b_sel)
        0: M_m1_b = R_regA;
        1: M_m1_b = R_regB;
        2: M_m1_b = R_regC;
        3: M_m1_b = R_regD;
    endcase
    case (M_m2_a_sel)
        0: M_m2_a = R_regA;
        1: M_m2_a = R_regB;
        2: M_m2_a = R_regC;
        3: M_m2_a = R_regD;
    endcase
    case (M_m2_b_sel)
        0: M_m2_b = R_regA;
        1: M_m2_b = R_regB;
        2: M_m2_b = R_regC;
        3: M_m2_b = R_regD;
    endcase
end

always @(posedge clock) begin
    if (reset) begin
        //Control
        M_m1_a_sel        <= 0;
        M_m1_b_sel        <= 0;
        M_m2_a_sel        <= 0;
        M_m2_b_sel        <= 0;
        M_op_sel          <= 0;
        M_zre_partial_sel <= 0;
        M_zim_partial_sel <= 0;
        M_zre_acc_sel     <= 0;
        M_zim_acc_sel     <= 0;
        M_zre_wr_en       <= 0;
        M_zim_wr_en       <= 0;

        //Datapath
        M_limb_ind <= 0;
        M_cre_limb <= 0;
        M_cim_limb <= 0;
        M_zre_limb <= 0;
        M_zim_limb <= 0;
    end else begin
        //Control
        M_m1_a_sel        <= R_m1_a_sel;
        M_m1_b_sel        <= R_m1_b_sel;
        M_m2_a_sel        <= R_m2_a_sel;
        M_m2_b_sel        <= R_m2_b_sel;
        M_op_sel          <= R_op_sel;
        M_zre_partial_sel <= R_zre_partial_sel;
        M_zim_partial_sel <= R_zim_partial_sel;
        M_zre_acc_sel     <= R_zre_acc_sel;
        M_zim_acc_sel     <= R_zim_acc_sel;
        M_zre_wr_en       <= R_zre_wr_en;
        M_zim_wr_en       <= R_zim_wr_en;

        //Datapath
        M_limb_ind <= R_limb_ind;
        M_cre_limb <= R_cre_limb;
        M_cim_limb <= R_cim_limb;
        M_zre_limb <= R_zre_limb;
        M_zim_limb <= R_zim_limb;
    end
end


// ---------- Execute Stage (X) -----------------------------------------------

//Control signals
reg        X_op_sel;
reg  [1:0] X_zre_partial_sel;
reg        X_zim_partial_sel;
reg  [1:0] X_zre_acc_sel;
reg  [1:0] X_zim_acc_sel;
reg        X_zre_wr_en;
reg        X_zim_wr_en;

//Datapath signals
reg  [LIMB_INDEX_BITS-1:0] X_limb_ind;

reg  [LIMB_SIZE_BITS-1:0] X_cre_limb;
reg  [LIMB_SIZE_BITS-1:0] X_cim_limb;
reg  [LIMB_SIZE_BITS-1:0] X_zre_limb;
reg  [LIMB_SIZE_BITS-1:0] X_zim_limb;

localparam ACCUMULATOR_BITS = (LIMB_SIZE_BITS << 1) + LIMB_INDEX_BITS;
reg  [ACCUMULATOR_BITS-1:0] X_m1_out;
reg  [ACCUMULATOR_BITS-1:0] X_m2_out;
reg  [ACCUMULATOR_BITS-1:0] X_m1_old;

wire X_diverged;

reg  signed [ACCUMULATOR_BITS-1:0] X_zre_acc; //zre accumulator
reg  signed [ACCUMULATOR_BITS-1:0] X_zim_acc; //zim accumulator

reg  [ACCUMULATOR_BITS-1:0] X_zre_partial;
reg  [ACCUMULATOR_BITS-1:0] X_zim_partial;
reg  [ACCUMULATOR_BITS-1:0] X_zre_acc_next;
reg  [ACCUMULATOR_BITS-1:0] X_zim_acc_next;

wire [LIMB_SIZE_BITS-1:0] X_zre_limb_out;
wire [LIMB_SIZE_BITS-1:0] X_zim_limb_out;

assign X_diverged = X_m1_old + X_m1_out >= DIVERGENCE_RADIUS;

assign X_zre_limb_out = X_zre_acc_next[LIMB_SIZE_BITS-1:0];
assign X_zim_limb_out = X_zim_acc_next[LIMB_SIZE_BITS-1:0];

always @* begin
    case (X_zre_partial_sel)
        0: X_zre_partial =  X_m1_out << 1;
        1: X_zre_partial = -X_m1_out << 1;
        2: X_zre_partial =  X_m1_out;
        3: X_zre_partial = -X_m1_out;
    endcase
    case (X_zim_partial_sel)
        0: X_zim_partial =  X_m2_out << 1;
        1: X_zim_partial = -X_m2_out << 1;
    endcase

    if (X_op_sel == 1'b0) begin
        //Multiply operation
        case (X_zre_acc_sel)
            0: X_zre_acc_next = X_zre_partial + X_zre_acc;                                                  //Add partial into the accumulator
            1: X_zre_acc_next = X_zre_partial + $signed(X_zre_acc >>> LIMB_SIZE_BITS) + X_cre_limb;         //Shift accumulator to only store the carry and add partial and c
            2: X_zre_acc_next = X_zre_partial + 0;                                                          //Set accumulator to partial
            3: X_zre_acc_next = X_zre_acc_next;                                                             //Do nothing
            default: $display("[ERROR] X_zre_acc_sel has illegal value: %b for multiply op", X_zre_acc_sel);
        endcase
        case (X_zim_acc_sel)
            0: X_zim_acc_next = X_zim_partial + X_zim_acc;                                                  //Add partial into the accumulator
            1: X_zim_acc_next = X_zim_partial + $signed(X_zim_acc >>> LIMB_SIZE_BITS) + X_cim_limb;         //Shift accumulator to only store the carry and add partial and c
            2: X_zim_acc_next = X_zim_partial + 0;                                                          //Set accumulator to partial
            3: X_zim_acc_next = X_zim_acc_next;                                                             //Do nothing
            default: $display("[ERROR] X_zim_acc_sel has illegal value: %b for multiply op", X_zim_acc_sel);
        endcase
    end else if (X_op_sel == 1'b1) begin
        //Negate operation
        case (X_zre_acc_sel)
            0: X_zre_acc_next =  X_zre_limb;                                                                //Do not modify z limb
            1: X_zre_acc_next = (X_zre_limb ^ {(LIMB_SIZE_BITS){1'b1}}) + 1;                                //Invert z limb and add 1
            2: X_zre_acc_next = (X_zre_limb ^ {(LIMB_SIZE_BITS){1'b1}}) + (X_zre_acc >>> LIMB_SIZE_BITS);   //Invert z limb and add carry
            default: $display("[ERROR] X_zre_acc_sel has illegal value: %b for negate op", X_zre_acc_sel);
        endcase
        case (X_zim_acc_sel)
            0: X_zim_acc_next =  X_zim_limb;                                                                //Do not modify z limb
            1: X_zim_acc_next = (X_zim_limb ^ {(LIMB_SIZE_BITS){1'b1}}) + 1;                                //Invert z limb and add 1
            2: X_zim_acc_next = (X_zim_limb ^ {(LIMB_SIZE_BITS){1'b1}}) + (X_zim_acc >>> LIMB_SIZE_BITS);   //Invert z limb and add carry
            default: $display("[ERROR] X_zim_acc_sel has illegal value: %b for negate op", X_zim_acc_sel);
        endcase
    end else begin
        $display("[ERROR] X_op_sel has illegal value: %b", X_op_sel);
    end
end

always @(posedge clock) begin
    if (reset) begin
        //Control
        X_op_sel          <= 0;
        X_zre_partial_sel <= 0;
        X_zim_partial_sel <= 0;
        X_zre_acc_sel     <= 0;
        X_zim_acc_sel     <= 0;
        X_zre_wr_en       <= 0;
        X_zim_wr_en       <= 0;

        //Datapath
        X_limb_ind <= 0;
        X_cre_limb <= 0;
        X_cim_limb <= 0;
        X_zre_limb <= 0;
        X_zim_limb <= 0;
        X_m1_out   <= 0;
        X_m2_out   <= 0;
        X_m1_old   <= 0;
        X_zre_acc  <= 0;
        X_zim_acc  <= 0;
    end else begin
        //Control
        X_zre_partial_sel <= M_zre_partial_sel;
        X_zim_partial_sel <= M_zim_partial_sel;
        X_zre_acc_sel     <= M_zre_acc_sel;
        X_zim_acc_sel     <= M_zim_acc_sel;
        X_zre_wr_en       <= M_zre_wr_en;
        X_zim_wr_en       <= M_zim_wr_en;

        //Datapath
        X_op_sel   <= M_op_sel;
        X_limb_ind <= M_limb_ind;
        X_cre_limb <= M_cre_limb;
        X_cim_limb <= M_cim_limb;
        X_zre_limb <= M_zre_limb;
        X_zim_limb <= M_zim_limb;
        X_m1_out   <= M_m1_out;
        X_m2_out   <= M_m2_out;
        X_m1_old   <= X_m1_out;
        X_zre_acc  <= X_zre_acc_next;
        X_zim_acc  <= X_zim_acc_next;
    end
end


// ---------- Write Stage (W) -------------------------------------------------

//Control signals
reg  W_zre_wr_en;
reg  W_zim_wr_en;

//Datapath signals
reg  [LIMB_INDEX_BITS-1:0] W_limb_ind;

reg  [LIMB_SIZE_BITS-1:0] W_zre_limb;
reg  [LIMB_SIZE_BITS-1:0] W_zim_limb;

assign W_zre_sign = W_zre_limb[LIMB_SIZE_BITS-1];
assign W_zim_sign = W_zim_limb[LIMB_SIZE_BITS-1];

always @(posedge clock) begin
    if (reset) begin
        //Control
        W_zre_wr_en   <= 0;
        W_zim_wr_en   <= 0;

        //Datapath
        W_limb_ind <= 0;
        W_zre_limb <= 0;
        W_zim_limb <= 0;
        W_diverged <= 0;
    end else begin
        //Control
        W_zre_wr_en   <= X_zre_wr_en;
        W_zim_wr_en   <= X_zim_wr_en;

        //Datapath
        W_limb_ind <= X_limb_ind;
        W_zre_limb <= X_zre_limb_out;
        W_zim_limb <= X_zim_limb_out;
        W_diverged <= X_diverged;

        if (W_zre_wr_en) $display("zre[%d] := %h", W_limb_ind, W_zre_limb);
        if (W_zim_wr_en) $display("zim[%d] := %h", W_limb_ind, W_zim_limb);
        if (W_zre_wr_en) zre_ram[W_limb_ind] <= W_zre_limb;
        if (W_zim_wr_en) zim_ram[W_limb_ind] <= W_zim_limb;
    end
end

always @(posedge clock) begin
    //*
    if (X_op_sel == 1'b1) begin
        //$display("NNNNNNNNNEEEEEEEEEEEEEEEEGGGGGGGGGGGGGGAAAAAAAAAAAAATTTTTTTTTTTTIIIIIIIIIIIINNNNNNNNNNNNNNGGGGGGGGGGGGGG");
        /*
        case (X_zre_acc_sel)
            0: $display("X_zre_acc_next(%h) =  X_zre_limb(%h); //Do not modify z limb", X_zre_acc_next, X_zre_limb);
            1: $display("X_zre_acc_next(%h) = (X_zre_limb(%h) ^ {(LIMB_SIZE_BITS){1'b1}})(%h) + 1; //Invert z limb and add 1", X_zre_acc_next, X_zre_limb, X_zre_limb ^ {(LIMB_SIZE_BITS){1'b1}});
            2: $display("X_zre_acc_next(%h) = (X_zre_limb(%h) ^ {(LIMB_SIZE_BITS){1'b1}})(%h) + (X_zre_acc(%h) >>> LIMB_SIZE_BITS)(%h); //Invert z limb and add carry", X_zre_acc_next, X_zre_limb, X_zre_limb ^ {(LIMB_SIZE_BITS){1'b1}}, X_zre_acc, X_zre_acc >>> LIMB_SIZE_BITS);
            default: $display("[ERROR] X_zre_acc_sel has illegal value: %b for negate op", X_zre_acc_sel);
        endcase
        //*/
        /*
        case (X_zim_acc_sel)
            0: $display("X_zim_acc_next(%h) =  X_zim_limb(%h); //Do not modify z limb", X_zim_acc_next, X_zim_limb);
            1: $display("X_zim_acc_next(%h) = (X_zim_limb(%h) ^ {(LIMB_SIZE_BITS){1'b1}})(%h) + 1; //Invert z limb and add 1", X_zim_acc_next, X_zim_limb, X_zim_limb ^ {(LIMB_SIZE_BITS){1'b1}});
            2: $display("X_zim_acc_next(%h) = (X_zim_limb(%h) ^ {(LIMB_SIZE_BITS){1'b1}})(%h) + (X_zim_acc(%h) >>> LIMB_SIZE_BITS)(%h); //Invert z limb and add carry", X_zim_acc_next, X_zim_limb, X_zim_limb ^ {(LIMB_SIZE_BITS){1'b1}}, X_zim_acc, X_zim_acc >>> LIMB_SIZE_BITS);
            default: $display("[ERROR] X_zim_acc_sel has illegal value: %b for negate op", X_zim_acc_sel);
        endcase
        //*/
    end
    //*/

    //$display("C_op_sel %b -> R_op_sel %b -> M_op_sel %b -> X_op_sel %b", C_op_sel, R_op_sel, M_op_sel, X_op_sel);
    //$display("R | cre[%d] -> %h | cim[%d] -> %h | zre[%d] -> %h | zim[%d] -> %h", R_limb_ind, R_cre_limb, R_limb_ind, R_cim_limb, R_zre_ind, R_zre_limb, R_zim_ind, R_zim_limb);
    //$display("M | m1: %h * %h = %h | m2: %h * %h = %h", M_m1_a, M_m1_b, M_m1_out, M_m2_a, M_m2_b, M_m2_out);
    /*
    case (X_zre_partial_sel)
        0: $display("X | zre_partial(%h) = [ m1_out(%h) << 1](%h)", X_zre_partial, X_m1_out, X_m1_out << 1);
        1: $display("X | zre_partial(%h) = [-m1_out(%h) << 1](%h)", X_zre_partial, X_m1_out, -X_m1_out << 1);
        2: $display("X | zre_partial(%h) = [ m1_out(%h)](%h)", X_zre_partial, X_m1_out, X_m1_out);
        3: $display("X | zre_partial(%h) = [-m1_out(%h)](%h)", X_zre_partial, X_m1_out, -X_m1_out);
    endcase
    case (X_zre_acc_sel)
        0: $display("X | zre_acc(%h) := partial(%h) + zre_acc(%h) //Add partial into the accumulator", X_zre_acc_next, X_zre_partial, X_zre_acc);
        1: $display("X | zre_acc(%h) := partial(%h) + [zre_acc(%h)>>>%d](%h) + cre_limb(%h) //Shift accumulator to only store the carry and add partial and c", X_zre_acc_next, X_zre_partial, X_zre_acc, LIMB_SIZE_BITS, X_zre_acc >>> LIMB_SIZE_BITS, X_cre_limb);
        2: $display("X | zre_acc(%h) := partial(%h) + 0 //Set accumulator to partial", X_zre_acc_next, X_zre_partial);
        3: $display("X | zre_acc(%h) := zre_acc(%h) //Do nothing", X_zre_acc_next, X_zre_acc_next);
        default: $display("[ERROR] X_zre_acc_sel has illegal value: %b", X_zre_acc_sel);
    endcase
    //*/
    /*
    case (X_zim_partial_sel)
        0: $display("X | zim_partial(%h) = [ m2_out(%h) << 1](%h)", X_zim_partial, X_m2_out, X_m2_out << 1);
        1: $display("X | zim_partial(%h) = [-m2_out(%h) << 1](%h)", X_zim_partial, X_m2_out, -X_m2_out << 1);
    endcase
    case (X_zim_acc_sel)
        0: $display("X | zim_acc(%h) := partial(%h) + zim_acc(%h) //Add partial into the accumulator", X_zim_acc_next, X_zim_partial, X_zim_acc);
        1: $display("X | zim_acc(%h) := partial(%h) + [zim_acc(%h)>>>%d](%h) + cim_limb(%h) //Shift accumulator to only store the carry and add partial and c", X_zim_acc_next, X_zim_partial, X_zim_acc, LIMB_SIZE_BITS, X_zim_acc >>> LIMB_SIZE_BITS, X_cre_limb);
        2: $display("X | zim_acc(%h) := partial(%h) + 0 //Set accumulator to partial", X_zim_acc_next, X_zim_partial);
        3: $display("X | zim_acc(%h) := zim_acc(%h) //Do nothing", X_zim_acc_next, X_zim_acc_next);
        default: $display("[ERROR] X_zim_acc_sel has illegal value: %b", X_zim_acc_sel);
    endcase
    //*/
end

endmodule

`endif
