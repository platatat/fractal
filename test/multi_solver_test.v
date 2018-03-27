`timescale 1ns/1ns

`include "multi_solver.v"

module top();
    reg clock;
    reg reset;

    reg [5:0] rd_solver_id;
    reg [18:0] rd_addr;

    initial begin
        $dumpfile("build/multi_solver_test.vcd");
        $dumpvars(0, top);

        clock <= 0;
        reset <= 1;
        rd_solver_id <= 6'd0;
        rd_addr<= 10'd0;
        #20
        reset <= 0;

        #10000000
        $finish;
    end

    always begin
        #10
        clock <= !clock;
        if (solver.done) $finish;
    end

    multi_solver #(1, 99, 66) solver(
        .clock(clock),
        .reset(reset),
        .min_x(-27'd2 <<< 20),
        .min_y(-27'd1 <<< 20),
        .dx(27'd31776),
        .dy(27'd31776),
        .rd_solver_id(rd_solver_id),
        .rd_addr(rd_addr)
    );

endmodule
