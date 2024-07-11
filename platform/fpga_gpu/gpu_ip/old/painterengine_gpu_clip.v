    
`timescale 1 ns / 1 ns

	`define FIFO_DEPTH 64
	`define DATA_WIDTH 32
	`define ADDRESS_WIDTH 32
	`define ADDRESS_ALIGN 64
	`define RW_CACHE_SIZE 256

    module painterengine_gpu_clip #
		(
		parameter integer PARAM_TEXTURE_MAX_SIZE	= 16
		)
		(
		input wire   												i_wire_clock,
		input wire   												i_wire_resetn,

		input wire [PARAM_TEXTURE_MAX_SIZE-1:0] 					i_wire_texture_width,
		input wire [PARAM_TEXTURE_MAX_SIZE-1:0] 					i_wire_texture_height,

		input wire [PARAM_TEXTURE_MAX_SIZE-1:0] 					i_wire_clip_x,
		input wire [PARAM_TEXTURE_MAX_SIZE-1:0] 					i_wire_clip_y,
		input wire [PARAM_TEXTURE_MAX_SIZE-1:0] 					i_wire_clip_width,
		input wire [PARAM_TEXTURE_MAX_SIZE-1:0] 					i_wire_clip_height,

		input wire [PARAM_TEXTURE_MAX_SIZE-1:0] 					i_wire_x,
		input wire [PARAM_TEXTURE_MAX_SIZE-1:0] 					i_wire_y,

		input wire [PARAM_TEXTURE_MAX_SIZE-1:0] 					i_wire_limit_left,
		input wire [PARAM_TEXTURE_MAX_SIZE-1:0] 					i_wire_limit_top,
		input wire [PARAM_TEXTURE_MAX_SIZE-1:0] 					i_wire_limit_right,
		input wire [PARAM_TEXTURE_MAX_SIZE-1:0] 					i_wire_limit_bottom,

		input wire [3:0]  											i_wire_texture_align,
		input wire [2:0]  											i_wire_texture_mirror_mode,

		output wire [PARAM_TEXTURE_MAX_SIZE-1:0] 					o_wire_x,
		output wire [PARAM_TEXTURE_MAX_SIZE-1:0] 					o_wire_y,

		output wire [PARAM_TEXTURE_MAX_SIZE-1:0] 					o_wire_clipx,
		output wire [PARAM_TEXTURE_MAX_SIZE-1:0] 					o_wire_clipy,
		output wire [PARAM_TEXTURE_MAX_SIZE-1:0] 					o_wire_clipw,
		output wire [PARAM_TEXTURE_MAX_SIZE-1:0] 					o_wire_cliph,

		output wire                                                 o_wire_done
		);

		reg signed [PARAM_TEXTURE_MAX_SIZE-1:0] 					reg_texture_width;
		reg signed [PARAM_TEXTURE_MAX_SIZE-1:0] 					reg_texture_height;

		reg signed [PARAM_TEXTURE_MAX_SIZE-1:0] 					reg_x;
		reg signed [PARAM_TEXTURE_MAX_SIZE-1:0] 					reg_y;
		reg signed [PARAM_TEXTURE_MAX_SIZE-1:0] 					reg_clipx;
		reg signed [PARAM_TEXTURE_MAX_SIZE-1:0] 					reg_clipy;
		reg signed [PARAM_TEXTURE_MAX_SIZE-1:0] 					reg_clipw;
		reg signed [PARAM_TEXTURE_MAX_SIZE-1:0] 					reg_cliph;

		reg signed [PARAM_TEXTURE_MAX_SIZE-1:0] 					reg_limit_left;
		reg signed [PARAM_TEXTURE_MAX_SIZE-1:0] 					reg_limit_top;
		reg signed [PARAM_TEXTURE_MAX_SIZE-1:0] 					reg_limit_right;
		reg signed [PARAM_TEXTURE_MAX_SIZE-1:0] 					reg_limit_bottom;

		reg [3:0]  													reg_align;
		reg [2:0]  													reg_mirror_mode;

		reg [3:0]													reg_state;

		localparam [3:0]  											PX_ALIGN_LEFTTOP = 'd7,
						 											PX_ALIGN_MIDTOP = 'd8,
						 											PX_ALIGN_RIGHTTOP = 'd9,
		                 											PX_ALIGN_LEFTMID = 'd4,
		                 											PX_ALIGN_CENTER = 'd5,
		                 											PX_ALIGN_RIGHTMID = 'd6,
		                 											PX_ALIGN_LEFTBOTTOM = 'd1,
		                 											PX_ALIGN_MIDBOTTOM = 'd2,
		                 											PX_ALIGN_RIGHTBOTTOM = 'd3;

		localparam [2:0]  											PX_TEXTURERENDER_MIRRROR_MODE_NONE = 'd0,
						 											PX_TEXTURERENDER_MIRRROR_MODE_H = 'd1,
						 											PX_TEXTURERENDER_MIRRROR_MODE_V = 'd2,
		                 											PX_TEXTURERENDER_MIRRROR_MODE_HV = 'd3;

		localparam [3:0] 											STATE_DONE='d15;

		always @(posedge i_wire_clock or negedge i_wire_resetn)
		begin
			if (!i_wire_resetn)
			begin
				reg_x<=0;
				reg_y<=0;
				reg_clipx<=0;
				reg_clipy<=0;
				reg_clipw<=0;
				reg_cliph<=0;
				reg_state<=0;
			end
			else
			begin
				case (reg_state)
					0:
					begin
						reg_x<=i_wire_x;
						reg_y<=i_wire_y;

						reg_texture_width<=i_wire_texture_width;
						reg_texture_height<=i_wire_texture_height;


						reg_limit_left<=i_wire_limit_left;
						reg_limit_top<=i_wire_limit_top;
						reg_limit_right<=i_wire_limit_right;
						reg_limit_bottom<=i_wire_limit_bottom;

						reg_clipx<=i_wire_clip_x;
						reg_clipy<=i_wire_clip_y;
						reg_clipw<=i_wire_clip_width;
						reg_cliph<=i_wire_clip_height;

						reg_align<=i_wire_texture_align;
						reg_mirror_mode<=i_wire_texture_mirror_mode;
						reg_state<=1;

					end
					1:
					begin
						case (reg_align)
							PX_ALIGN_LEFTTOP:
							begin
								reg_x<=reg_x;
								reg_y<=reg_y;
							end
							PX_ALIGN_MIDTOP:
							begin
								reg_x<=reg_x-reg_clipw/2;
								reg_y<=reg_y;
							end
							PX_ALIGN_RIGHTTOP:
							begin
								reg_x<=reg_x-reg_clipw;
								reg_y<=reg_y;
							end
							PX_ALIGN_LEFTMID:
							begin
								reg_x<=reg_x;
								reg_y<=reg_y-reg_cliph/2;
							end
							PX_ALIGN_CENTER:
							begin
								reg_x<=reg_x-reg_clipw/2;
								reg_y<=reg_y-reg_cliph/2;
							end
							PX_ALIGN_RIGHTMID:
							begin
								reg_x<=reg_x-reg_clipw;
								reg_y<=reg_y-reg_cliph/2;
							end
							PX_ALIGN_LEFTBOTTOM:
							begin
								reg_x<=reg_x;
								reg_y<=reg_y-reg_cliph;
							end
							PX_ALIGN_MIDBOTTOM:
							begin
								reg_x<=reg_x-reg_clipw/2;
								reg_y<=reg_y-reg_cliph;
							end
							PX_ALIGN_RIGHTBOTTOM:
							begin
								reg_x<=reg_x-reg_clipw;
								reg_y<=reg_y-reg_cliph;
							end
							default:
							begin
								reg_x<=reg_x;
								reg_y<=reg_y;
							end
						endcase
						reg_state<=2;
					end
					2:
					begin
						if(reg_clipx<0)
						begin
							reg_clipx<=0;
							reg_state<=3;
						end
						else if(reg_clipx>=reg_texture_width)
						begin
							reg_clipw<=0;
							reg_cliph<=0;
							reg_state<=STATE_DONE;
						end
						else
						begin
							reg_clipx<=reg_clipx;
							reg_state<=3;
						end
					end
					3:
					begin
						if(reg_clipy<0)
						begin
							reg_clipy<=0;
							reg_state<=4;
						end
						else if(reg_clipy>=reg_texture_height)
						begin
							reg_clipw<=0;
							reg_cliph<=0;
							reg_state<=STATE_DONE;
						end
						else
						begin
							reg_clipy<=reg_clipy;
							reg_state<=4;
						end
					end	
					4:
					begin
						if (reg_clipw<=0)
						begin
							reg_clipw<=0;
							reg_cliph<=0;
							reg_state<=STATE_DONE;
						end
						else if(reg_clipx+reg_clipw>reg_texture_width)
						begin
							reg_clipw<=reg_texture_width-reg_clipx;
							reg_state<=5;
						end
						else
						begin
							reg_clipw<=reg_clipw;
							reg_state<=5;
						end
					end
					5:
					begin
						if (reg_cliph<=0)
						begin
							reg_clipw<=0;
							reg_cliph<=0;
							reg_state<=STATE_DONE;
						end
						else if(reg_clipy+reg_cliph>reg_texture_height)
						begin
							reg_cliph<=reg_texture_height-reg_clipy;
							reg_state<=6;
						end
						else
						begin
							reg_cliph<=reg_cliph;
							reg_state<=6;
						end
					end
					6:
					begin
						case(reg_mirror_mode)
						PX_TEXTURERENDER_MIRRROR_MODE_NONE:
						begin
							if(reg_x<reg_limit_left)
							begin
								reg_clipx<=reg_clipx+reg_limit_left-reg_x;
							end
							else
							begin
								reg_clipx<=reg_clipx;
							end

							if(reg_y<reg_limit_top)
							begin
								reg_clipy<=reg_clipy+reg_limit_top-reg_y;
							end
							else
							begin
								reg_clipy<=reg_clipy;
							end
							reg_state<=7;
						end
						PX_TEXTURERENDER_MIRRROR_MODE_H:
						begin
							if(reg_x+reg_clipw>reg_limit_right+1)
							begin
								reg_clipx<=reg_clipx+reg_x+reg_clipw-reg_limit_right-1;
							end
							else
							begin
								reg_clipx<=reg_clipx;
							end

							if(reg_clipy<reg_limit_top)
							begin
								reg_clipy<=reg_clipy+reg_limit_top-reg_y;
							end
							else
							begin
								reg_clipy<=reg_clipy;
							end
							reg_state<=7;
						end
						PX_TEXTURERENDER_MIRRROR_MODE_V:
						begin
							if(reg_clipx<reg_limit_left)
							begin
								reg_clipx<=reg_clipx+reg_limit_left-reg_x;
							end
							else
							begin
								reg_clipx<=reg_clipx;
							end

							if(reg_y+reg_cliph>reg_limit_bottom+1)
							begin
								reg_clipy<=reg_clipy+reg_y+reg_cliph-reg_limit_bottom-1;
							end
							else
							begin
								reg_clipy<=reg_clipy;
							end
							reg_state<=7;
						end
						PX_TEXTURERENDER_MIRRROR_MODE_HV:
						begin
							if(reg_x+reg_clipw>reg_limit_right+1)
							begin
								reg_clipx<=reg_clipx+reg_x+reg_clipw-reg_limit_right-1;
							end
							else
							begin
								reg_clipx<=reg_clipx;
							end

							if(reg_y+reg_cliph>reg_limit_bottom+1)
							begin
								reg_clipy<=reg_clipy+reg_y+reg_cliph-reg_limit_bottom-1;
							end
							else
							begin
								reg_clipy<=reg_clipy;
							end
							reg_state<=7;
						end
						default:
						begin
							reg_clipw<=0;
							reg_cliph<=0;
							reg_state<=STATE_DONE;
						end
						endcase
					end
					7:
					begin
						if (reg_x<reg_limit_left) 
						begin
							if(reg_x+reg_clipw<reg_limit_left)
							begin
								reg_clipw<=0;
								reg_cliph<=0;
								reg_state<=STATE_DONE;
							end
							else
							begin
								if(reg_x+reg_clipw<=reg_limit_right+1)
								begin
									reg_clipw<=reg_x+reg_clipw-reg_limit_left;
									reg_x<=reg_limit_left;
								end
								else
								begin
									reg_clipw<=reg_limit_right-reg_limit_left+1;
									reg_x<=reg_limit_left;
								end
								reg_state<=8;
							end
						end
						else if(reg_x>=reg_limit_left)
						begin
							if(reg_x+reg_clipw>reg_limit_right+1)
							begin
								reg_clipw<=reg_limit_right+1-reg_x;
							end
							else
							begin
								reg_clipw<=reg_clipw;
							end
							reg_state<=8;
						end
						else
						begin
							reg_x<=reg_x;
							reg_state<=8;
						end
					end
					8:
					begin
						if (reg_y<reg_limit_top) 
						begin
							if(reg_y+reg_cliph<reg_limit_top)
							begin
								reg_clipw<=0;
								reg_cliph<=0;
								reg_state<=STATE_DONE;
							end
							else
							begin
								if(reg_y+reg_cliph<=reg_limit_bottom+1)
								begin
									reg_cliph<=reg_y+reg_cliph-reg_limit_top;
									reg_y<=reg_limit_top;
								end
								else
								begin
									reg_cliph<=reg_limit_bottom-reg_limit_top+1;
									reg_y<=reg_limit_top;
								end
								reg_state<=STATE_DONE;
							end
						end
						else if(reg_y>=reg_limit_top)
						begin
							if(reg_y+reg_cliph>reg_limit_bottom+1)
							begin
								reg_cliph<=reg_limit_bottom+1-reg_y;
							end
							else
							begin
								reg_cliph<=reg_cliph;
							end
							reg_state<=STATE_DONE;
						end
						else
						begin
							reg_y<=reg_y;
							reg_state<=STATE_DONE;
						end
					end
					STATE_DONE:
					begin
						reg_state<=reg_state;
					end
					default: 
					begin
						reg_state<=0;
					end
				endcase
			end
		end

		assign o_wire_x=reg_x;
		assign o_wire_y=reg_y;

		assign o_wire_clipx=reg_clipx;
		assign o_wire_clipy=reg_clipy;
		assign o_wire_clipw=reg_clipw;
		assign o_wire_cliph=reg_cliph;
		assign o_wire_done=(reg_state==STATE_DONE);
	endmodule
