
`timescale 1 ns / 1 ps

	`define FIFO_DEPTH 64
	`define DATA_WIDTH 32
	`define ADDRESS_WIDTH 32
	`define ADDRESS_ALIGN 64
	`define RW_CACHE_SIZE 256

	`define VIDEO_DISPLAY_MODE_1280_720   3'b000
	`define VIDEO_DISPLAY_MODE_480_272    3'b001
	`define VIDEO_DISPLAY_MODE_640_480    3'b010
	`define VIDEO_DISPLAY_MODE_800_480    3'b011
	`define VIDEO_DISPLAY_MODE_800_600    3'b100
	`define VIDEO_DISPLAY_MODE_1024_768   3'b101
	`define VIDEO_DISPLAY_MODE_1920_1080  3'b110

	`define DVI_RGB_MODE_ARGB 3'b00
	`define DVI_RGB_MODE_RGBA 3'b01
	`define DVI_RGB_MODE_ABGR 3'b10
	`define DVI_RGB_MODE_BGRA 3'b11

	module clock_50MHZ(
		input  wire i_wire_50MHZ,
	    output wire o_wire_clock_50MHZ,
	    output wire o_wire_enabled
	    );
		reg enabled=0;
		reg [15:0] counter=0;
		always @(posedge i_wire_50MHZ)
		begin
			if (counter<100)
			begin
				counter<=counter+1;
				enabled<=0;
			end
			else
			begin
				counter<=counter;
				enabled<=1;
			end
		end
		assign o_wire_clock_50MHZ = i_wire_50MHZ;
		assign o_wire_enabled = enabled;
	endmodule
    
	module painterengine_gpu_fifo #
		(
			parameter integer PARAM_DATA_WIDTH	= 32,
			parameter integer PARAM_FIFO_DEPTH	= `FIFO_DEPTH //2^6

		)
		(
			input wire   									i_wire_write_clock,
			input wire   									i_wire_read_clock,
			input wire   									i_wire_resetn,

			input wire   									i_wire_write,
			input wire   									i_wire_read,

			input wire [PARAM_DATA_WIDTH-1:0] 				i_wire_data_in,
			output wire [PARAM_DATA_WIDTH-1:0] 				o_wire_data_out,

			output wire   									o_wire_almost_full,
			output wire   									o_wire_full,
			
			output wire   									o_wire_alomost_empty,
			output wire   									o_wire_empty
		);

		function integer clogb2 (input integer bit_depth);              
		begin                                                           
		    for(clogb2=0; bit_depth>0; clogb2=clogb2+1)                   
		      bit_depth = bit_depth >> 1;                                 
		end                                                           
		endfunction  
		//fifo
		reg [PARAM_DATA_WIDTH-1:0] 						reg_fifo[PARAM_FIFO_DEPTH-1:0];

		reg [clogb2(PARAM_FIFO_DEPTH):0]				reg_fifo_write_index;
		reg [clogb2(PARAM_FIFO_DEPTH):0]				reg_fifo_read_index;
		wire [clogb2(PARAM_FIFO_DEPTH):0]				wire_fifo_data_count;
		wire [clogb2(PARAM_FIFO_DEPTH)-1:0]				wire_fifo_true_read_index;
		wire [clogb2(PARAM_FIFO_DEPTH)-1:0]				wire_fifo_true_write_index;

		assign wire_fifo_data_count=reg_fifo_write_index>=reg_fifo_read_index?reg_fifo_write_index-reg_fifo_read_index:(PARAM_FIFO_DEPTH*2)-reg_fifo_read_index+reg_fifo_write_index;
		assign o_wire_full=(wire_fifo_data_count==PARAM_FIFO_DEPTH);
		assign o_wire_almost_full=(wire_fifo_data_count==PARAM_FIFO_DEPTH-1);
		assign o_wire_empty=(wire_fifo_data_count==0);
		assign o_wire_alomost_empty=(wire_fifo_data_count==1);
		assign wire_fifo_true_read_index=reg_fifo_read_index[clogb2(PARAM_FIFO_DEPTH)-2:0];
		assign wire_fifo_true_write_index=reg_fifo_write_index[clogb2(PARAM_FIFO_DEPTH)-2:0];

		assign o_wire_data_out=reg_fifo[wire_fifo_true_read_index];


		always @(posedge i_wire_write_clock or negedge i_wire_resetn)
		begin
			if (!i_wire_resetn)
			begin
				reg_fifo[wire_fifo_true_write_index]<=0;
			end
			else
			begin
				if(i_wire_write)
				begin
					if(wire_fifo_data_count<PARAM_FIFO_DEPTH)
					begin
						reg_fifo[wire_fifo_true_write_index]<=i_wire_data_in;
					end
				end
				else
				begin
					reg_fifo[wire_fifo_true_write_index]<=reg_fifo[wire_fifo_true_write_index];
				end
			end
		end

		always @(posedge i_wire_write_clock or negedge i_wire_resetn)
		begin
			if (!i_wire_resetn)
			begin
				reg_fifo_write_index<=0;
			end
			else
			begin
				if(i_wire_write&&wire_fifo_data_count<PARAM_FIFO_DEPTH)
				begin
					reg_fifo_write_index<=(reg_fifo_write_index+1)&(PARAM_FIFO_DEPTH*2-1);
				end
				else
				begin
					reg_fifo_write_index<=reg_fifo_write_index;
				end
			end
		end

		always @(posedge i_wire_read_clock or negedge i_wire_resetn)
		begin
			if (!i_wire_resetn)
			begin
				reg_fifo_read_index<=0;
			end
			else
			begin
				if(i_wire_read&&wire_fifo_data_count>0)
				begin
					reg_fifo_read_index<=(reg_fifo_read_index+1)&(PARAM_FIFO_DEPTH*2-1);
				end
				else
				begin
					reg_fifo_read_index<=reg_fifo_read_index;
				end
			end
		end
	endmodule

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


	module painterengine_gpu_rotate #
		(
		parameter integer iteration	= 22
		)
		(
  		input 			i_wire_clock,
  		input 	signed 	[31:0] i_wire_x,
		input 	signed 	[31:0] i_wire_y,
  		input 	signed 	[31:0] i_wire_angle,
  		output 	signed  [31:0] o_wire_x,
		output 	signed  [31:0] o_wire_y
		);
  		wire signed 	[31:0] deg_table [0:iteration-1];
                          
  		assign deg_table[0] = 32'd2949120;
		assign deg_table[1] = 32'd1740967;
		assign deg_table[2] = 32'd919879;
		assign deg_table[3] = 32'd466945;
		assign deg_table[4] = 32'd234378;
		assign deg_table[5] = 32'd117303;
		assign deg_table[6] = 32'd58666;
		assign deg_table[7] = 32'd29334;
		assign deg_table[8] = 32'd14667;
		assign deg_table[9] = 32'd7333;
		assign deg_table[10] = 32'd3666;
		assign deg_table[11] = 32'd1833;
		assign deg_table[12] = 32'd916;
		assign deg_table[13] = 32'd458;
		assign deg_table[14] = 32'd229;
		assign deg_table[15] = 32'd114;
		assign deg_table[16] = 32'd57;
		assign deg_table[17] = 32'd28;
		assign deg_table[18] = 32'd14;
		assign deg_table[19] = 32'd7;
		assign deg_table[20] = 32'd3;
		assign deg_table[21] = 32'd1;


  		reg signed [31:0] x [0:iteration-1];
  		reg signed [31:0] y [0:iteration-1];
  		reg signed [31:0] z [0:iteration-1];

		wire [31:0] aligned_x, aligned_y,aligned_angle;

		assign aligned_x=(i_wire_x>>>10)*622;
		assign aligned_y=(i_wire_y>>>10)*622;
		assign aligned_angle=aligned_angle>0?{i_wire_angle[31:16]%360,16'b0}:{(360+(i_wire_angle[31:16]%360)),16'b0};

  		always @(posedge i_wire_clock)
  		begin 
			if ((aligned_angle>>16)<=90) 
			begin
				x[0] <= aligned_x;
        		y[0] <= aligned_y;
        		z[0] <= aligned_angle;
			end	
			else if ((aligned_angle>>16)<=180)
			begin
				x[0] <= -aligned_y;
				y[0] <= aligned_x;
				z[0] <= aligned_angle-(32'd90*65536);
			end
			else if ((aligned_angle>>16)<=270)
			begin
				x[0] <= -aligned_x;
				y[0] <= -aligned_y;
				z[0] <= aligned_angle-(32'd180*65536);
			end
			else
			begin
				x[0] <= aligned_y;
				y[0] <= -aligned_x;
				z[0] <= aligned_angle-(32'd270*65536);
			end
 		 end

 		genvar i;
 		generate
 		for (i=0; i < (iteration-1); i=i+1)
 		begin: xyz
 		  wire z_sign;
 		  wire signed [iteration-1:0] x_shr, y_shr;

 		  //Xn+1 = Xn-dn(2^(-i)Yn)
		  //Yn+1 = Yn+dn(2^(-i)Xn)
		  //Zn+1=Zn-dn(theta)
 		  assign x_shr = x[i] >>> i;
 		  assign y_shr = y[i] >>> i;
 		  assign z_sign = z[i][31];

 		  always @(posedge i_wire_clock)
 		  begin
 		    x[i+1] <= z_sign ? x[i] + y_shr : x[i] - y_shr;
 		    y[i+1] <= z_sign ? y[i] - x_shr : y[i] + x_shr;
 		    z[i+1] <= z_sign ? z[i] + deg_table[i] : z[i] - deg_table[i];
 		  end
 		end
 		endgenerate

  		assign o_wire_x = x[iteration-1];
  		assign o_wire_y = y[iteration-1];
	endmodule


	module painterengine_gpu_dma_reader #
		(
		parameter integer PARAM_DATA_ALIGN	= `ADDRESS_ALIGN,
		parameter integer PARAM_ADDRESS_WIDTH	= `ADDRESS_WIDTH,
		parameter integer PARAM_DATA_WIDTH	= `DATA_WIDTH,
		parameter integer PARAM_CACHE_MAX_SIZE	= `RW_CACHE_SIZE
		)
		(
		//input clk
		input wire   									i_wire_clock,
		input wire   									i_wire_resetn,
		output wire 									o_wire_done,

		input wire [PARAM_ADDRESS_WIDTH-1:0] 			i_wire_address,
		input wire [8:0] 								i_wire_length,
		
		output wire [PARAM_ADDRESS_WIDTH*PARAM_CACHE_MAX_SIZE-1:0] 	o_wire_cache,
		///////////////////////////////////////////////////////////////////////////////////////////
		//AXI full ports
		///////////////////////////////////////////////////////////////////////////////////////////
		output wire 									o_wire_M_AXI_ARID,
		output wire [PARAM_ADDRESS_WIDTH-1:0]			o_wire_M_AXI_ARADDR,
		output wire [7 : 0] 							o_wire_M_AXI_ARLEN,
		output wire [2 : 0] 							o_wire_M_AXI_ARSIZE,
		output wire [1 : 0] 							o_wire_M_AXI_ARBURST,
		output wire  									o_wire_M_AXI_ARLOCK,
		output wire [3 : 0] 							o_wire_M_AXI_ARCACHE,
		output wire [2 : 0] 							o_wire_M_AXI_ARPROT,
		output wire [3 : 0] 							o_wire_M_AXI_ARQOS,
		output wire  									o_wire_M_AXI_ARVALID,
		input wire   									i_wire_M_AXI_ARREADY,

		input wire  									i_wire_M_AXI_RID,
		input wire [PARAM_DATA_WIDTH-1 : 0] 			i_wire_M_AXI_RDATA,
		input wire [1 : 0] 								i_wire_M_AXI_RRESP,
		input wire  									i_wire_M_AXI_RLAST,
		input wire  									i_wire_M_AXI_RVALID,
		output wire  									o_wire_M_AXI_RREADY
		);
		reg [PARAM_DATA_WIDTH-1:0] 						reg_cache[PARAM_CACHE_MAX_SIZE-1:0];
		genvar i;
		generate
		for ( i = 0; i < PARAM_CACHE_MAX_SIZE; i = i+1)
		begin:cache_assign
			assign o_wire_cache[32*i +: 32] = reg_cache[i] ; 
  		end
		endgenerate

		reg [PARAM_ADDRESS_WIDTH-1 : 0] 				reg_address=0;
		reg [8:0] 										reg_length;
		reg [7:0] 										reg_offset;
		reg [7:0] 										reg_burst_counter;
		reg [15:0] 										reg_timeout_error;
		reg [2:0]										reg_state=0;

		reg [PARAM_ADDRESS_WIDTH-1 : 0] 				reg_axi_araddr;
		reg  											reg_axi_arvalid;
		reg [7:0] 										reg_axi_burstlen;
		reg  											reg_axi_rready;
	
		//Read Address (AR)
		assign o_wire_M_AXI_ARADDR	= reg_axi_araddr;
		assign o_wire_M_AXI_ARLEN	= reg_axi_burstlen - 1;
		assign o_wire_M_AXI_ARVALID	= reg_axi_arvalid;
		assign o_wire_M_AXI_RREADY	= reg_axi_rready;

		assign o_wire_M_AXI_ARID	= 'b0;
		assign o_wire_M_AXI_ARSIZE	= 3'b10;
		assign o_wire_M_AXI_ARBURST	= 2'b01;
		assign o_wire_M_AXI_ARLOCK	= 1'b0;
		assign o_wire_M_AXI_ARCACHE	= 4'b0010;
		assign o_wire_M_AXI_ARPROT	= 3'h0;
		assign o_wire_M_AXI_ARQOS	= 4'h0;
		

		`define fsm_state_idle 3'b000
		`define fsm_state_address_write 3'b001
		`define fsm_state_data_read 3'b010
		`define fsm_state_done 3'b100
		`define fsm_state_error 3'b111

		wire [15:0] wire_first_burst_aligned_len;
		assign wire_first_burst_aligned_len = PARAM_DATA_ALIGN-((i_wire_address>>2)&(PARAM_DATA_ALIGN-1));

		task task_idle;
	    if(i_wire_resetn)
		begin
			if((i_wire_address%4)||i_wire_length==0)
			begin
				reg_timeout_error<=0;
				reg_offset<=0;
				reg_burst_counter<=0;
				reg_state<=`fsm_state_error;
				reg_axi_araddr<=0;
				reg_axi_arvalid<=0;
				reg_axi_burstlen<=0;
			end
			else
			begin
				reg_timeout_error<=0;
				reg_address<=i_wire_address;
				reg_length<=i_wire_length;
				reg_offset<=0;
				reg_burst_counter<=0;
				reg_axi_rready<=0;
				reg_state<=`fsm_state_address_write;
				//first axi address
				reg_axi_araddr<=i_wire_address;
				reg_axi_burstlen<=wire_first_burst_aligned_len>i_wire_length?i_wire_length:wire_first_burst_aligned_len;
				reg_axi_arvalid<=1;
			end
		end
		else
		begin
			reg_timeout_error<=0;
			reg_state<=`fsm_state_idle;
			reg_axi_araddr<=0;
			reg_axi_arvalid<=0;
			reg_axi_burstlen<=0;
			reg_offset<=0;
			reg_burst_counter<=0;
			reg_axi_rready<=0;
			reg_address<=0;
			reg_length<=0;
		end
		endtask
	
		//write address fsm
		wire  [15:0] wire_reserved_len;
		wire  [15:0] wire_reserved_len2;
		wire  [15:0] wire_burst_aligned_len;
		wire  [15:0] wire_burst_aligned_len2;
		assign wire_reserved_len = reg_length-reg_offset;
		assign wire_reserved_len2 = reg_length-reg_offset-reg_axi_burstlen;
		assign wire_burst_aligned_len = PARAM_DATA_ALIGN-(((reg_address>>2)+reg_offset)&(PARAM_DATA_ALIGN-1));
		assign wire_burst_aligned_len2 = PARAM_DATA_ALIGN-(((reg_address>>2)+reg_offset+reg_axi_burstlen)&(PARAM_DATA_ALIGN-1));
		task task_write_address;
			if(reg_axi_arvalid&&i_wire_M_AXI_ARREADY)
			begin
				reg_axi_araddr<=0;
				reg_axi_arvalid<=0;
				reg_axi_burstlen<=reg_axi_burstlen;

				//first axi data burst
				reg_axi_rready<=1;
				reg_burst_counter<=0;

				//fsm
				reg_timeout_error<=0;
				reg_state<=`fsm_state_data_read;
			end
			else
			begin
				//next axi address
				reg_axi_araddr<=reg_address+reg_offset*(PARAM_DATA_WIDTH/8);
				reg_axi_arvalid<=1;
				reg_axi_burstlen<=wire_burst_aligned_len>wire_reserved_len?wire_reserved_len:wire_burst_aligned_len;
				reg_burst_counter<=0;

				//error
				reg_state<=reg_state;
				reg_timeout_error<=reg_timeout_error+1;
			end
		endtask

		//read data fsm
		task task_read_one_data;
			if(reg_burst_counter>=reg_axi_burstlen-1)
			begin
				if(reg_offset+reg_axi_burstlen>=reg_length)
				begin
					reg_timeout_error<=0;
					reg_cache[reg_offset+reg_burst_counter]<=i_wire_M_AXI_RDATA;
					reg_offset<=reg_offset+reg_axi_burstlen;
					reg_state<=`fsm_state_done;
				end
				else
				begin
					reg_timeout_error<=0;
					//last axi data
					reg_cache[reg_offset+reg_burst_counter]<=i_wire_M_AXI_RDATA;
					reg_offset<=reg_offset+reg_axi_burstlen;
					reg_state<=`fsm_state_address_write;
					//first axi address
					reg_axi_araddr<=reg_address+(reg_offset+reg_axi_burstlen)*(PARAM_DATA_WIDTH/8);
					reg_axi_arvalid<=1;
					reg_axi_burstlen<=wire_burst_aligned_len2>wire_reserved_len2?wire_reserved_len2:wire_burst_aligned_len2;
					reg_burst_counter<=0;
				end
			end
			else
			begin
				reg_cache[reg_offset+reg_burst_counter]<=i_wire_M_AXI_RDATA;
				reg_burst_counter<=reg_burst_counter+1;
				reg_axi_rready<=1;
				reg_timeout_error<=0;
				reg_state<=reg_state;
			end
		endtask


		task task_read_data;
			if(i_wire_M_AXI_RVALID&&reg_axi_rready)
			begin
				if (i_wire_M_AXI_RLAST) 
				begin
					if (reg_burst_counter!=reg_axi_burstlen-1) 
					begin
						//error
						reg_state<=`fsm_state_error;
					end
					else
					begin
						task_read_one_data;
					end
				end
				else
				begin
					task_read_one_data;
				end
			end
			else
			begin
				reg_axi_rready<=1;
				reg_state<=reg_state;
				reg_timeout_error<=reg_timeout_error+1;
			end
		endtask

		assign o_wire_done=(reg_state==`fsm_state_done);

		task fsm_process;
				case (reg_state)
					`fsm_state_idle:
					begin
						task_idle;
					end
					`fsm_state_address_write:
					begin
						task_write_address;
					end
					`fsm_state_data_read:
					begin
						task_read_data;
					end
					`fsm_state_done:
					begin
						reg_timeout_error<=0;
						reg_state<=reg_state;
					end
					`fsm_state_error:
					begin
						reg_timeout_error<=0;
						reg_state<=reg_state;
					end
					default:
					begin
						reg_timeout_error<=0;
						reg_state<=reg_state;
					end
				endcase
		endtask

		integer j;
		always @(posedge i_wire_clock or negedge i_wire_resetn)
		begin
			if (!i_wire_resetn) 
			begin
				reg_state<=`fsm_state_idle;
				reg_address<=0;
				reg_length<=0;
				reg_offset<=0;
				reg_burst_counter<=0;
				reg_timeout_error<=0;
				reg_axi_araddr<=0;
				reg_axi_arvalid<=0;
				reg_axi_burstlen<=0;
				reg_axi_rready<=0;
				for ( j = 0; j < PARAM_CACHE_MAX_SIZE; j = j+1)
				begin:cache_clear
					reg_cache[j]<=0;
				end
			end
			else
			begin
				if (reg_state!=`fsm_state_error)
				begin
					if(reg_timeout_error==65535)
					begin
						reg_state<=`fsm_state_error;
					end
					else
					begin
						fsm_process;
					end
				end
				else
				begin
					reg_state<=`fsm_state_error;
				end
			end
		end
	endmodule

	module painterengine_gpu_dma_writer #
		(
		parameter integer PARAM_DATA_ALIGN	= `ADDRESS_ALIGN,
		parameter integer PARAM_ADDRESS_WIDTH	= `ADDRESS_WIDTH,
		parameter integer PARAM_DATA_WIDTH	= `DATA_WIDTH,
		parameter integer PARAM_CACHE_MAX_SIZE	= `RW_CACHE_SIZE
		)
		(
		//input clk
		input wire   									i_wire_clock,
		input wire   									i_wire_resetn,
		output wire 									o_wire_done,

		input wire [PARAM_ADDRESS_WIDTH-1:0] 			i_wire_address,
		input wire [8:0] 								i_wire_length,
		
		input wire [PARAM_DATA_WIDTH*PARAM_CACHE_MAX_SIZE-1:0] 	i_wire_cache,
		///////////////////////////////////////////////////////////////////////////////////////////
		//AXI full ports
		///////////////////////////////////////////////////////////////////////////////////////////
		output wire [0 : 0]								o_wire_M_AXI_AWID,//assign to 0
		output wire [PARAM_ADDRESS_WIDTH-1 : 0] 		o_wire_M_AXI_AWADDR,//assign to axi_awaddr
		output wire [7 : 0] 							o_wire_M_AXI_AWLEN,//assign to axi_burstlen-1
		output wire [2 : 0] 							o_wire_M_AXI_AWSIZE,//assign to 32bit-'b101
		output wire [1 : 0] 							o_wire_M_AXI_AWBURST,//assign to 2'b01
		output wire  									o_wire_M_AXI_AWLOCK,//assign to 1'b0
		output wire [3 : 0] 							o_wire_M_AXI_AWCACHE,//assign to 4'b0010
		output wire [2 : 0] 							o_wire_M_AXI_AWPROT,//assign to 3'h0
		output wire [3 : 0] 							o_wire_M_AXI_AWQOS,//assign to 4'h0
		output wire  									o_wire_M_AXI_AWVALID,//assign to axi_awvalid
		input wire 										i_wire_M_AXI_AWREADY,
		output wire [PARAM_DATA_WIDTH-1 : 0] 			o_wire_M_AXI_WDATA,//assign to axi_wdata
		output wire [PARAM_DATA_WIDTH/8-1 : 0] 			o_wire_M_AXI_WSTRB,//assign to {(PIXEL/8){1'b1}}
		output wire  									o_wire_M_AXI_WLAST,//assign to axi_wlast
		output wire 		 							o_wire_M_AXI_WVALID,//assign to axi_wvalid
		input wire  									i_wire_M_AXI_WREADY,

		input wire [0 : 0]								i_wire_M_AXI_BID,//assign to 0
		input wire [1 : 0] 								i_wire_M_AXI_BRESP,//assign to 2'b00
		input wire  									i_wire_M_AXI_BVALID,
		output wire 									o_wire_M_AXI_BREADY
		);
		reg [PARAM_DATA_WIDTH-1:0] 		reg_cache[PARAM_CACHE_MAX_SIZE-1:0];
		reg [PARAM_ADDRESS_WIDTH-1 : 0] reg_address=0;
		reg [8:0] 						reg_length;
		reg [7:0] 						reg_offset;
		reg [7:0] 						reg_burst_counter;
		reg [2:0]						reg_state=0;
		reg [15:0]						reg_timeout_error;
		integer i;
		always @(*) 
		begin	
			for ( i = 0; i < PARAM_CACHE_MAX_SIZE; i = i+1) 
			begin:cache_assign
    			reg_cache[i]= i_wire_cache[32*i +: 32] ; 
  			end 
		end
		////////////////////////////////////////////////////////////
		//axi registers////////////////////////////////////////////
		///////////////////////////////////////////////////////////

		reg [PARAM_ADDRESS_WIDTH-1 : 0] 					reg_axi_awaddr;
		reg 												reg_axi_awvalid;
		reg [PARAM_ADDRESS_WIDTH-1 : 0] 					reg_axi_wdata;
		reg  												reg_axi_wlast;
		reg  												reg_axi_wvalid;
		reg  												reg_axi_bready;
		reg [7:0]											reg_axi_burstlen;


		//write address (AW)
		assign o_wire_M_AXI_AWADDR	= reg_axi_awaddr;
		assign o_wire_M_AXI_AWLEN	= reg_axi_burstlen - 1;
		assign o_wire_M_AXI_AWVALID	= reg_axi_awvalid;

		assign o_wire_M_AXI_AWID	= 'b0;//transaction ID
		assign o_wire_M_AXI_AWSIZE	= 3'b10;//32bits
		assign o_wire_M_AXI_AWBURST	= 2'b01;//incrementing mode
		assign o_wire_M_AXI_AWLOCK	= 1'b0;//normal access
		assign o_wire_M_AXI_AWCACHE	= 4'b0010;//bufferable, modifiable, no allocate, write through
		assign o_wire_M_AXI_AWPROT	= 3'h0;//non-secure, non-privileged, data access
		assign o_wire_M_AXI_AWQOS	= 4'h0;//quality of service

		//write data (DW)
		assign o_wire_M_AXI_WDATA		= reg_axi_wdata;
		assign o_wire_M_AXI_WSTRB		= 4'b1111;
		assign o_wire_M_AXI_WLAST		= reg_axi_wlast;
		assign o_wire_M_AXI_WUSER		= 'b0;
		assign o_wire_M_AXI_WVALID		= reg_axi_wvalid;
		assign o_wire_M_AXI_BREADY		= reg_axi_bready;


		`define fsm_state_idle 3'b000
		`define fsm_state_address_write 3'b001
		`define fsm_state_data_write 3'b010
		`define fsm_state_data_wait_resp 3'b011
		`define fsm_state_done 3'b100
		`define fsm_state_error 3'b111

		wire [15:0] wire_first_burst_aligned_len;
		assign wire_first_burst_aligned_len = PARAM_DATA_ALIGN-((i_wire_address>>2)&(PARAM_DATA_ALIGN-1));

		task task_idle;
		    if(i_wire_resetn)
			begin
				if((i_wire_address%4)||i_wire_length==0)
				begin
					reg_timeout_error<=0;
					reg_offset<=0;
					reg_burst_counter<=0;
					reg_state<=`fsm_state_error;
					reg_axi_awaddr<=0;
					reg_axi_awvalid<=0;
					reg_axi_burstlen<=0;
				end
				else
				begin
					reg_timeout_error<=0;
					reg_address<=i_wire_address;
					reg_length<=i_wire_length;
					reg_offset<=0;
					reg_burst_counter<=0;
					reg_axi_bready<=0;
					reg_state<=`fsm_state_address_write;
					//first axi address
					reg_axi_awaddr<=i_wire_address;
					reg_axi_burstlen<=wire_first_burst_aligned_len>i_wire_length?i_wire_length:wire_first_burst_aligned_len;
					reg_axi_awvalid<=1;
				end
			end
			else
			begin
				reg_timeout_error<=0;
				reg_state<=`fsm_state_idle;
				reg_axi_awaddr<=0;
				reg_axi_awvalid<=0;
				reg_axi_burstlen<=0;
				reg_offset<=0;
				reg_burst_counter<=0;
				reg_axi_bready<=0;
				reg_address<=0;
				reg_length<=0;
			end
		endtask

		//write address fsm
		wire  [15:0] wire_reserved_len;
		wire  [15:0] wire_burst_aligned_len;
		assign wire_reserved_len = reg_length-reg_offset;
		assign wire_burst_aligned_len = PARAM_DATA_ALIGN-(((reg_address>>2)+reg_offset)&(PARAM_DATA_ALIGN-1));

		task task_write_address;
			if(reg_axi_awvalid&&i_wire_M_AXI_AWREADY)
			begin
				reg_axi_awaddr<=0;
				reg_axi_awvalid<=0;
				reg_axi_burstlen<=reg_axi_burstlen;

				//first axi data burst
				reg_axi_wdata<=reg_cache[reg_offset+reg_burst_counter];
				reg_axi_wlast<=(reg_axi_burstlen==1);
				reg_axi_wvalid<=1;
				reg_burst_counter<=1;

				//fsm
				reg_timeout_error<=0;
				reg_state<=`fsm_state_data_write;
			end
			else
			begin
				//next axi address
				reg_axi_awaddr<=reg_address+reg_offset*(PARAM_DATA_WIDTH/8);
				reg_axi_awvalid<=1;
				reg_axi_burstlen<=wire_burst_aligned_len>wire_reserved_len?wire_reserved_len:wire_burst_aligned_len;
				reg_burst_counter<=0;

				//error
				reg_state<=reg_state;
				reg_timeout_error<=reg_timeout_error+1;
			end
		endtask

		//write data fsm
		task task_write_data;
			if (reg_axi_wvalid && i_wire_M_AXI_WREADY) 
			begin
				if (reg_burst_counter>=reg_axi_burstlen) 
				begin
					//end of data
					reg_axi_wdata<=0;
					reg_axi_wlast<=0;
					reg_axi_wvalid<=0;
					reg_timeout_error<=0;
					reg_offset<=reg_offset+reg_axi_burstlen;
					reg_burst_counter<=0;

					//wait response
					reg_axi_bready<=1;
					reg_state<=`fsm_state_data_wait_resp;
				end
				else
				begin
					//next axi data burst
					reg_axi_wdata<=reg_cache[reg_offset+reg_burst_counter];
					reg_axi_wlast<=(reg_burst_counter==reg_axi_burstlen-1)?1:0;
					reg_axi_wvalid<=1;
					reg_burst_counter<=reg_burst_counter+1;
					reg_timeout_error<=0;
					reg_axi_bready<=0;
				end
			end
			else
			begin
				//keep going
				reg_timeout_error<=reg_timeout_error+1;
				reg_state<=reg_state;
				reg_axi_wdata<=reg_axi_wdata;
				reg_axi_wlast<=reg_axi_wlast;
				reg_axi_wvalid<=reg_axi_wvalid;
				reg_axi_bready<=reg_axi_bready;
				reg_burst_counter<=reg_burst_counter;
			end
		endtask


		//wait response fsm
		task task_wait_resp;
			if (i_wire_M_AXI_BVALID) 
			begin
				reg_timeout_error<=0;
				if (i_wire_M_AXI_BRESP<=2'b01) 
				begin
					if (reg_offset>=reg_length) 
					begin
						reg_axi_bready<=0;
						reg_state<=`fsm_state_done;
					end
					else
					begin
						reg_axi_bready<=0;
						reg_state<=`fsm_state_address_write;
						//next axi address inmediately
						reg_axi_awaddr<=reg_address+reg_offset*(PARAM_DATA_WIDTH/8);
						reg_axi_awvalid<=1;
						reg_axi_burstlen<=wire_burst_aligned_len>wire_reserved_len?wire_reserved_len:wire_burst_aligned_len;
						reg_burst_counter<=0;
					end
				end
				else
				begin
					reg_axi_bready<=0;
					reg_state<=`fsm_state_error;
				end
			end
			else
			begin
				reg_timeout_error<=reg_timeout_error+1;
				reg_state<=reg_state;
			end
		endtask

		//fsm

		assign o_wire_done=(reg_state==`fsm_state_done);

		task fsm_process;
				case (reg_state)
					`fsm_state_idle:
					begin
						task_idle;
					end
					`fsm_state_address_write:
					begin
						task_write_address;
					end
					`fsm_state_data_write:
					begin
						task_write_data;
					end
					`fsm_state_data_wait_resp:
					begin
						task_wait_resp;
					end
					`fsm_state_done:
					begin
						reg_timeout_error<=0;
						reg_state<=reg_state;
					end
					`fsm_state_error:
					begin
						reg_timeout_error<=0;
						reg_state<=reg_state;
					end
					default:
					begin
						reg_timeout_error<=0;
						reg_state<=reg_state;
					end
				endcase
		endtask

		integer j;
		always @(posedge i_wire_clock or negedge i_wire_resetn)
		begin
			if (!i_wire_resetn) 
			begin
				reg_state<=`fsm_state_idle;
				reg_address<=0;
				reg_length<=0;
				reg_offset<=0;
				reg_burst_counter<=0;
				reg_axi_awaddr<=0;
				reg_axi_awvalid<=0;
				reg_axi_burstlen<=0;
				reg_axi_wdata<=0;
				reg_axi_wlast<=0;
				reg_axi_wvalid<=0;
				reg_axi_bready<=0;
				reg_timeout_error<=0;
			end
			else
			begin
				if (reg_state!=`fsm_state_error)
				begin
					if(reg_timeout_error==65535)
					begin
						reg_state<=`fsm_state_error;
					end
					else
					begin
						fsm_process;
					end
				end
				else
				begin
					reg_state<=`fsm_state_error;
				end
			end
		end
	endmodule

	module painterengine_gpu_dvi #
		(
		parameter integer PARAM_DATA_WIDTH	= `DATA_WIDTH
		)
		(
		input wire             										i_wire_pixel_clock,   //pixel clock
		input wire   												i_wire_resetn,

		output wire 												o_wire_next_rgb,
		input wire[31:0]											i_wire_rgba,
		input wire[2:0]												i_wire_rgba_mode,
		
		input wire[2:0]             								i_wire_display_mode,  
		input wire[15:0]             								i_wire_clip_width,
		input wire[15:0]             								i_wire_clip_height,
		//rgb
		
		output  wire              									o_wire_hs,            //horizontal synchronization
		output  wire             									o_wire_vs,            //vertical synchronization
		output  wire              									o_wire_de,            //video valid
		output  wire [23:0]           								o_wire_rgb,
		output	wire												o_wire_done
		);
		parameter VIDEO_1280_720_H_ACTIVE = 16'd1280;           //horizontal active time (pixels)
		parameter VIDEO_1280_720_H_FP = 16'd110;                //horizontal front porch (pixels)
		parameter VIDEO_1280_720_H_SYNC = 16'd40;               //horizontal sync time(pixels)
		parameter VIDEO_1280_720_H_BP = 16'd220;                //horizontal back porch (pixels)
		parameter VIDEO_1280_720_V_ACTIVE = 16'd720;            //vertical active Time (lines)
		parameter VIDEO_1280_720_V_FP  = 16'd5;                 //vertical front porch (lines)
		parameter VIDEO_1280_720_V_SYNC  = 16'd5;               //vertical sync time (lines)
		parameter VIDEO_1280_720_V_BP  = 16'd20;                //vertical back porch (lines)
		parameter VIDEO_1280_720_HS_POL = 1'b1;                 //horizontal sync polarity, 1 : POSITIVE,0 : NEGATIVE;
		parameter VIDEO_1280_720_VS_POL = 1'b1;                 //vertical sync polarity, 1 : POSITIVE,0 : NEGATIVE;

		parameter VIDEO_480_272_H_ACTIVE = 16'd480; 
		parameter VIDEO_480_272_H_FP = 16'd2;       
		parameter VIDEO_480_272_H_SYNC = 16'd41;    
		parameter VIDEO_480_272_H_BP = 16'd2;       
		parameter VIDEO_480_272_V_ACTIVE = 16'd272; 
		parameter VIDEO_480_272_V_FP  = 16'd2;     
		parameter VIDEO_480_272_V_SYNC  = 16'd10;   
		parameter VIDEO_480_272_V_BP  = 16'd2;     
		parameter VIDEO_480_272_HS_POL = 1'b0;
		parameter VIDEO_480_272_VS_POL = 1'b0;

		parameter VIDEO_640_480_H_ACTIVE = 16'd640; 
		parameter VIDEO_640_480_H_FP = 16'd16;      
		parameter VIDEO_640_480_H_SYNC = 16'd96;    
		parameter VIDEO_640_480_H_BP = 16'd48;      
		parameter VIDEO_640_480_V_ACTIVE = 16'd480; 
		parameter VIDEO_640_480_V_FP  = 16'd10;    
		parameter VIDEO_640_480_V_SYNC  = 16'd2;    
		parameter VIDEO_640_480_V_BP  = 16'd33;    
		parameter VIDEO_640_480_HS_POL = 1'b0;
		parameter VIDEO_640_480_VS_POL = 1'b0;

		parameter VIDEO_800_480_H_ACTIVE = 16'd800; 
		parameter VIDEO_800_480_H_FP = 16'd40;      
		parameter VIDEO_800_480_H_SYNC = 16'd128;   
		parameter VIDEO_800_480_H_BP = 16'd88;      
		parameter VIDEO_800_480_V_ACTIVE = 16'd480; 
		parameter VIDEO_800_480_V_FP  = 16'd1;     
		parameter VIDEO_800_480_V_SYNC  = 16'd3;    
		parameter VIDEO_800_480_V_BP  = 16'd21;    
		parameter VIDEO_800_480_HS_POL = 1'b0;
		parameter VIDEO_800_480_VS_POL = 1'b0;

		parameter VIDEO_800_600_H_ACTIVE = 16'd800; 
		parameter VIDEO_800_600_H_FP = 16'd40;      
		parameter VIDEO_800_600_H_SYNC = 16'd128;   
		parameter VIDEO_800_600_H_BP = 16'd88;      
		parameter VIDEO_800_600_V_ACTIVE = 16'd600; 
		parameter VIDEO_800_600_V_FP  = 16'd1;     
		parameter VIDEO_800_600_V_SYNC  = 16'd4;    
		parameter VIDEO_800_600_V_BP  = 16'd23;    
		parameter VIDEO_800_600_HS_POL = 1'b1;
		parameter VIDEO_800_600_VS_POL = 1'b1;

		parameter VIDEO_1024_768_H_ACTIVE = 16'd1024;
		parameter VIDEO_1024_768_H_FP = 16'd24;      
		parameter VIDEO_1024_768_H_SYNC = 16'd136;   
		parameter VIDEO_1024_768_H_BP = 16'd160;     
		parameter VIDEO_1024_768_V_ACTIVE = 16'd768; 
		parameter VIDEO_1024_768_V_FP  = 16'd3;      
		parameter VIDEO_1024_768_V_SYNC  = 16'd6;    
		parameter VIDEO_1024_768_V_BP  = 16'd29;     
		parameter VIDEO_1024_768__HS_POL = 1'b0;
		parameter VIDEO_1024_768__VS_POL = 1'b0;

		parameter VIDEO_1920_1080_H_ACTIVE = 16'd1920;
		parameter VIDEO_1920_1080_H_FP = 16'd88;
		parameter VIDEO_1920_1080_H_SYNC = 16'd44;
		parameter VIDEO_1920_1080_H_BP = 16'd148; 
		parameter VIDEO_1920_1080_V_ACTIVE = 16'd1080;
		parameter VIDEO_1920_1080_V_FP  = 16'd4;
		parameter VIDEO_1920_1080_V_SYNC  = 16'd5;
		parameter VIDEO_1920_1080_V_BP  = 16'd36;
		parameter VIDEO_1920_1080_HS_POL = 1'b1;
		parameter VIDEO_1920_1080_VS_POL = 1'b1;

		//dvi registers
		reg [15:0] 				H_ACTIVE; 
		reg [15:0] 				H_FP;      
		reg [15:0] 				H_SYNC;   
		reg [15:0] 				H_BP;      
		reg [15:0] 				V_ACTIVE; 
		reg [15:0] 				V_FP;     
		reg [15:0] 				V_SYNC;    
		reg [15:0] 				V_BP;    
		reg  					HS_POL;
		reg  					VS_POL;

		wire [15:0] 			H_TOTAL;
		wire [15:0] 			V_TOTAL;

		always @(*)
		begin
			case(i_wire_display_mode)
				`VIDEO_DISPLAY_MODE_1280_720:
				begin
					H_ACTIVE = VIDEO_1280_720_H_ACTIVE;
					H_FP = VIDEO_1280_720_H_FP;
					H_SYNC = VIDEO_1280_720_H_SYNC;
					H_BP = VIDEO_1280_720_H_BP;
					V_ACTIVE = VIDEO_1280_720_V_ACTIVE;
					V_FP = VIDEO_1280_720_V_FP;
					V_SYNC = VIDEO_1280_720_V_SYNC;
					V_BP = VIDEO_1280_720_V_BP;
					HS_POL = VIDEO_1280_720_HS_POL;
					VS_POL = VIDEO_1280_720_VS_POL;
				end
				`VIDEO_DISPLAY_MODE_480_272:
				begin
					H_ACTIVE = VIDEO_480_272_H_ACTIVE;
					H_FP = VIDEO_480_272_H_FP;
					H_SYNC = VIDEO_480_272_H_SYNC;
					H_BP = VIDEO_480_272_H_BP;
					V_ACTIVE = VIDEO_480_272_V_ACTIVE;
					V_FP = VIDEO_480_272_V_FP;
					V_SYNC = VIDEO_480_272_V_SYNC;
					V_BP = VIDEO_480_272_V_BP;
					HS_POL = VIDEO_480_272_HS_POL;
					VS_POL = VIDEO_480_272_VS_POL;
				end
				`VIDEO_DISPLAY_MODE_640_480:
				begin
					H_ACTIVE = VIDEO_640_480_H_ACTIVE;
					H_FP = VIDEO_640_480_H_FP;
					H_SYNC = VIDEO_640_480_H_SYNC;
					H_BP = VIDEO_640_480_H_BP;
					V_ACTIVE = VIDEO_640_480_V_ACTIVE;
					V_FP = VIDEO_640_480_V_FP;
					V_SYNC = VIDEO_640_480_V_SYNC;
					V_BP = VIDEO_640_480_V_BP;
					HS_POL = VIDEO_640_480_HS_POL;
					VS_POL = VIDEO_640_480_VS_POL;

				end
				`VIDEO_DISPLAY_MODE_800_480:
				begin
					H_ACTIVE = VIDEO_800_480_H_ACTIVE;
					H_FP = VIDEO_800_480_H_FP;
					H_SYNC = VIDEO_800_480_H_SYNC;
					H_BP = VIDEO_800_480_H_BP;
					V_ACTIVE = VIDEO_800_480_V_ACTIVE;
					V_FP = VIDEO_800_480_V_FP;
					V_SYNC = VIDEO_800_480_V_SYNC;
					V_BP = VIDEO_800_480_V_BP;
					HS_POL = VIDEO_800_480_HS_POL;
					VS_POL = VIDEO_800_480_VS_POL;

				end
				`VIDEO_DISPLAY_MODE_800_600:
				begin
					H_ACTIVE = VIDEO_800_600_H_ACTIVE;
					H_FP = VIDEO_800_600_H_FP;
					H_SYNC = VIDEO_800_600_H_SYNC;
					H_BP = VIDEO_800_600_H_BP;
					V_ACTIVE = VIDEO_800_600_V_ACTIVE;
					V_FP = VIDEO_800_600_V_FP;
					V_SYNC = VIDEO_800_600_V_SYNC;
					V_BP = VIDEO_800_600_V_BP;
					HS_POL = VIDEO_800_600_HS_POL;
					VS_POL = VIDEO_800_600_VS_POL;

				end
				`VIDEO_DISPLAY_MODE_1024_768:
				begin
					H_ACTIVE = VIDEO_1024_768_H_ACTIVE;
					H_FP = VIDEO_1024_768_H_FP;
					H_SYNC = VIDEO_1024_768_H_SYNC;
					H_BP = VIDEO_1024_768_H_BP;
					V_ACTIVE = VIDEO_1024_768_V_ACTIVE;
					V_FP = VIDEO_1024_768_V_FP;
					V_SYNC = VIDEO_1024_768_V_SYNC;
					V_BP = VIDEO_1024_768_V_BP;
					HS_POL = VIDEO_1024_768__HS_POL;
					VS_POL = VIDEO_1024_768__VS_POL;

				end
				`VIDEO_DISPLAY_MODE_1920_1080:
				begin
					H_ACTIVE = VIDEO_1920_1080_H_ACTIVE;
					H_FP = VIDEO_1920_1080_H_FP;
					H_SYNC = VIDEO_1920_1080_H_SYNC;
					H_BP = VIDEO_1920_1080_H_BP;
					V_ACTIVE = VIDEO_1920_1080_V_ACTIVE;
					V_FP = VIDEO_1920_1080_V_FP;
					V_SYNC = VIDEO_1920_1080_V_SYNC;
					V_BP = VIDEO_1920_1080_V_BP;
					HS_POL = VIDEO_1920_1080_HS_POL;
					VS_POL = VIDEO_1920_1080_VS_POL;

				end
				default:
				begin
					H_ACTIVE = 16'd0;
					H_FP = 16'd0;
					H_SYNC = 16'd0;
					H_BP = 16'd0;
					V_ACTIVE = 16'd0;
					V_FP = 16'd0;
					V_SYNC = 16'd0;
					V_BP = 16'd0;
					HS_POL = 1'b0;
					VS_POL = 1'b0;

				end
			endcase
		end

		assign H_TOTAL = H_ACTIVE + H_FP + H_SYNC + H_BP;//horizontal total time (pixels)
		assign V_TOTAL = V_ACTIVE + V_FP + V_SYNC + V_BP;//vertical total time (lines)
		
		reg 		hs_reg;                      //horizontal sync register
		reg 		vs_reg;                      //vertical sync register
		reg 		hs_reg_d0;                   //delay 1 clock of 'hs_reg'
		reg			vs_reg_d0;                   //delay 1 clock of 'vs_reg'
		reg[11:0] 	h_cnt;                 		 //horizontal counter
		reg[11:0] 	v_cnt;                 		 //vertical counter
		reg[11:0] 	active_x;              		 //video x position 
		reg[11:0] 	active_y;              		 //video y position 

		reg 		rgb_next;                    
		reg[7:0] 	rgb_r_reg;              	 //video red data register
		reg[7:0] 	rgb_g_reg;                   //video green data register
		reg[7:0] 	rgb_b_reg;                   //video blue data register
		reg 		h_active;                    //horizontal video active
		reg 		v_active;                    //vertical video active
		wire 		video_active;               //video active(horizontal active and vertical active)
		reg 		video_active_d0;             //delay 1 clock of video_active
		reg 		reg_done;
		assign 		o_wire_done = reg_done;
		assign 		video_active = h_active & v_active;
		
		assign 		o_wire_next_rgb = rgb_next;
		assign 		o_wire_rgb = {rgb_r_reg,rgb_g_reg,rgb_b_reg};
		assign 		o_wire_hs = hs_reg_d0;
		assign 		o_wire_vs = vs_reg_d0;
		assign 		o_wire_de = video_active_d0;

		always@(posedge i_wire_pixel_clock or negedge i_wire_resetn)
		begin
			if(!i_wire_resetn)
				begin
					reg_done <= 1'b0;
				end
			else
				begin
					if(v_cnt == V_TOTAL - 1&&reg_done==0)
						reg_done <= 1'b1;
					else
						reg_done <= reg_done;
				end
		end


		always@(posedge i_wire_pixel_clock or negedge i_wire_resetn)
		begin
			if(!i_wire_resetn)
				begin
					hs_reg_d0 <= 1'b0;
					vs_reg_d0 <= 1'b0;
					video_active_d0 <= 1'b0;
				end
			else
				begin
					hs_reg_d0 <= hs_reg;
					vs_reg_d0 <= vs_reg;
					video_active_d0 <= video_active;
				end
		end

		always@(posedge i_wire_pixel_clock or negedge i_wire_resetn)
		begin
			if(!i_wire_resetn||reg_done)
				h_cnt <= 12'd0;
			else if(h_cnt == H_TOTAL - 1)//horizontal counter maximum value
				h_cnt <= 12'd0;
			else
				h_cnt <= h_cnt + 12'd1;
		end

		always@(posedge i_wire_pixel_clock or negedge i_wire_resetn)
		begin
			if(!i_wire_resetn||reg_done)
				active_x <= 12'd0;
			else if(h_cnt >= H_FP + H_SYNC + H_BP - 1)//horizontal video active
				active_x <= h_cnt - (H_FP[11:0] + H_SYNC[11:0] + H_BP[11:0] - 12'd1);
			else
				active_x <= active_x;
		end

		always@(posedge i_wire_pixel_clock or negedge i_wire_resetn)
		begin
			if(!i_wire_resetn||reg_done)
				active_y <= 12'd0;
			else if(v_cnt >= V_FP + V_SYNC + V_BP)//vertical video active
				active_y <= v_cnt - (V_FP + V_SYNC + V_BP );
			else
				active_y <= active_y;
		end

		always@(posedge i_wire_pixel_clock or negedge i_wire_resetn)
		begin
			if(!i_wire_resetn||reg_done)
				v_cnt <= 12'd0;
			else if(h_cnt == H_FP  - 1)//horizontal sync time
				if(v_cnt == V_TOTAL - 1)//vertical counter maximum value
					v_cnt <= 12'd0;
				else
					v_cnt <= v_cnt + 12'd1;
			else
				v_cnt <= v_cnt;
		end

		always@(posedge i_wire_pixel_clock or negedge i_wire_resetn)
		begin
			if(!i_wire_resetn)
				hs_reg <= 1'b0;
			else if(h_cnt == H_FP - 1)//horizontal sync begin
				hs_reg <= HS_POL;
			else if(h_cnt == H_FP + H_SYNC - 1)//horizontal sync end
				hs_reg <= ~hs_reg;
			else
				hs_reg <= hs_reg;
		end

		always@(posedge i_wire_pixel_clock or negedge i_wire_resetn)
		begin
			if(!i_wire_resetn||reg_done)
				h_active <= 1'b0;
			else if(h_cnt == H_FP + H_SYNC + H_BP - 1)//horizontal active begin
				h_active <= 1'b1;
			else if(h_cnt == H_TOTAL - 1)//horizontal active end
				h_active <= 1'b0;
			else
				h_active <= h_active;
		end

		always@(posedge i_wire_pixel_clock or negedge i_wire_resetn)
		begin
			if(!i_wire_resetn||reg_done)
				vs_reg <= 1'd0;
			else if((v_cnt == V_FP - 1) && (h_cnt == H_FP - 1))//vertical sync begin
				vs_reg <= HS_POL;
			else if((v_cnt == V_FP + V_SYNC - 1) && (h_cnt == H_FP - 1))//vertical sync end
				vs_reg <= ~vs_reg;  
			else
				vs_reg <= vs_reg;
		end

		always@(posedge i_wire_pixel_clock or negedge i_wire_resetn)
		begin
			if(!i_wire_resetn||reg_done)
				v_active <= 1'd0;
			else if((v_cnt == V_FP + V_SYNC + V_BP - 1) && (h_cnt == H_FP - 1))//vertical active begin
				v_active <= 1'b1;
			else if((v_cnt == V_TOTAL - 1) && (h_cnt == H_FP - 1)) //vertical active end
				v_active <= 1'b0;   
			else
				v_active <= v_active;
		end


		always@(posedge i_wire_pixel_clock or negedge i_wire_resetn)
		begin
			if(!i_wire_resetn||reg_done)
				begin
					rgb_next <= 1'b0;
					rgb_r_reg <= 8'h00;
					rgb_g_reg <= 8'h00;
					rgb_b_reg <= 8'h00;
				end
			else 
			if(video_active && active_x < i_wire_clip_width && active_y < i_wire_clip_height)
			begin
				rgb_next <= 1'b1;
				case (i_wire_rgba_mode)
					`DVI_RGB_MODE_ARGB:
					begin
						//rgb_r_reg <= 8'hff;
						//rgb_g_reg <= 8'd0;
						//rgb_b_reg <= 8'd0;
						rgb_r_reg <= i_wire_rgba[31:24];
						rgb_g_reg <= i_wire_rgba[23:16];
						rgb_b_reg <= i_wire_rgba[15:8];
					end
					`DVI_RGB_MODE_RGBA:
					begin
						rgb_r_reg <= i_wire_rgba[23:16];
						rgb_g_reg <= i_wire_rgba[31:24];
						rgb_b_reg <= i_wire_rgba[15:8];
					end
					`DVI_RGB_MODE_ABGR:
					begin
						rgb_r_reg <= i_wire_rgba[15:8];
						rgb_g_reg <= i_wire_rgba[23:16];
						rgb_b_reg <= i_wire_rgba[31:24];
					end
					`DVI_RGB_MODE_BGRA:
					begin
						rgb_r_reg <= i_wire_rgba[15:8];
						rgb_g_reg <= i_wire_rgba[23:16];
						rgb_b_reg <= i_wire_rgba[31:24];
					end
					default:
					begin
						rgb_r_reg <= 8'h00;
						rgb_g_reg <= 8'h00;
						rgb_b_reg <= 8'h00;
					end
				endcase
			end
			else
				begin
					rgb_next <= 1'b0;
					rgb_r_reg <= 8'h00;
					rgb_g_reg <= 8'h00;
					rgb_b_reg <= 8'h00;
				end
		end
	endmodule

	module painterengine_gpu_displayclip
		(
		input wire[2:0]             								i_wire_display_mode,  
		input wire[15:0]   											i_wire_image_width,         //texture width
		input wire[15:0]   											i_wire_image_height,        //texture height
		output wire[15:0]   										o_wire_clip_width,         //display width
		output wire[15:0]   										o_wire_clip_height       //display height
		);
		reg [15:0]   												reg_clip_width;
		reg [15:0]   												reg_clip_height;
		assign o_wire_clip_width=reg_clip_width;
		assign o_wire_clip_height=reg_clip_height;

		always @(*)
		begin
			case (i_wire_display_mode)
			`VIDEO_DISPLAY_MODE_1280_720:
			begin
				reg_clip_width=i_wire_image_width>1280?1280:i_wire_image_width;
				reg_clip_height=i_wire_image_height>720?720:i_wire_image_height;
			end
			`VIDEO_DISPLAY_MODE_480_272:
			begin
				reg_clip_width=i_wire_image_width>480?480:i_wire_image_width;
				reg_clip_height=i_wire_image_height>272?272:i_wire_image_height;
			end
			`VIDEO_DISPLAY_MODE_640_480:
			begin
				reg_clip_width=i_wire_image_width>640?640:i_wire_image_width;
				reg_clip_height=i_wire_image_height>480?480:i_wire_image_height;
			end
			`VIDEO_DISPLAY_MODE_800_480:
			begin
				reg_clip_width=i_wire_image_width>800?800:i_wire_image_width;
				reg_clip_height=i_wire_image_height>480?480:i_wire_image_height;
			end
			`VIDEO_DISPLAY_MODE_800_600:
			begin
				reg_clip_width=i_wire_image_width>800?800:i_wire_image_width;
				reg_clip_height=i_wire_image_height>600?600:i_wire_image_height;
			end
			`VIDEO_DISPLAY_MODE_1024_768:
			begin
				reg_clip_width=i_wire_image_width>1024?1024:i_wire_image_width;
				reg_clip_height=i_wire_image_height>768?768:i_wire_image_height;
			end
			`VIDEO_DISPLAY_MODE_1920_1080:
			begin
				reg_clip_width=i_wire_image_width>1920?1920:i_wire_image_width;
				reg_clip_height=i_wire_image_height>1080?1080:i_wire_image_height;
			end
			default:
			begin
				reg_clip_width<=0;
				reg_clip_height<=0;
			end
			endcase
		end
	endmodule

	module painterengine_gpu_display #
		(
		parameter integer PARAM_DATA_ALIGN	= `ADDRESS_ALIGN,
		parameter integer PARAM_ADDRESS_WIDTH	= `ADDRESS_WIDTH,
		parameter integer PARAM_DATA_WIDTH	= `DATA_WIDTH,
		parameter integer PARAM_CACHE_MAX_SIZE =`RW_CACHE_SIZE
		)
		(
		input wire   												i_wire_axi_clock,
		input wire             										i_wire_pixel_clock,   //pixel clock

		
		input wire   												i_wire_resetn,

		input wire [PARAM_ADDRESS_WIDTH-1:0] 						i_wire_image_address,
		input wire [15:0]											i_wire_image_width,
		input wire [15:0]											i_wire_image_height,

		input wire [2:0]											i_wire_display_mode,
		input wire [2:0]											i_wire_color_mode,
		output wire													o_wire_done,
		
		//reader
		output wire [PARAM_ADDRESS_WIDTH-1:0] 						o_wire_reader_address,
		output wire [8:0] 											o_wire_reader_length,
		output wire 												o_wire_reader_enable,
		input wire 													i_wire_reader_done,
		input wire [PARAM_CACHE_MAX_SIZE*PARAM_DATA_WIDTH-1:0] 		i_wire_reader_data,

		//rgb
		output wire              									o_wire_hs,            //horizontal synchronization
		output wire              									o_wire_vs,            //vertical synchronization
		output wire               									o_wire_de,            //video valid
		output wire[23:0]           								o_wire_rgb
		
		);

		wire[15:0] reg_clip_width;
		wire[15:0] reg_clip_height;

		//clip
		painterengine_gpu_displayclip gpu_displayclip(
			.i_wire_display_mode(i_wire_display_mode),
			.i_wire_image_width(i_wire_image_width),
			.i_wire_image_height(i_wire_image_height),
			.o_wire_clip_width(reg_clip_width),
			.o_wire_clip_height(reg_clip_height)
		);



		//reader
		reg [15:0]													reg_reader_image_x;
		reg [15:0]													reg_reader_image_y;

		reg [PARAM_ADDRESS_WIDTH-1:0] 								reg_reader_address;
		reg [8:0] 													reg_reader_length;
		reg															reg_reader_enable;
		reg [7:0] 													reg_reader_offset;
		
		`define 		READER_STATE_CONFIG 2'b000
		`define 		READER_STATE_READING 2'b001
		`define 		READER_STATE_WRITE_FIFO 	2'b010
		`define 		READER_STATE_DONE 	2'b011
		reg [1:0]													reg_reader_state;
		reg 														reg_reader_fifo_write_enable;
		reg [PARAM_DATA_WIDTH-1:0] 									reg_reader_fifo_data;
		wire 														wire_fifo_reader_full;
		wire 														wire_fifo_reader_almost_full;

		assign o_wire_reader_address=reg_reader_address;
		assign o_wire_reader_length=reg_reader_length;
		assign o_wire_reader_enable=reg_reader_enable;

		wire  wire_eof_image=(reg_reader_image_x==reg_clip_width-1&&reg_reader_image_y==reg_clip_height-1);
		always @(posedge i_wire_axi_clock or negedge i_wire_resetn)
		begin
			if (!i_wire_resetn)
			begin
				reg_reader_image_x<=0;
				reg_reader_image_y<=0;
				reg_reader_address<=0;
				reg_reader_length<=0;
				reg_reader_enable<=0;
				reg_reader_offset<=0;
				reg_reader_fifo_data<=0;
				reg_reader_state<=`READER_STATE_CONFIG;
				reg_reader_fifo_write_enable<=0;
			end
			else
			begin
				case(reg_reader_state)
				`READER_STATE_CONFIG:
				begin
					reg_reader_image_x<=reg_reader_image_x;
					reg_reader_image_y<=reg_reader_image_y;
					reg_reader_address<=i_wire_image_address+reg_reader_image_y*i_wire_image_width*4+reg_reader_image_x*4;
					reg_reader_length<=reg_clip_width-reg_reader_image_x>`RW_CACHE_SIZE?`RW_CACHE_SIZE:reg_clip_width-reg_reader_image_x;
					reg_reader_enable<=0;//close reader
					reg_reader_offset<=0;
					reg_reader_fifo_data<=0;
					reg_reader_state<=`READER_STATE_READING;
					reg_reader_fifo_write_enable<=0;
				end
				`READER_STATE_READING:
				begin
					if(i_wire_reader_done)
					begin
						reg_reader_image_x<=reg_reader_image_x;
						reg_reader_image_y<=reg_reader_image_y;
						reg_reader_address<=reg_reader_address;
						reg_reader_length<=reg_reader_length;
						reg_reader_enable<=1;//open reader
						reg_reader_offset<=0;
						reg_reader_fifo_data<=0;
						reg_reader_state<=`READER_STATE_WRITE_FIFO;
						reg_reader_fifo_write_enable<=0;
					end
					else
					begin
						reg_reader_image_x<=reg_reader_image_x;
						reg_reader_image_y<=reg_reader_image_y;
						reg_reader_address<=reg_reader_address;
						reg_reader_length<=reg_reader_length;
						reg_reader_enable<=1;//open reader
						reg_reader_offset<=0;
						reg_reader_fifo_data<=0;
						reg_reader_state<=`READER_STATE_READING;
						reg_reader_fifo_write_enable<=0;
					end
				end
				`READER_STATE_WRITE_FIFO:
				begin
					if (reg_reader_offset<reg_reader_length)
					begin
						reg_reader_image_y<=reg_reader_image_y;
						reg_reader_address<=reg_reader_address;
						reg_reader_length<=reg_reader_length;
						reg_reader_enable<=1;//reader enabled
						reg_reader_state<=reg_reader_state;
						if(!wire_fifo_reader_full&&!wire_fifo_reader_almost_full)
						begin
							reg_reader_image_x<=reg_reader_image_x+1;
							reg_reader_fifo_data<=i_wire_reader_data[reg_reader_offset*32 +:32];
							reg_reader_fifo_write_enable<=1;
							reg_reader_offset<=reg_reader_offset+1;
						end
						else
						begin
							reg_reader_image_x<=reg_reader_image_x;
							reg_reader_fifo_data<=reg_reader_fifo_data;
							reg_reader_fifo_write_enable<=0;
							reg_reader_offset<=reg_reader_offset;
						end
					end
					else
					begin
						if (reg_reader_image_x==reg_clip_width) 
						begin
							if (reg_reader_image_y==reg_clip_height-1)
							begin
								reg_reader_image_x<=reg_reader_image_x;
								reg_reader_image_y<=reg_reader_image_y;
								reg_reader_state<=`READER_STATE_DONE;
							end
							else
							begin
								reg_reader_image_x<=0;
								reg_reader_image_y<=reg_reader_image_y+1;
								reg_reader_state<=`READER_STATE_CONFIG;
							end
						end
						else
						begin
							reg_reader_image_x<=reg_reader_image_x;
							reg_reader_image_y<=reg_reader_image_y;
							reg_reader_state<=`READER_STATE_CONFIG;
						end
						reg_reader_address<=reg_reader_address;
						reg_reader_length<=reg_reader_length;
						reg_reader_enable<=0;//close reader
						reg_reader_offset<=0;
						reg_reader_fifo_data<=0;
						reg_reader_fifo_write_enable<=0;
					end
				end
				`READER_STATE_DONE:
				begin
					reg_reader_image_x<=0;
					reg_reader_image_y<=0;
					reg_reader_address<=0;
					reg_reader_length<=0;
					reg_reader_enable<=0;
					reg_reader_offset<=0;
					reg_reader_fifo_data<=0;
					reg_reader_state<=`READER_STATE_DONE;
					reg_reader_fifo_write_enable<=0;
				end
				endcase
			end
		end

		wire wire_dvi_fifo_read_enable;
		wire wire_fifo_dvi_empty;
		wire [PARAM_DATA_WIDTH-1:0] wire_fifo_out_color;
		wire wire_div_done;
		//fifo
		painterengine_gpu_fifo dvi_fifo(
			.i_wire_write_clock(i_wire_axi_clock),
			.i_wire_read_clock(i_wire_pixel_clock),
			.i_wire_resetn(i_wire_resetn),
			.i_wire_write(reg_reader_fifo_write_enable),
			.i_wire_read(wire_dvi_fifo_read_enable),
			.i_wire_data_in(reg_reader_fifo_data),
			.o_wire_data_out(wire_fifo_out_color),
			.o_wire_full(wire_fifo_reader_full),
			.o_wire_almost_full(wire_fifo_reader_almost_full),
			.o_wire_empty(wire_fifo_dvi_empty)
		);

		painterengine_gpu_dvi gpu_dvi(
			.i_wire_pixel_clock(i_wire_pixel_clock),
			.i_wire_resetn(i_wire_resetn),
			.o_wire_next_rgb(wire_dvi_fifo_read_enable),
			.i_wire_rgba(wire_fifo_out_color),
			.i_wire_rgba_mode(i_wire_color_mode),
			.i_wire_display_mode(i_wire_display_mode),
			.i_wire_clip_width(reg_clip_width),
			.i_wire_clip_height(reg_clip_height),
			.o_wire_hs(o_wire_hs),
			.o_wire_vs(o_wire_vs),
			.o_wire_de(o_wire_de),
			.o_wire_rgb(o_wire_rgb),
			.o_wire_done(wire_div_done)
		);

		assign o_wire_done=(reg_reader_state==`READER_STATE_DONE)&&wire_fifo_dvi_empty&&wire_div_done;

	endmodule

	module painterengine_gpu_circletest #
		(
		parameter integer PARAM_DATA_ALIGN	= `ADDRESS_ALIGN,
		parameter integer PARAM_ADDRESS_WIDTH	= `ADDRESS_WIDTH,
		parameter integer PARAM_DATA_WIDTH	= `DATA_WIDTH, 
		parameter integer PARAM_CACHE_MAX_SIZE	= `RW_CACHE_SIZE
		)
		(
		input wire   												i_wire_axi_clock,
		input wire   												i_wire_resetn,
		input wire [15:0] 											i_wire_display_width,
		input wire [15:0] 											i_wire_display_height,
		input wire [PARAM_ADDRESS_WIDTH-1:0] 						i_wire_cache_address,
		output wire [PARAM_ADDRESS_WIDTH-1:0] 						o_wire_cache_address,
		output wire [8:0] 											o_wire_cache_length,
		output wire [PARAM_DATA_WIDTH*PARAM_CACHE_MAX_SIZE-1:0] 	o_wire_cache,
		output wire 												o_wire_writer_enable,
		input wire 													i_wire_writer_done,
		output wire 												o_wire_done
		);

		reg [16:0] reg_y;

		reg reg_writer_enable;
		assign o_wire_writer_enable=reg_writer_enable;
		assign o_wire_cache_length=9'd64;

		reg [31:0] reg_writer_data[PARAM_CACHE_MAX_SIZE-1:0];
		reg reg_done;
		assign o_wire_done=reg_done;


		assign o_wire_cache_address=i_wire_cache_address+reg_y*i_wire_display_width*4;
		assign i_wire_cache_length=PARAM_CACHE_MAX_SIZE;

		genvar i;
		generate
			for ( i = 0; i < PARAM_CACHE_MAX_SIZE; i = i+1) 
			begin:cache_assign
    			assign o_wire_cache[32*i +: 32]= reg_writer_data[i] ; 
  			end 
		endgenerate


		always @(posedge i_wire_axi_clock or negedge i_wire_resetn)
		begin
			if (!i_wire_resetn||reg_done)
			begin
				reg_y<=0;
			end
			else
			begin
				if (reg_y<i_wire_display_height&&i_wire_writer_done) 
				begin
					reg_y<=reg_y+1;
				end
				else
				begin
					reg_y<=reg_y;
				end
			end
		end

		always @(posedge i_wire_axi_clock or negedge i_wire_resetn)
		begin
			if (!i_wire_resetn||reg_done)
			begin
				reg_writer_enable<=0;
			end
			else
			begin
				if (i_wire_writer_done)
				begin
					reg_writer_enable<=0;
				end
				else
				begin
					reg_writer_enable<=1;
				end
			end
		end

		integer j;
		always @(posedge i_wire_axi_clock or negedge i_wire_resetn)
		begin
			if (!i_wire_resetn)
				begin
					for ( j = 0; j < PARAM_CACHE_MAX_SIZE; j = j+1)
					begin:cache_clear
						reg_writer_data[j]=32'hff0000ff|(j<<16)|(reg_y<<8);	
					end
				end
			else
			begin
			 if(i_wire_writer_done)
				for ( j = 0; j < PARAM_CACHE_MAX_SIZE; j = j+1)
					begin:cache_copy
						begin
							reg_writer_data[j]=32'hff0000ff|(j<<16)|((reg_y+1)<<8);			
						end
					end
			end
		end

		always @(posedge i_wire_axi_clock or negedge i_wire_resetn)
		begin
			if (!i_wire_resetn)
			begin
				reg_done<=0;
			end
			else
			begin
				if (reg_y>=i_wire_display_height&&i_wire_writer_done)
				begin
					reg_done<=1;
				end
				else
				begin
					reg_done<=0;
				end
			end
		end
	endmodule

	module painterengine_gpu #
		(
		parameter integer PARAM_DATA_ALIGN	= 64,
		parameter integer PARAM_ADDRESS_WIDTH	= 32,
		parameter integer PARAM_DATA_WIDTH	= 32,
		parameter integer PARAM_CACHE_MAX_SIZE	= 256
		)
		(
		//input clk
		input wire   									i_wire_axi_clock,
		input wire   									i_wire_pixel_clock,
		input wire 										i_wire_5x_pixel_clock,   //5x pixel clock
		
		//enable
		input wire   									i_wire_resetn,

		//mode
		input wire [2:0]								i_wire_color_mode,

		//display port
		input wire [PARAM_ADDRESS_WIDTH-1:0] 			i_wire_display_address,
		input wire [15:0] 								i_wire_display_width,
		input wire [15:0] 								i_wire_display_height,
		input wire [2:0]								i_wire_display_mode,
		
		//hdmi dvi
		output wire [2:0] 	TMDS_DATA_p,
    	output wire [2:0] 	TMDS_DATA_n,
    	output wire     	TMDS_CLK_p,
    	output wire     	TMDS_CLK_n,
		/////////////////////////////////////////////////////////////
		//axi ports
		/////////////////////////////////////////////////////////////
			output wire m00_axi_awid,
			output wire [PARAM_ADDRESS_WIDTH-1 : 0] m00_axi_awaddr,
			output wire [7 : 0] m00_axi_awlen,
			output wire [2 : 0] m00_axi_awsize,
			output wire [1 : 0] m00_axi_awburst,
			output wire  m00_axi_awlock,
			output wire [3 : 0] m00_axi_awcache,
			output wire [2 : 0] m00_axi_awprot,
			output wire [3 : 0] m00_axi_awqos,
			output wire  m00_axi_awvalid,
			input wire  m00_axi_awready,
			output wire [PARAM_DATA_WIDTH-1 : 0] m00_axi_wdata,
			output wire [PARAM_DATA_WIDTH/8-1 : 0] m00_axi_wstrb,
			output wire  m00_axi_wlast,
			output wire  m00_axi_wvalid,
			input wire  m00_axi_wready,
			input wire  m00_axi_bid,
			input wire  [1 : 0] m00_axi_bresp,
			input wire  m00_axi_bvalid,
			output wire m00_axi_bready,
			output wire m00_axi_arid,
			output wire [PARAM_ADDRESS_WIDTH-1 : 0] m00_axi_araddr,
			output wire [7 : 0] m00_axi_arlen,
			output wire [2 : 0] m00_axi_arsize,
			output wire [1 : 0] m00_axi_arburst,
			output wire  m00_axi_arlock,
			output wire [3 : 0] m00_axi_arcache,
			output wire [2 : 0] m00_axi_arprot,
			output wire [3 : 0] m00_axi_arqos,
			output wire  m00_axi_arvalid,
			input wire  m00_axi_arready,
			input wire  m00_axi_rid,
			input wire [PARAM_DATA_WIDTH-1 : 0] m00_axi_rdata,
			input wire [1 : 0] m00_axi_rresp,
			input wire  m00_axi_rlast,
			input wire  m00_axi_rvalid,
			output wire  m00_axi_rready
		);
	
		reg [PARAM_DATA_WIDTH-1:0] 	reg_s_cache[PARAM_CACHE_MAX_SIZE-1:0];
		wire [PARAM_DATA_WIDTH*PARAM_CACHE_MAX_SIZE-1:0]    wire_s_cache;
		reg [PARAM_DATA_WIDTH-1:0] 	reg_t_cache[PARAM_CACHE_MAX_SIZE-1:0];
		wire [PARAM_DATA_WIDTH*PARAM_CACHE_MAX_SIZE-1:0]    wire_write_cache;



		wire [PARAM_ADDRESS_WIDTH-1:0] 	reg_read_address;
		wire [8:0]						reg_read_length;
		wire 							reg_reader_enable;
		wire 							reg_read_done;

		wire [PARAM_ADDRESS_WIDTH-1:0] 	reg_write_address;
		wire [8:0]						reg_write_length;
		wire 							reg_write_enable;
		wire 							reg_write_done;

		wire 							wire_display_done;

		//tb
		reg 							reg_state;
		reg 							reg_circletest_enable;
		wire 							wire_circletest_done;	
		reg 							reg_display_enable;

		//rgb
		wire               						wire_display_div_hs;            //horizontal synchronization
		wire              						wire_display_div_vs;            //vertical synchronization
		wire               						wire_display_div_de;            //video valid
		
		wire [23:0]           					wire_display_div_rgb;

		always @(posedge i_wire_axi_clock or negedge i_wire_resetn)
		begin
			if (!i_wire_resetn)
			begin
				reg_state<=0;
			end
			else
			begin
				case (reg_state)
					0: 
					begin
						if (!wire_circletest_done)
						begin
							reg_state<=reg_state;
						end
						else
						begin
							reg_state<=1;
						end
					end
					1:
					if (wire_display_done)
					begin
						reg_state<=2;
					end
					else
					begin
						reg_state<=reg_state;
					end
					2:
					begin
						reg_state<=1;
					end
					default: reg_state<=reg_state;
				endcase
			end
		end

		always @(posedge i_wire_axi_clock or negedge i_wire_resetn)
		begin
			if (!i_wire_resetn)
			begin
				reg_display_enable<=0;
			end
			else
			begin
				if(reg_state==1)
					reg_display_enable<=1;
				else
					reg_display_enable<=0;
			end
		end

		always @(posedge i_wire_axi_clock or negedge i_wire_resetn)
		begin
			if (!i_wire_resetn)
			begin
				reg_circletest_enable<=0;
			end
			else
			begin
				if(reg_state==0)
					reg_circletest_enable<=1;
				else
					reg_circletest_enable<=0;
			end
		end

		painterengine_gpu_dma_writer gpu_dma_writer(
			.i_wire_clock(i_wire_axi_clock),
			.i_wire_resetn(reg_write_enable),
			.i_wire_cache(wire_write_cache),
			.i_wire_address(reg_write_address),
			.i_wire_length(reg_write_length),
			.o_wire_done(reg_write_done),
			.o_wire_M_AXI_AWID(m00_axi_awid),
			.o_wire_M_AXI_AWADDR(m00_axi_awaddr),
			.o_wire_M_AXI_AWLEN(m00_axi_awlen),
			.o_wire_M_AXI_AWSIZE(m00_axi_awsize),
			.o_wire_M_AXI_AWBURST(m00_axi_awburst),
			.o_wire_M_AXI_AWLOCK(m00_axi_awlock),
			.o_wire_M_AXI_AWCACHE(m00_axi_awcache),
			.o_wire_M_AXI_AWPROT(m00_axi_awprot),
			.o_wire_M_AXI_AWQOS(m00_axi_awqos),
			.o_wire_M_AXI_AWVALID(m00_axi_awvalid),
			.i_wire_M_AXI_AWREADY(m00_axi_awready),
			.o_wire_M_AXI_WDATA(m00_axi_wdata),
			.o_wire_M_AXI_WSTRB(m00_axi_wstrb),
			.o_wire_M_AXI_WLAST(m00_axi_wlast),
			.o_wire_M_AXI_WVALID(m00_axi_wvalid),
			.i_wire_M_AXI_WREADY(m00_axi_wready),
			.i_wire_M_AXI_BID(m00_axi_bid),
			.i_wire_M_AXI_BRESP(m00_axi_bresp),
			.i_wire_M_AXI_BVALID(m00_axi_bvalid),
			.o_wire_M_AXI_BREADY(m00_axi_bready)
		);

		painterengine_gpu_dma_reader gpu_dma_reader(
			.i_wire_clock(i_wire_axi_clock),
			.i_wire_resetn(reg_reader_enable),
			.o_wire_cache(wire_s_cache),
			.i_wire_address(reg_read_address),
			.i_wire_length(reg_read_length),
			.o_wire_done(reg_read_done),
			.o_wire_M_AXI_ARID(m00_axi_arid),
			.o_wire_M_AXI_ARADDR(m00_axi_araddr),
			.o_wire_M_AXI_ARLEN(m00_axi_arlen),
			.o_wire_M_AXI_ARSIZE(m00_axi_arsize),
			.o_wire_M_AXI_ARBURST(m00_axi_arburst),
			.o_wire_M_AXI_ARLOCK(m00_axi_arlock),
			.o_wire_M_AXI_ARCACHE(m00_axi_arcache),
			.o_wire_M_AXI_ARPROT(m00_axi_arprot),
			.o_wire_M_AXI_ARQOS(m00_axi_arqos),
			.o_wire_M_AXI_ARVALID(m00_axi_arvalid),
			.i_wire_M_AXI_ARREADY(m00_axi_arready),
			.i_wire_M_AXI_RID(m00_axi_rid),
			.i_wire_M_AXI_RDATA(m00_axi_rdata),
			.i_wire_M_AXI_RRESP(m00_axi_rresp),
			.i_wire_M_AXI_RLAST(m00_axi_rlast),
			.i_wire_M_AXI_RVALID(m00_axi_rvalid),
			.o_wire_M_AXI_RREADY(m00_axi_rready)
		);

		painterengine_gpu_circletest gpu_circletest(
			.i_wire_axi_clock(i_wire_axi_clock),
			.i_wire_resetn(reg_circletest_enable),
			.i_wire_display_width('d64),
			.i_wire_display_height('d64),
			.i_wire_cache_address(32'hc000_0000),
			.o_wire_cache_address(reg_write_address),
			.o_wire_cache_length(reg_write_length),
			.o_wire_cache(wire_write_cache),
			.o_wire_writer_enable(reg_write_enable),
			.i_wire_writer_done(reg_write_done),
			.o_wire_done(wire_circletest_done)
		);

		painterengine_gpu_display gpu_display(
			.i_wire_axi_clock(i_wire_axi_clock),
			.i_wire_pixel_clock(i_wire_pixel_clock),
			.i_wire_resetn(reg_display_enable),
			.i_wire_color_mode(i_wire_color_mode),
			.i_wire_image_address(i_wire_display_address),
			.i_wire_image_width(i_wire_display_width),
			.i_wire_image_height(i_wire_display_height),
			.o_wire_reader_address(reg_read_address),
			.o_wire_reader_length(reg_read_length),
			.o_wire_reader_enable(reg_reader_enable),
			.i_wire_reader_done(reg_read_done),
			.i_wire_reader_data(wire_s_cache),
			.i_wire_display_mode(i_wire_display_mode),
			.o_wire_hs(wire_display_div_hs),
			.o_wire_vs(wire_display_div_vs),
			.o_wire_de(wire_display_div_de),
			.o_wire_rgb(wire_display_div_rgb),
			.o_wire_done(wire_display_done)
		);

		rgb2dvi_0 gpu_dvi(
        	.aRst_n(1'd1),
        	.SerialClk(i_wire_5x_pixel_clock),
        	.PixelClk(i_wire_pixel_clock),
        	.TMDS_Clk_p(TMDS_CLK_p),
        	.TMDS_Clk_n(TMDS_CLK_n),
        	.TMDS_Data_p(TMDS_DATA_p),
        	.TMDS_Data_n(TMDS_DATA_n),
        	.vid_pData(wire_display_div_rgb),  
        	.vid_pHSync(wire_display_div_hs),
        	.vid_pVSync(wire_display_div_vs),
        	.vid_pVDE(wire_display_div_de)
    	);
	endmodule
