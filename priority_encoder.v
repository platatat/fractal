`ifndef _priority_encoder_v_
`define _priority_encoder_v_

module priority_encoder(
    input [9:0] in,
    output reg [3:0] out
);

    always @(in) begin
        casez(in)
            10'b1111111111 : out = 4'b1111;
            10'b1????????? : out = 4'd10;
            10'b01???????? : out = 4'd9;
            10'b001??????? : out = 4'd8;
            10'b0001?????? : out = 4'd7;
            10'b00001????? : out = 4'd6;
            10'b000001???? : out = 4'd5;
            10'b0000001??? : out = 4'd4;
            10'b00000001?? : out = 4'd3;
            10'b000000001? : out = 4'd2;
            10'b0000000001 : out = 4'd1;
            10'b0000000000 : out = 4'd0;
        endcase
    end

endmodule

`endif
