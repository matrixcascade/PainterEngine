`timescale 1 ns / 1 ns

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
		parameter VIDEO_800_480_V_ACTIVE = 16'd488; 
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
			if(video_active && active_x < i_wire_clip_width &&active_y>0&&active_y <= i_wire_clip_height)
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
