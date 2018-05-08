`ifndef _solver_control_v_
`define _sovler_control_v_

module solver_control #(
    parameter LIMB_INDEX_BITS = 6
) (
    input clock, reset,

    input                       wr_real_en,
    input                       wr_imag_en,
    input [LIMB_INDEX_BITS-1:0] wr_ind,
    input                       wr_num_limbs_en,
    input [LIMB_INDEX_BITS-1:0] num_limbs_data,
    input                       wr_iter_lim_en,
    input [15:0]                iter_lim_data,
    input start,

    output reg [LIMB_INDEX_BITS-1:0] limb_ind,
    output reg [LIMB_INDEX_BITS-1:0] zre_ind,
    output reg [LIMB_INDEX_BITS-1:0] zim_ind,
    output reg       cre_wr_en,
    output reg       cim_wr_en,
    output reg [1:0] zre_reg_sel,
    output reg [1:0] zim_reg_sel,
    output reg [1:0] m1_a_sel,
    output reg [1:0] m1_b_sel,
    output reg [1:0] m2_a_sel,
    output reg [1:0] m2_b_sel,
    output reg       op_sel,
    output reg [1:0] zre_partial_sel,
    output reg       zim_partial_sel,
    output reg [1:0] zre_acc_sel,
    output reg [1:0] zim_acc_sel,
    output reg       zre_wr_en,
    output reg       zim_wr_en,

    input zre_sign,
    input zim_sign,
    input diverged,

    output reg        out_ready,
    output reg [15:0] iteration_count
);

    //States
    localparam STATE_LOAD       = 0; // Load c / Output iteration count from last solve
    localparam STATE_ABS        = 1; // z = abs(z)
    localparam STATE_ABS_FLUSH  = 2; // Wait for pipeline to flush
    localparam STATE_ITER       = 3; // Compute a z series iteration
    localparam STATE_ITER_FLUSH = 4; // Wait for pipeline to flush
    localparam STATE_CHECK      = 5; // Check if z diverged
    reg  [2:0] state;
    reg  [2:0] next_state;

    localparam FLUSH_WAIT = 4;
    reg  [2:0] flush_counter;
    reg  [2:0] next_flush_counter;

    reg         next_out_ready;
    reg  [15:0] iteration_limit;
    reg  [15:0] next_iteration_count;

    reg  [LIMB_INDEX_BITS-1:0] num_limbs;
    reg  [LIMB_INDEX_BITS-1:0] limb_index;
    reg  [LIMB_INDEX_BITS-1:0] next_limb_index;

    reg  [LIMB_INDEX_BITS-1:0] partial_index;
    reg  [LIMB_INDEX_BITS-1:0] next_partial_index;
    reg                        flip_partial;
    reg                        next_flip_partial;

    reg [1:0] pattern_index;

    reg  last_zre_sign;
    reg  last_zim_sign;

    localparam OP_ITER = 0;
    localparam OP_ABS  = 1;

    localparam ZRE_PART_DOUBLE_POS = 0;
    localparam ZRE_PART_DOUBLE_NEG = 1;
    localparam ZRE_PART_SINGLE_POS = 2;
    localparam ZRE_PART_SINGLE_NEG = 3;
    localparam ZIM_PART_POS = 0;
    localparam ZIM_PART_NEG = 1;

    localparam ITER_ADD_PARTIAL = 0;
    localparam ITER_CARRY       = 1;
    localparam ITER_SET         = 2;
    localparam ITER_NOP         = 3;
    localparam ABS_NOP          = 0;
    localparam ABS_START        = 1;
    localparam ABS_CARRY        = 2;

    always @* begin
        next_state           = state;
        next_out_ready       = out_ready;
        next_iteration_count = iteration_count;
        next_flush_counter   = FLUSH_WAIT;
        next_limb_index      = num_limbs - 1;
        next_partial_index   = 0;
        next_flip_partial    = 0;

        limb_ind        = limb_index;
        zre_ind         = 0;
        zim_ind         = 0;
        cre_wr_en       = 0;
        cim_wr_en       = 0;
        zre_reg_sel     = 0;
        zim_reg_sel     = 0;
        m1_a_sel        = 0;
        m1_b_sel        = 0;
        m2_a_sel        = 0;
        m2_b_sel        = 0;
        op_sel          = OP_ITER;
        zre_partial_sel = 0;
        zim_partial_sel = 0;
        zre_acc_sel     = ITER_NOP;
        zim_acc_sel     = ITER_NOP;
        zre_wr_en       = 0;
        zim_wr_en       = 0;

        if (state == STATE_LOAD)
        begin
            cre_wr_en = wr_real_en;
            cim_wr_en = wr_imag_en;

            if (start) begin
                next_state = STATE_ABS;
                next_out_ready = 0;
            end
        end
        else if (state == STATE_ABS)
        begin
            op_sel    = OP_ABS;
            zre_wr_en = 1;
            zim_wr_en = 1;

            next_limb_index = limb_index - 1;
            zre_ind         = limb_index;
            zim_ind         = limb_index;

            if (last_zre_sign) zre_acc_sel = limb_index == num_limbs - 1 ? ABS_START : ABS_CARRY;
            else               zre_acc_sel = ABS_NOP;

            if (last_zim_sign) zim_acc_sel = limb_index == num_limbs - 1 ? ABS_START : ABS_CARRY;
            else               zim_acc_sel = ABS_NOP;

            if (limb_index == 0) next_state = STATE_ABS_FLUSH;
        end
        else if (state == STATE_ABS_FLUSH)
        begin
            next_flush_counter = flush_counter - 1;
            if (flush_counter == 0) next_state = STATE_ITER;
        end
        else if (state == STATE_ITER)
        begin
            op_sel = OP_ITER;

            next_limb_index    = limb_index;
            next_partial_index = partial_index;
            next_flip_partial  = !flip_partial;
            if (flip_partial == 1) begin
                next_partial_index = partial_index + 1;
                if (partial_index == limb_index >> 1) begin
                    next_partial_index = 0;
                    next_limb_index = limb_index - 1;
                    zre_wr_en = 1;
                    zim_wr_en = 1;
                    if (limb_index == 0) next_state = STATE_ITER_FLUSH;
                end
            end

            zre_ind         = flip_partial ?              partial_index : limb_index - partial_index;
            zim_ind         = flip_partial ? limb_index - partial_index :              partial_index;

            zre_reg_sel     = pattern_index == 0 ? 0 :
                              pattern_index == 1 ? 2 :
                              pattern_index == 2 ? 0 :
                                                   1;

            zim_reg_sel     = pattern_index == 0 ? 1 :
                              pattern_index == 1 ? 3 :
                              pattern_index == 2 ? 2 :
                                                   3;

            m1_a_sel        = pattern_index == 0 ? 2 :
                              pattern_index == 1 ? 0 :
                              pattern_index == 2 ? 1 :
                                                   0;

            m1_b_sel        = pattern_index == 0 ? 3 :
                              pattern_index == 1 ? 2 :
                              pattern_index == 2 ? 3 :
                                                   1;

            m2_a_sel        = zre_reg_sel;
            m2_b_sel        = zim_reg_sel;
            zre_partial_sel = zre_ind == zim_ind ? (flip_partial ? ZRE_PART_SINGLE_NEG : ZRE_PART_SINGLE_POS) :
                                                   (flip_partial ? ZRE_PART_DOUBLE_NEG : ZRE_PART_DOUBLE_POS);
            zim_partial_sel = (last_zre_sign ^ last_zim_sign) ? ZIM_PART_NEG : ZIM_PART_POS;
            if (partial_index == 0) begin
                zre_acc_sel = flip_partial ? (limb_index == num_limbs - 1 ? ITER_SET : ITER_CARRY) : ITER_NOP;
                zim_acc_sel = flip_partial ? ITER_ADD_PARTIAL                                      : (limb_index == num_limbs - 1 ? ITER_SET : ITER_CARRY);
            end else begin
                zre_acc_sel = ITER_ADD_PARTIAL;
                zim_acc_sel = ITER_ADD_PARTIAL;
            end
            if (zre_ind == zim_ind && flip_partial) zim_acc_sel = ITER_NOP;
        end
        else if (state == STATE_ITER_FLUSH)
        begin
            next_flush_counter = flush_counter - 1;
            if (flush_counter == 0) next_state = STATE_CHECK;
        end
        else if (state == STATE_CHECK)
        begin
            if (diverged) begin
                next_state <= STATE_LOAD;
                next_out_ready = 1;
            end else if (iteration_count == iteration_limit) begin
                next_state <= STATE_LOAD;
                next_out_ready = 1;
                next_iteration_count <= -1;
            end else begin
                next_state <= STATE_ABS;
                next_iteration_count = iteration_count + 1;
            end
        end
        else
        begin
            $display("Unknown state: %d", state);
        end
    end

    always @(posedge clock) begin
        if (reset) begin
            state           <= STATE_LOAD;
            out_ready       <= 0;
            iteration_count <= 0;

            num_limbs       <= 0;
            limb_index      <= 0;

            partial_index   <= 0;
            flip_partial    <= 0;

            pattern_index   <= 0;
        end else begin
            state           <= next_state;
            flush_counter   <= next_flush_counter;
            out_ready       <= next_out_ready;
            iteration_count <= next_iteration_count;
            limb_index      <= next_limb_index;
            partial_index   <= next_partial_index;
            flip_partial    <= next_flip_partial;

            pattern_index   <= pattern_index + 1;

            if (state == STATE_LOAD) begin
                if (wr_num_limbs_en) num_limbs <= num_limbs_data;
                if (wr_iter_lim_en)  iteration_limit <= iter_lim_data;
            end else if (state == STATE_CHECK) begin
                last_zre_sign <= zre_sign;
                last_zim_sign <= zim_sign;
            end
        end
    end

endmodule

`endif
