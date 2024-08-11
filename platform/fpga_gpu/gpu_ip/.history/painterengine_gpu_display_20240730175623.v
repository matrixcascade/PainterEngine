`timescale 1 ns / 1 ns

	`define VIDEO_DISPLAY_MODE_1280_720   3'b000
	`define VIDEO_DISPLAY_MODE_480_272    3'b001
	`define VIDEO_DISPLAY_MODE_640_480    3'b010
	`define VIDEO_DISPLAY_MODE_800_480    3'b011
	`define VIDEO_DISPLAY_MODE_800_600    3'b100
	`define VIDEO_DISPLAY_MODE_1024_768   3'b101
	`define VIDEO_DISPLAY_MODE_1920_1080  3'b110
	`define VIDEO_DISPLAY_MODE_128_64  	  3'b111

	`define DISPLAY_STATE_INIT 			  4'd0
	`define DISPLAY_STATE_CHECK			  4'd1
	`define DISPLAY_STATE_CALC1 		  4'd2
	`define DISPLAY_STATE_CALC2 		  4'd3
	`define DISPLAY_STATE_CALC3 		  4'd4
	`define DISPLAY_STATE_CALC4 		  4'd5
	`define DISPLAY_STATE_CALC5 		  4'd6
	`define DISPLAY_STATE_CALC6 		  4'd7
	`define DISPLAY_STATE_WRITE_ADDRESS	  4'd8
	`define DISPLAY_STATE_STREAMING 	  4'd9
	`define DISPLAY_STATE_INC 	  		  4'd10
	`define DISPLAY_STATE_CHECKX		  4'd11
	`define DISPLAY_STATE_CHECKY		  4'd12
	`define DISPLAY_STATE_DONE 			  4'd13
	`define DISPLAY_STATE_ERROR 		  4'd14

	`define DISPLAY_FIFO_SIZE 			  192
	`define DISPLAY_READER_LAUNCH_SIZE    128
	`define DISPLAY_READER_BLOCK_SIZE     128

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

		reg                                                         reg_fifo_resetn;
		reg 													    reg_dvi_resetn;

		//reader
		reg [15:0]													reg_x;
		reg [15:0]													reg_y;
		reg [31:0]												    reg_read_pixel_counter;	


		reg [31:0] 													reg_reader_address;
		reg [31:0] 													reg_reader_length;
		reg															reg_reader_resetn;
		
		reg [3:0]													reg_display_state;
		                                                         
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
		painterengine_gpu_fifo #(32,`DISPLAY_FIFO_SIZE) dvi_fifo
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

		assign o_wire_state={27'd0,wire_div_done,reg_display_state};
		assign o_wire_reader_data_next=!wire_fifo_full;
		reg[31:0] reg_i_wire_image_address;
		reg[15:0] reg_i_wire_image_width;
		reg[15:0] reg_i_wire_image_height;
		reg[31:0] reg_read_address_op1;
		reg[31:0] reg_read_address_op2;
		reg[31:0] reg_read_address_op3;
		reg[31:0] reg_read_address_op4;
		reg[15:0] reg_reserved_x;
		reg[7:0]  reg_address_align_size;
		reg[7:0]  reg_current_read_size;


		always @(posedge i_wire_clock or negedge i_wire_resetn)
		begin
			if (!i_wire_resetn)
			begin
				reg_x<=0;
				reg_y<=0;
				reg_reader_address<=0;
				reg_reader_length<=0;
				reg_reader_resetn<=0;
				reg_fifo_resetn<=0;
				reg_dvi_resetn<=0;
				reg_display_state<=`DISPLAY_STATE_INIT;
				reg_read_address_op1<=0;
				reg_read_address_op2<=0;
				reg_read_address_op3<=0;
				reg_read_address_op4<=0;
				reg_reserved_x<=0;
				reg_address_align_size<=0;
				reg_clip_width<=0;
				reg_clip_height<=0;
				reg_current_read_size<=0;
				reg_i_wire_image_address<=0;
				reg_i_wire_image_width<=0;
				reg_i_wire_image_height<=0;
				reg_read_pixel_counter<=0;


			end
			else
			begin
				case(reg_display_state)
				`DISPLAY_STATE_INIT:
				begin
					reg_dvi_resetn<=1'b1;//open dvi
					reg_y<=0;
					reg_x<=0;
					reg_i_wire_image_address<=i_wire_image_address;
					reg_i_wire_image_width<=i_wire_image_width;
					reg_i_wire_image_height<=i_wire_image_height;
					reg_reader_address<=0;
					reg_reader_length<=0;
					reg_reader_resetn<=0;
					reg_fifo_resetn<=1;
					if(wire_clip_valid)
					begin
						reg_display_state<=`DISPLAY_STATE_CHECK;
						reg_clip_width<=wire_clip_width;
						reg_clip_height<=wire_clip_height;
					end
					else
					begin
						reg_display_state<=`DISPLAY_STATE_INIT;
					end
				end
				`DISPLAY_STATE_CHECK:
				begin
					if(reg_clip_width==0||reg_clip_height==0)
					begin
						reg_display_state<=`DISPLAY_STATE_ERROR;
					end
					else
					begin
						reg_display_state<=`DISPLAY_STATE_CALC1;
					end
				end
				`DISPLAY_STATE_CALC1:
				begin
					reg_reserved_x<=reg_clip_width-reg_x;
					reg_read_address_op1<=reg_y*reg_i_wire_image_width;
					
					if(wire_fifo_empty_count>=`DISPLAY_READER_LAUNCH_SIZE)
						reg_display_state<=`DISPLAY_STATE_CALC2;
					else
						reg_display_state<=reg_display_state;//hold on
				end
				`DISPLAY_STATE_CALC2:
				begin
					reg_read_address_op2<=reg_read_address_op1+reg_x;
					reg_display_state<=`DISPLAY_STATE_CALC3;
				end
				`DISPLAY_STATE_CALC3:
				begin
					reg_read_address_op3<=reg_read_address_op2*4;
					reg_display_state<=`DISPLAY_STATE_CALC4;
				end
				`DISPLAY_STATE_CALC4:
				begin
					reg_read_address_op4<=reg_i_wire_image_address+reg_read_address_op3;
					reg_display_state<=`DISPLAY_STATE_CALC5;
				end
				`DISPLAY_STATE_CALC5:
				begin
					reg_address_align_size<=`DISPLAY_READER_BLOCK_SIZE-reg_read_address_op4[2+:5];
					reg_display_state<=`DISPLAY_STATE_CALC6;
				end
				`DISPLAY_STATE_CALC6:
				begin
					if(reg_reserved_x>=reg_address_align_size)
					begin
						reg_current_read_size<=reg_address_align_size;
						reg_display_state<=`DISPLAY_STATE_WRITE_ADDRESS;
					end
					else
					begin
						reg_current_read_size<=reg_reserved_x;
						reg_display_state<=`DISPLAY_STATE_WRITE_ADDRESS;
					end
				end
				`DISPLAY_STATE_WRITE_ADDRESS:
				begin
					reg_reader_address<=reg_read_address_op4;
					reg_reader_length<=reg_current_read_size;
					reg_reader_resetn<=1;
					reg_display_state<=`DISPLAY_STATE_STREAMING;
				end
				`DISPLAY_STATE_STREAMING:
				begin
					if(i_wire_reader_error)
					begin
						reg_reader_resetn<=0;//close reader
						reg_display_state<=`DISPLAY_STATE_ERROR;
					end
					else if(i_wire_reader_done)
					begin
						reg_reader_resetn<=0;//close reader
						reg_display_state<=`DISPLAY_STATE_INC;
					end
					else
					begin
						reg_reader_resetn<=1'b1;//open reader
						//wait for reader done
						reg_display_state<=`DISPLAY_STATE_STREAMING;
					end
				end
				`DISPLAY_STATE_INC:
				begin
					reg_x<=reg_x+reg_current_read_size;
					reg_read_pixel_counter<=reg_read_pixel_counter+reg_current_read_size;
					reg_display_state<=`DISPLAY_STATE_CHECKX;
				end
				`DISPLAY_STATE_CHECKX:
				begin
					if (reg_x==reg_clip_width)
					begin
						reg_x<=0;
						reg_y<=reg_y+1;
						reg_display_state<=`DISPLAY_STATE_CHECKY;
					end
					else
					begin
						reg_display_state<=`DISPLAY_STATE_CALC1;
					end
				end
				`DISPLAY_STATE_CHECKY:
				begin
					if(reg_y==reg_clip_height)
					begin
						reg_display_state<=`DISPLAY_STATE_DONE;
					end
					else
					begin
						reg_display_state<=`DISPLAY_STATE_CALC1;
					end
				end
				`DISPLAY_STATE_DONE:
				begin
					reg_display_state<=reg_display_state;
				end
				`DISPLAY_STATE_ERROR:
				begin
					reg_display_state<=reg_display_state;
				end
				endcase
			end
		end

	endmodule
