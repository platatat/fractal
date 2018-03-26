`ifndef _pixel_iterator_v_
`define _pixel_iterator_v_

module pixel_iterator #(
    parameter NUM_SOLVERS = 1
) (
    input clock, reset,
    input en,

    output reg [5:0] solver_id,
    output reg [18:0] solver_addr,

    output reg start_stream,
    output reg end_stream
);

    reg [18:0] start_addr;
    reg [8:0] line_num;

    always @(posedge clock) begin
        if (reset | (line_num == 479 & solver_addr >= start_addr + 639)) begin
            solver_id <= 0;
            start_addr <= 0;
            solver_addr <= 0;
            line_num <= 0;
            start_stream <= 1;
            end_stream <= 0;
        end else if (en) begin
            start_stream <= 0;
            if (solver_addr == start_addr + 638) begin
                if (line_num == 479) begin
                    end_stream <= 1;
                end else begin
                    end_stream <= 0;
                end
                solver_addr <= solver_addr + 1;
            end else if (solver_addr == start_addr + 639) begin
                // New line
                line_num <= line_num + 1;
                if (solver_id + 1 == NUM_SOLVERS) begin
                    // Go back to solver 0
                    solver_id <= 0;
                    start_addr <= start_addr + 640;
                    solver_addr <= start_addr + 640;
                end else begin
                    solver_id <= solver_id + 1;
                    solver_addr <= start_addr;
                end
            end else begin
                solver_addr <= solver_addr + 1;
            end
        end
    end

endmodule

`endif
