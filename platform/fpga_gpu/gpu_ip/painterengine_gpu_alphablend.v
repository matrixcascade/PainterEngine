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
reg [7:0] reg_br_0;
reg [7:0] reg_bg_0;
reg [7:0] reg_bb_0;
reg [7:0] reg_ba0_0;
reg [7:0] reg_ba1_0;
reg [7:0] reg_ba2_0;
reg [7:0] reg_ba3_0;

reg [7:0] reg_a1a_0;
reg [7:0] reg_a1b_0;
reg [7:0] reg_a1c_0;

reg [7:0] reg_r1_0;
reg [7:0] reg_g1_0;
reg [7:0] reg_b1_0;

reg [7:0] reg_a2_0;
reg [7:0] reg_r2_0;
reg [7:0] reg_g2_0;
reg [7:0] reg_b2_0;

reg [7:0] reg_r2_1;
reg [7:0] reg_g2_1;
reg [7:0] reg_b2_1;

reg [7:0] reg_r2_2;
reg [7:0] reg_g2_2;
reg [7:0] reg_b2_2;

reg [7:0] reg_a2_1;
reg [7:0] wa0_1;
reg [7:0] wa1_1;
reg [7:0] wa2_1;
reg [7:0] wa3_1;
reg [7:0] wa4_1;
reg [7:0] wa5_1;
reg [7:0] wa6_1;

reg [7:0] wr_1;
reg [7:0] wg_1;
reg [7:0] wb_1;

reg [7:0] wr_2;
reg [7:0] wg_2;
reg [7:0] wb_2;

reg reg_valid_d0;
reg reg_valid_d1; 
reg reg_valid_d2;
reg reg_valid_d3;
reg reg_valid_d4;

reg [8:0] ra1_2;
reg [8:0] ra2_2;

reg [15:0] br2_2;
reg [15:0] bg2_2;
reg [15:0] bb2_2;

reg [15:0] wr2_2;
reg [15:0] wg2_2;
reg [15:0] wb2_2;

reg [15:0] br3;
reg [15:0] bg3;
reg [15:0] bb3;

reg [15:0] wr3;
reg [15:0] wg3;
reg [15:0] wb3;

reg [18:0] ra1_ra2;

reg [7:0] o_a;
reg [7:0] o_r;
reg [7:0] o_g;
reg [7:0] o_b;

always @(posedge i_wire_clock or negedge i_wire_resetn)
begin
    if (!i_wire_resetn||!i_wire_valid)
    begin
        reg_br_0 <= 8'd0;
        reg_bg_0 <= 8'd0;
        reg_bb_0 <= 8'd0;
        reg_ba0_0 <= 8'd0;
        reg_ba1_0 <= 8'd0;
        reg_ba2_0 <= 8'd0;
        reg_ba3_0 <= 8'd0;

        reg_a2_0 <= 8'd0;
        reg_r2_0 <= 8'd0;
        reg_g2_0 <= 8'd0;
        reg_b2_0 <= 8'd0;

        reg_a1a_0 <= 8'd0;
        reg_a1b_0 <= 8'd0;
        reg_a1c_0<= 8'd0;
        reg_r1_0 <= 8'd0;
        reg_g1_0 <= 8'd0;
        reg_b1_0 <= 8'd0;

        reg_valid_d0 <= 1'b0;
    end
    else
    begin
        reg_br_0 <= br;
        reg_bg_0 <= bg;
        reg_bb_0 <= bb;
        reg_ba0_0 <= ba;
        reg_ba1_0 <= ba;
        reg_ba2_0 <= ba;
        reg_ba3_0 <= ba;

        reg_a2_0 <= a2;
        reg_r2_0 <= r2;
        reg_g2_0 <= g2;
        reg_b2_0 <= b2;

        reg_a1a_0 <= a1;
        reg_a1b_0 <= a1;
        reg_a1c_0<= a1;
        reg_r1_0 <= r1;
        reg_g1_0 <= g1;
        reg_b1_0 <= b1;

        reg_valid_d0 <= 1'b1;
    end
end


