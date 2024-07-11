`timescale 1 ns / 1 ns


module painterengine_gpu_dma_reader
		(
		//input clk
		input wire   									i_wire_clock,
		input wire   									i_wire_resetn,
		output wire 									o_wire_done,

		input wire  [4*32-1:0] 							i_wire_address,
		input wire  [4*32-1:0] 							i_wire_length,
		
		input wire  [3:0] 								i_wire_router,
		output reg  [4*32-1:0]         					o_wire_data,
		output reg  [3:0]							    o_wire_data_valid,
		input  wire [3:0]								i_wire_data_next,
		output wire  									o_wire_error,

		///////////////////////////////////////////////////////////////////////////////////////////
		//AXI full ports
		///////////////////////////////////////////////////////////////////////////////////////////
		output wire 									o_wire_M_AXI_ARID,
		output wire [31:0]								o_wire_M_AXI_ARADDR,
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
		input wire [31 : 0] 							i_wire_M_AXI_RDATA,
		input wire [1 : 0] 								i_wire_M_AXI_RRESP,
		input wire  									i_wire_M_AXI_RLAST,
		input wire  									i_wire_M_AXI_RVALID,
		output wire  									o_wire_M_AXI_RREADY
		);


		`define fsm_state_idle 3'b000
		`define fsm_state_address_write 3'b001
		`define fsm_state_data_read 3'b010
		`define fsm_state_done 3'b100
		`define fsm_state_error 3'b111
		
		


		reg [4*32-1 : 0] 								reg_address;
		reg [4*32-1:0] 									reg_length;
		reg [31:0] 										reg_offset;
		reg [7:0] 										reg_burst_counter;
		reg [15:0] 										reg_timeout_error;
		reg [2:0]										reg_state;
		reg [31 : 0] 									reg_axi_araddr;
		reg  											reg_axi_arvalid;
		reg [7:0] 										reg_axi_burstlen;

		wire [2:0] wire_router_index=i_wire_router==8?3:(i_wire_router>>1);
	
		//Read Address (AR)
		assign o_wire_M_AXI_ARADDR	= reg_axi_araddr;
		assign o_wire_M_AXI_ARLEN	= reg_axi_burstlen - 1;
		assign o_wire_M_AXI_ARVALID	= reg_axi_arvalid;
		assign o_wire_M_AXI_RREADY	= i_wire_data_next[wire_router_index];

		assign o_wire_M_AXI_ARID	= 'b0;
		assign o_wire_M_AXI_ARSIZE	= 3'b10;
		assign o_wire_M_AXI_ARBURST	= 2'b01;
		assign o_wire_M_AXI_ARLOCK	= 1'b0;
		assign o_wire_M_AXI_ARCACHE	= 4'b0010;
		assign o_wire_M_AXI_ARPROT	= 3'h0;
		assign o_wire_M_AXI_ARQOS	= 4'h0;
		


		
		wire [15:0] wire_first_burst_aligned_len;
		assign wire_first_burst_aligned_len = 32-((i_wire_address[wire_router_index*32+:32]>>2)&(32-1));

		task task_idle;
	    if(i_wire_resetn)
		begin
			if((i_wire_address[wire_router_index*32+:32]%4)||i_wire_length[wire_router_index*32+:32]==0)
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
				reg_address<=i_wire_address[wire_router_index*32+:32];
				reg_length<=i_wire_length[wire_router_index*32+:32];
				reg_offset<=0;
				reg_burst_counter<=0;
				reg_state<=`fsm_state_address_write;
				//first axi address
				reg_axi_araddr<=i_wire_address[wire_router_index*32+:32];
				reg_axi_burstlen<=wire_first_burst_aligned_len>i_wire_length[wire_router_index*32+:32]?i_wire_length[wire_router_index*32+:32]:wire_first_burst_aligned_len;
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
		assign wire_burst_aligned_len = 32-(((reg_address>>2)+reg_offset)&(32-1));
		assign wire_burst_aligned_len2 = 32-(((reg_address>>2)+reg_offset+reg_axi_burstlen)&(32-1));
		task task_write_address;
			if(reg_axi_arvalid&&i_wire_M_AXI_ARREADY)
			begin
				reg_axi_araddr<=0;
				reg_axi_arvalid<=0;
				reg_axi_burstlen<=reg_axi_burstlen;

				//first axi data burst
				reg_burst_counter<=0;

				//fsm
				reg_timeout_error<=0;
				reg_state<=`fsm_state_data_read;
			end
			else
			begin
				//next axi address
				reg_axi_araddr<=reg_address+reg_offset*(32>>3);
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
		  	if(i_wire_data_next)
			begin
				if(reg_burst_counter>=reg_axi_burstlen-1)
				begin
					if(reg_offset+reg_axi_burstlen>=reg_length)
					begin
						reg_timeout_error<=0;
						reg_offset<=reg_offset+reg_axi_burstlen;
						reg_state<=`fsm_state_done;
					end
					else
					begin
						reg_timeout_error<=0;
						//last axi data
						reg_offset<=reg_offset+reg_axi_burstlen;
						reg_state<=`fsm_state_address_write;
						//first axi address
						reg_axi_araddr<=reg_address+(reg_offset+reg_axi_burstlen)*(32/8);
						reg_axi_arvalid<=1;
						reg_axi_burstlen<=wire_burst_aligned_len2>wire_reserved_len2?wire_reserved_len2:wire_burst_aligned_len2;
						reg_burst_counter<=0;
					end
				end
				else
				begin
					reg_burst_counter<=reg_burst_counter+1;
					reg_timeout_error<=0;
					reg_state<=reg_state;
				end
			end
			else
			begin
				reg_burst_counter<=reg_burst_counter;
				reg_state<=reg_state;
				reg_timeout_error<=reg_timeout_error+1;
			end
		endtask


		task task_read_data;
			if(i_wire_M_AXI_RVALID&&i_wire_data_next)
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
		always @(*) 
		begin
			case(i_wire_router)
			1:
			begin
				o_wire_data[0*32+:32]=i_wire_M_AXI_RDATA;
				o_wire_data_valid[0*1+:1]=i_wire_M_AXI_RVALID;
				o_wire_data[1*32+:32]=0;
				o_wire_data_valid[1*1+:1]=0;
				o_wire_data[2*32+:32]=0;
				o_wire_data_valid[2*1+:1]=0;
				o_wire_data[3*32+:32]=0;
				o_wire_data_valid[3*1+:1]=0;
			end
			2:
			begin
				
				o_wire_data[0*32+:32]<=0;
				o_wire_data_valid[0*1+:1]<=0;
				o_wire_data[1*32+:32]<=i_wire_M_AXI_RDATA;
				o_wire_data_valid[1*1+:1]<=i_wire_M_AXI_RVALID;
				o_wire_data[2*32+:32]<=0;
				o_wire_data_valid[2*1+:1]<=0;
				o_wire_data[3*32+:32]<=0;
				o_wire_data_valid[3*1+:1]<=0;
			end
			4:
			begin
				o_wire_data[2*32+:32]<=i_wire_M_AXI_RDATA;
				o_wire_data_valid[2*1+:1]<=i_wire_M_AXI_RVALID;
				o_wire_data[0*32+:32]<=0;
				o_wire_data_valid[0*1+:1]<=0;
				o_wire_data[1*32+:32]<=0;
				o_wire_data_valid[1*1+:1]<=0;
				o_wire_data[3*32+:32]<=0;
				o_wire_data_valid[3*1+:1]<=0;
			end
			8:
			begin
				o_wire_data[3*32+:32]<=i_wire_M_AXI_RDATA;
				o_wire_data_valid[3*1+:1]<=i_wire_M_AXI_RVALID;
				o_wire_data[0*32+:32]<=0;
				o_wire_data_valid[0*1+:1]<=0;
				o_wire_data[1*32+:32]<=0;
				o_wire_data_valid[1*1+:1]<=0;
				o_wire_data[2*32+:32]<=0;
				o_wire_data_valid[2*1+:1]<=0;
			end
			default:
			begin
				o_wire_data[0*32+:32]<=0;
				o_wire_data_valid[0*1+:1]<=0;
				o_wire_data[1*32+:32]<=0;
				o_wire_data_valid[1*1+:1]<=0;
				o_wire_data[2*32+:32]<=0;
				o_wire_data_valid[2*1+:1]<=0;
				o_wire_data[3*32+:32]<=0;
				o_wire_data_valid[3*1+:1]<=0;
			end
			endcase
		end

		assign 		o_wire_error=reg_state==`fsm_state_error;
	endmodule