`ifndef _hps_fifo_decoder_v_
`define _hps_fifo_decoder_v_


/**
 * Output Data Types:
 * 0 = tile output address
 * 1 = zoom level
 * 2 = c_real
 * 3 = c_imag
 */
module hps_fifo_decoder (
    input [31:0]    raw_data,

    output [2:0]    data_type,
    output [31:0]   decoded_data
);

    assign data_type = raw_data[31:29];
    assign in_data = {3'd0, raw_data[28:0]};

endmodule

`endif
