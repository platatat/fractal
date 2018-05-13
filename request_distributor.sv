`ifndef _request_distributor_v_
`define _request_distributor_v_

module request_distributor #(
    parameter NUM_SOLVERS = 2
) (
    input clock,
    input reset,

    input [31:0] in_data,
    input        in_valid,
    input        in_end_of_stream,
    output logic in_ready,

    output logic [31:0]              out_data,
    output logic [NUM_SOLVERS-1 : 0] out_valid,
    output logic                     out_end_of_stream,
    input        [NUM_SOLVERS-1 : 0] out_waiting
);

// Input signals

logic next_in_ready;

logic [31:0]              next_out_data;
logic [NUM_SOLVERS-1 : 0] next_out_valid;
logic                     next_out_end_of_stream;

always @(posedge clock) begin
    in_ready <= next_in_ready;

    out_data  <= next_out_data;
    out_valid <= next_out_valid;
    out_end_of_stream <= next_out_end_of_stream;
end

// State
typedef enum {
    ST_SELECT_OUTPUT,
    ST_SEND_EXTRA,
    ST_SEND_FROM_FIFO,
    ST_COLLECT_EXTRA
} state_t;

state_t state, next_state;

logic [NUM_SOLVERS-1 : 0] active_solver;
logic [NUM_SOLVERS-1 : 0] next_active_solver;

logic extra_valid, next_extra_valid;
logic [31:0] extra_data;
logic [31:0] next_extra_data;
logic extra_end_of_stream;
logic next_extra_end_of_stream;

always @(posedge clock) begin
    if (reset) begin
        state <= ST_SELECT_OUTPUT;
        active_solver <= '0;

        extra_valid <= '0;
        extra_data <= 'x;
        next_extra_end_of_stream <= 'x;
    end else begin
        state <= next_state;
        active_solver <= next_active_solver;

        extra_valid <= next_extra_valid;
        extra_data <= next_extra_data;
        extra_end_of_stream <= next_extra_end_of_stream;
    end
end

// Combinatorial priority encoder
wire [NUM_SOLVERS-1 : 0] available_solver;
wire [NUM_SOLVERS : 0] out_waiting_appended = {out_waiting, 1'b0};
generate
    genvar i;
    for (i = 0; i < NUM_SOLVERS; i = i + 1) begin : foo
        assign available_solver[i] = out_waiting[i] == 1'b1 && out_waiting_appended[i:0] == '0;
    end
endgenerate

// State machine
always @* begin
    // Default values
    next_state = state;
    next_active_solver = active_solver;

    next_extra_valid = extra_valid;
    next_extra_data = extra_data;
    next_extra_end_of_stream = extra_end_of_stream;

    next_in_ready = '0;

    next_out_data = 'x;
    next_out_valid = '0;
    next_out_end_of_stream = 'x;

    if (state == ST_SELECT_OUTPUT) begin
        if (available_solver != '0) begin
            next_active_solver = available_solver;
            next_state = ST_SEND_EXTRA;
        end
    end

    if (state == ST_SEND_EXTRA) begin
        next_in_ready = 1;

        next_out_data = extra_data;
        next_out_end_of_stream = extra_end_of_stream;

        next_state = ST_SEND_FROM_FIFO;

        if (extra_valid) begin
            next_out_valid = active_solver;

            next_extra_valid = 1'b0;
            next_extra_data = 'x;
            next_extra_end_of_stream = 'x;
        end
    end

    if (state == ST_SEND_FROM_FIFO) begin
        next_in_ready = 1;

        next_out_data = in_data;
        next_out_end_of_stream = in_end_of_stream;

        if (in_valid) begin
            next_out_valid = active_solver;
        end

        if (in_valid && in_end_of_stream) begin
            next_state = ST_COLLECT_EXTRA;
        end
    end

    if (state == ST_COLLECT_EXTRA) begin
        next_extra_valid = in_valid;
        next_extra_data = in_data;
        next_extra_end_of_stream = in_end_of_stream;

        next_state = ST_SELECT_OUTPUT;
    end
end

endmodule

`endif
