`timescale 1ns/1ns

`include "priority_encoder.v"

module top();
    reg [9:0] in;

    initial begin
        $dumpfile("build/priority_encoder_test.vcd");
        $dumpvars(0, top);

        in <= 10'd0;
        #1
        in <= 10'd3;
        #1
        in <= 10'd234;
        #1
        in <= 10'd1023;
        #1
        in <= 10'd1000;
        #1
        in <= 10'd512;
        #1
        in <= 10'd256;
        #1
        in <= 10'd128;
        #1
        in <= 10'd64;
        #1
        in <= 10'd32;
        #1
        in <= 10'd16;
        #1
        in <= 10'd8;
        #1
        in <= 10'd4;
        #1
        in <= 10'd2;
        #1
        in <= 10'd1;
        #1
        in <= 10'd340;
        #1
        $finish;
    end

    priority_encoder enc(
        .in(in)
    );

endmodule
