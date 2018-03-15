
module pipeline_mult #(
    parameter INT_BITS  = 1,
    parameter FRAC_BITS = 17
) (
    input clock, reset,
    input signed [NUM_BITS - 1 : 0] a, b,
    output reg signed [NUM_BITS - 1 : 0] out
);
    localparam NUM_BITS = INT_BITS + FRAC_BITS;

    reg signed [NUM_BITS - 1 : 0] reg_a;
    reg signed [NUM_BITS - 1 : 0] reg_b;

    wire signed [NUM_BITS - 1 : 0] mult;
    signed_mult s_mult(
        .a(reg_a),
        .b(reg_b),
        .out(mult)
    );

    always @(posedge clock) begin
        if (reset) begin
            reg_a <= 0;
            reg_b <= 0;
            out   <= 0;
        end else begin
            reg_a <= a;
            reg_b <= b;
            out   <= mult;
        end
    end
endmodule


module signed_mult #(
    parameter INT_BITS  = 1,
    parameter FRAC_BITS = 17
) (
    input  signed [NUM_BITS - 1 : 0] a, b,
    output signed [NUM_BITS - 1 : 0] out
);
    localparam NUM_BITS = INT_BITS + FRAC_BITS;
    localparam HIGH_BIT = NUM_BITS + NUM_BITS - 1;

    wire signed [HIGH_BIT : 0] mult_out = a * b;

    assign out = mult_out[NUM_BITS + FRAC_BITS - 1 : FRAC_BITS];
endmodule
