
`timescale 1 ns / 1 ps



module clock_100MHZ(
    output wire o_wire_clock_100MHZ,
    output wire o_wire_enabled
    );

reg clock_100MHZ=0;
reg enabled=0;
initial begin
    clock_100MHZ = 1'b0;
    forever #5 clock_100MHZ = ~clock_100MHZ;
end

initial begin
    #100 enabled = 1;
end

assign o_wire_clock_100MHZ = clock_100MHZ;
assign o_wire_enabled = enabled;

endmodule


    module painterengine_gpu_clip #
	(
		parameter integer PARAM_TEXTURE_MAX_SIZE	= 16
	)
	(
	input wire   												i_wire_clock,
	input wire   												i_wire_enable,

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
																

//FSM
	always @(posedge i_wire_clock or negedge i_wire_enable)
	begin
		if (!i_wire_enable)
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
		parameter integer PARAM_DATA_ALIGN	= 64,
		parameter integer PARAM_ADDRESS_WIDTH	= 32,
		parameter integer PARAM_DATA_WIDTH	= 32,
		parameter integer PARAM_CACHE_MAX_SIZE	= 256
	)
	(
		//input clk
		input wire   									i_wire_clock,
		input wire   									i_wire_enable,
		output wire 									o_wire_done,

		input wire [PARAM_ADDRESS_WIDTH-1:0] 			i_wire_address,
		input wire [7:0] 								i_wire_length,
		
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
		reg [7:0] 										reg_length;
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
	    if(i_wire_enable)
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
	always @(posedge i_wire_clock or negedge i_wire_enable)
	begin
		if (!i_wire_enable) 
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
		parameter integer PARAM_DATA_ALIGN	= 64,
		parameter integer PARAM_ADDRESS_WIDTH	= 32,
		parameter integer PARAM_DATA_WIDTH	= 32,
		parameter integer PARAM_CACHE_MAX_SIZE	= 256
	)
	(
		//input clk
		input wire   									i_wire_clock,
		input wire   									i_wire_enable,
		output wire 									o_wire_done,

		input wire [PARAM_ADDRESS_WIDTH-1:0] 			i_wire_address,
		input wire [7:0] 								i_wire_length,
		
		input wire [PARAM_ADDRESS_WIDTH*PARAM_CACHE_MAX_SIZE-1:0] 	i_wire_cache,
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
	reg [7:0] 						reg_length;
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
	
	reg [PARAM_ADDRESS_WIDTH-1 : 0] 					reg_axi_awaddr=0;
	reg 												reg_axi_awvalid=0;
	reg [PARAM_ADDRESS_WIDTH-1 : 0] 					reg_axi_wdata=0;
	reg  												reg_axi_wlast=0;
	reg  												reg_axi_wvalid=0;
	reg  												reg_axi_bready=0;
	reg [7:0]											reg_axi_burstlen=0;
	

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
	    if(i_wire_enable)
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
	always @(posedge i_wire_clock or negedge i_wire_enable)
	begin
		if (!i_wire_enable) 
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


	module painterengine_gpu_hdmi #
	(
		parameter integer PARAM_DATA_ALIGN	= 64,
		parameter integer PARAM_ADDRESS_WIDTH	= 32,
		parameter integer PARAM_DATA_WIDTH	= 32,
		parameter integer PARAM_DATA_MAX_SIZE	= 1024
	)
	(
		input wire             										i_wire_pixel_clock,   //pixel clock
		input wire   												i_wire_axi_clock,
		input wire   												i_wire_enable,
		input wire [PARAM_ADDRESS_WIDTH-1:0] 						i_wire_address,
		input wire [15:0]											i_wire_width,
		input wire [15:0]											i_wire_height,
		input wire [2:0]											i_wire_display_mode,
		output wire 												o_wire_done,
		output                										o_wire_hs,            //horizontal synchronization
		output               										o_wire_vs,            //vertical synchronization
		output                										o_wire_de,            //video valid
		output[7:0]           										o_wire_rgb_r,         //video red data
		output[7:0]           										o_wire_rgb_g,         //video green data
		output[7:0]          										o_wire_rgb_b          //video blue data
	);
		parameter integer VIDEO_DISPLAY_MODE_1280_720 = 3'b000;
		parameter integer VIDEO_DISPLAY_MODE_480_272 = 3'b001;
		parameter integer VIDEO_DISPLAY_MODE_640_480 = 3'b010;
		parameter integer VIDEO_DISPLAY_MODE_800_480 = 3'b011;
		parameter integer VIDEO_DISPLAY_MODE_800_600 = 3'b100;
		parameter integer VIDEO_DISPLAY_MODE_1024_768 = 3'b101;
		parameter integer VIDEO_DISPLAY_MODE_1920_1080 = 3'b110;


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

		
		//HDMI registers

		//define the RGB values for 8 colors
		//parameter WHITE_R       = 8'hff;
		//parameter WHITE_G       = 8'hff;
		//parameter WHITE_B       = 8'hff;
		//parameter YELLOW_R      = 8'hff;
		//parameter YELLOW_G      = 8'hff;
		//parameter YELLOW_B      = 8'h00;                                
		//parameter CYAN_R        = 8'h00;
		//parameter CYAN_G        = 8'hff;
		//parameter CYAN_B        = 8'hff;                                
		//parameter GREEN_R       = 8'h00;
		//parameter GREEN_G       = 8'hff;
		//parameter GREEN_B       = 8'h00;
		//parameter MAGENTA_R     = 8'hff;
		//parameter MAGENTA_G     = 8'h00;
		//parameter MAGENTA_B     = 8'hff;
		//parameter RED_R         = 8'hff;
		//parameter RED_G         = 8'h00;
		//parameter RED_B         = 8'h00;
		//parameter BLUE_R        = 8'h00;
		//parameter BLUE_G        = 8'h00;
		//parameter BLUE_B        = 8'hff;
		//parameter BLACK_R       = 8'h00;
		//parameter BLACK_G       = 8'h00;
		//parameter BLACK_B       = 8'h00;

		wire [15:0] 				H_ACTIVE; 
		wire [15:0] 				H_FP;      
		wire [15:0] 				H_SYNC;   
		wire [15:0] 				H_BP;      
		wire [15:0] 				V_ACTIVE; 
		wire [15:0] 				V_FP;     
		wire [15:0] 				V_SYNC;    
		wire [15:0] 				V_BP;    
		wire  						HS_POL;
		wire  						VS_POL;
		wire [15:0] 				H_TOTAL;
		wire [15:0] 				V_TOTAL;

		assign H_ACTIVE = (i_wire_display_mode==VIDEO_DISPLAY_MODE_1280_720)?VIDEO_1280_720_H_ACTIVE:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_480_272)?VIDEO_480_272_H_ACTIVE:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_640_480)?VIDEO_640_480_H_ACTIVE:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_800_480)?VIDEO_800_480_H_ACTIVE:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_800_600)?VIDEO_800_600_H_ACTIVE:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_1024_768)?VIDEO_1024_768_H_ACTIVE:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_1920_1080)?VIDEO_1920_1080_H_ACTIVE:16'd0;
		assign H_FP = (i_wire_display_mode==VIDEO_DISPLAY_MODE_1280_720)?VIDEO_1280_720_H_FP:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_480_272)?VIDEO_480_272_H_FP:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_640_480)?VIDEO_640_480_H_FP:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_800_480)?VIDEO_800_480_H_FP:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_800_600)?VIDEO_800_600_H_FP:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_1024_768)?VIDEO_1024_768_H_FP:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_1920_1080)?VIDEO_1920_1080_H_FP:16'd0;
		assign H_SYNC = (i_wire_display_mode==VIDEO_DISPLAY_MODE_1280_720)?VIDEO_1280_720_H_SYNC:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_480_272)?VIDEO_480_272_H_SYNC:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_640_480)?VIDEO_640_480_H_SYNC:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_800_480)?VIDEO_800_480_H_SYNC:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_800_600)?VIDEO_800_600_H_SYNC:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_1024_768)?VIDEO_1024_768_H_SYNC:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_1920_1080)?VIDEO_1920_1080_H_SYNC:16'd0;
		assign H_BP = (i_wire_display_mode==VIDEO_DISPLAY_MODE_1280_720)?VIDEO_1280_720_H_BP:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_480_272)?VIDEO_480_272_H_BP:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_640_480)?VIDEO_640_480_H_BP:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_800_480)?VIDEO_800_480_H_BP:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_800_600)?VIDEO_800_600_H_BP:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_1024_768)?VIDEO_1024_768_H_BP:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_1920_1080)?VIDEO_1920_1080_H_BP:16'd0;
		assign V_ACTIVE = (i_wire_display_mode==VIDEO_DISPLAY_MODE_1280_720)?VIDEO_1280_720_V_ACTIVE:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_480_272)?VIDEO_480_272_V_ACTIVE:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_640_480)?VIDEO_640_480_V_ACTIVE:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_800_480)?VIDEO_800_480_V_ACTIVE:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_800_600)?VIDEO_800_600_V_ACTIVE:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_1024_768)?VIDEO_1024_768_V_ACTIVE:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_1920_1080)?VIDEO_1920_1080_V_ACTIVE:16'd0;
		assign V_FP = (i_wire_display_mode==VIDEO_DISPLAY_MODE_1280_720)?VIDEO_1280_720_V_FP:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_480_272)?VIDEO_480_272_V_FP:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_640_480)?VIDEO_640_480_V_FP:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_800_480)?VIDEO_800_480_V_FP:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_800_600)?VIDEO_800_600_V_FP:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_1024_768)?VIDEO_1024_768_V_FP:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_1920_1080)?VIDEO_1920_1080_V_FP:16'd0;
		assign V_SYNC = (i_wire_display_mode==VIDEO_DISPLAY_MODE_1280_720)?VIDEO_1280_720_V_SYNC:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_480_272)?VIDEO_480_272_V_SYNC:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_640_480)?VIDEO_640_480_V_SYNC:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_800_480)?VIDEO_800_480_V_SYNC:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_800_600)?VIDEO_800_600_V_SYNC:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_1024_768)?VIDEO_1024_768_V_SYNC:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_1920_1080)?VIDEO_1920_1080_V_SYNC:16'd0;
		assign V_BP = (i_wire_display_mode==VIDEO_DISPLAY_MODE_1280_720)?VIDEO_1280_720_V_BP:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_480_272)?VIDEO_480_272_V_BP:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_640_480)?VIDEO_640_480_V_BP:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_800_480)?VIDEO_800_480_V_BP:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_800_600)?VIDEO_800_600_V_BP:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_1024_768)?VIDEO_1024_768_V_BP:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_1920_1080)?VIDEO_1920_1080_V_BP:16'd0;
		assign HS_POL = (i_wire_display_mode==VIDEO_DISPLAY_MODE_1280_720)?VIDEO_1280_720_HS_POL:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_480_272)?VIDEO_480_272_HS_POL:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_640_480)?VIDEO_640_480_HS_POL:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_800_480)?VIDEO_800_480_HS_POL:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_800_600)?VIDEO_800_600_HS_POL:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_1024_768)?VIDEO_1024_768__HS_POL:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_1920_1080)?VIDEO_1920_1080_HS_POL:1'b0;
		assign VS_POL = (i_wire_display_mode==VIDEO_DISPLAY_MODE_1280_720)?VIDEO_1280_720_VS_POL:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_480_272)?VIDEO_480_272_VS_POL:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_640_480)?VIDEO_640_480_VS_POL:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_800_480)?VIDEO_800_480_VS_POL:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_800_600)?VIDEO_800_600_VS_POL:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_1024_768)?VIDEO_1024_768__VS_POL:
							(i_wire_display_mode==VIDEO_DISPLAY_MODE_1920_1080)?VIDEO_1920_1080_VS_POL:1'b0;
							


		assign H_TOTAL = H_ACTIVE + H_FP + H_SYNC + H_BP;//horizontal total time (pixels)
		assign V_TOTAL = V_ACTIVE + V_FP + V_SYNC + V_BP;//vertical total time (lines)

		reg [PARAM_DATA_WIDTH-1:0] 	reg_cache[PARAM_DATA_MAX_SIZE-1:0];
		reg [15:0] 					reg_cache_ready_length=0;
		reg [15:0] 					reg_cache_offset=0;


		reg 		hs_reg;                      //horizontal sync register
		reg 		vs_reg;                      //vertical sync register
		reg 		hs_reg_d0;                   //delay 1 clock of 'hs_reg'
		reg			vs_reg_d0;                   //delay 1 clock of 'vs_reg'
		reg[11:0] 	h_cnt;                 		 //horizontal counter
		reg[11:0] 	v_cnt;                 		 //vertical counter
		reg[11:0] 	active_x;              		 //video x position 
		reg[11:0] 	active_y;              		 //video y position 
		reg[7:0] 	rgb_r_reg;              	 //video red data register
		reg[7:0] 	rgb_g_reg;                   //video green data register
		reg[7:0] 	rgb_b_reg;                   //video blue data register
		reg 		h_active;                    //horizontal video active
		reg 		v_active;                    //vertical video active
		wire 		video_active;               //video active(horizontal active and vertical active)
		reg 		video_active_d0;             //delay 1 clock of video_active
		assign 		hs = hs_reg_d0;
		assign 		vs = vs_reg_d0;
		assign 		video_active = h_active & v_active;
		assign 		de = video_active_d0;
		assign 		rgb_r = rgb_r_reg;
		assign 		rgb_g = rgb_g_reg;
		assign 		rgb_b = rgb_b_reg;
/*
		always@(posedge clk or posedge rst)
		begin
			if(rst == 1'b1)
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

		always@(posedge clk or posedge rst)
		begin
			if(rst == 1'b1)
				h_cnt <= 12'd0;
			else if(h_cnt == H_TOTAL - 1)//horizontal counter maximum value
				h_cnt <= 12'd0;
			else
				h_cnt <= h_cnt + 12'd1;
		end

		always@(posedge clk or posedge rst)
		begin
			if(rst == 1'b1)
				active_x <= 12'd0;
			else if(h_cnt >= H_FP + H_SYNC + H_BP - 1)//horizontal video active
				active_x <= h_cnt - (H_FP[11:0] + H_SYNC[11:0] + H_BP[11:0] - 12'd1);
			else
				active_x <= active_x;
		end

		always@(posedge clk or posedge rst)
		begin
			if(rst == 1'b1)
				v_cnt <= 12'd0;
			else if(h_cnt == H_FP  - 1)//horizontal sync time
				if(v_cnt == V_TOTAL - 1)//vertical counter maximum value
					v_cnt <= 12'd0;
				else
					v_cnt <= v_cnt + 12'd1;
			else
				v_cnt <= v_cnt;
		end

		always@(posedge clk or posedge rst)
		begin
			if(rst == 1'b1)
				hs_reg <= 1'b0;
			else if(h_cnt == H_FP - 1)//horizontal sync begin
				hs_reg <= HS_POL;
			else if(h_cnt == H_FP + H_SYNC - 1)//horizontal sync end
				hs_reg <= ~hs_reg;
			else
				hs_reg <= hs_reg;
		end

		always@(posedge clk or posedge rst)
		begin
			if(rst == 1'b1)
				h_active <= 1'b0;
			else if(h_cnt == H_FP + H_SYNC + H_BP - 1)//horizontal active begin
				h_active <= 1'b1;
			else if(h_cnt == H_TOTAL - 1)//horizontal active end
				h_active <= 1'b0;
			else
				h_active <= h_active;
		end

		always@(posedge clk or posedge rst)
		begin
			if(rst == 1'b1)
				vs_reg <= 1'd0;
			else if((v_cnt == V_FP - 1) && (h_cnt == H_FP - 1))//vertical sync begin
				vs_reg <= HS_POL;
			else if((v_cnt == V_FP + V_SYNC - 1) && (h_cnt == H_FP - 1))//vertical sync end
				vs_reg <= ~vs_reg;  
			else
				vs_reg <= vs_reg;
		end

		always@(posedge clk or posedge rst)
		begin
			if(rst == 1'b1)
				v_active <= 1'd0;
			else if((v_cnt == V_FP + V_SYNC + V_BP - 1) && (h_cnt == H_FP - 1))//vertical active begin
				v_active <= 1'b1;
			else if((v_cnt == V_TOTAL - 1) && (h_cnt == H_FP - 1)) //vertical active end
				v_active <= 1'b0;   
			else
				v_active <= v_active;
		end

		always@(posedge clk or posedge rst)
		begin
			if(rst == 1'b1)
				begin
					rgb_r_reg <= 8'h00;
					rgb_g_reg <= 8'h00;
					rgb_b_reg <= 8'h00;
				end
			else if(video_active)
				if(active_x == 12'd0)
					begin
						rgb_r_reg <= WHITE_R;
						rgb_g_reg <= WHITE_G;
						rgb_b_reg <= WHITE_B;
					end
				else if(active_x == (H_ACTIVE/8) * 1)
					begin
						rgb_r_reg <= YELLOW_R;
						rgb_g_reg <= YELLOW_G;
						rgb_b_reg <= YELLOW_B;
					end         
				else if(active_x == (H_ACTIVE/8) * 2)
					begin
						rgb_r_reg <= CYAN_R;
						rgb_g_reg <= CYAN_G;
						rgb_b_reg <= CYAN_B;
					end
				else if(active_x == (H_ACTIVE/8) * 3)
					begin
						rgb_r_reg <= GREEN_R;
						rgb_g_reg <= GREEN_G;
						rgb_b_reg <= GREEN_B;
					end
				else if(active_x == (H_ACTIVE/8) * 4)
					begin
						rgb_r_reg <= MAGENTA_R;
						rgb_g_reg <= MAGENTA_G;
						rgb_b_reg <= MAGENTA_B;
					end
				else if(active_x == (H_ACTIVE/8) * 5)
					begin
						rgb_r_reg <= RED_R;
						rgb_g_reg <= RED_G;
						rgb_b_reg <= RED_B;
					end
				else if(active_x == (H_ACTIVE/8) * 6)
					begin
						rgb_r_reg <= BLUE_R;
						rgb_g_reg <= BLUE_G;
						rgb_b_reg <= BLUE_B;
					end 
				else if(active_x == (H_ACTIVE/8) * 7)
					begin
						rgb_r_reg <= BLACK_R;
						rgb_g_reg <= BLACK_G;
						rgb_b_reg <= BLACK_B;
					end
				else
					begin
						rgb_r_reg <= rgb_r_reg;
						rgb_g_reg <= rgb_g_reg;
						rgb_b_reg <= rgb_b_reg;
					end         
			else
				begin
					rgb_r_reg <= 8'h00;
					rgb_g_reg <= 8'h00;
					rgb_b_reg <= 8'h00;
				end
		end
*/
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
		input wire   									i_wire_clock,
		input wire   									i_wire_enable,

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
	wire [PARAM_DATA_WIDTH*PARAM_CACHE_MAX_SIZE-1:0]    wire_t_cache;

	wire reg_write_done;
	wire reg_read_done;

 	genvar i;
  	generate for (i = 0; i < PARAM_CACHE_MAX_SIZE; i = i+1) 
	begin:instmem
    	assign wire_s_cache[32*i +: 32] = reg_s_cache[i];
		assign wire_t_cache[32*i +: 32] = reg_t_cache[i];
  	end 
	endgenerate

	integer j;
	always @(posedge i_wire_clock)
	begin
		if (!i_wire_enable) 
		begin
  			for (j = 0; j < PARAM_CACHE_MAX_SIZE; j = j+1) 
			begin:gen0
    			reg_s_cache[j] <= j+8;
				reg_t_cache[j] <= j+8;
  			end 
		end
	end

	reg [1:0] state=0;
	reg reg_write_enable=0;
	reg reg_reader_enable=0;
	always @(posedge i_wire_clock)
	begin
		if (!i_wire_enable) 
		begin
			state<=0;
		end
		else
		begin
			if (state==0) 
			begin
				reg_write_enable<=1;
				reg_reader_enable<=0;
				if (reg_write_done) begin
					state<=1;
				end
			end
			else if(state==1)
			begin
				reg_reader_enable<=1;
				reg_write_enable<=0;
				if (reg_read_done) begin
					state<=2;
				end
			end
			else
			begin
				state<=state;
			end
		end
	end

	painterengine_gpu_dma_writer gpu_dma_writer(
		.i_wire_clock(i_wire_clock),
		.i_wire_enable(reg_write_enable),
		.i_wire_cache(wire_t_cache),
		.i_wire_address(32'hc0000000+'d16),
		.i_wire_length(16'd168),
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
		.i_wire_clock(i_wire_clock),
		.i_wire_enable(reg_reader_enable),
		.i_wire_address(32'hc0000000+'d16),
		.i_wire_length(16'd60),
		.o_wire_cache(wire_s_cache),
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
	


	endmodule
