	    `define GPU_MEMCPY_STATE_INIT						32'h00000000
		`define GPU_MEMCPY_STATE_PUSH_PARAM					32'h00000001
		`define GPU_MEMCPY_STATE_CALC_PROCESS				32'h00000002
		`define GPU_MEMCPY_STATE_RUN						32'h00000003
		`define GPU_MEMCPY_STATE_WAIT						32'h00000004
		`define GPU_MEMCPY_STATE_CHECKSIZE					32'h00000005
		`define GPU_MEMCPY_STATE_DONE						32'h00000006
		`define GPU_MEMCPY_STATE_LENGTH_ERROR				32'h00000007
		`define GPU_MEMCPY_STATE_DMA_READER_ERROR			32'h00000008
		`define GPU_MEMCPY_STATE_DMA_WRITER_ERROR			32'h00000009

		module painterengine_gpu_memcpy
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



		reg [31:0] 	reg_state;
		reg			reg_dma_writer_resetn;
		reg			reg_dma_reader_resetn;
		reg			reg_fifo_resetn;
		reg [31:0] 	reg_task_memcpy_s_address;
		reg [31:0] 	reg_task_memcpy_d_address;
		reg [31:0] 	reg_task_memcpy_offset;
		reg [31:0] 	reg_task_memcpy_lenght;
		reg [31:0] 	reg_task_memcpy_block_size;
		
		wire [31:0] wire_reserved_size;
		assign wire_reserved_size=reg_task_memcpy_lenght-reg_task_memcpy_offset;
		assign o_wire_state=reg_state;
		assign o_wire_dma_reader_resetn=reg_dma_reader_resetn;
		assign o_wire_fifo_resetn=reg_fifo_resetn;
		assign o_wire_dma_reader_address=reg_task_memcpy_s_address;
		assign o_wire_dma_reader_length=reg_task_memcpy_block_size;
		assign o_wire_dma_writer_resetn=reg_dma_writer_resetn;
		assign o_wire_dma_writer_address=reg_task_memcpy_d_address;
		assign o_wire_dma_writer_length=reg_task_memcpy_block_size;

		task GPU_TASK_RESET;
		begin
			reg_state<=`GPU_MEMCPY_STATE_INIT;
			reg_dma_writer_resetn<=0;
			reg_dma_reader_resetn<=0;
			reg_fifo_resetn<=0;
			reg_task_memcpy_s_address<=0;
			reg_task_memcpy_d_address<=0;
			reg_task_memcpy_offset<=0;
			reg_task_memcpy_lenght<=0;
			reg_task_memcpy_block_size<=0;
		end
		endtask


		task GPU_TASK_MEMCPY;
		begin
			case(reg_state)
			`GPU_MEMCPY_STATE_INIT:
			begin
				//reset
				reg_dma_writer_resetn<=0;
				reg_dma_reader_resetn<=0;
				reg_fifo_resetn<=0;
				//init
				if(reg_task_memcpy_lenght&2'b11!=0)
				begin
					reg_state<=`GPU_MEMCPY_STATE_LENGTH_ERROR;
				end
				else 
				begin
					reg_task_memcpy_offset<=0;
					reg_task_memcpy_lenght<=i_wire_length;
					reg_state<=`GPU_MEMCPY_STATE_PUSH_PARAM;
				end
			end
			`GPU_MEMCPY_STATE_PUSH_PARAM:
			begin
				
				//0 source address
				//1 destination address
				//2 data size
				reg_fifo_resetn<=1'b0;
				reg_dma_writer_resetn<=1'b0;
				reg_dma_reader_resetn<=1'b0;

				reg_task_memcpy_s_address<=i_wire_source_address+reg_task_memcpy_offset;
				reg_task_memcpy_d_address<=i_wire_dest_address+reg_task_memcpy_offset;

				if (wire_reserved_size)
				begin
					if (wire_reserved_size>256)
					begin
						reg_task_memcpy_block_size<=256;
						reg_state<=`GPU_MEMCPY_STATE_RUN;
					end
					else
					begin
						reg_task_memcpy_block_size<=wire_reserved_size;
						reg_state<=`GPU_MEMCPY_STATE_RUN;
					end
				end
				else
				begin
					reg_state<=`GPU_MEMCPY_STATE_DONE;
				end
			end
			`GPU_MEMCPY_STATE_RUN:
			begin
				reg_fifo_resetn<=1'b1;
				reg_dma_writer_resetn<=1'b1;
				reg_dma_reader_resetn<=1'b1;
				reg_state<=`GPU_MEMCPY_STATE_WAIT;
			end
			`GPU_MEMCPY_STATE_WAIT:
			begin
				//wait done
				reg_fifo_resetn<=reg_fifo_resetn;
				reg_dma_writer_resetn<=reg_dma_writer_resetn;
				reg_dma_reader_resetn<=reg_dma_reader_resetn;
				
				if(i_wire_dma_writer_error)
				begin
					reg_state<=`GPU_MEMCPY_STATE_DMA_WRITER_ERROR;
				end
				else if(i_wire_dma_reader_error)
				begin
					reg_state<=`GPU_MEMCPY_STATE_DMA_READER_ERROR;
				end
				else
				begin
					if(i_wire_dma_writer_done)
					begin
						reg_state<=`GPU_MEMCPY_STATE_PUSH_PARAM;
						reg_task_memcpy_offset<=reg_task_memcpy_offset+reg_task_memcpy_block_size;
					end
					else
					begin
						reg_state<=reg_state;
					end
				end
			end
			default:
			begin
				reg_fifo_resetn<=1'b0;
				reg_dma_writer_resetn<=1'b0;
				reg_dma_reader_resetn<=1'b0;
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