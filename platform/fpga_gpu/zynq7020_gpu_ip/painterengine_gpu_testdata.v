`timescale 1 ns / 1 ns

`define FIFO_DEPTH 64
`define DATA_WIDTH 32
`define ADDRESS_WIDTH 32
`define ADDRESS_ALIGN 64
`define RW_CACHE_SIZE 256

module painterengine_gpu_testdata #
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
		integer k;
		always @(posedge i_wire_axi_clock or negedge i_wire_resetn)
		begin
			if (!i_wire_resetn)
				begin
					for ( j = 0; j < PARAM_CACHE_MAX_SIZE; j = j+1)
					begin:cache_clear
						reg_writer_data[j]=32'hffffffff;	
					end
				end
			else
			begin
			 if(i_wire_writer_done)
				for ( j = 0; j < PARAM_CACHE_MAX_SIZE; j = j+1)
					begin:cache_copy
						begin
							if (reg_y&(1<<(j>>3)))
							begin
								reg_writer_data[j]=32'hffff00ff;	
							end
							else
							begin
								reg_writer_data[j]=32'hff00ffff;	
							end
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