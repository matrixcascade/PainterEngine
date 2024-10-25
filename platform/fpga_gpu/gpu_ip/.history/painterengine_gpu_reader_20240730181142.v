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
		output reg 	[3:0]							    o_wire_data_valid,
		input  wire [3:0]								i_wire_data_next,
		output wire  									o_wire_error,
		output wire [2:0]							    o_wire_error_type,
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

		`define fsm_state_routing 3'b000
		`define fsm_state_param_check 3'b001
		`define fsm_state_calc_address 3'b010
		`define fsm_state_address_write 3'b011
		`define fsm_state_address_write2 3'b100
		`define fsm_state_data_read 3'b101
		`define fsm_state_done 3'b110
		`define fsm_state_error 3'b111
		
		`define reader_error_type_ok 3'b000
		`define reader_error_type_router_error 3'b001
		`define reader_error_type_address_error 3'b010
		`define reader_error_type_address_response_timeout 3'b011
		`define reader_error_type_data_response_timeout 3'b100
		`define reader_error_type_protocal_error 3'b101



		reg[31:0] 										reg_address;
		reg[31:0] 										reg_length;
		reg [31:0] 										reg_offset;
		reg [8:0] 										reg_burst_counter;
		reg [18:0] 										reg_timeout_error;
		reg [2:0]										reg_state;
		reg [2:0]										reg_error_type;
		reg [31 : 0] 									reg_axi_araddr;
		reg  											reg_axi_arvalid;
		reg [8:0] 										reg_axi_burstlen;
		reg [1:0] 										reg_router_index;
	
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
		

		
		
		assign o_wire_error_type=reg_error_type;


		task task_routing;
		begin
			case(i_wire_router)
			1:
			begin
				reg_address<=i_wire_address[0*32+:32];
				reg_length<=i_wire_length[0*32+:32];
				reg_router_index<=2'd0;
				reg_state<=`fsm_state_param_check;
			end
			2:
			begin
				reg_address<=i_wire_address[1*32+:32];
				reg_length<=i_wire_length[1*32+:32];
				reg_router_index<=2'd1;
				reg_state<=`fsm_state_param_check;
			end
			4:
			begin
				reg_address<=i_wire_address[2*32+:32];
				reg_length<=i_wire_length[2*32+:32];
				reg_router_index<=2'd2;
				reg_state<=`fsm_state_param_check;
			end
			8:
			begin
				reg_address<=i_wire_address[3*32+:32];
				reg_length<=i_wire_length[3*32+:32];
				reg_router_index<=2'd3;
				reg_state<=`fsm_state_param_check;
			end
			default:
			begin
				reg_address<=0;
				reg_length<=0;
				reg_router_index<=2'd0;
				//error
				reg_state<=`fsm_state_error;
				reg_error_type<=`reader_error_type_router_error;
			end
			endcase
		end
		endtask

		task task_param_check;
		begin
			//check address
			if((reg_address[1:0])||!reg_length)
			begin
				reg_timeout_error<=0;
				reg_offset<=0;
				reg_burst_counter<=0;
				reg_state<=`fsm_state_error;
				reg_error_type<=`reader_error_type_address_error;
				reg_axi_araddr<=0;
				reg_axi_arvalid<=0;
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
				reg_state<=`fsm_state_calc_address;
				reg_axi_araddr<=0;
				reg_axi_arvalid<=0;
				reg_axi_burstlen<=0;
			end
		end
		endtask
	
		//calc address fsm
		reg  [31:0] reg_reserved_len;
		reg [7:0] reg_unalign_size;
		reg  [8:0] reg_burst_aligned_len;

		task task_calc_address;
		begin
			reg_unalign_size<=(reg_address[2+:8]+reg_offset[0+:8]);;
			reg_state<=`fsm_state_address_write;
		end
		endtask


		task task_calc_address2;
		begin
			reg_reserved_len<=reg_length-reg_offset;
			reg_burst_aligned_len<=9'd256-reg_unalign_size;
			reg_state<=`fsm_state_address_write;
		end
		endtask

		//write address fsm

		task task_write_address;
			if(reg_axi_arvalid&&i_wire_M_AXI_ARREADY)
			begin
				reg_axi_araddr<=reg_axi_araddr;
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
				reg_axi_araddr<=reg_address+reg_offset*4;
				reg_axi_arvalid<=1;
				reg_axi_burstlen<=reg_burst_aligned_len>reg_reserved_len?reg_reserved_len:reg_burst_aligned_len;
				reg_burst_counter<=0;

				//error
				reg_state<=reg_state;
				reg_timeout_error<=reg_timeout_error+1;
			end
		endtask


		task task_read_data;
			if(i_wire_M_AXI_RVALID&&i_wire_data_next[reg_router_index])
			begin
				if(reg_burst_counter>=reg_axi_burstlen-1)
				begin
					if (i_wire_M_AXI_RLAST) 
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
							reg_state<=`fsm_state_calc_address;
						end
					end
					else
					begin
						//error
						reg_timeout_error<=reg_timeout_error;
						reg_state<=`fsm_state_error;
						reg_error_type<=`reader_error_type_protocal_error;
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
				reg_state<=reg_state;
				reg_timeout_error<=reg_timeout_error+1;
			end
		endtask

		assign o_wire_done=(reg_state==`fsm_state_done);

		task fsm_process;
				case (reg_state)
					`fsm_state_routing:
					begin
						task_routing;
					end
					`fsm_state_param_check:
					begin
						task_param_check;
					end
					`fsm_state_calc_address:
					begin
						task_calc_address;
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
						reg_error_type<=`reader_error_type_ok;
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
				reg_state<=`fsm_state_routing;
				reg_address<=0;
				reg_length<=0;
				reg_offset<=0;
				reg_burst_counter<=0;
				reg_timeout_error<=0;
				reg_axi_araddr<=0;
				reg_axi_arvalid<=0;
				reg_axi_burstlen<=0;
				reg_router_index<=0;
				reg_error_type<=`reader_error_type_ok;
				reg_reserved_len<=0;
				reg_burst_aligned_len<=0;
			end
			else
			begin
				if (reg_state!=`fsm_state_error)
				begin
					if(reg_timeout_error[18])
					begin
						case(reg_state)
							`fsm_state_address_write:
							begin
								reg_state<=`fsm_state_error;
								reg_error_type<=`reader_error_type_address_response_timeout;
							end
							`fsm_state_data_read:
							begin
								reg_state<=`fsm_state_error;
								reg_error_type<=`reader_error_type_data_response_timeout;
							end
							default:
							begin
								reg_state<=`fsm_state_error;
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

		assign 		o_wire_error=reg_state==`fsm_state_error;
	endmodule