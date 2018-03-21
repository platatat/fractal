`include "pattern_solver.v"
`include "ram.v"

module multi_solver #(
    parameter NUM_SOLVERS = 1
) (
    input clock, reset,

    input signed [26:0] min_x, min_y, max_x, max_y,
    input signed [26:0] dx, dy,

    input [5:0] rd_solver_id,
    input [9:0] rd_addr,
    output signed [7:0] rd_data_out,

    output done
);

    wire signed [NUM_SOLVERS-1:0] solvers_done;
    wire signed [7:0] solvers_rd_out [NUM_SOLVERS:0];

    generate
        genvar i;
        for (i = 0; i < NUM_SOLVERS; i = i + 1) begin : solvers
            wire signed [7:0] solver_out;
            wire solver_ready;
            reg [9:0] solver_addr;

            RAM_1024_8 ram(
                .clock(clock),
                .wr_en(solver_ready),
                .rd_en(rd_solver_id == i),
                .wr_addr(solver_addr),
                .rd_addr(rd_addr),
                .data_in(solver_out),
                .data_out(solvers_rd_out[i])
            );

            pattern_solver solver(
                .clock(clock),
                .reset(reset),
                .min_x(min_x), .min_y(min_y), .max_x(max_x), .max_y(max_y),
                .dx(dx), .dy(dy),
                .continue(solver_ready & ~solvers_done[i]),
                .solver_out(solver_out),
                .solver_ready(solver_ready),
                .done(solvers_done[i])
            );

            always @(posedge clock) begin
                if (reset) begin
                    solver_addr <= 10'd0;
                end else begin
                    solver_addr <= solver_addr + 1;
                end
            end
        end
    endgenerate

    assign rd_data_out = solvers_rd_out[rd_solver_id];
    assign done = solvers_done == -1;

endmodule
