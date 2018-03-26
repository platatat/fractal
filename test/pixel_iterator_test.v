`timescale 1ns/1ns

`include "pixel_iterator.v"

module top();
    reg clock;
    reg reset;
    reg en;

    initial begin
        $dumpfile("build/pixel_iterator_test.vcd");
        $dumpvars(0, top);

        clock <= 0;
        reset <= 1;
        en <= 1;
        #20
        reset <= 0;

        #160000
        en <= 0;
        #400
        en <= 1;

        #6000000
        en <= 0;
        #400
        en <= 1;

        #6160000
        $finish;
    end

    always begin
        #10
        clock <= !clock;
    end

    pixel_iterator #(7) it(
        .clock(clock),
        .reset(reset),
        .en(en)
    );

endmodule
