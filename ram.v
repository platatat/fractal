`ifndef _ram_v_
`define _ram_v_

module RAM #(
    parameter SIZE = 1023,
    parameter DATA_SIZE = 4
) (
    input clock, wr_en, rd_en,
    input [18:0] wr_addr, rd_addr,
    input signed [DATA_SIZE-1:0] data_in,
    output reg signed [DATA_SIZE-1:0] data_out
);

    reg rd_en_reg;
    reg [18:0] rd_addr_reg;
    reg signed [DATA_SIZE-1:0] mem [SIZE:0];

    always @(posedge clock) begin
        if (wr_en) mem[wr_addr] <= data_in;
        if (rd_en_reg) data_out <= mem[rd_addr_reg];

        rd_en_reg   <= rd_en;
        rd_addr_reg <= rd_addr;
    end

endmodule

`endif
