	
    `timescale 1 ns / 1 ns

	`define DATA_WIDTH 32


    module painterengine_gpu_fifo #
		(
			parameter integer PARAM_DATA_WIDTH	= 32,
			parameter integer PARAM_FIFO_DEPTH	= 64

		)
		(
			input wire   									i_wire_write_clock,
			input wire   									i_wire_read_clock,
			input wire   									i_wire_resetn,

			input wire   									i_wire_write,
			input wire   									i_wire_read,

			input wire [PARAM_DATA_WIDTH-1:0] 				i_wire_data_in,
			output reg [PARAM_DATA_WIDTH-1:0] 				o_wire_data_out,

			output wire   									o_wire_almost_full,
			output wire   									o_wire_full,
			
			output wire   									o_wire_almost_empty,
			output wire   									o_wire_empty,
			output wire [8:0]								o_wire_data_count,
			output wire [8:0] 								o_wire_empty_count
		);

		function integer clogb2 (input integer bit_depth);              
		begin                                                           
		    for(clogb2=0; bit_depth>0; clogb2=clogb2+1)                   
		      bit_depth = bit_depth >> 1;                                 
		end                                                           
		endfunction  
		//fifo
		reg [PARAM_DATA_WIDTH-1:0] 						reg_fifo[PARAM_FIFO_DEPTH-1:0];

		reg [clogb2(PARAM_FIFO_DEPTH)-1:0]				reg_fifo_write_index;
		reg [clogb2(PARAM_FIFO_DEPTH)-1:0]				reg_fifo_read_index;
		wire [clogb2(PARAM_FIFO_DEPTH)-1:0]				wire_fifo_data_count;
		wire [clogb2(PARAM_FIFO_DEPTH)-2:0]				wire_fifo_true_read_index;
		wire [clogb2(PARAM_FIFO_DEPTH)-2:0]				wire_fifo_true_read_index_next;
		wire [clogb2(PARAM_FIFO_DEPTH)-2:0]				wire_fifo_true_write_index;

		assign wire_fifo_data_count=reg_fifo_write_index-reg_fifo_read_index;
		assign o_wire_full=(wire_fifo_data_count==PARAM_FIFO_DEPTH);
		assign o_wire_almost_full=(wire_fifo_data_count==PARAM_FIFO_DEPTH-1);
		assign o_wire_empty=(wire_fifo_data_count==0);
		assign o_wire_almost_empty=(wire_fifo_data_count==1);
		assign wire_fifo_true_read_index=reg_fifo_read_index[clogb2(PARAM_FIFO_DEPTH)-2:0];
		assign wire_fifo_true_write_index=reg_fifo_write_index[clogb2(PARAM_FIFO_DEPTH)-2:0];
		assign wire_fifo_true_read_index_next=reg_fifo_read_index+1'b1;

		assign o_wire_data_count=wire_fifo_data_count;
		assign o_wire_empty_count=PARAM_FIFO_DEPTH-wire_fifo_data_count;

		always @(*)
		begin
			o_wire_data_out=reg_fifo[wire_fifo_true_read_index];
		end


		integer i;
		always @(posedge i_wire_write_clock)
		begin
			if(i_wire_write)
			begin
				if(wire_fifo_data_count<PARAM_FIFO_DEPTH)
				begin
					reg_fifo[wire_fifo_true_write_index]<=i_wire_data_in;
				end
				else
				begin
					reg_fifo[wire_fifo_true_write_index]<=reg_fifo[wire_fifo_true_write_index];
				end
			end
			else
			begin
				reg_fifo[wire_fifo_true_write_index]<=reg_fifo[wire_fifo_true_write_index];
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
					reg_fifo_write_index<=reg_fifo_write_index+1'b1;
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
					reg_fifo_read_index<=reg_fifo_read_index+1'b1;
				end
				else
				begin
					reg_fifo_read_index<=reg_fifo_read_index;
				end
			end
		end

	endmodule
