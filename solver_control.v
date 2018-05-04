`ifndef _solver_control_v_
`define _sovler_control_v_

module solver_control #(
    parameter LIMB_INDEX_BITS = 6
) (
    input  clock, reset,

    //c loading val/rdy
    input  c_val,
    output c_rdy,


    output [LIMB_INDEX_BITS-1:0] limb_ind,
);

    //States:
    // 0 - Wait / Output iteration count
    // 1 - Load c
    // 2 - Abs(z)
    // 3 - Compute a z series iteration
    // 4 - Check if z diverged
    reg  [2:0] state;

    reg [LIMB_INDEX_BITS-1:0] num_limbs;
    reg [LIMB_INDEX_BITS-1:0] limb_index;
    reg [15:0] iteration_count;

    assign c_rdy = state == 1;

    always @(posedge clock) begin
        if (reset) begin
            state <= 0;
            iteration_count <= 0;
        end else begin
            if (state == 0) begin                   //Wait / Output iteration count
                if (c_val) begin
                    state <= 1;
                    c_rdy <= 1;
                    cre_wr_en <= 1;
                    cim_wr_en <= 1;
                    num_limbs <= 0;
                    limb_index <= 0;
                end
            end else if (state == 1) begin          //Load c
                limb_index <= limb_index + 1;

                if (!c_valu) begin
                    state <= 2;
                    c_rdy <= 0;
                    cre_wr_en <= 0;
                    cim_wr_en <= 0;
                end else begin
                    num_limbs = num_limbs + 1;
                end
            end else if (state == 2) begin          //Abs(z)

                if (limb_index == 0) state <= 3;
            end else if (state == 3) begin          //Compute a z series iteration

                if (limb_index == 0) state <= 4;
            end else if (state == 4) begin          //Check if z diverged

                state <= diverged ? 0 : 2;
            end
        end
    end

endmodule

`endif
