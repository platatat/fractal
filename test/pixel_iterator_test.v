`timescale 1ns/1ns

`include "pixel_iterator.v"

module top();
    reg clock;
    reg reset;

    initial begin
        $dumpfile("build/pixel_iterator_test.vcd");
        $dumpvars(0, top);

        clock <= 0;
        reset <= 1;
        #20
        reset <= 0;

        #6160000
        reset <= 1;
        #40
        reset <= 0;

        #6160000
        $finish;
    end

    always begin
        #10
        clock <= !clock;
    end

    pixel_iterator #(2) it(
        .clock(clock),
        .reset(reset)
    );

endmodule
