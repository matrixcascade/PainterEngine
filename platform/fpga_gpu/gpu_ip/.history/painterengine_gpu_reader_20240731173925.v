`timescale 1 ns / 1 ns


module painterengine_gpu_dma_reader
		(
		//input clk
		input wire   									i_wire_clock,
		input wire   									i_wire_resetn,
		

		input wire  [4*32-1:0] 							i_wire_address,
		input wire  [4*32-1:0] 							i_wire_length,
		
		input wire  [3:0] 								i_wire_router,
		output reg  [4*32-1:0]         					o_wire_data,
		output reg 	[3:0]							    o_wire_data_valid,
		input  wire [3:0]								i_wire_data_next,

		output wire 									o_wire_done,
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

		`define reader_state_routing 					5'h1
		`define reader_state_param_check 				5'h2
		`define reader_state_calc 						5'h3
		`define reader_state_calc2						5'h4
		`define reader_state_calc3						5'h5
		`define reader_state_address_write 				5'h6
		`define reader_state_data_read 					5'h7
		`define reader_state_data_confirm 				5'h8
		`define reader_state_done 						5'h9
		`define reader_state_routing_error 		    	5'b10
		`define reader_state_address_align_error		5'h11
		`define reader_state_zero_length_error			5'h12
		`define reader_state_arresp_error 				5'h13
		`define reader_state_dataresp_error 			5'h14
		`define reader_state_last_data_error 			5'h15
		

		`define reader_error_counter                    16'd256
		

		reg [31:0] 									reg_address;
		reg [31:0] 									reg_waddress;
		reg [31:0] 									reg_length;
		reg [31:0] 									reg_wlength;
		reg [31:0] 									reg_offset;
		reg [8:0] 									reg_burst_counter;
		reg [15:0] 									reg_error_counter;
		reg [4:0]									reg_state;
		reg [31 : 0] 								reg_axi_araddr;
		reg  										reg_axi_arvalid;
		reg [8:0] 									reg_axi_burstlen;
		reg [1:0] 									reg_router_index;
		reg [31:0] 								    reg_reserved_len;
		reg [7:0] 									reg_unalign_size;
		reg [8:0] 									reg_burst_aligned_len;
	
		//Read Address (AR)
		assign o_wire_M_AXI_ARADDR	= reg_axi_araddr;
		assign o_wire_M_AXI_ARLEN	= reg_axi_burstlen - 1;
		assign o_wire_M_AXI_ARVALID	= reg_axi_arvalid;
		assign o_wire_M_AXI_RREADY	= i_wire_data_next[reg_router_index];

		assign o_wire_M_AXI_ARID	= 'b0;
		assign o_wire_M_AXI_ARSIZE	= 3'b10;
		assign o_wire_M_AXI_ARBURST	= 2'b01;
		assign o_wire_M_AXI_ARLOCK	= 1'b0;
		assign o_wire_M_AXI_ARCACHE	= 4'b0010;
		assign o_wire_M_AXI_ARPROT	= 3'h0;
		assign o_wire_M_AXI_ARQOS	= 4'h0;


		task task_routing;
		begin
			case(i_wire_router)
			1:
			begin
				reg_address<=i_wire_address[0*32+:32];
				reg_length<=i_wire_length[0*32+:32];
				reg_router_index<=2'd0;
				reg_state<=`reader_state_param_check;
			end
			2:
			begin
				reg_address<=i_wire_address[1*32+:32];
				reg_length<=i_wire_length[1*32+:32];
				reg_router_index<=2'd1;
				reg_state<=`reader_state_param_check;
			end
			4:
			begin
				reg_address<=i_wire_address[2*32+:32];
				reg_length<=i_wire_length[2*32+:32];
				reg_router_index<=2'd2;
				reg_state<=`reader_state_param_check;
			end
			8:
			begin
				reg_address<=i_wire_address[3*32+:32];
				reg_length<=i_wire_length[3*32+:32];
				reg_router_index<=2'd3;
				reg_state<=`reader_state_param_check;
			end
			default:
			begin
				reg_address<=0;
				reg_length<=0;
				reg_router_index<=2'd0;
				//error
				reg_state<=`reader_state_routing_error;
			end
			endcase
		end
		endtask
	
	
		always @(posedge i_wire_clock or negedge i_wire_resetn)
		begin
			if (!i_wire_resetn) 
			begin
				reg_address<=0;
				reg_waddress<=0;
				reg_length<=0;
				reg_wlength<=0;
				reg_offset<=0;
				reg_burst_counter<=0;
				reg_error_counter<=0;
				reg_state<=`reader_state_routing;
				reg_axi_araddr<=0;
				reg_axi_arvalid<=0;
				reg_axi_burstlen<=0;
				reg_router_index<=0;
				reg_reserved_len<=0;
				reg_unalign_size<=0;
				reg_burst_aligned_len<=0;
			end
			else
			begin
				case (reg_state)
					`reader_state_routing:
					begin
						task_routing;
					end
					`reader_state_param_check:
					begin
						if((reg_address[1:0]))
						begin
							reg_state<=`reader_state_address_align_error;
						end
						else if(reg_length==0)
						begin
							reg_state<=`reader_state_zero_length_error;
						end
						else
						begin
							reg_state<=`reader_state_calc;
						end
					end
					`reader_state_calc:
					begin
						reg_unalign_size<=(reg_address[2+:8]+reg_offset[0+:8]);
						reg_state<=`reader_state_calc2;
					end
					`reader_state_calc2:
					begin
						reg_reserved_len<=reg_length-reg_offset;
						reg_burst_aligned_len<=9'd256-reg_unalign_size;
						reg_state<=`reader_state_calc3;
					end
					`reader_state_calc3:
					begin
						reg_waddress<=reg_address+reg_offset*4;
						reg_wlength<=reg_burst_aligned_len>reg_reserved_len?reg_reserved_len:reg_burst_aligned_len;
						reg_axi_arvalid<=0;
						reg_state<=`reader_state_address_write;
					end
					`reader_state_address_write:
					begin
						if(reg_axi_arvalid&&i_wire_M_AXI_ARREADY)
						begin
							reg_axi_arvalid<=0;
							reg_burst_counter<=0;
							reg_error_counter<=0;
							reg_state<=`reader_state_data_read;
						end
						else
						begin
							reg_axi_araddr<=reg_waddress;
							reg_axi_burstlen<=reg_wlength;
							reg_axi_arvalid<=1;

							if(reg_error_counter<`reader_error_counter)
								reg_error_counter<=reg_error_counter+1;
							else
								reg_state<=`reader_state_arresp_error;
						end
					end
					`reader_state_data_read:
					begin
						if(i_wire_M_AXI_RVALID&&i_wire_data_next[reg_router_index])
						begin
							if(reg_burst_counter==reg_axi_burstlen-1)
							begin
								if (i_wire_M_AXI_RLAST) 
								begin
									reg_error_counter<=0;
									reg_offset<=reg_offset+reg_axi_burstlen;
									reg_state<=`reader_state_data_confirm;
								end
								else
								begin
									reg_state<=`reader_state_last_data_error;
								end
							end
							else
							begin
								reg_state<=reg_state;
								reg_burst_counter<=reg_burst_counter+1;
								reg_error_counter<=0;
							end
						end
						else
						begin
							reg_state<=reg_state;
							if(reg_error_counter<`reader_error_counter)
								reg_error_counter<=reg_error_counter+1;
							else
								reg_state<=`reader_state_dataresp_error;
						end
					end
					`reader_state_data_confirm:
					begin
						if(reg_offset==reg_length)
						begin
							reg_state<=`reader_state_done;
						end
						else
						begin
							reg_state<=`reader_state_calc;
						end
					end
					default:
					begin
						reg_state<=reg_state;
					end
				endcase
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
				
				o_wire_data[0*32+:32]=0;
				o_wire_data_valid[0*1+:1]=0;
				o_wire_data[1*32+:32]=i_wire_M_AXI_RDATA;
				o_wire_data_valid[1*1+:1]=i_wire_M_AXI_RVALID;
				o_wire_data[2*32+:32]=0;
				o_wire_data_valid[2*1+:1]=0;
				o_wire_data[3*32+:32]=0;
				o_wire_data_valid[3*1+:1]=0;
			end
			4:
			begin
				o_wire_data[2*32+:32]=i_wire_M_AXI_RDATA;
				o_wire_data_valid[2*1+:1]=i_wire_M_AXI_RVALID;
				o_wire_data[0*32+:32]=0;
				o_wire_data_valid[0*1+:1]=0;
				o_wire_data[1*32+:32]=0;
				o_wire_data_valid[1*1+:1]=0;
				o_wire_data[3*32+:32]=0;
				o_wire_data_valid[3*1+:1]=0;
			end
			8:
			begin
				o_wire_data[3*32+:32]=i_wire_M_AXI_RDATA;
				o_wire_data_valid[3*1+:1]=i_wire_M_AXI_RVALID;
				o_wire_data[0*32+:32]=0;
				o_wire_data_valid[0*1+:1]=0;
				o_wire_data[1*32+:32]=0;
				o_wire_data_valid[1*1+:1]=0;
				o_wire_data[2*32+:32]=0;
				o_wire_data_valid[2*1+:1]=0;
			end
			default:
			begin
				o_wire_data[0*32+:32]=0;
				o_wire_data_valid[0*1+:1]=0;
				o_wire_data[1*32+:32]=0;
				o_wire_data_valid[1*1+:1]=0;
				o_wire_data[2*32+:32]=0;
				o_wire_data_valid[2*1+:1]=0;
				o_wire_data[3*32+:32]=0;
				o_wire_data_valid[3*1+:1]=0;
			end
			endcase
		end

		assign o_wire_done=(reg_state==`reader_state_done);
		assign o_wire_error=reg_state[4];
	endmodule