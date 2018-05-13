`timescale 1ns/1ns

`include "request_distributor.sv"

module top();
    initial begin
        $dumpfile("build/request_distributor_test.vcd");
        $dumpvars(0, top);
    end

    logic clock;
    logic reset;

    initial begin
        reset <= 1;
        clock <= 0;

        in_valid <= 0;
        out_waiting <= 0;

        #20;
        reset <= 0;
    end

    always begin
        #10
        clock <= !clock;
    end

    // Input thread
    task put_input;
        input [31:0] data;
        input end_of_stream;
        begin
            in_data = data;
            in_end_of_stream = end_of_stream;
            in_valid <= 1;

            while (!in_ready) begin
                #20;
            end

            #20;

            in_data = 'x;
            in_end_of_stream = 'x;
            in_valid <= 0;
        end
    endtask

    initial begin
        #20;
        put_input(0, 0);
        put_input(1, 0);
        put_input(2, 1);

        put_input(10, 0);
        put_input(11, 0);
        put_input(12, 0);
        put_input(13, 1);

        put_input(20, 0);
        put_input(21, 1);
        #40;
        $finish();
    end

    // Output threads
    localparam NUM_SOLVERS = 2;
    task automatic process_output;
        input [31:0] id;
        begin
            out_waiting[id] <= 0;
            #40;

            while(1) begin
                out_waiting[id] <= 1;
                #20;

                while(!(out_valid[id] && out_end_of_stream)) begin
                    if (out_valid[id]) begin
                        $display("Worker %d got value %d", id, out_data);
                    end else begin
                        $display("Worker %d sleeping", id);
                    end
                    #20;
                end

                $display("Worker %d got final value %d", id, out_data);
                #20;

                out_waiting[id] <= 0;
                #200;
            end
        end
    endtask

    initial begin
        process_output(0);
    end

    initial begin
        #260;
        process_output(1);
    end

    // Dist
    logic [31:0] in_data;
    logic        in_valid;
    logic        in_end_of_stream;
    wire         in_ready;

    wire [31:0]               out_data;
    wire [NUM_SOLVERS-1 : 0]  out_valid;
    wire                      out_end_of_stream;
    logic [NUM_SOLVERS-1 : 0] out_waiting;

    request_distributor #(NUM_SOLVERS) distrib (
        .clock(clock),
        .reset(reset),

        .in_data(in_data),
        .in_valid(in_valid),
        .in_end_of_stream(in_end_of_stream),
        .in_ready(in_ready),

        .out_data(out_data),
        .out_valid(out_valid),
        .out_end_of_stream(out_end_of_stream),
        .out_waiting(out_waiting)
    );
endmodule
