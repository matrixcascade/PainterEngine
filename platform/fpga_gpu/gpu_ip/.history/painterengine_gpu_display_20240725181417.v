`timescale 1 ns / 1 ns

	`define VIDEO_DISPLAY_MODE_1280_720   3'b000
	`define VIDEO_DISPLAY_MODE_480_272    3'b001
	`define VIDEO_DISPLAY_MODE_640_480    3'b010
	`define VIDEO_DISPLAY_MODE_800_480    3'b011
	`define VIDEO_DISPLAY_MODE_800_600    3'b100
	`define VIDEO_DISPLAY_MODE_1024_768   3'b101
	`define VIDEO_DISPLAY_MODE_1920_1080  3'b110
	`define VIDEO_DISPLAY_MODE_128_64  	  3'b111

	`define DISPLAY_STATE_INIT 			  3'b000
	`define DISPLAY_STATE_CALC1 		  3'b001
	`define DISPLAY_STATE_CALC2 		  3'b010
	`define DISPLAY_STATE_STREAMING 	  3'b011
	`define DISPLAY_STATE_CHECK		 	  3'b100
	`define DISPLAY_STATE_DONE 			  3'b101
	`define DISPLAY_STATE_ERROR 		  3'b111

	`define DISPLAY_FIFO_SIZE 			  128
	`define DISPLAY_READER_LAUNCH_SIZE    48
	`define DISPLAY_READER_BLOCK_SIZE     64

    module painterengine_gpu_display
		(
		input wire   												i_wire_clock,
		input wire             										i_wire_pixel_clock,   //pixel clock

		
		input wire   												i_wire_resetn,

		input wire [31:0] 											i_wire_image_address,
		input wire [15:0]											i_wire_image_width,
		input wire [15:0]											i_wire_image_height,

		input wire [2:0]											i_wire_display_mode, 
		input wire [2:0]											i_wire_color_mode,
		output wire[31:0]											o_wire_state,
		
		//DMA reader
		output wire [31:0] 											o_wire_reader_address,
		output wire [31:0] 											o_wire_reader_length,
		output wire 												o_wire_reader_resetn,
		input wire 													i_wire_reader_done,
		input wire                                                  i_wire_reader_error,
		input wire [31:0] 											i_wire_reader_data,
		input wire                                                  i_wire_reader_data_valid,
		output wire 												o_wire_reader_data_next,					

		//rgb
		output wire              									o_wire_hs,            //horizontal synchronization
		output wire              									o_wire_vs,            //vertical synchronization
		output wire               									o_wire_de,            //video valid
		output wire[23:0]           								o_wire_rgb
		);

		wire[15:0] wire_clip_width;
		wire[15:0] wire_clip_height;
		reg[15:0]  reg_clip_width;
		reg[15:0]  reg_clip_height;
		wire       wire_clip_valid;

		//clip
		painterengine_gpu_displayclip gpu_displayclip(
			.i_wire_clock(i_wire_clock),
			.i_wire_resetn(i_wire_resetn),
			.o_wire_valid(wire_clip_valid),
			.i_wire_display_mode(i_wire_display_mode),
			.i_wire_image_width(i_wire_image_width),
			.i_wire_image_height(i_wire_image_height),
			.o_wire_clip_width(wire_clip_width),
			.o_wire_clip_height(wire_clip_height)
		);

		reg                                                        reg_fifo_resetn;
		reg 													   reg_dvi_resetn;

		//reader
		reg [15:0]													reg_streaming_x;
		reg [15:0]													reg_streaming_y;


		reg [31:0] 													reg_reader_address;
		reg [31:0] 													reg_reader_length;
		reg															reg_reader_resetn;
		
		reg [2:0]													reg_display_state;
		                                                         
		wire 														wire_fifo_full;
		wire 														wire_fifo_almost_full;

		assign o_wire_reader_address=reg_reader_address;
		assign o_wire_reader_length=reg_reader_length;
		assign o_wire_reader_resetn=reg_reader_resetn;

		wire wire_fifo_read;
		wire wire_fifo_dvi_empty;
		wire wire_fifo_almost_empty;
		wire [31:0] wire_fifo_out_color;
		wire [7:0] wire_fifo_data_count;
		wire [7:0] wire_fifo_empty_count;
		wire wire_div_done;
		wire [31:0] wire_dvi_pixel_counter;
		//fifo
		painterengine_gpu_fifo #(32,128) dvi_fifo
		(
			.i_wire_write_clock(i_wire_clock),
			.i_wire_read_clock(i_wire_pixel_clock),
			.i_wire_resetn(reg_fifo_resetn),
			.i_wire_write(i_wire_reader_data_valid),
			.i_wire_read(wire_fifo_read),
			.i_wire_data_in(i_wire_reader_data),
			.o_wire_data_out(wire_fifo_out_color),
			.o_wire_full(wire_fifo_full),
			.o_wire_almost_full(wire_fifo_almost_full),
			.o_wire_empty(wire_fifo_dvi_empty),
			.o_wire_almost_empty(wire_fifo_almost_empty),
			.o_wire_data_count(wire_fifo_data_count),
			.o_wire_empty_count(wire_fifo_empty_count)
		);

		painterengine_gpu_dvi gpu_dvi(
			.i_wire_pixel_clock(i_wire_pixel_clock),
			.i_wire_resetn(reg_dvi_resetn),
			.o_wire_next_rgb(wire_fifo_read),
			.i_wire_rgba(wire_fifo_out_color),
			.i_wire_rgba_mode(i_wire_color_mode),
			.i_wire_display_mode(i_wire_display_mode),
			.i_wire_clip_width(wire_clip_width),
			.i_wire_clip_height(wire_clip_height),
			.o_wire_hs(o_wire_hs),
			.o_wire_vs(o_wire_vs),
			.o_wire_de(o_wire_de),
			.o_wire_rgb(o_wire_rgb),
			.o_wire_done(wire_div_done),
			.o_wire_pixel_counter(wire_dvi_pixel_counter)
		);

		assign o_wire_state={28'd0,wire_div_done,reg_display_state};
		assign o_wire_reader_data_next=!wire_fifo_full;
		wire  wire_eof_image=(reg_streaming_y==wire_clip_height);
		reg[31:0] reg_streaming_y_mul_4_img_width;
		reg[31:0] reg_streaming_adr_x_mul_4;
		always @(posedge i_wire_clock or negedge i_wire_resetn)
		begin
			if (!i_wire_resetn)
			begin
				reg_streaming_x<=0;
				reg_streaming_y<=0;
				reg_reader_address<=0;
				reg_reader_length<=0;
				reg_reader_resetn<=0;
				reg_fifo_resetn<=0;
				reg_dvi_resetn<=0;
				reg_display_state<=`DISPLAY_STATE_INIT;
				reg_streaming_y_mul_4_img_width<=0;
				reg_streaming_adr_x_mul_4<=0;
				reg_clip_width<=0;
				reg_clip_height<=0;
			end
			else
			begin
				case(reg_display_state)
				`DISPLAY_STATE_INIT:
				begin
					reg_streaming_y<=0;
					reg_streaming_x<=0;
					reg_reader_address<=0;
					reg_reader_length<=0;
					reg_reader_resetn<=0;
					if(wire_clip_valid)
					    if(wire_clip_width==0||wire_clip_height==0)
						begin
							reg_display_state<=`DISPLAY_STATE_DONE;
						end
						else
						begin
							reg_clip_width<=wire_clip_width;
							reg_clip_height<=wire_clip_height;
							reg_display_state<=`DISPLAY_STATE_CALC1;
						end
					else
					begin
						reg_display_state<=`DISPLAY_STATE_INIT;
						reg_fifo_resetn<=1;
						reg_dvi_resetn<=1;
					end
				end
				`DISPLAY_STATE_CALC1:
				begin
					reg_streaming_y_mul_4_img_width<=reg_streaming_y*i_wire_image_width*4;
					reg_streaming_adr_x_mul_4<=i_wire_image_address+reg_streaming_x*4;
					reg_display_state<=`DISPLAY_STATE_CALC2;
				end
				`DISPLAY_STATE_CALC2:
				begin
					reg_streaming_x<=reg_streaming_x;
					reg_streaming_y<=reg_streaming_y;
					reg_reader_address<=reg_streaming_adr_x_mul_4+reg_streaming_y_mul_4_img_width;
					reg_reader_length<=reg_clip_width-reg_streaming_x>`DISPLAY_READER_BLOCK_SIZE?`DISPLAY_READER_BLOCK_SIZE:reg_clip_width-reg_streaming_x;
					reg_reader_resetn<=0;//close reader
					if(wire_fifo_empty_count>=DISPLAY_READER_LAUNCH_SIZE)
						reg_display_state<=`DISPLAY_STATE_STREAMING;
					else
						reg_display_state<=reg_display_state;//hold on
				end
				`DISPLAY_STATE_STREAMING:
				begin
					if(i_wire_reader_error)
					begin
						reg_streaming_x<=reg_streaming_x;
						reg_streaming_y<=reg_streaming_y;
						reg_reader_address<=reg_reader_address;
						reg_reader_length<=reg_reader_length;
						reg_reader_resetn<=0;//close reader
						reg_display_state<=`DISPLAY_STATE_ERROR;
					end
					else if(i_wire_reader_done)
					begin
						reg_streaming_x<=reg_streaming_x+reg_reader_length;
						reg_streaming_y<=reg_streaming_y;
						reg_reader_address<=reg_reader_address;
						reg_reader_length<=reg_reader_length;
						reg_reader_resetn<=1;//hold reader
						reg_display_state<=`DISPLAY_STATE_CHECK;
					end
					else
					begin
						reg_streaming_y<=reg_streaming_y;
						reg_reader_address<=reg_reader_address;
						reg_reader_length<=reg_reader_length;
						reg_reader_resetn<=1'b1;//open reader
						//wait for reader done
						reg_display_state<=`DISPLAY_STATE_STREAMING;
					end
				end
				`DISPLAY_STATE_CHECK:
				begin
					if (reg_streaming_x==reg_clip_width) 
					begin
						reg_streaming_y<=reg_streaming_y+1;
						reg_streaming_x<=0;
						reg_display_state<=`DISPLAY_STATE_CHECK;
					end
					else
					begin
						if (reg_streaming_y==reg_clip_height)
						begin
							reg_display_state<=`DISPLAY_STATE_DONE;
						end
						else
						begin
							reg_display_state<=`DISPLAY_STATE_CALC1;
						end
						reg_reader_address<=reg_reader_address;
						reg_reader_length<=reg_reader_length;
						reg_reader_resetn<=0;//close reader
					end
				end
				`DISPLAY_STATE_DONE:
				begin
					reg_streaming_x<=reg_streaming_x;
					reg_streaming_y<=reg_streaming_y;
					reg_reader_address<=reg_reader_address;
					reg_reader_length<=reg_reader_length;
					reg_reader_resetn<=0;
					reg_display_state<=`DISPLAY_STATE_DONE;
				end
				`DISPLAY_STATE_ERROR:
				begin
					reg_streaming_x<=reg_streaming_x;
					reg_streaming_y<=reg_streaming_y;
					reg_reader_address<=reg_reader_address;
					reg_reader_length<=reg_reader_length;
					reg_reader_resetn<=reg_reader_resetn;
					reg_display_state<=`DISPLAY_STATE_ERROR;
				end
				endcase
			end
		end

	endmodule
