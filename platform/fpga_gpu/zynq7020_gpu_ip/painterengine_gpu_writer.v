`timescale 1 ns / 1 ns

`define FIFO_DEPTH 64
`define DATA_WIDTH 32
`define ADDRESS_WIDTH 32
`define ADDRESS_ALIGN 64





	module painterengine_gpu_dma_writer #
		(
		parameter integer PARAM_DATA_ALIGN	= `ADDRESS_ALIGN,
		parameter integer PARAM_ADDRESS_WIDTH	= `ADDRESS_WIDTH,
		parameter integer PARAM_DATA_WIDTH	= `DATA_WIDTH
		)
		(
		//input clk
		input wire   									i_wire_clock,
		input wire   									i_wire_resetn,
		output wire 									o_wire_done,

		input wire [PARAM_ADDRESS_WIDTH-1:0] 			i_wire_address,
		input wire [31:0] 								i_wire_length,
		
		input wire [PARAM_DATA_WIDTH-1:0] 				i_wire_data,
		input wire 										i_wire_data_valid,
		output wire 									o_wire_data_next,
		output wire 									o_wire_error,
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
		`define fsm_state_idle 3'b000
		`define fsm_state_address_write 3'b001
		`define fsm_state_data_write 3'b010
		`define fsm_state_data_wait_resp 3'b011
		`define fsm_state_done 3'b100
		`define fsm_state_error 3'b111
		
		reg [PARAM_ADDRESS_WIDTH-1 : 0] reg_address=0;
		reg [31:0] 						reg_length;
		reg [31:0] 						reg_offset;
		reg [7:0] 						reg_burst_counter;
		reg [2:0]						reg_state=0;
		reg [15:0]						reg_timeout_error;
		

		////////////////////////////////////////////////////////////
		//axi registers////////////////////////////////////////////
		///////////////////////////////////////////////////////////

		reg [PARAM_ADDRESS_WIDTH-1 : 0] 					reg_axi_awaddr;
		reg 												reg_axi_awvalid;
		reg  												reg_axi_wlast;				
		reg  												reg_axi_bready;
		reg [7:0]											reg_axi_burstlen;

		assign o_wire_error=(reg_state==`fsm_state_error);
		
		
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

		assign o_wire_M_AXI_WSTRB		= 4'b1111;
		assign o_wire_M_AXI_WLAST		= reg_axi_wlast;
		assign o_wire_M_AXI_WUSER		= 'b0;
		assign o_wire_M_AXI_WDATA 		= i_wire_data;
		assign o_wire_M_AXI_WVALID		= i_wire_data_valid&&(reg_state==`fsm_state_data_write);
		assign o_wire_data_next 		= i_wire_M_AXI_WREADY&&i_wire_data_valid;
		assign o_wire_M_AXI_BREADY		= reg_axi_bready;


		wire [15:0] wire_first_burst_aligned_len;
		assign wire_first_burst_aligned_len = PARAM_DATA_ALIGN-((i_wire_address>>2)&(PARAM_DATA_ALIGN-1));

		task task_idle;
		    if(i_wire_resetn)
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
				reg_axi_wlast<=(reg_axi_burstlen==1);

				if(i_wire_data_valid)
				begin
					reg_burst_counter<=1;
				end
				else
				begin
					reg_burst_counter<=0;
				end
				
				//fsm
				reg_timeout_error<=0;
				reg_state<=`fsm_state_data_write;
			end
			else
			begin
				//next axi address
				reg_axi_awaddr<=reg_address+reg_offset*(PARAM_DATA_WIDTH>>3);
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
			if(i_wire_data_valid)
			begin
				if(i_wire_M_AXI_WREADY)
				begin
					//write done,next data
					if (reg_burst_counter>=reg_axi_burstlen) 
					begin
						//end of data
						reg_axi_wlast<=0;
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
						if(i_wire_data_valid)
						begin
							reg_burst_counter<=reg_burst_counter+1;
						end
						else
						begin
							reg_burst_counter<=reg_burst_counter;
						end
						reg_axi_wlast<=(reg_burst_counter==reg_axi_burstlen-1)&&i_wire_data_valid?1:0;
						reg_timeout_error<=0;
						reg_axi_bready<=0;
					end
				end
				else
				begin
					//keep going
					reg_burst_counter<=reg_burst_counter;
				end
			end
			else
			begin
				//keep going
				if (i_wire_data_valid)
				begin
					reg_burst_counter<=reg_burst_counter+1;
					reg_axi_wlast<=(reg_burst_counter==reg_axi_burstlen-1)&&i_wire_data_valid?1:0;
				end
				else
				begin
					reg_burst_counter<=reg_burst_counter;
					reg_axi_wlast<=reg_axi_wlast;
				end
				reg_timeout_error<=reg_timeout_error+1;
				reg_state<=reg_state;
				reg_axi_bready<=reg_axi_bready;
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

		always @(posedge i_wire_clock or negedge i_wire_resetn)
		begin
			if (!i_wire_resetn) 
			begin
				reg_state<=`fsm_state_idle;
				reg_address<=0;
				reg_length<=0;
				reg_offset<=0;
				reg_burst_counter<=0;
				reg_axi_awaddr<=0;
				reg_axi_awvalid<=0;
				reg_axi_burstlen<=0;
				reg_axi_wlast<=0;
				reg_axi_bready<=0;
				reg_timeout_error<=0;
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