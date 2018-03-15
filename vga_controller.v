module vga_controller(clock, reset, control, vga_data, vga_address);
    parameter vga_base_address = 32'h0000_0000;

    input clock;
    input reset;

    input [31:0] control;
    wire [3:0] pan_right_rate = control[ 3: 0];
    wire [3:0] pan_down_rate  = control[ 7: 4];
    wire [3:0] pan_up_rate    = control[11: 8];
    wire [3:0] pan_left_rate  = control[15:12];
    wire [3:0] zoom_out_rate  = control[19:16];
    wire [3:0] zoom_in_rate   = control[23:20];

    output reg [7:0] vga_data;
    output reg [31:0] vga_address;

    reg [3:0] state;

    // VGA coordinates in pixels.
    reg [9:0] x_vga;
    reg [9:0] y_vga;

    // Convert vga coordinates to solver coordinates.
    wire signed [26:0] x_solver = (x_vga << 12) - (27'd320 <<< 12);
    wire signed [26:0] y_solver = (y_vga << 12) - (27'd240 <<< 12);
    reg signed [26:0] zoom_scale;

    wire [9:0] x_min = 10'd0;
    wire [9:0] x_max = 10'd640;
    wire [9:0] y_min = 10'd0;
    wire [9:0] y_max = 10'd480;

    wire signed [26:0] x_zoom_center = -27'd786480;
    wire signed [26:0] y_zoom_center = 27'd5019;

    reg solver_reset;
    reg signed [26:0] solver_c_im;
    reg signed [26:0] solver_c_re;
    wire solver_out_ready;
    wire signed [31:0] solver_out;

    mand_solver mand_solver(
        .clock(clock),
        .reset(solver_reset),
        .c_im(solver_c_im),
        .c_re(solver_c_re),
        .out_ready(solver_out_ready),
        .out(solver_out)
    );

    wire signed [26:0] x_solver_scaled;
    signed_mult x_solver_scaled_mult(
        .a(x_solver),
        .b(zoom_scale),
        .out(x_solver_scaled)
    );

    wire signed [26:0] y_solver_scaled;
    signed_mult y_solver_scaled_mult(
        .a(y_solver),
        .b(zoom_scale),
        .out(y_solver_scaled)
    );

    always @(posedge clock) begin
        if (reset) begin
            state <= 4'd0 ;
            x_vga <= x_min;
            y_vga <= y_min;
            zoom_scale <= 27'd1 <<< 20;
        end

        // Pan and zoom based on control signals.
        x_zoom_center <= x_zoom_center + ({23'b0, pan_right_rate});
        x_zoom_center <= x_zoom_center - ({23'b0, pan_left_rate});
        y_zoom_center <= y_zoom_center + ({23'b0, pan_up_rate});
        y_zoom_center <= y_zoom_center - ({23'b0, pan_down_rate});

        if (state == 4'd0) begin
            // Initialize solver.
            solver_reset <= 1'b1;
            solver_c_re <= x_zoom_center + x_solver_scaled;
            solver_c_im <= y_zoom_center + y_solver_scaled;
            state <= 4'd1;
        end else if (state == 4'd1) begin
            // Run solver.
            solver_reset <= 1'b0;
            state <= 4'd2;
        end else if (state == 4'd2) begin
            // Wait for solver to finish.
            if (solver_out_ready) begin
                state <= 4'd3;
            end else begin
                state <= 4'd2;
            end
        end else if (state == 4'd3) begin
            // Write solver output to VGA.
            if (solver_out == -32'd1)        vga_data <= 8'b11111111;
            else if ((solver_out >>> 7) > 0) vga_data <= 8'b00011100;
            else if ((solver_out >>> 6) > 0) vga_data <= 8'b00011000;
            else if ((solver_out >>> 5) > 0) vga_data <= 8'b00010100;
            else if ((solver_out >>> 4) > 0) vga_data <= 8'b00010000;
            else if ((solver_out >>> 3) > 0) vga_data <= 8'b00001100;
            else if ((solver_out >>> 2) > 0) vga_data <= 8'b00001000;
            else if ((solver_out >>> 1) > 0) vga_data <= 8'b00000100;
            else                             vga_data <= 8'b00000000;

            vga_address <= vga_base_address + {22'b0, x_vga} + ({22'b0, y_vga} * 640);

            // Iterate over x and y.
            if (x_vga < x_max) begin
                x_vga <= x_vga + 10'd1;
            end else begin
                x_vga <= x_min;
                if (y_vga < y_max) begin
                    y_vga <= y_vga + 10'd1;
                end else begin
                    y_vga <= y_min;
                    zoom_scale <= zoom_scale - (zoom_scale >> 6);
                end
            end

            state  <= 4'd0;
        end
    end
endmodule
