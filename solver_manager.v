`ifndef _solver_manager_v_
`define _solver_manager_v_

`include "tile_solver.v"


module solver_manager #(
    parameter NUM_SOLVERS       = 1,
    parameter LIMB_INDEX_BITS   = 6,
    parameter LIMB_SIZE_BITS    = 27,
    parameter DIVERGENCE_RADIUS = 4
) (
    input           clock,
    input           reset,

    input           fifo_valid,
    input [2:0]     fifo_data_type,
    input [31:0]    fifo_data,
    
    output reg      fifo_ready
);

    reg [4:0] state;
    reg [7:0] solver_index;

    wire [4:0] data_header;
    wire [26:0] data_body;

    wire solver_write_real_en;
    wire solver_write_imag_en;
    wire solver_start;

    reg  [31:0]                 solvers_output_addr [NUM_SOLVERS-1:0];
    reg  [31:0]                 solvers_zoom_level  [NUM_SOLVERS-1:0];

    reg  [LIMB_INDEX_BITS-1:0]  solver_write_limb_real;
    reg  [LIMB_INDEX_BITS-1:0]  solver_write_limb_imag;
    wire [LIMB_INDEX_BITS-1:0]  solver_write_limb;
    wire [LIMB_SIZE_BITS-1:0]   solver_write_data;

    wire                        solvers_ready       [NUM_SOLVERS-1:0];

    generate
        genvar i;
        for (i = 0; i < NUM_SOLVERS; i = i + 1) begin : tile_solvers
            tile_solver #(LIMB_INDEX_BITS, 
                          LIMB_SIZE_BITS, 
                          DIVERGENCE_RADIUS) tile_solver(
                .clock          (clock),
                .reset          (reset),
                .write_real_en  (solver_index == i && solver_write_real_en),
                .write_imag_en  (solver_index == i && solver_write_imag_en),
                .write_limb     (solver_write_limb),
                .write_data     (solver_write_data),
                .zoom_level     (solvers_zoom_level[i]),
                .output_addr    (solvers_output_addr[i]),
                .start          (solver_index == i && solver_start),
                .ready          (solvers_ready[i])
            );

            always @(posedge clock) begin
                // TODO: Do we need reset?
                /*
                if (reset) begin
                    solvers_output_addr[i] <= 0;
                    solvers_zoom_level[i] <= 0;
                end
                */
            end
        end
    endgenerate

    assign solver_write_data = fifo_data[LIMB_SIZE_BITS-1:0];

    wire write_en = state == 4'd1 && fifo_valid;
    wire write_real = fifo_data_type == 3'd2;
    wire write_imag = fifo_data_type == 3'd3;
    assign solver_write_real_en = write_en && write_real;
    assign solver_write_imag_en = write_en && write_imag;

    assign solver_write_limb = write_real ? solver_write_limb_real :
                               write_imag ? solver_write_limb_imag : 0;

    assign solver_start = write_en && fifo_data_type == 3'd4;

    always @(posedge clock) begin
        if (reset) begin
            state <= 4'd0;
            solver_index <= 8'd0;
            solver_write_limb_real <= 0;
            solver_write_limb_imag <= 0;
        end else begin
            // Wait for a free solver.
            if (state == 4'd0) begin
                if (solvers_ready[solver_index]) begin
                    state <= 4'd1;
                    fifo_ready <= 1;
                    solver_write_limb_real <= 0;
                    solver_write_limb_imag <= 0;
                end else begin
                    solver_index = (solver_index + 1) % NUM_SOLVERS;
                    fifo_ready <= 0;
                end
            end 

            // Send data from fifo to solver.
            else if (state == 4'd1) begin
                if (fifo_valid) begin
                    // Set tile output address
                    if (fifo_data_type == 3'd0) begin
                        solvers_output_addr[solver_index] <= fifo_data;
                    end

                    // Set zoom level
                    else if (fifo_data_type == 3'd1) begin
                        solvers_zoom_level[solver_index] <= fifo_data;
                    end

                    // Write a limb of c_real.
                    else if (fifo_data_type == 3'd2) begin
                        solver_write_limb_real <= solver_write_limb_real + 1;
                    end

                    // Write a limb of c_imag.
                    else if (fifo_data_type == 3'd3) begin
                        solver_write_limb_imag <= solver_write_limb_imag + 1;
                    end

                    // Start the solver
                    else if (fifo_data_type == 3'd4) begin
                        state <= 4'd0;
                    end
                end
            end
        end
    end

endmodule


`endif
