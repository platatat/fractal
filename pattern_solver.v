`ifndef _pattern_solver_v_
`define _pattern_solver_v_

`include "mand_solver.v"

module pattern_solver #(
    parameter SOLVER_ID = 0,
    parameter NUM_SOLVERS = 1
) (
    input clock, reset,

    input signed [26:0] min_x, min_y, max_x, max_y,
    input signed [26:0] dx, dy,

    input continue,

    output signed [3:0] solver_out,
    output solver_ready,
    output reg done
);

    reg signed [26:0] x;
    reg signed [26:0] y;

    always @(posedge clock) begin
        if (solver_ready) begin
            //$display("%d %d %d", x, y, solver_out);
        end
    end

    wire signed [31:0] s_out;
    mand_solver solver(
        .clock(clock),
        .reset(reset | continue | done),
        .c_im(y),
        .c_re(x),
        .out_ready(solver_ready),
        .out(s_out)
    );
    assign solver_out = s_out[7:4];

    wire signed [26:0] inc_x = x + dx;
    wire signed [26:0] inc_y = y + (NUM_SOLVERS * dy);
    wire wrap_x = inc_x >= max_x;
    wire wrap_y = inc_y >= max_y;

    always @(posedge clock) begin
        if (reset) begin
            done <= 1'b0;
            x <= min_x;
            y <= min_y + (SOLVER_ID * dy);
        end else if (!done) begin
            if (wrap_y && wrap_x & solver_ready) begin
                done <= 1'b1;
            end else if (solver_ready) begin
                //iterate to next (x, y)
                x <= wrap_x ? min_x : inc_x;
                y <= wrap_x ? inc_y : y;
            end
        end
    end

endmodule

`endif
