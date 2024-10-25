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
		
		`define writer_state_routing 				5'h01
		`define writer_state_param_check 			5'h02
		`define writer_state_calc 					5'h03
		`define writer_state_calc2 					5'h03
		`define writer_state_calc3 					5'h03
		`define writer_state_address_write 			5'h04
		`define writer_state_data_write 			5'h05
		`define writer_state_data_wait_resp 		5'h06
		`define writer_state_done 					5'h07
		`define writer_state_routing_error 			5'h10
		`define writer_state_address_align_error	5'h11
		`define writer_state_length_error			5'h12
		`define writer_state_arresp_error			5'h13
		`define writer_state_dataaccept_error		5'h14
		`define writer_state_dataresp_error			5'h15

		`define writer_error_counter                16'd256


		reg	[1:0] 						reg_router_index;
		reg	[7:0] 						reg_router_bit_index;
		reg [31:0] 						reg_address;
		reg [31:0] 						reg_waddress;
		reg [31:0] 						reg_length;
		reg [31:0] 						reg_wlength;
		reg [31:0] 						reg_offset;
		reg [8:0] 						reg_burst_counter;
		reg [2:0]						reg_state=0;
		reg [15:0]						reg_error_counter;

		reg [31 : 0] 					reg_axi_awaddr;
		reg 							reg_axi_awvalid;
		reg  							reg_axi_wlast;				
		reg  							reg_axi_bready;
		reg [8:0]						reg_axi_burstlen;

		//calc address fsm
		reg  [31:0] 					reg_reserved_len;
		reg  [7:0] 						reg_unalign_size;
		reg  [8:0] 						reg_burst_aligned_len;

	
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
		
		assign o_wire_M_AXI_WVALID		= i_wire_data_valid[reg_router_index]&&(reg_state==`writer_state_data_write);

		assign o_wire_data_next[0] 		= i_wire_M_AXI_WREADY&&i_wire_data_valid[0]&&(reg_state==`writer_state_data_write);
		assign o_wire_data_next[1] 		= i_wire_M_AXI_WREADY&&i_wire_data_valid[1]&&(reg_state==`writer_state_data_write);
		assign o_wire_data_next[2] 		= i_wire_M_AXI_WREADY&&i_wire_data_valid[2]&&(reg_state==`writer_state_data_write);
		assign o_wire_data_next[3] 		= i_wire_M_AXI_WREADY&&i_wire_data_valid[3]&&(reg_state==`writer_state_data_write);
		
		assign o_wire_M_AXI_BREADY		= reg_axi_bready;

		task task_routing;
			case(i_wire_router)
			1:
			begin
				reg_router_index<=2'd0;
				reg_router_bit_index<=8'd0;
				reg_address<=i_wire_address[0*32+:32];
				reg_length<=i_wire_length[0*32+:32];
				
				reg_state<=`writer_state_param_check;
			end
			2:
			begin
				reg_router_index<=2'd1;
				reg_router_bit_index<=8'd32;
				reg_address<=i_wire_address[1*32+:32];
				reg_length<=i_wire_length[1*32+:32];
				reg_state<=`writer_state_param_check;
			end
			4:
			begin
				reg_router_index<=2'd2;
				reg_router_bit_index<=8'd64;
				reg_address<=i_wire_address[2*32+:32];
				reg_length<=i_wire_length[2*32+:32];
				reg_state<=`writer_state_param_check;
			end
			8:
			begin
				reg_router_index<=2'd3;
				reg_router_bit_index<=8'd96;
				reg_address<=i_wire_address[3*32+:32];
				reg_length<=i_wire_length[3*32+:32];
				reg_state<=`writer_state_param_check;
			end
			default:
			begin
				reg_address<=0;
				reg_length<=0;
				reg_router_index<=2'd0;
				reg_router_bit_index<=8'd0;
				//error
				reg_state<=`writer_state_error;
			end
			endcase
		endtask


		task process;
				case (reg_state)
					`writer_state_routing:
					begin
						task_routing;
					end
					`writer_state_param_check:
					begin
						if(reg_address[1:0])
						begin
							reg_state<=`writer_state_address_align_error;
						end
						else if(reg_length==0)
						begin
							reg_state<=`writer_state_length_error
						end
						else
						begin
							reg_state<=`writer_state_calc;
						end
					end
					`writer_state_calc:
					begin
						reg_unalign_size<=(reg_address[2+:8]+reg_offset[0+:8]);
						reg_state<=`writer_state_calc2;
					end
					`writer_state_calc2:
					begin
						reg_burst_aligned_len<=9'd256-reg_unalign_size;
						reg_reserved_len<=reg_length-reg_offset;
						reg_state<=`writer_state_calc3;
					end
					`writer_state_calc3:
					begin
						reg_waddress<=reg_address+reg_offset*4;
						reg_wlength<=reg_burst_aligned_len>reg_reserved_len?reg_reserved_len:reg_burst_aligned_len;
						reg_axi_awvalid<=0;
						reg_state<=`writer_state_address_write;
					end
					`writer_state_address_write:
					begin
						if(reg_axi_awvalid&&i_wire_M_AXI_AWREADY)
						begin
							reg_axi_awaddr<=reg_axi_awaddr;
							reg_axi_awvalid<=0;
							reg_burst_counter<=0;
							reg_error_counter<=0;
							reg_state<=`writer_state_data_write;
						end
						else
						begin
							reg_axi_awaddr<=reg_waddress;
							reg_axi_burstlen<=reg_wlength;
							reg_axi_awvalid<=1;
							reg_burst_counter<=0;
							reg_state<=reg_state;
							if(reg_error_counter<`writer_error_counter)
								reg_error_counter<=reg_error_counter+1;
							else
								reg_state<=`writer_state_arresp_error;
						end
					end
					`writer_state_data_write:
					begin
						if(i_wire_data_valid[reg_router_index]&&i_wire_M_AXI_WREADY)
						begin
							//write done
							if (reg_burst_counter==reg_axi_burstlen) 
							begin
								//end of data
								reg_axi_wlast<=0;
								reg_error_counter<=0;
								reg_offset<=reg_offset+reg_axi_burstlen;
								reg_burst_counter<=0;
								//wait response
								reg_axi_bready<=1;
								reg_state<=`writer_state_data_wait_resp;
							end
							else
							begin
								reg_burst_counter<=reg_burst_counter+1;
								reg_axi_wlast<=((reg_burst_counter==reg_axi_burstlen-1)&&i_wire_data_valid[reg_router_index]);
								reg_error_counter<=0;
								reg_axi_bready<=0;
							end
						end
						else
						begin
							reg_burst_counter<=reg_burst_counter;
							reg_axi_wlast<=reg_axi_wlast;
							reg_state<=reg_state;
							reg_axi_bready<=reg_axi_bready;
							if(reg_error_counter<`writer_error_counter)
								reg_error_counter<=reg_error_counter+1;
							else
								reg_state<=`writer_state_dataaccept_error;
						end
					end
					`writer_state_data_wait_resp:
					begin
						if (i_wire_M_AXI_BVALID) 
						begin
							reg_error_counter<=0;
							if (i_wire_M_AXI_BRESP<=2'b01) 
							begin
								if (reg_offset>=reg_length) 
								begin
									reg_axi_bready<=0;
									reg_state<=`writer_state_done;
								end
								else
								begin
									reg_axi_bready<=0;
									reg_state<=`writer_state_calc;
								end
							end
							else
							begin
								reg_axi_bready<=0;
								reg_state<=`writer_state_dataresp_error;
							end
						end
						else
						begin
							reg_error_counter<=reg_error_counter+1;
							reg_state<=reg_state;
						end
					end
					default:
					begin
						reg_state<=reg_state;
					end
				endcase
		endtask

		always @(posedge i_wire_clock or negedge i_wire_resetn)
		begin
			if (!i_wire_resetn) 
			begin
				reg_router_index<=0;
				reg_router_bit_index<=0;
				reg_address<=0;
				reg_waddress<=0;
				reg_length<=0;
				reg_wlength<=0;
				reg_offset<=0;
				reg_burst_counter<=0;
				reg_state<=`writer_state_routing;
				reg_error_counter<=0;
				reg_axi_awaddr<=0;
				reg_axi_awvalid<=0;
				reg_axi_wlast<=0;				
				reg_axi_bready<=0;
				reg_axi_burstlen<=0;
				reg_reserved_len<=0;
				reg_unalign_size<=0;
				reg_burst_aligned_len<=0;
			end
			else
			begin
				process;
			end
		end

	assign o_wire_error=(reg_state[4]);
	assign o_wire_done=(reg_state==`writer_state_done);
	endmodule