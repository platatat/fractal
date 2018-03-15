`include "mult.v"

/*
 * Computes iterations of z(n+1) = z(n)^2 + c. Returns the number of iterations until
 * divergence, or -1 for convergence.
 *
 * out_ready = 0 while the solver is running, and 1 after the solution is ready.
 */
module mand_solver(clock, reset, c_im, c_re, out_ready, out);
    // Max iterations before assuming convergence.
    parameter CONVERGENCE_ITER = 100;
    // Max *squared* magnitude of z before assuming divergence.
    parameter DIVERGENCE_MAG = 27'd4 << 20;

    input clock;
    input reset;

    input signed [26:0] c_im;
    input signed [26:0] c_re;

    output reg out_ready;
    output reg signed [31:0] out;

    reg [31:0] iteration;
    reg signed [26:0] z_im;
    reg signed [26:0] z_re;

    /*
     * z^2      = (z_im + j * z_re)(z_im + j * z_re)
     *          = (z_im^2 - z_re^2 + j * 2 * z_im * z_re)
     *
     * (z^2)_im = 2 * z_im * z_re
     * (z^2)_re = z_re^2 - z_im^2
     *
     * z_new = z^2 + c
     */

    wire signed [26:0] z_im_sq;
    signed_mult #(7, 20) z_im_sq_mult(
        .a(z_im),
        .b(z_im),
        .out(z_im_sq)
    );

    wire signed [26:0] z_re_sq;
    signed_mult #(7, 20) z_re_sq_mult(
        .a(z_re),
        .b(z_re),
        .out(z_re_sq)
    );

    wire signed [26:0] z_im_z_re;
    signed_mult #(7, 20) z_im_z_re_mult(
        .a(z_im),
        .b(z_re),
        .out(z_im_z_re)
    );

    wire signed [26:0] z_im_new = (z_im_z_re <<< 1) + c_im;
    wire signed [26:0] z_re_new = (z_re_sq - z_im_sq) + c_re;

    /*
     * Compute magnitude to test for divergence. It doesn't really matter if we get magnitude
     * for this iteration or the last one because that will just change the effective
     * convergence iteration limit by 1. So we can reuse some of the multiplications from 
     * computing z_new.
     */
    wire signed [26:0] z_mag_sq = z_im_sq + z_re_sq;

    always @(posedge clock) begin
        if (reset) begin
            iteration <= 0;
            out_ready <= 0;
            out <= 0;
            z_im <= c_im;
            z_re <= c_re;
        end else begin
            if (iteration > CONVERGENCE_ITER) begin
                out <= -32'd1;
                out_ready <= 1;
            end else if (z_mag_sq > DIVERGENCE_MAG) begin
                out <= iteration;
                out_ready <= 1;
            end else begin
                iteration <= iteration + 1;
                z_im <= z_im_new;
                z_re <= z_re_new;
            end
        end
    end
endmodule
