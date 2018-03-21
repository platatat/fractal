`timescale 1ns/1ns

`include "multi_solver.v"

module top();
    reg clock;
    reg reset;

    initial begin
        $dumpfile("build/multi_solver_test.vcd");
        $dumpvars(0, top);

        clock <= 0;
        reset <= 1;
        #20
        reset <= 0;
        #20
        reset <= 1;
        #20
        reset <= 0;

        #500000
        $finish;
    end

    always begin
        #10
        clock <= !clock;
    end

    multi_solver #(1) solver(
        .clock(clock),
        .reset(reset),
        .min_x(27'b0),// <<< 20),
        .min_y(27'b0),// <<< 20),
        .max_x(27'd8),// <<< 20),
        .max_y(27'd8),// <<< 20),
        .dx(27'b1),// <<< 17),
        .dy(27'b1)// <<< 17)
    );

endmodule
