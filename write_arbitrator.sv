`ifndef _write_arbitrator_v_
`define _write_arbitrator_v_

module write_arbitrator #(
    parameter NUM_SOLVERS = 2
) (
    input clock,
    input reset,

    input        [NUM_SOLVERS-1:0] [15:0] in_data,
    input        [NUM_SOLVERS-1:0] [31:0] in_addr,
    input        [NUM_SOLVERS-1:0]        in_valid,
    output logic [NUM_SOLVERS-1:0]        in_ack,

    output logic [15:0] out_data,
    output logic [31:0] out_addr,
    output logic        out_write_en,
    input               out_ack
);

typedef enum {
    ST_WAIT_DATA,
    ST_WRITE
} state_t;

state_t state;



// Combinatorial priority encoder
wire [NUM_SOLVERS-1 : 0] available_index;
wire [NUM_SOLVERS : 0] in_valid_appended = {in_valid, 1'b0};
generate
    genvar i;
    for (i = 0; i < NUM_SOLVERS; i = i + 1) begin : foo
        assign available_index[i] = in_valid[i] == 1'b1 && in_valid_appended[i:0] == '0;
    end
endgenerate

logic [15:0] available_data;
logic [31:0] available_addr;
always @* begin
    available_data = 'x;
    available_addr = 'x;
    for (int i = 0; i < NUM_SOLVERS; i = i + 1) begin : foo
        if (available_index[i]) begin
            available_data = in_data[i];
            available_addr = in_addr[i];
        end
    end
end

// State machine
always @(posedge clock) begin
    if (reset) begin
        state <= ST_WAIT_DATA;
        in_ack <= 0;

        out_data <= 'x;
        out_addr <= 'x;
    end else if (state == ST_WAIT_DATA) begin
        // Wait for data
        if (available_index != '0) begin
            state <= ST_WRITE;
            in_ack <= available_index;

            out_data <= available_data;
            out_addr <= available_addr;
        end else begin
            state <= ST_WAIT_DATA;
            in_ack <= 0;
        end
    end else if (state == ST_WRITE) begin
        // Wait for ack
        if (out_ack) begin
            state <= ST_WAIT_DATA;

            out_data <= 'x;
            out_addr <= 'x;
        end else begin
            state <= ST_WRITE;
        end

        in_ack <= 0;
    end
end

assign out_write_en = (state == ST_WRITE);

endmodule

`endif
