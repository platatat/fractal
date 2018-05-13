`timescale 1ns/1ns

`include "write_arbitrator.sv"

module top();
    initial begin
        $dumpfile("build/write_arbitrator_test.vcd");
        $dumpvars(0, top);
    end

    logic clock;
    logic reset;

    initial begin
        reset <= 1;
        clock <= 0;

        in_valid <= '0;
        out_ack <= '0;

        #20;
        reset <= 0;
    end

    always begin
        #10
        clock <= !clock;
    end

    task automatic put_input;
        input [15:0] data;
        input [31:0] addr;
        input [31:0] id;
        begin
            in_data[id] <= data;
            in_addr[id] <= addr;
            in_valid[id] <= 1;

            #20;

            while (!in_ack[id]) begin
                #20;
            end

            #20;

            in_data[id] = 'x;
            in_addr[id] = 'x;
            in_valid[id] <= 0;
        end
    endtask

    initial begin
        #20;
        put_input(16'h10, 32'hfffffffe, 0);
        put_input(16'h11, 32'hfffffffe, 0);
        #300;
        put_input(16'h12, 32'hfffffffe, 0);
        #20;
        put_input(16'h13, 32'hfffffffe, 0);
        #100;
    end

    initial begin
        #20;
        put_input(1, 32'h10, 1);
        put_input(2, 32'h10, 1);
        put_input(3, 32'h10, 1);
        put_input(4, 32'h10, 1);
        put_input(5, 32'h10, 1);
        #400;
        put_input(6, 32'h10, 1);
        #100;
        $finish();
    end

    // Output thread
    initial begin
        #40;
        while(1) begin
            out_ack <= 0;

            while (!out_write_en) begin
                #20;
            end

            #60;

            out_ack <= 1;

            #20;
        end
    end

    localparam NUM_SOLVERS = 2;

    logic [NUM_SOLVERS-1:0] [15:0] in_data;
    logic [NUM_SOLVERS-1:0] [31:0] in_addr;
    logic [NUM_SOLVERS-1:0]        in_valid;
    wire  [NUM_SOLVERS-1:0]        in_ack;

    wire [15:0] out_data;
    wire [31:0] out_addr;
    wire        out_write_en;
    logic       out_ack;

    write_arbitrator #(NUM_SOLVERS) arb(.*);
endmodule
