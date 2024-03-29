`timescale 1ns / 1ps

module PX_HDL_alphablend(
input wire [7:0] a1,
input wire [7:0] r1,
input wire [7:0] g1,
input wire [7:0] b1,
input wire [7:0] a2,
input wire [7:0] r2,
input wire [7:0] g2,
input wire [7:0] b2,
output wire [7:0] a,
output wire [7:0] r,
output wire [7:0] g,
output wire [7:0] b
);

assign a = 16'd255 - (((16'd256 - {8'd0,a1}) * (16'd255 - {8'd0,a2})) >>8);
assign r= ((16'd256 - {8'd0,a1}) * r2 + r1 * ({8'd0,a1}+1)) >>8;
assign g= ((16'd256 - {8'd0,a1}) * g2 + g1 * ({8'd0,a1}+1)) >>8;
assign b= ((16'd256 - {8'd0,a1}) * b2 + b1 * ({8'd0,a1}+1)) >>8;

endmodule
