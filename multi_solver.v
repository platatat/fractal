`ifndef _multi_solver_v_
`define _multi_solver_v_

`include "pattern_solver.v"
`include "ram.v"

module multi_solver #(
    parameter NUM_SOLVERS = 1,
    parameter NUM_COLUMNS = 640,
    parameter NUM_ROWS = 480
) (
    input clock, reset,

    input signed [26:0] min_x, min_y,
    input signed [26:0] dx, dy,

    input [5:0] rd_solver_id,
    input [18:0] rd_addr,
    output signed [3:0] rd_data_out,

    output reg [31:0] solve_time,
    output done
);

    wire signed [NUM_SOLVERS-1:0] solvers_done;
    wire signed [3:0] solvers_rd_out [NUM_SOLVERS-1:0];

    generate
        genvar i;
        for (i = 0; i < NUM_SOLVERS; i = i + 1) begin : solvers
            wire signed [3:0] solver_out;
            wire solver_ready;
            reg [18:0] solver_addr;

            RAM #((308000 / NUM_SOLVERS) + 640, 4) ram(
                .clock(clock),
                .wr_en(solver_ready),
                .rd_en(rd_solver_id == i),
                .wr_addr(solver_addr),
                .rd_addr(rd_addr),
                .data_in(solver_out),
                .data_out(solvers_rd_out[i])
            );

            pattern_solver #(i, NUM_SOLVERS, NUM_COLUMNS, NUM_ROWS) solver(
                .clock(clock),
                .reset(reset),
                .min_x(min_x), .min_y(min_y),
                .dx(dx), .dy(dy),
                .continue(solver_ready),
                .solver_out(solver_out),
                .solver_ready(solver_ready),
                .done(solvers_done[i])
            );

            always @(posedge clock) begin
                if (reset) begin
                    solver_addr <= 19'd0;
                end else begin
                    if (solver_ready) begin
                        solver_addr <= solver_addr + 1;
                    end
                end
            end
        end
    endgenerate

    always @(posedge clock) begin
        if (reset) begin
            solve_time <= 0;
        end else if (~done) begin
            solve_time <= solve_time + 1;
        end
    end

    assign rd_data_out = solvers_rd_out[rd_solver_id];
    assign done = solvers_done == -1;

endmodule

`endif
