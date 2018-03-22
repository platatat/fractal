`timescale 1ns/1ns

`include "multi_solver.v"

module top();
    reg clock;
    reg reset;

    reg [5:0] rd_solver_id;
    reg [9:0] rd_addr;

    initial begin
        $dumpfile("build/multi_solver_test.vcd");
        $dumpvars(0, top);

        clock <= 0;
        reset <= 1;
        rd_solver_id <= 6'd0;
        rd_addr<= 10'd0;
        #20
        reset <= 0;
        #20
        reset <= 1;
        #20
        reset <= 0;

        #78000
        rd_solver_id <= 6'd0;
        rd_addr <= 10'h11;
        #40
        rd_addr <= 10'h8c;
        #20000
        rd_solver_id <= 6'd1;

        #1000000000
        $finish;
    end

    always begin
        #10
        clock <= !clock;
    end

    multi_solver #(10) solver(
        .clock(clock),
        .reset(reset),
        .min_x(-27'd2 <<< 20),
        .min_y(-27'd1 <<< 20),
        .max_x(27'd1 <<< 20),
        .max_y(27'd1 <<< 20),
        .dx(27'd1638),
        .dy(27'd2185),
        .rd_solver_id(rd_solver_id),
        .rd_addr(rd_addr)
    );

endmodule
