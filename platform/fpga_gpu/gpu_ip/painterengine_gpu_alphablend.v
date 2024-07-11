`timescale 1 ns / 1 ns

module painterengine_gpu_alphablend(
input wire i_wire_clock,
input wire i_wire_resetn,
input wire i_wire_valid,
output wire o_wire_valid,
input wire [7:0] a1,
input wire [7:0] r1,
input wire [7:0] g1,
input wire [7:0] b1,
input wire [7:0] a2,
input wire [7:0] r2,
input wire [7:0] g2,
input wire [7:0] b2,
input wire [7:0] ba,
input wire [7:0] br,
input wire [7:0] bg,
input wire [7:0] bb,
output wire [7:0] a,
output wire [7:0] r,
output wire [7:0] g,
output wire [7:0] b
);
reg [7:0] wa;
reg [7:0] wr;
reg [7:0] wg;
reg [7:0] wb;

reg reg_valid_d0;
reg reg_valid_d1; 
reg reg_valid_d2;

reg [2:0] state;

reg [15:0] ra1;
reg [15:0] ra2;

reg [15:0] br2;
reg [15:0] bg2;
reg [15:0] bb2;

reg [15:0] wr2;
reg [15:0] wg2;
reg [15:0] wb2;

reg [7:0] o_a;
reg [7:0] o_r;
reg [7:0] o_g;
reg [7:0] o_b;

always @(posedge i_wire_clock or negedge i_wire_resetn)
begin
    if (!i_wire_resetn)
    begin
        wa<=8'd0;
        wr<=8'd0;
        wg<=8'd0;
        wb<=8'd0;
        reg_valid_d0 <= 1'b0;
    end
    else if (i_wire_valid)
    begin
        reg_valid_d0 <= 1'b1;
        wa<=({8'd0,a1}*ba)>>7;
        wr<=({8'd0,r1}*br)>>7;
        wg<=({8'd0,g1}*bg)>>7;
        wb<=({8'd0,b1}*bb)>>7;
    end
    else
    begin
        reg_valid_d0 <= 1'b0;
    end
end

always @(posedge i_wire_clock or negedge i_wire_resetn)
begin
    if (!i_wire_resetn)
    begin
        wa<=8'd0;
        wr<=8'd0;
        wg<=8'd0;
        wb<=8'd0;
        reg_valid_d1 <= 1'b0;
    end
    else if (reg_valid_d0)
    begin
        ra1<=(16'd256 - {8'd0,a2});
        ra2<=(16'd255 - {8'd0,wa});

        br2<=(16'd256 - {8'd0,wa}) * r2;
        bg2<=(16'd256 - {8'd0,wa}) * g2;
        bb2<=(16'd256 - {8'd0,wa}) * b2;

        wr2<=wr * ({8'd0,wa}+1);
        wg2<=wg * ({8'd0,wa}+1);
        wb2<=wb * ({8'd0,wa}+1);

        reg_valid_d1 <= 1'b1;
    end
    else
    begin
        reg_valid_d1 <= 1'b0;
    end
end

always @(posedge i_wire_clock or negedge i_wire_resetn)
begin
    if (!i_wire_resetn)
    begin
        o_a<=8'd0;
        o_r<=8'd0;
        o_g<=8'd0;
        o_b<=8'd0;
        reg_valid_d2 <= 1'b0;
    end
    else if (reg_valid_d1)
    begin
        o_a<=16'd255 - ((ra1 * ra2) >>8);
        o_r<=(br2 + wr2) >>8;
        o_g<=(bg2 + wg2) >>8;
        o_b<=(bb2 + wb2) >>8;
        reg_valid_d2 <= 1'b1;
    end
    else
    begin
        reg_valid_d2 <= 1'b0;
    end

end

assign a = o_a;
assign r = o_r;
assign g = o_g;
assign b = o_b;
assign o_wire_valid = reg_valid_d2;

endmodule

`define BLENDER_ARGB_MODE_AXXX  0
`define BLENDER_ARGB_MODE_XXXA  1

module painterengine_gpu_blender
(
    input wire i_wire_clock,
    input wire i_wire_resetn,

    input wire  i_wire_argb_mode,

	input wire [31:0] 	i_wire_data1_in,
    input wire [31:0] 	i_wire_data2_in,
    input wire [31:0] 	i_wire_blend,

	output wire [31:0] 	o_wire_data_out,
    output wire  o_wire_data_valid,

	input wire  i_wire_fifo1_empty,
    input wire  i_wire_fifo2_empty,

    output wire  o_wire_fifo1_read,
    output wire  o_wire_fifo2_read
);

wire both_ready;

assign both_ready = !i_wire_fifo1_empty && !i_wire_fifo2_empty;
assign o_wire_fifo1_read=both_ready;
assign o_wire_fifo2_read=both_ready;
wire wire_o_blend_valid1;
wire wire_o_blend_valid2;

wire [31:0] 				wire_data_argb_mode_1_out;
wire [31:0] 				wire_data_argb_mode_2_out;

assign o_wire_data_out = i_wire_argb_mode ? wire_data_argb_mode_1_out : wire_data_argb_mode_2_out;
assign o_wire_data_valid = i_wire_argb_mode ? wire_o_blend_valid1 : wire_o_blend_valid2;

painterengine_gpu_alphablend blender1(
    .i_wire_clock(i_wire_clock),
    .i_wire_resetn(i_wire_resetn),
    .i_wire_valid(both_ready),
    .o_wire_valid(wire_o_blend_valid1),

    .a1(i_wire_data1_in[31:24]),
    .r1(i_wire_data1_in[23:16]),
    .g1(i_wire_data1_in[15:8]),
    .b1(i_wire_data1_in[7:0]),
    .a2(i_wire_data2_in[31:24]),
    .r2(i_wire_data2_in[23:16]),
    .g2(i_wire_data2_in[15:8]),
    .b2(i_wire_data2_in[7:0]),
    .ba(i_wire_blend[31:24]),
    .br(i_wire_blend[23:16]),
    .bg(i_wire_blend[15:8]),
    .bb(i_wire_blend[7:0]),
    .a(wire_data_argb_mode_1_out[31:24]),
    .r(wire_data_argb_mode_1_out[23:16]),
    .g(wire_data_argb_mode_1_out[15:8]),
    .b(wire_data_argb_mode_1_out[7:0])
);

painterengine_gpu_alphablend blender2(
    .i_wire_clock(i_wire_clock),
    .i_wire_resetn(i_wire_resetn),
    .i_wire_valid(both_ready),
    .o_wire_valid(wire_o_blend_valid2),

    .a1(i_wire_data1_in[7:0]),
    .r1(i_wire_data1_in[15:8]),
    .g1(i_wire_data1_in[23:16]),
    .b1(i_wire_data1_in[31:24]),
    .a2(i_wire_data2_in[7:0]),
    .r2(i_wire_data2_in[15:8]),
    .g2(i_wire_data2_in[23:16]),
    .b2(i_wire_data2_in[31:24]),
    .ba(i_wire_blend[31:24]),
    .br(i_wire_blend[23:16]),
    .bg(i_wire_blend[15:8]),
    .bb(i_wire_blend[7:0]),
    .a(wire_data_argb_mode_2_out[31:24]),
    .r(wire_data_argb_mode_2_out[23:16]),
    .g(wire_data_argb_mode_2_out[15:8]),
    .b(wire_data_argb_mode_2_out[7:0])
);



endmodule
