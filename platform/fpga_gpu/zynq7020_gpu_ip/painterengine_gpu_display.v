`timescale 1 ns / 1 ns

    `define FIFO_DEPTH 64
    `define DATA_WIDTH 32
    `define ADDRESS_WIDTH 32
    `define ADDRESS_ALIGN 64
    `define RW_CACHE_SIZE 256



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
		input wire [PARAM_DATA_WIDTH-1:0] 							i_wire_reader_data,
		output wire 												o_wire_reader_data_next,					

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
		reg                                                         reg_reader_data_next;
		assign 														o_wire_reader_data_next=reg_reader_data_next;
		
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
				reg_reader_data_next<=0;
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
					reg_reader_data_next<=0;
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
						reg_reader_data_next<=0;
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
						reg_reader_data_next<=0;
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
							reg_reader_fifo_data<=i_wire_reader_data;
							reg_reader_data_next<=1;
							reg_reader_fifo_write_enable<=1;
							reg_reader_offset<=reg_reader_offset+1;
						end
						else
						begin
							reg_reader_data_next<=0;
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
						reg_reader_data_next<=0;
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
					reg_reader_data_next<=0;
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
