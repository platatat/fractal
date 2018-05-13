`ifndef _multi_tile_solver_v_
`define _multi_tile_solver_v_

`include "request_distributor.sv"
`include "tile_solver_legit.sv"
`include "write_arbitrator.sv"

module multi_tile_solver #(
    parameter NUM_SOLVERS       = 2,
    parameter TILE_WIDTH_BITS   = 5,
    parameter LIMB_INDEX_BITS   = 6,
    parameter LIMB_SIZE_BITS    = 27,
    parameter DIVERGENCE_RADIUS = 4
) (
    input clock,
    input reset,

    input [31:0] in_data,
    input        in_valid,
    input        in_end_of_stream,
    output       in_ready,

    output [15:0] out_data,
    output [31:0] out_addr,
    output        out_write_en,
    input         out_ack
);

// request distrib to solver
wire                   [31:0] req_data;
wire [NUM_SOLVERS-1:0]        req_valid;
wire                          req_end_of_stream;
wire [NUM_SOLVERS-1:0]        req_waiting;

// Solver to write arbitration
wire [NUM_SOLVERS-1:0] [15:0] write_data;
wire [NUM_SOLVERS-1:0] [31:0] write_addr;
wire [NUM_SOLVERS-1:0]        write_valid;
wire [NUM_SOLVERS-1:0]        write_ack;

// Modules

request_distributor #(NUM_SOLVERS) distrib (
    .clock,
    .reset,

    .in_data,
    .in_valid,
    .in_end_of_stream,
    .in_ready,

    .out_data(req_data),
    .out_valid(req_valid),
    .out_end_of_stream(req_end_of_stream),
    .out_waiting(req_waiting)
);

generate
    genvar i;
    for (i = 0; i < NUM_SOLVERS; i = i + 1) begin : solver
        tile_solver_legit #(
            TILE_WIDTH_BITS,
            LIMB_INDEX_BITS,
            LIMB_SIZE_BITS,
            DIVERGENCE_RADIUS
        ) solver (
            .clock,
            .reset,

            .in_valid(req_valid[i]),
            .in_data(req_data),
            .in_ready(req_waiting[i]),
            .in_end_of_stream(req_end_of_stream),

            .out_addr(write_addr[i]),
            .out_data(write_data[i]),
            .out_valid(write_valid[i]),
            .out_ready(write_ack[i])
        );
    end
endgenerate

write_arbitrator #(NUM_SOLVERS) arbit (
    .clock,
    .reset,

    .in_data(write_data),
    .in_addr(write_addr),
    .in_valid(write_valid),
    .in_ack(write_ack),

    .out_data,
    .out_addr,
    .out_write_en,
    .out_ack
);

endmodule
`endif
