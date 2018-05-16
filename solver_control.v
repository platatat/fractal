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

    output reg                       cre_wr_en,
    output reg                       cim_wr_en,
    output reg [LIMB_INDEX_BITS-1:0] c_limb_ind,
    output reg [LIMB_INDEX_BITS-1:0] zre_rd_ind,
    output reg [LIMB_INDEX_BITS-1:0] zim_rd_ind,
    output reg [1:0]                 zre_neg_sel,
    output reg [1:0]                 zim_neg_sel,
    output reg [1:0]                 zre_reg_sel,
    output reg [1:0]                 zim_reg_sel,
    output reg                       mov_CtoA,
    output reg                       mov_DtoB,
    output reg [2:0]                 zre_partial_sel,
    output reg [1:0]                 zim_partial_sel,
    output reg [1:0]                 zre_acc_sel,
    output reg [1:0]                 zim_acc_sel,
    output reg                       clear_lsd,
    output reg                       zre_wr_en,
    output reg                       zim_wr_en,
    output reg [LIMB_INDEX_BITS-1:0] zre_wr_ind,
    output reg [LIMB_INDEX_BITS-1:0] zim_wr_ind,

    input zre_sign,
    input zim_sign,
    input diverged,
    input [LIMB_INDEX_BITS-1:0] zre_lsd,
    input [LIMB_INDEX_BITS-1:0] zim_lsd,

    output reg        out_ready,
    output reg [15:0] iteration_count
);

    //States
    localparam STATE_LOAD       = 0; // Load c / Output iteration count from last solve
    localparam STATE_ITER       = 1; // Compute a z series iteration
    localparam STATE_ITER_FLUSH = 2; // Wait for pipeline to flush
    localparam STATE_CHECK      = 3; // Check if z diverged
    reg  [1:0] state;
    reg  [1:0] last_state;
    reg  [1:0] next_state;

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

    reg        next_mov_CtoA;
    reg        next_mov_DtoB;
    reg  [2:0] next_zre_partial_sel;
    reg  [1:0] next_zre_acc_sel;
    reg        next_zre_wr_en;

    reg  last_zre_sign;
    reg  last_zim_sign;

    reg  [LIMB_INDEX_BITS-1:0] last_zre_lsd;
    reg  [LIMB_INDEX_BITS-1:0] last_zim_lsd;

    localparam NEGATE_NOP       = 0;
    localparam NEGATE_FLIP      = 1;
    localparam NEGATE_FLIP_INC  = 2;

    localparam ZRE_PART_ZERO       = 0;
    localparam ZRE_PART_DOUBLE_POS = 1;
    localparam ZRE_PART_DOUBLE_NEG = 2;
    localparam ZRE_PART_SINGLE_POS = 3;
    localparam ZRE_PART_SINGLE_NEG = 4;
    localparam ZIM_PART_ZERO = 0;
    localparam ZIM_PART_POS  = 1;
    localparam ZIM_PART_NEG  = 2;

    localparam ITER_ADD_PARTIAL = 0;
    localparam ITER_CARRY       = 1;
    localparam ITER_SET         = 2;
    localparam ITER_NOP         = 3;

    always @* begin
        next_state           = state;
        next_out_ready       = out_ready;
        next_iteration_count = iteration_count;
        next_flush_counter   = FLUSH_WAIT;
        next_limb_index      = num_limbs;
        next_partial_index   = 1;
        next_flip_partial    = 0;

        c_limb_ind           = limb_index;
        zre_rd_ind           = 0;
        zim_rd_ind           = 0;
        cre_wr_en            = 0;
        cim_wr_en            = 0;
        zre_neg_sel          = 0;
        zim_neg_sel          = 0;
        zre_reg_sel          = 0;
        zim_reg_sel          = 0;
        next_mov_CtoA        = 0;
        next_mov_DtoB        = 0;
        next_zre_partial_sel = ZRE_PART_ZERO;
        zim_partial_sel      = ZIM_PART_ZERO;
        next_zre_acc_sel     = ITER_NOP;
        zim_acc_sel          = ITER_NOP;
        next_zre_wr_en       = 0;
        clear_lsd            = 0;
        zim_wr_en            = 0;
        zre_wr_ind           = 0;
        zim_wr_ind           = 0;

        if (state == STATE_LOAD)
        begin
            c_limb_ind = wr_ind;
            cre_wr_en  = wr_real_en;
            cim_wr_en  = wr_imag_en;

            if (start) begin
                next_state = STATE_ITER;
                next_out_ready = 0;
                next_iteration_count <= 0;
            end
        end
        else if (state == STATE_ITER)
        begin
            next_limb_index    = limb_index;
            next_partial_index = partial_index;
            next_flip_partial  = !flip_partial;
            if (flip_partial == 1) begin
                next_partial_index = partial_index + 1;
                if (partial_index == limb_index >> 1) begin
                    next_partial_index = 0;
                    next_limb_index    = limb_index - 1;
                    if (limb_index < num_limbs) begin
                        next_zre_wr_en = 1;
                        zim_wr_en      = 1;
                    end
                    if (limb_index == 0) next_state = STATE_ITER_FLUSH;
                end
            end

            zre_rd_ind      = flip_partial ?              partial_index : limb_index - partial_index;
            zim_rd_ind      = flip_partial ? limb_index - partial_index :              partial_index;

            if (last_zre_sign == 1 && zre_rd_ind <= last_zre_lsd) begin
                zre_neg_sel = (zre_rd_ind == last_zre_lsd) ? NEGATE_FLIP_INC : NEGATE_FLIP;
            end
            if (last_zim_sign == 1 && zim_rd_ind <= last_zim_lsd) begin
                zim_neg_sel = (zim_rd_ind == last_zim_lsd) ? NEGATE_FLIP_INC : NEGATE_FLIP;
            end

            if (last_state != state) begin
                zre_reg_sel = 0;
                zim_reg_sel = 3;
            end else begin
                zre_reg_sel = flip_partial ? 1 : 2;
                zim_reg_sel = flip_partial ? 2 : 3;
            end

            next_mov_CtoA = last_state == state;
            next_mov_DtoB = flip_partial;

            if (iteration_count > 0) begin
                next_zre_partial_sel = zre_rd_ind == zim_rd_ind ? (flip_partial ? ZRE_PART_SINGLE_NEG : ZRE_PART_SINGLE_POS) :
                                                                  (flip_partial ? ZRE_PART_DOUBLE_NEG : ZRE_PART_DOUBLE_POS);
                zim_partial_sel = (last_zre_sign ^ last_zim_sign) ? ZIM_PART_NEG : ZIM_PART_POS;
            end

            if (limb_index == num_limbs) begin
                next_zre_acc_sel = (partial_index == 1 && !flip_partial) ? ITER_SET : ITER_ADD_PARTIAL;
                zim_acc_sel      = (partial_index == 1 && !flip_partial) ? ITER_SET : ITER_ADD_PARTIAL;
            end else begin
                next_zre_acc_sel = (partial_index == 0 && !flip_partial) ? ITER_CARRY : ITER_ADD_PARTIAL;
                zim_acc_sel      = (partial_index == 0 && !flip_partial) ? ITER_CARRY : ITER_ADD_PARTIAL;
            end
            if (zre_rd_ind == zim_rd_ind && flip_partial) zim_acc_sel = ITER_NOP;

            zre_wr_ind = limb_index + 1;
            zim_wr_ind = limb_index;
        end
        else if (state == STATE_ITER_FLUSH)
        begin
            next_flush_counter = flush_counter - 1;
            if (flush_counter == 0) next_state = STATE_CHECK;
        end
        else if (state == STATE_CHECK)
        begin
            clear_lsd = 1;
            if (diverged && iteration_count > 0) begin
                next_state <= STATE_LOAD;
                next_out_ready = 1;
            end else if (iteration_count == iteration_limit - 1) begin
                next_state <= STATE_LOAD;
                next_out_ready = 1;
                next_iteration_count <= iteration_limit - 1;
            end else begin
                next_state <= STATE_ITER;
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
            last_state       <= 0;
            flush_counter   <= 0;
            out_ready       <= 0;
            iteration_count <= 0;
            num_limbs       <= 0;
            limb_index      <= 0;
            partial_index   <= 0;
            flip_partial    <= 0;

            mov_CtoA        <= 0;
            mov_DtoB        <= 0;
            zre_partial_sel <= 0;
            zre_acc_sel     <= 0;
            zre_wr_en       <= 0;

            last_zre_sign   <= 0;
            last_zim_sign   <= 0;

            last_zre_lsd    <= 0;
            last_zim_lsd    <= 0;
        end else begin
            state           <= next_state;
            last_state      <= state;
            flush_counter   <= next_flush_counter;
            out_ready       <= next_out_ready;
            iteration_count <= next_iteration_count;
            limb_index      <= next_limb_index;
            partial_index   <= next_partial_index;
            flip_partial    <= next_flip_partial;

            mov_CtoA        <= next_mov_CtoA;
            mov_DtoB        <= next_mov_DtoB;
            zre_partial_sel <= next_zre_partial_sel;
            zre_acc_sel     <= next_zre_acc_sel;
            zre_wr_en       <= next_zre_wr_en;

            if (state == STATE_LOAD) begin
                if (wr_num_limbs_en) num_limbs <= num_limbs_data;
                if (wr_iter_lim_en)  iteration_limit <= iter_lim_data;
                last_zre_sign <= 0;
                last_zim_sign <= 0;
            end else if (state == STATE_CHECK) begin
                last_zre_sign <= zre_sign;
                last_zim_sign <= zim_sign;
                last_zre_lsd  <= zre_lsd;
                last_zim_lsd  <= zim_lsd;
            end
        end
    end

endmodule

`endif
