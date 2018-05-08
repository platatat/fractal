`ifndef _tile_solver_v_
`define _tile_solver_v_


module tile_solver #(
    parameter LIMB_INDEX_BITS   = 6,
    parameter LIMB_SIZE_BITS    = 27,
    parameter DIVERGENCE_RADIUS = 4
) (
    input clock,
    input reset,

    input write_real_en,
    input write_imag_en,
    input [LIMB_INDEX_BITS-1:0] write_limb,
    input [LIMB_SIZE_BITS-1:0]  write_data,
    input [31:0]                zoom_level,
    input [31:0]                output_addr,

    input start,

    output reg ready
);

    reg running;
    reg [3:0] counter;

    always @(posedge clock) begin
        $display("wren %d, limb %d, data %d", write_real_en, write_limb, write_data);

        if (reset) begin
            ready <= 1;
            running <= 0;
            counter <= 4'd0;
        end else begin
            if (!running) begin
                running <= start;
                ready <= !start;
            end else begin
                if (counter == 4'b1111) begin
                    ready <= 1;
                    running <= 0;
                end else begin
                    counter <= counter + 1;
                end
            end
        end
    end


endmodule


`endif