always @(posedge i_wire_clock or negedge i_wire_resetn)
begin
    if (!i_wire_resetn||!reg_valid_d0)
    begin
        wa0_1<=8'd0;
        wa1_1<=8'd0;
        wa2_1<=8'd0;
        wa3_1<=8'd0;
        wa4_1<=8'd0;
        wa5_1<=8'd0;
        wa6_1<=8'd0;
        wr_1<=8'd0;
        wg_1<=8'd0;
        wb_1<=8'd0;
        reg_a2_1<=8'd0;
        reg_r2_1<=8'd0;
        reg_g2_1<=8'd0;
        reg_b2_1<=8'd0;
        reg_valid_d1 <= 1'b0;
    end
    else
    begin
        wa0_1<=({8'd0,reg_a1a_0}*reg_ba0_0)>>7;
        wa1_1<=({8'd0,reg_a1a_0}*reg_ba0_0)>>7;
        wa2_1<=({8'd0,reg_a1b_0}*reg_ba1_0)>>7;
        wa3_1<=({8'd0,reg_a1b_0}*reg_ba1_0)>>7;
        wa4_1<=({8'd0,reg_a1c_0}*reg_ba2_0)>>7;
        wa5_1<=({8'd0,reg_a1c_0}*reg_ba2_0)>>7;
        wa6_1<=({8'd0,reg_a1c_0}*reg_ba3_0)>>7;
        wr_1<=({8'd0,reg_r1_0}*reg_br_0)>>7;
        wg_1<=({8'd0,reg_g1_0}*reg_bg_0)>>7;
        wb_1<=({8'd0,reg_b1_0}*reg_bb_0)>>7;

        reg_a2_1<=reg_a2_0;
        reg_r2_1<=reg_r2_0;
        reg_g2_1<=reg_g2_0;
        reg_b2_1<=reg_b2_0;
        reg_valid_d1 <= 1'b1;
    end
end

always @(posedge i_wire_clock or negedge i_wire_resetn)
begin
    if (!i_wire_resetn||!reg_valid_d1)
    begin
        ra1_2<=16'd0;
        ra2_2<=16'd0;
        br2_2<=16'd0;
        bg2_2<=16'd0;
        bb2_2<=16'd0;
        wr2_2<=16'd0;
        wg2_2<=16'd0;
        wb2_2<=16'd0;
        reg_r2_2<=8'd0;
        reg_g2_2<=8'd0;
        reg_b2_2<=8'd0;
        wr_2<=16'd0;
        wg_2<=16'd0;
        wb_2<=16'd0;


        reg_valid_d2 <= 1'b0;
    end
    else
    begin
        ra1_2<=(16'd256 - {8'd0,reg_a2_1});
        ra2_2<=(16'd255 - {8'd0,wa0_1});

        br2_2<=(16'd256 - {8'd0,wa1_1}) ;
        bg2_2<=(16'd256 - {8'd0,wa2_1}) ;
        bb2_2<=(16'd256 - {8'd0,wa3_1}) ;

        wr2_2<=({8'd0,wa4_1}+1);
        wg2_2<=({8'd0,wa5_1}+1);
        wb2_2<=({8'd0,wa6_1}+1);

        reg_r2_2<=reg_r2_1;
        reg_g2_2<=reg_g2_1;
        reg_b2_2<=reg_b2_1;
        wr_2<=wr_1;
        wg_2<=wg_1;
        wb_2<=wb_1;

        reg_valid_d2 <= 1'b1;
    end
end
always @(posedge i_wire_clock or negedge i_wire_resetn)
begin
    if (!i_wire_resetn||!reg_valid_d2)
    begin
        br3<=16'd0;
        bg3<=16'd0;
        bb3<=16'd0;
        wr3<=16'd0;
        wg3<=16'd0;
        wb3<=16'd0;
        ra1_ra2<=32'd0;
        reg_valid_d3 <= 1'b0;
    end
    else
    begin
        br3<=br2_2 * reg_r2_2;
        bg3<=bg2_2 * reg_g2_2;
        bb3<=bb2_2 * reg_b2_2;
        wr3<=wr_2 * wr2_2;
        wg3<=wg_2 * wg2_2;
        wb3<=wb_2 * wb2_2;
        ra1_ra2<=ra1_2 * ra2_2;
        reg_valid_d3 <= 1'b1;
    end
end


always @(posedge i_wire_clock or negedge i_wire_resetn)
begin
    if (!i_wire_resetn||!reg_valid_d3)
    begin
        o_a<=8'd0;
        o_r<=8'd0;
        o_g<=8'd0;
        o_b<=8'd0;
        reg_valid_d4 <= 1'b0;
    end
    else
    begin
        o_a<=16'd255 - (ra1_ra2 >>8);
        o_r<=(br3 + wr3) >>8;
        o_g<=(bg3 + wg3) >>8;
        o_b<=(bb3 + wb3) >>8;
        reg_valid_d4 <= 1'b1;
    end
end

assign a = o_a;
assign r = o_r;
assign g = o_g;
assign b = o_b;
assign o_wire_valid = reg_valid_d4;

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
reg[31:0] reg_data1_in;
reg[31:0] reg_data2_in;
reg[31:0] reg_blend;

reg reg_data_ready_d0;
wire wire_both_ready;
assign wire_both_ready=(!i_wire_fifo1_empty && !i_wire_fifo2_empty);

always @(posedge i_wire_clock or negedge i_wire_resetn)
begin
    if (!i_wire_resetn)
    begin
        reg_data_ready_d0 <= 1'b0;
        reg_data1_in <= 32'd0;
        reg_data2_in <= 32'd0;
        reg_blend <= 32'd0;
    end
    else
    begin
        reg_data_ready_d0 <= wire_both_ready;
        reg_data1_in <= i_wire_data1_in;
        reg_data2_in <= i_wire_data2_in;
        reg_blend <= i_wire_blend;
    end
end


assign o_wire_fifo1_read=wire_both_ready;
assign o_wire_fifo2_read=wire_both_ready;

wire wire_o_blend_valid1;
wire wire_o_blend_valid2;

wire [31:0] 	wire_data_argb_mode_1_out;
wire [31:0] 	wire_data_argb_mode_2_out;

painterengine_gpu_alphablend blender1(
    .i_wire_clock(i_wire_clock),
    .i_wire_resetn(i_wire_resetn),
    .i_wire_valid(reg_data_ready_d0),
    .o_wire_valid(wire_o_blend_valid1),

    .a1(reg_data1_in[31:24]),
    .r1(reg_data1_in[23:16]),
    .g1(reg_data1_in[15:8]),
    .b1(reg_data1_in[7:0]),
    .a2(reg_data2_in[31:24]),
    .r2(reg_data2_in[23:16]),
    .g2(reg_data2_in[15:8]),
    .b2(reg_data2_in[7:0]),
    .ba(reg_blend[31:24]),
    .br(reg_blend[23:16]),
    .bg(reg_blend[15:8]),
    .bb(reg_blend[7:0]),
    .a(wire_data_argb_mode_1_out[31:24]),
    .r(wire_data_argb_mode_1_out[23:16]),
    .g(wire_data_argb_mode_1_out[15:8]),
    .b(wire_data_argb_mode_1_out[7:0])
);

painterengine_gpu_alphablend blender2(
    .i_wire_clock(i_wire_clock),
    .i_wire_resetn(i_wire_resetn),
    .i_wire_valid(reg_data_ready_d0),
    .o_wire_valid(wire_o_blend_valid2),

    .a1(reg_data1_in[7:0]),
    .r1(reg_data1_in[15:8]),
    .g1(reg_data1_in[23:16]),
    .b1(reg_data1_in[31:24]),
    .a2(reg_data2_in[7:0]),
    .r2(reg_data2_in[15:8]),
    .g2(reg_data2_in[23:16]),
    .b2(reg_data2_in[31:24]),
    .ba(reg_blend[31:24]),
    .br(reg_blend[23:16]),
    .bg(reg_blend[15:8]),
    .bb(reg_blend[7:0]),
    .a(wire_data_argb_mode_2_out[31:24]),
    .r(wire_data_argb_mode_2_out[23:16]),
    .g(wire_data_argb_mode_2_out[15:8]),
    .b(wire_data_argb_mode_2_out[7:0])
);

assign o_wire_data_out = i_wire_argb_mode ? wire_data_argb_mode_1_out : wire_data_argb_mode_2_out;
assign o_wire_data_valid = i_wire_argb_mode ? wire_o_blend_valid1 : wire_o_blend_valid2;

endmodule
