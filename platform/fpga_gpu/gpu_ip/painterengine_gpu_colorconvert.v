	    `define GPU_COLORCONVERT_STATE_INIT						8'h00
		`define GPU_COLORCONVERT_STATE_PUSH_PARAM					8'h01
		`define GPU_COLORCONVERT_STATE_CALC_PROCESS				8'h02
		`define GPU_COLORCONVERT_STATE_READ						8'h03
		`define GPU_COLORCONVERT_STATE_READ_WAIT					8'h04
		`define GPU_COLORCONVERT_STATE_WRITE						8'h05
		`define GPU_COLORCONVERT_STATE_WRITE_WAIT					8'h06
		`define GPU_COLORCONVERT_STATE_CHECKSIZE					8'h07
		`define GPU_COLORCONVERT_STATE_DONE						8'h08
		`define GPU_COLORCONVERT_STATE_LENGTH_ERROR				8'h09
		`define GPU_COLORCONVERT_STATE_DMA_READER_ERROR			8'h0A
		`define GPU_COLORCONVERT_STATE_DMA_WRITER_ERROR			8'h0B

		`define GPU_COLORCONVERT_BLOCK_SIZE 					64
		`define GPU_COLORCONVERT_PIXEL_SIZE		 				48

		module painterengine_gpu_colorconvert
		(
			input wire i_wire_clock,
			input wire i_wire_resetn,
			input wire [31:0] i_wire_source_address,
			input wire [31:0] i_wire_dest_address,
			input wire [31:0] i_wire_length,
			//////////////////////////////////////////////
			//fifo
			output wire o_wire_fifo_resetn,
			
			//////////////////////////////////////////////
			//dma reader
			output wire o_wire_dma_reader_resetn,
			output wire [31:0] o_wire_dma_reader_address,
			output wire [31:0] o_wire_dma_reader_length,
			input wire i_wire_dma_reader_done,
			input wire i_wire_dma_reader_error,
			//////////////////////////////////////////////

			//dma writer
			output wire o_wire_dma_writer_resetn,
			output wire [31:0] o_wire_dma_writer_address,
			output wire [31:0] o_wire_dma_writer_length,
			input wire i_wire_dma_writer_done,
			input wire i_wire_dma_writer_error,
			//////////////////////////////////////////////

			output wire[31:0] 	o_wire_state
		);



		reg [7:0] 	reg_state;
		reg			reg_dma_writer_resetn;
		reg			reg_dma_reader_resetn;
		reg			reg_fifo_resetn;
		reg [31:0] 	reg_task_colorconvert_s_address;
		reg [31:0] 	reg_task_colorconvert_d_address;
		reg [31:0] 	reg_task_colorconvert_src_offset;
		reg [31:0] 	reg_task_colorconvert_dst_offset;
		reg [31:0] 	reg_task_colorconvert_lenght;
		reg [7:0] 	reg_task_colorconvert_block_size;
		reg [7:0] 	reg_task_colorconvert_pixel_size;
		
		wire [31:0] wire_reserved_size;
		assign wire_reserved_size=reg_task_colorconvert_lenght-reg_task_colorconvert_src_offset;
		assign o_wire_state={24'd0,reg_state};
		assign o_wire_dma_reader_resetn=reg_dma_reader_resetn;
		assign o_wire_fifo_resetn=reg_fifo_resetn;
		assign o_wire_dma_reader_address=reg_task_colorconvert_s_address;
		assign o_wire_dma_reader_length={24'd0,reg_task_colorconvert_block_size};
		assign o_wire_dma_writer_resetn=reg_dma_writer_resetn;
		assign o_wire_dma_writer_address=reg_task_colorconvert_d_address;
		assign o_wire_dma_writer_length={24'd0,reg_task_colorconvert_pixel_size};

		task GPU_TASK_RESET;
		begin
			reg_state<=`GPU_COLORCONVERT_STATE_INIT;
			reg_dma_writer_resetn<=0;
			reg_dma_reader_resetn<=0;
			reg_fifo_resetn<=0;
			reg_task_colorconvert_s_address<=0;
			reg_task_colorconvert_d_address<=0;
			reg_task_colorconvert_src_offset<=0;
			reg_task_colorconvert_dst_offset<=0;
			reg_task_colorconvert_lenght<=0;
			reg_task_colorconvert_block_size<=8'd0;
			reg_task_colorconvert_pixel_size<=8'd0;
		end
		endtask


		task GPU_TASK_MEMCPY;
		begin
			case(reg_state)
			`GPU_COLORCONVERT_STATE_INIT:
			begin
				//reset
				reg_dma_writer_resetn<=0;
				reg_dma_reader_resetn<=0;
				reg_fifo_resetn<=0;
				//init
				if(reg_task_colorconvert_lenght[1:0]!=0)
				begin
					reg_state<=`GPU_COLORCONVERT_STATE_LENGTH_ERROR;
				end
				else 
				begin
					reg_task_colorconvert_src_offset<=0;
					reg_task_colorconvert_dst_offset<=0;
					reg_task_colorconvert_lenght<=i_wire_length;
					reg_state<=`GPU_COLORCONVERT_STATE_PUSH_PARAM;
				end
			end
			`GPU_COLORCONVERT_STATE_PUSH_PARAM:
			begin
				
				//0 source address
				//1 destination address
				//2 data size
				reg_fifo_resetn<=1'b0;
				reg_dma_writer_resetn<=1'b0;
				reg_dma_reader_resetn<=1'b0;

				reg_task_colorconvert_s_address<=i_wire_source_address+reg_task_colorconvert_src_offset*4;
				reg_task_colorconvert_d_address<=i_wire_dest_address+reg_task_colorconvert_dst_offset*4;

				if (wire_reserved_size)
				begin
					if (wire_reserved_size>`GPU_COLORCONVERT_BLOCK_SIZE)
					begin
						reg_task_colorconvert_block_size<=`GPU_COLORCONVERT_BLOCK_SIZE;
						reg_task_colorconvert_pixel_size<=(`GPU_COLORCONVERT_BLOCK_SIZE>>2)*3;
						reg_state<=`GPU_COLORCONVERT_STATE_READ;
					end
					else
					begin
						reg_task_colorconvert_block_size<=wire_reserved_size[7:0];
						reg_task_colorconvert_pixel_size<=(wire_reserved_size[7:0]>>2)*3;
						reg_state<=`GPU_COLORCONVERT_STATE_READ;
					end
				end
				else
				begin
					reg_state<=`GPU_COLORCONVERT_STATE_DONE;
				end
			end
			`GPU_COLORCONVERT_STATE_READ:
			begin
				reg_fifo_resetn<=1'b1;
				reg_dma_writer_resetn<=1'b0;
				reg_dma_reader_resetn<=1'b1;
				reg_state<=`GPU_COLORCONVERT_STATE_READ_WAIT;
			end

			`GPU_COLORCONVERT_STATE_READ_WAIT:
			begin
				reg_fifo_resetn<=reg_fifo_resetn;
				reg_dma_writer_resetn<=reg_dma_writer_resetn;
				reg_dma_reader_resetn<=reg_dma_reader_resetn;
				if(i_wire_dma_reader_error)
				begin
					reg_state<=`GPU_COLORCONVERT_STATE_DMA_READER_ERROR;
				end
				else if(i_wire_dma_reader_done)
				begin
					reg_state<=`GPU_COLORCONVERT_STATE_WRITE;
				end
				else
				begin
					reg_state<=reg_state;//continue
				end
			end
			`GPU_COLORCONVERT_STATE_WRITE:
			begin
				reg_fifo_resetn<=1'b1;
				reg_dma_writer_resetn<=1'b1;
				reg_dma_reader_resetn<=1'b0;
				reg_state<=`GPU_COLORCONVERT_STATE_WRITE_WAIT;
			end
			`GPU_COLORCONVERT_STATE_WRITE_WAIT:
			begin
				//wait done
				reg_fifo_resetn<=reg_fifo_resetn;
				reg_dma_writer_resetn<=reg_dma_writer_resetn;
				reg_dma_reader_resetn<=reg_dma_reader_resetn;
				
				if(i_wire_dma_writer_error)
				begin
					reg_state<=`GPU_COLORCONVERT_STATE_DMA_WRITER_ERROR;
				end
				else if(i_wire_dma_writer_done)
				begin
					reg_state<=`GPU_COLORCONVERT_STATE_PUSH_PARAM;
					reg_task_colorconvert_src_offset<=reg_task_colorconvert_src_offset+reg_task_colorconvert_block_size;
					reg_task_colorconvert_dst_offset<=reg_task_colorconvert_dst_offset+reg_task_colorconvert_pixel_size;
				end
				else
				begin
					reg_state<=reg_state;//continue
				end
			end
			default:
			begin
				reg_fifo_resetn<=reg_fifo_resetn;
				reg_dma_writer_resetn<=reg_dma_writer_resetn;
				reg_dma_reader_resetn<=reg_dma_reader_resetn;
				reg_state<=reg_state;
			end
			endcase
		end
		endtask

		always @(posedge i_wire_clock or negedge i_wire_resetn)
		begin
			if(!i_wire_resetn)
			begin
				GPU_TASK_RESET();
			end
			else
			begin
				GPU_TASK_MEMCPY();
			end
		end

endmodule