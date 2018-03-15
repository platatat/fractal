//////////////////////////////////////////////////
//// signed mult of 4.23 format 2'comp////////////
//////////////////////////////////////////////////

module signed_mult (out, a, b);
    output signed [26:0] out;
    input  signed [26:0] a;
    input  signed [26:0] b;
    wire   signed [53:0] mult_out; // intermediate full bit length

    assign mult_out = a * b;

    // select bits for 4.23 fixed point
    assign out = {mult_out[53], mult_out[45:20]};
endmodule
//////////////////////////////////////////////////