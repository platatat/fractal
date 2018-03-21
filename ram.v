module RAM_1024_8(
    input clock, wr_en, rd_en,
    input [9:0] wr_addr, rd_addr, 
    input signed [7:0] data_in, 
    output reg signed [7:0] data_out
);

    reg rd_en_reg;
    reg [9:0] rd_addr_reg;
    reg signed [7:0] mem [1023:0];

    always @(posedge clock) begin
        if (wr_en) mem[wr_addr] <= data_in;
        if (rd_en_reg) data_out <= mem[rd_addr_reg];

        rd_en_reg   <= rd_en;
        rd_addr_reg <= rd_addr;
    end

endmodule
