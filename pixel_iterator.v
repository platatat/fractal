`ifndef _pixel_iterator_v_
`define _pixel_iterator_v_

module pixel_iterator #(
    parameter NUM_SOLVERS = 1
) (
    input clock, reset,
    input en,

    output reg [5:0] solver_id,
    output reg [18:0] solver_addr,

    output done
);

    reg [18:0] start_addr;
    reg [8:0] line_num;

    always @(posedge clock) begin
        if (reset) begin
            solver_id <= 0;
            start_addr <= 0;
            solver_addr <= 0;
            line_num <= 0;
        end else if (~done & en) begin
            if (solver_addr >= start_addr + 639) begin
                line_num <= line_num + 1;
                if (solver_id + 1 == NUM_SOLVERS) begin
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

    assign done = line_num >= 480;

endmodule

`endif
