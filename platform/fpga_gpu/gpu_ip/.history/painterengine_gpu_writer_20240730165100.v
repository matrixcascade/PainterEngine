`timescale 1 ns / 1 ns


	module painterengine_gpu_dma_writer #
		(
		parameter integer PARAM_DATA_ALIGN	= 32
		)
		(
		//input clk
		input wire   									i_wire_clock,
		input wire   									i_wire_resetn,
		input wire [3:0]								i_wire_router,
		output wire 									o_wire_done,

		input wire [32*4-1:0] 							i_wire_address,
		input wire [32*4-1:0] 							i_wire_length,
		
		input wire [32*4-1:0] 							i_wire_data,
		input wire [3:0]								i_wire_data_valid,
		output wire[3:0]								o_wire_data_next,
		output wire 									o_wire_error,
		output wire[2:0]								o_wire_error_type,
		///////////////////////////////////////////////////////////////////////////////////////////
		//AXI full ports
		///////////////////////////////////////////////////////////////////////////////////////////
		output wire [0 : 0]								o_wire_M_AXI_AWID,//assign to 0
		output wire [31 : 0] 							o_wire_M_AXI_AWADDR,//assign to axi_awaddr
		output wire [7 : 0] 							o_wire_M_AXI_AWLEN,//assign to axi_burstlen-1
		output wire [2 : 0] 							o_wire_M_AXI_AWSIZE,//assign to 32bit-'b101
		output wire [1 : 0] 							o_wire_M_AXI_AWBURST,//assign to 2'b01
		output wire  									o_wire_M_AXI_AWLOCK,//assign to 1'b0
		output wire [3 : 0] 							o_wire_M_AXI_AWCACHE,//assign to 4'b0010
		output wire [2 : 0] 							o_wire_M_AXI_AWPROT,//assign to 3'h0
		output wire [3 : 0] 							o_wire_M_AXI_AWQOS,//assign to 4'h0
		output wire  									o_wire_M_AXI_AWVALID,//assign to axi_awvalid
		input wire 										i_wire_M_AXI_AWREADY,
		output wire [31 : 0] 							o_wire_M_AXI_WDATA,//assign to axi_wdata
		output wire [32/8-1 : 0] 						o_wire_M_AXI_WSTRB,//assign to {(PIXEL/8){1'b1}}
		output wire  									o_wire_M_AXI_WLAST,//assign to axi_wlast
		output wire 		 							o_wire_M_AXI_WVALID,//assign to axi_wvalid
		input wire  									i_wire_M_AXI_WREADY,

		input wire [0 : 0]								i_wire_M_AXI_BID,//assign to 0
		input wire [1 : 0] 								i_wire_M_AXI_BRESP,//assign to 2'b00
		input wire  									i_wire_M_AXI_BVALID,
		output wire 									o_wire_M_AXI_BREADY
		);
		
		`define writer_fsm_state_routing 3'b000
		`define writer_fsm_state_param_check 3'b001
		`define writer_fsm_state_calc_address 3'b010
		`define writer_fsm_state_address_write 3'b011
		`define writer_fsm_state_data_write 3'b100
		`define writer_fsm_state_data_wait_resp 3'b101
		`define writer_fsm_state_done 3'b110
		`define writer_fsm_state_error 3'b111

		`define writer_error_ok 3'b000
		`define writer_error_type_router_error 3'b001
		`define writer_error_address_error 3'b010
		`define writer_error_address_response_error 3'b011
		`define writer_error_data_response_timeout 3'b100

		reg[1:0] reg_router_index;
		reg[7:0] reg_router_bit_index;

		reg [31 : 0] 					reg_address;
		reg [31:0] 						reg_length;
		reg [31:0] 						reg_offset;
		reg [7:0] 						reg_burst_counter;
		reg [2:0]						reg_state=0;
		reg [15:0]						reg_timeout_error;
		reg [2:0]						reg_error_type;
		

		////////////////////////////////////////////////////////////
		//axi registers////////////////////////////////////////////
		///////////////////////////////////////////////////////////

		reg [31 : 0] 					reg_axi_awaddr;
		reg 							reg_axi_awvalid;
		reg  							reg_axi_wlast;				
		reg  							reg_axi_bready;
		reg [7:0]						reg_axi_burstlen;

		assign o_wire_error=(reg_state==`writer_fsm_state_error);
		assign o_wire_error_type=reg_error_type;
		
	
		reg reg_axi_arvalid;
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
		assign o_wire_M_AXI_WDATA 		= i_wire_data[reg_router_bit_index+:32];
		
		assign o_wire_M_AXI_WVALID		= i_wire_data_valid[reg_router_index]&&(reg_state==`writer_fsm_state_data_write);

		assign o_wire_data_next[0] 		= i_wire_M_AXI_WREADY&&i_wire_data_valid[0]&&(reg_state==`writer_fsm_state_data_write);
		assign o_wire_data_next[1] 		= i_wire_M_AXI_WREADY&&i_wire_data_valid[1]&&(reg_state==`writer_fsm_state_data_write);
		assign o_wire_data_next[2] 		= i_wire_M_AXI_WREADY&&i_wire_data_valid[2]&&(reg_state==`writer_fsm_state_data_write);
		assign o_wire_data_next[3] 		= i_wire_M_AXI_WREADY&&i_wire_data_valid[3]&&(reg_state==`writer_fsm_state_data_write);
		
		assign o_wire_M_AXI_BREADY		= reg_axi_bready;

		task task_routing;
			case(i_wire_router)
			1:
			begin
				reg_router_index<=2'd0;
				reg_router_bit_index<=8'd0;
				reg_address<=i_wire_address[0*32+:32];
				reg_length<=i_wire_length[0*32+:32];
				
				reg_state<=`writer_fsm_state_param_check;
			end
			2:
			begin
				reg_router_index<=2'd1;
				reg_router_bit_index<=8'd32;
				reg_address<=i_wire_address[1*32+:32];
				reg_length<=i_wire_length[1*32+:32];
				reg_state<=`writer_fsm_state_param_check;
			end
			4:
			begin
				reg_router_index<=2'd2;
				reg_router_bit_index<=8'd64;
				reg_address<=i_wire_address[2*32+:32];
				reg_length<=i_wire_length[2*32+:32];
				reg_state<=`writer_fsm_state_param_check;
			end
			8:
			begin
				reg_router_index<=2'd3;
				reg_router_bit_index<=8'd96;
				reg_address<=i_wire_address[3*32+:32];
				reg_length<=i_wire_length[3*32+:32];
				reg_state<=`writer_fsm_state_param_check;
			end
			default:
			begin
				reg_address<=0;
				reg_length<=0;
				reg_router_index<=2'd0;
				reg_router_bit_index<=8'd0;
				//error
				reg_state<=`writer_fsm_state_error;
				reg_error_type<=`reader_error_type_router_error;
			end
			endcase
		endtask

		task task_param_check;
			if((reg_address[1:0])||reg_length==0)
			begin
				reg_timeout_error<=0;
				reg_offset<=0;
				reg_burst_counter<=0;
				reg_state<=`writer_fsm_state_error;
				reg_error_type<=`writer_error_address_error;
				reg_axi_awaddr<=0;
				reg_axi_awvalid<=0;
				reg_axi_burstlen<=0;
			end
			else
			begin
				//confirm address
				reg_timeout_error<=0;
				reg_offset<=0;
				reg_burst_counter<=0;
				reg_address<=reg_address;
				reg_length<=reg_length;
				reg_state<=`writer_fsm_state_calc_address;
				reg_axi_awaddr<=0;
				reg_axi_awvalid<=0;
				reg_axi_burstlen<=0;
			end
		endtask

		//calc address fsm
		reg  [31:0] reg_reserved_len;
		wire [7:0] wire_unalign_size=(reg_address[2+:8]+reg_offset[0+:8]);
		reg  [7:0] reg_burst_aligned_len;

		task task_calc_address;
		begin
			reg_reserved_len<=reg_length-reg_offset;
			reg_burst_aligned_len<=6'd32-wire_unalign_size;
			reg_state<=`fsm_state_address_write;
		end
		endtask

		task task_write_address;
			if(reg_axi_awvalid&&i_wire_M_AXI_AWREADY)
			begin
				reg_axi_awaddr<=reg_axi_awaddr;
				reg_axi_awvalid<=0;
				
				reg_axi_burstlen<=reg_axi_burstlen;

				//first axi data burst
				reg_axi_wlast<=(reg_axi_burstlen==1);

				if(i_wire_data_valid[reg_router_index])
				begin
					reg_burst_counter<=1;
				end
				else
				begin
					reg_burst_counter<=0;
				end
				
				//fsm
				reg_timeout_error<=0;
				reg_state<=`writer_fsm_state_data_write;
			end
			else
			begin
				//next axi address
				reg_axi_awaddr<=reg_address+reg_offset*4;
				reg_axi_awvalid<=1;
				reg_axi_burstlen<=reg_burst_aligned_len>reg_reserved_len?reg_reserved_len:reg_burst_aligned_len;
				reg_burst_counter<=0;
				//error
				reg_state<=reg_state;
				reg_timeout_error<=reg_timeout_error+1;
			end
		endtask

		//write data fsm
		task task_write_data;
			if(i_wire_data_valid[reg_router_index])
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
						reg_state<=`writer_fsm_state_data_wait_resp;
					end
					else
					begin
						//next axi data burst
						if(i_wire_data_valid[reg_router_index])
						begin
							reg_burst_counter<=reg_burst_counter+1;
						end
						else
						begin
							reg_burst_counter<=reg_burst_counter;
						end
						reg_axi_wlast<=(reg_burst_counter==reg_axi_burstlen-1)&&i_wire_data_valid[reg_router_index]?1:0;
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
				if (i_wire_data_valid[reg_router_index])
				begin
					reg_burst_counter<=reg_burst_counter+1;
					reg_axi_wlast<=(reg_burst_counter==reg_axi_burstlen-1)&&i_wire_data_valid[reg_router_index]?1:0;
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
						reg_state<=`writer_fsm_state_done;
					end
					else
					begin
						reg_axi_bready<=0;
						reg_state<=`writer_fsm_state_calc_address;
					end
				end
				else
				begin
					reg_axi_bready<=0;
					reg_state<=`writer_fsm_state_error;
					reg_error_type<=`writer_error_address_response_error;
				end
			end
			else
			begin
				reg_timeout_error<=reg_timeout_error+1;
				reg_state<=reg_state;
			end
		endtask

		//fsm

		assign o_wire_done=(reg_state==`writer_fsm_state_done);

		task fsm_process;
				case (reg_state)
					`writer_fsm_state_routing:
					begin
						task_routing;
					end
					`writer_fsm_state_param_check:
					begin
						task_param_check;
					end
					`writer_fsm_state_calc_address:
					begin
						task_calc_address;
					end
					`writer_fsm_state_address_write:
					begin
						task_write_address;
					end
					`writer_fsm_state_data_write:
					begin
						task_write_data;
					end
					`writer_fsm_state_data_wait_resp:
					begin
						task_wait_resp;
					end
					`writer_fsm_state_done:
					begin
						reg_timeout_error<=0;
						reg_state<=reg_state;
					end
					`writer_fsm_state_error:
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
				reg_state<=`writer_fsm_state_routing;
				reg_address<=0;
				reg_length<=0;
				reg_offset<=0;
				reg_burst_counter<=0;
				reg_axi_awaddr<=0;
				reg_axi_awvalid<=0;
				reg_axi_burstlen<=0;
				reg_axi_wlast<=0;
				reg_axi_bready<=0;
				reg_router_index<=0;
				reg_timeout_error<=0;
				reg_error_type<=`writer_error_ok;
				reg_router_bit_index<=0;
				reg_burst_aligned_len<=0;
				reg_reserved_len<=0;
			end
			else
			begin
				if (reg_state!=`writer_fsm_state_error)
				begin
					if(reg_timeout_error==65535)
					begin
						case(reg_state)
							`writer_fsm_state_routing:
							begin
								reg_state<=`writer_fsm_state_error;
								reg_error_type<=`writer_error_type_router_error;
							end
							`writer_fsm_state_param_check:
							begin
								reg_state<=`writer_fsm_state_error;
								reg_error_type<=`writer_error_address_error;
							end
							`writer_fsm_state_calc_address:
							begin
								reg_state<=`writer_fsm_state_error;
								reg_error_type<=`writer_error_address_response_error;
							end
							`writer_fsm_state_address_write:
							begin
								reg_state<=`writer_fsm_state_error;
								reg_error_type<=`writer_error_address_response_error;
							end
							`writer_fsm_state_data_write:
							begin
								reg_state<=`writer_fsm_state_error;
								reg_error_type<=`writer_error_data_response_timeout;
							end
							`writer_fsm_state_data_wait_resp:
							begin
								reg_state<=`writer_fsm_state_error;
								reg_error_type<=`writer_error_data_response_timeout;
							end
							default:
							begin
								reg_state<=`writer_fsm_state_error;
								reg_error_type<=reg_error_type;
							end
						endcase
					end
					else
					begin
						fsm_process;
					end
				end
				else
				begin
					reg_state<=`writer_fsm_state_error;
				end
			end
		end
	endmodule