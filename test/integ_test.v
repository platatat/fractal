`timescale 1ns/1ns

`include "multi_solver.v"
`include "pixel_iterator.v"

module top();
    localparam NUM_SOLVERS = 1;

    reg clock;
    reg reset;

    initial begin
        $dumpfile("build/integ_test.vcd");
        $dumpvars(0, top);

        clock <= 0;
        reset <= 1;
        can_finish <= 0;
        #20
        reset <= 0;

        #7643680
        $finish;
    end

    always begin
        #10
        clock <= !clock;
        if (end_stream2) $finish;
    end

    pixel_iterator #(NUM_SOLVERS, 99, 66) it(
        .clock(clock),
        .reset(reset),
        .en(solver.done)
    );

    multi_solver #(NUM_SOLVERS) solver(
        .clock(clock),
        .reset(reset),
        .min_x(-27'd2 <<< 20),
        .min_y(-27'd1 <<< 20),
        .max_x(27'd1 <<< 20),
        .max_y(27'd1 <<< 20),
        .dx(27'd31776),
        .dy(27'd31776),
        .rd_solver_id(it.solver_id),
        .rd_addr(it.solver_addr)
    );

    reg [5:0] solver_id0;
    reg [5:0] solver_id1;
    reg [18:0] solver_addr0;
    reg [18:0] solver_addr1;
    reg end_stream0;
    reg end_stream1;
    reg end_stream2;
    reg can_finish;

    always @(posedge clock) begin
        solver_id0 <= it.solver_id;
        solver_id1 <= solver_id0;
        solver_addr0 <= it.solver_addr;
        solver_addr1 <= solver_addr0;
        end_stream0 <= it.end_stream;
        end_stream1 <= end_stream0;
        end_stream2 <= end_stream1;
        if (solver.done) begin
            $display("%d %d %d", solver_id1, solver_addr1, solver.rd_data_out);
        end
    end

endmodule
