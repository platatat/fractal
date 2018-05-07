`timescale 1ns/1ns

`include "solver_datapath.v"

module top();
    localparam LIMB_INDEX_BITS = 6;
    localparam LIMB_SIZE_BITS = 8;
    localparam DIVERGENCE_RADIUS = 4;

    reg clock;
    reg reset;
    reg [16:0] cycle_num;

    reg [LIMB_SIZE_BITS-1:0] c_re_data [3:0];
    reg [LIMB_SIZE_BITS-1:0] c_im_data [3:0];

    initial begin
        $dumpfile("build/solver_datapath_test.vcd");
        $dumpvars(0, top);

        reset   <= 1;
        c_re_data[0] <= 8'h01;
        c_re_data[1] <= 8'h00;
        c_im_data[0] <= 8'h01;
        c_im_data[1] <= 8'h00;
        #20

        reset <= 0;
        wr_en <= 1;
        wr_limb <= 0;
        wr_data_re <= c_re_data[0];
        wr_data_im <= c_im_data[0];
        #20

        wr_limb <= 1;
        wr_data_re <= c_re_data[1];
        wr_data_im <= c_im_data[1];
        #20

        wr_en <= 0;
        start <= 1;
        #20
    end

    always begin
        #10
        clock <= !clock;
        $display("cycle %d", cycle_num);
        cycle_num <= cycle_num + 1;

        if (solver_done) begin
            $finish;
        end
    end

    reg                         wr_en;
    reg [LIMB_INDEX_BITS-1:0]   wr_limb;
    reg [LIMB_SIZE_BITS-1:0]    wr_data_re;
    reg [LIMB_SIZE_BITS-1:0]    wr_data_im;
    reg                         start;

    wire                        solver_done;
    wire                        solver_output;

    solver #(LIMB_INDEX_BITS, LIMB_SIZE_BITS, DIVERGENCE_RADIUS) solver (
        .clock      (clock),
        .reset      (reset),
        .wr_en      (wr_en),
        .wr_limb    (wr_limb),
        .wr_data_re (wr_data_re),
        .wr_data_im (wr_data_im),
        .start      (start),
        .done       (solver_done),
        .output     (solver_output)
    );

endmodule
