
`timescale 1 ns / 1 ns

	`define FIFO_DEPTH 64
	`define DATA_WIDTH 32
	`define ADDRESS_WIDTH 32
	`define ADDRESS_ALIGN 64
	`define RW_CACHE_SIZE 256

	`define VIDEO_DISPLAY_MODE_1280_720   3'b000
	`define VIDEO_DISPLAY_MODE_480_272    3'b001
	`define VIDEO_DISPLAY_MODE_640_480    3'b010
	`define VIDEO_DISPLAY_MODE_800_480    3'b011
	`define VIDEO_DISPLAY_MODE_800_600    3'b100
	`define VIDEO_DISPLAY_MODE_1024_768   3'b101
	`define VIDEO_DISPLAY_MODE_1920_1080  3'b110

	`define DVI_RGB_MODE_ARGB 3'b00
	`define DVI_RGB_MODE_RGBA 3'b01
	`define DVI_RGB_MODE_ABGR 3'b10
	`define DVI_RGB_MODE_BGRA 3'b11

	`define GPU_OPCODE_NOP 		 	32'h00000000
	`define GPU_OPCODE_GETVERSION 	32'h00000001
	`define GPU_OPCODE_RESET 		32'h00000002
	`define GPU_OPCODE_MEMCPY   	32'h00000003

	`define GPU_STATE_IDLE   		32'h00000000
	`define GPU_STATE_PROCESSING	32'h00000001
	`define GPU_STATE_ERROR    		32'h00000002
	`define GPU_STATE_DONE    		32'h00000003

	module painterengine_gpu_pixel_clock
	(
		input wire i_wire_5x_pixel_clock,
		input wire i_wire_resetn,
		output wire o_wire_pixel_clock
	);
		reg 	[2:0]reg_count;
		reg 	reg_pixel_clock;
		assign 	o_wire_pixel_clock=reg_pixel_clock;
		
    	always@(posedge i_wire_5x_pixel_clock or negedge i_wire_resetn)
		begin
			if(!i_wire_resetn)
			begin
				reg_count<=3'd0;
				reg_pixel_clock<=1'b0;
			end
			else
			begin
				if(reg_count>=3'd4)
				begin 
					reg_pixel_clock<=1'b1;
					reg_count<=3'd0;
				end
       		else 
				begin  
					reg_pixel_clock<=1'b0; 
					reg_count<=reg_count+1'b1;
				end
			end
    	end
	endmodule


	module painterengine_gpu #
		(
		parameter integer PARAM_DATA_ALIGN	= 64,
		parameter integer PARAM_ADDRESS_WIDTH	= `ADDRESS_WIDTH,
		parameter integer PARAM_DATA_WIDTH	= `DATA_WIDTH,
		parameter integer PARAM_CACHE_MAX_SIZE	= 256
		)
		(
		//input clk
		input wire   									i_wire_axi_clock,
		input wire 										i_wire_5x_pixel_clock,   //5x pixel clock
		
		//enable
		input wire   			i_wire_resetn,

		//hdmi dvi
		output wire [2:0] 	TMDS_DATA_p,
    	output wire [2:0] 	TMDS_DATA_n,
    	output wire     	TMDS_CLK_p,
    	output wire     	TMDS_CLK_n,
		/////////////////////////////////////////////////////////////
		//axi ports
		/////////////////////////////////////////////////////////////

			//axi4lite slaver signals
			input wire [PARAM_ADDRESS_WIDTH-1 : 0] S_AXI_AWADDR,
			input wire [2 : 0] S_AXI_AWPROT,
			input wire  S_AXI_AWVALID,
			output wire  S_AXI_AWREADY,
			input wire [PARAM_DATA_WIDTH-1 : 0] S_AXI_WDATA,
			input wire [(PARAM_DATA_WIDTH/8)-1 : 0] S_AXI_WSTRB,
			input wire  S_AXI_WVALID,
			output wire  S_AXI_WREADY,
			output wire [1 : 0] S_AXI_BRESP,
			output wire  S_AXI_BVALID,
			input wire  S_AXI_BREADY,
			input wire [PARAM_ADDRESS_WIDTH-1 : 0] S_AXI_ARADDR,
			input wire [2 : 0] S_AXI_ARPROT,
			input wire  S_AXI_ARVALID,
			output wire  S_AXI_ARREADY,
			output wire [PARAM_DATA_WIDTH-1 : 0] S_AXI_RDATA,
			output wire [1 : 0] S_AXI_RRESP,
			output wire  S_AXI_RVALID,
			input wire  S_AXI_RREADY,


			//axi4full master signals
			output wire m00_axi_awid,
			output wire [PARAM_ADDRESS_WIDTH-1 : 0] m00_axi_awaddr,
			output wire [7 : 0] m00_axi_awlen,
			output wire [2 : 0] m00_axi_awsize,
			output wire [1 : 0] m00_axi_awburst,
			output wire  m00_axi_awlock,
			output wire [3 : 0] m00_axi_awcache,
			output wire [2 : 0] m00_axi_awprot,
			output wire [3 : 0] m00_axi_awqos,
			output wire  m00_axi_awvalid,
			input wire  m00_axi_awready,
			output wire [PARAM_DATA_WIDTH-1 : 0] m00_axi_wdata,
			output wire [PARAM_DATA_WIDTH/8-1 : 0] m00_axi_wstrb,
			output wire  m00_axi_wlast,
			output wire  m00_axi_wvalid,
			input wire  m00_axi_wready,
			input wire  m00_axi_bid,
			input wire  [1 : 0] m00_axi_bresp,
			input wire  m00_axi_bvalid,
			output wire m00_axi_bready,
			output wire m00_axi_arid,
			output wire [PARAM_ADDRESS_WIDTH-1 : 0] m00_axi_araddr,
			output wire [7 : 0] m00_axi_arlen,
			output wire [2 : 0] m00_axi_arsize,
			output wire [1 : 0] m00_axi_arburst,
			output wire  m00_axi_arlock,
			output wire [3 : 0] m00_axi_arcache,
			output wire [2 : 0] m00_axi_arprot,
			output wire [3 : 0] m00_axi_arqos,
			output wire  m00_axi_arvalid,
			input wire  m00_axi_arready,
			input wire  m00_axi_rid,
			input wire [PARAM_DATA_WIDTH-1 : 0] m00_axi_rdata,
			input wire [1 : 0] m00_axi_rresp,
			input wire  m00_axi_rlast,
			input wire  m00_axi_rvalid,
			output wire  m00_axi_rready
		);

		
		/////////////////////////////////////////////////////
		//gpu registers
		/////////////////////////////////////////////////////
	
		/////////////////////////////////////////////////////
		//interface
		wire [PARAM_DATA_WIDTH-1:0] r_wire_parameter0;
		wire [PARAM_DATA_WIDTH-1:0] r_wire_parameter1;
		wire [PARAM_DATA_WIDTH-1:0] r_wire_parameter2;
		wire [PARAM_DATA_WIDTH-1:0] r_wire_parameter3;
		wire [PARAM_DATA_WIDTH-1:0] r_wire_parameter4;
		wire [PARAM_DATA_WIDTH-1:0] r_wire_parameter5;
		wire [PARAM_DATA_WIDTH-1:0] r_wire_parameter6;
		wire [PARAM_DATA_WIDTH-1:0] r_wire_parameter7;

		wire [PARAM_DATA_WIDTH-1:0] w_wire_parameter0;
		wire [PARAM_DATA_WIDTH-1:0] w_wire_parameter1;
		wire [PARAM_DATA_WIDTH-1:0] w_wire_parameter2;
		wire [PARAM_DATA_WIDTH-1:0] w_wire_parameter3;
		wire [PARAM_DATA_WIDTH-1:0] w_wire_parameter4;
		wire [PARAM_DATA_WIDTH-1:0] w_wire_parameter5;
		wire [PARAM_DATA_WIDTH-1:0] w_wire_parameter6;
		wire [PARAM_DATA_WIDTH-1:0] w_wire_parameter7;

		
		/////////////////////////////////////////////////////
		//instance
		painterengine_gpu_registers gpu_registers(
			.S_AXI_ACLK(i_wire_axi_clock),
			.S_AXI_ARESETN(i_wire_resetn),
			.S_AXI_AWADDR(S_AXI_AWADDR),
			.S_AXI_AWPROT(S_AXI_AWPROT),
			.S_AXI_AWVALID(S_AXI_AWVALID),
			.S_AXI_AWREADY(S_AXI_AWREADY),
			.S_AXI_WDATA(S_AXI_WDATA),
			.S_AXI_WSTRB(S_AXI_WSTRB),
			.S_AXI_WVALID(S_AXI_WVALID),
			.S_AXI_WREADY(S_AXI_WREADY),
			.S_AXI_BRESP(S_AXI_BRESP),
			.S_AXI_BVALID(S_AXI_BVALID),
			.S_AXI_BREADY(S_AXI_BREADY),
			.S_AXI_ARADDR(S_AXI_ARADDR),
			.S_AXI_ARPROT(S_AXI_ARPROT),
			.S_AXI_ARVALID(S_AXI_ARVALID),
			.S_AXI_ARREADY(S_AXI_ARREADY),
			.S_AXI_RDATA(S_AXI_RDATA),
			.S_AXI_RRESP(S_AXI_RRESP),
			.S_AXI_RVALID(S_AXI_RVALID),
			.S_AXI_RREADY(S_AXI_RREADY),
			.o_wire_parameter0(r_wire_parameter0),
			.o_wire_parameter1(r_wire_parameter1),
			.o_wire_parameter2(r_wire_parameter2),
			.o_wire_parameter3(r_wire_parameter3),
			.o_wire_parameter4(r_wire_parameter4),
			.o_wire_parameter5(r_wire_parameter5),
			.o_wire_parameter6(r_wire_parameter6),
			.o_wire_parameter7(r_wire_parameter7),
			.i_wire_parameter0(w_wire_parameter0),
			.i_wire_parameter1(w_wire_parameter1),
			.i_wire_parameter2(w_wire_parameter2),
			.i_wire_parameter3(w_wire_parameter3),
			.i_wire_parameter4(w_wire_parameter4),
			.i_wire_parameter5(w_wire_parameter5),
			.i_wire_parameter6(w_wire_parameter6),
			.i_wire_parameter7(w_wire_parameter7)	
		);

		 /////////////////////////////////////////////////////
		//gpu fifo
		/////////////////////////////////////////////////////
		//controller reg
		reg reg_dma_fifo_resetn;
		////////////////////////////////////////////////////
		//interface
		wire wire_rwfifo_read;
		wire wire_rwfifo_write;
		wire [PARAM_DATA_WIDTH-1:0] wire_rwfifo_data_in;
		wire [PARAM_DATA_WIDTH-1:0] wire_rwfifo_data_out;
		wire wire_rwfifo_almost_empty;
		wire wire_rwfifo_almost_full;
		wire wire_rwfifo_empty;
		wire wire_rwfifo_full;
		////////////////////////////////////////////////////
		//instance
		painterengine_gpu_fifo gpu_rwfifo(
			.i_wire_write_clock(i_wire_axi_clock),
			.i_wire_read_clock(i_wire_axi_clock),
			.i_wire_resetn(reg_dma_fifo_resetn),
			.i_wire_read(wire_rwfifo_read),
			.i_wire_write(wire_rwfifo_write),
			.i_wire_data_in(wire_rwfifo_data_in),
			.o_wire_data_out(wire_rwfifo_data_out),
			.o_wire_almost_empty(wire_rwfifo_almost_empty),
			.o_wire_almost_full(wire_rwfifo_almost_full),
			.o_wire_empty(wire_rwfifo_empty),
			.o_wire_full(wire_rwfifo_full)
		);

		/////////////////////////////////////////////////////
		//gpu dma
		/////////////////////////////////////////////////////
		/////////////////////////////////////////////////////
		//controller reg
		reg reg_dma_resetn;
		reg  reg_gpu_dma_opcode;
		reg [PARAM_DATA_WIDTH-1:0] reg_gpu_dma_address;
		reg [PARAM_DATA_WIDTH-1:0] reg_gpu_dma_data_size;
		////////////////////////////////////////////////////
		//instance
		wire wire_dma_done;
		wire wire_dma_error;

		////////////////////////////////////////////////////
		//instance
		painterengine_gpu_dma gpu_dma(
			.i_wire_clock(i_wire_axi_clock),
			.i_wire_resetn(reg_dma_resetn),
			.i_wire_opcode_0r_1w(reg_gpu_dma_opcode),
			.i_wire_address(reg_gpu_dma_address),
			.i_wire_data_size(reg_gpu_dma_data_size),

			.i_wire_data(wire_rwfifo_data_out),
			.i_wire_data_valid(~wire_rwfifo_empty),
			.o_wire_data_next(wire_rwfifo_read),

			.o_wire_data(wire_rwfifo_data_in),
			.o_wire_data_valid(wire_rwfifo_write),
			.i_wire_data_next(~wire_rwfifo_full),

			.o_wire_done(wire_dma_done),
			.o_wire_error(wire_dma_error),

			//axi full ports
			.m00_axi_awid(m00_axi_awid),
			.m00_axi_awaddr(m00_axi_awaddr),
			.m00_axi_awlen(m00_axi_awlen),
			.m00_axi_awsize(m00_axi_awsize),
			.m00_axi_awburst(m00_axi_awburst),
			.m00_axi_awlock(m00_axi_awlock),
			.m00_axi_awcache(m00_axi_awcache),
			.m00_axi_awprot(m00_axi_awprot),
			.m00_axi_awqos(m00_axi_awqos),
			.m00_axi_awvalid(m00_axi_awvalid),
			.m00_axi_awready(m00_axi_awready),
			.m00_axi_wdata(m00_axi_wdata),
			.m00_axi_wstrb(m00_axi_wstrb),
			.m00_axi_wlast(m00_axi_wlast),
			.m00_axi_wvalid(m00_axi_wvalid),
			.m00_axi_wready(m00_axi_wready),
			.m00_axi_bid(m00_axi_bid),
			.m00_axi_bresp(m00_axi_bresp),
			.m00_axi_bvalid(m00_axi_bvalid),
			.m00_axi_bready(m00_axi_bready),
			.m00_axi_arid(m00_axi_arid),
			.m00_axi_araddr(m00_axi_araddr),
			.m00_axi_arlen(m00_axi_arlen),
			.m00_axi_arsize(m00_axi_arsize),
			.m00_axi_arburst(m00_axi_arburst),
			.m00_axi_arlock(m00_axi_arlock),
			.m00_axi_arcache(m00_axi_arcache),
			.m00_axi_arprot(m00_axi_arprot),
			.m00_axi_arqos(m00_axi_arqos),
			.m00_axi_arvalid(m00_axi_arvalid),
			.m00_axi_arready(m00_axi_arready),
			.m00_axi_rid(m00_axi_rid),
			.m00_axi_rdata(m00_axi_rdata),
			.m00_axi_rresp(m00_axi_rresp),
			.m00_axi_rlast(m00_axi_rlast),
			.m00_axi_rvalid(m00_axi_rvalid),
			.m00_axi_rready(m00_axi_rready)
		);

		/////////////////////////////////////////////////////
		//gpu vm distributor
		reg [7:0] reg_processer_fsm;
		reg [PARAM_DATA_WIDTH-1:0] reg_state;
		reg [PARAM_DATA_WIDTH-1:0] reg_return;
		reg [PARAM_DATA_WIDTH-1:0] reg_debug0;
		reg [PARAM_DATA_WIDTH-1:0] reg_debug1;
		reg [PARAM_DATA_WIDTH-1:0] reg_debug2;
		reg [PARAM_DATA_WIDTH-1:0] reg_debug3;
		reg [PARAM_DATA_WIDTH-1:0] reg_debug4;
		reg [PARAM_DATA_WIDTH-1:0] reg_debug5;

		wire [PARAM_DATA_WIDTH-1:0] wire_opcode;
		assign wire_opcode=r_wire_parameter0;


		reg [PARAM_DATA_WIDTH-1:0] reg_opcode;
		reg [PARAM_DATA_WIDTH-1:0] reg_parameter1;
		reg [PARAM_DATA_WIDTH-1:0] reg_parameter2;
		reg [PARAM_DATA_WIDTH-1:0] reg_parameter3;
		reg [PARAM_DATA_WIDTH-1:0] reg_parameter4;
		reg [PARAM_DATA_WIDTH-1:0] reg_parameter5;
		reg [PARAM_DATA_WIDTH-1:0] reg_parameter6;
		reg [PARAM_DATA_WIDTH-1:0] reg_parameter7;

		assign w_wire_parameter0=reg_state;
		assign w_wire_parameter1=reg_return;
		assign w_wire_parameter2=reg_processer_fsm;
		assign w_wire_parameter3=reg_parameter1;
		assign w_wire_parameter4=reg_parameter2;
		assign w_wire_parameter5=reg_parameter3;
		assign w_wire_parameter6=reg_debug4;
		assign w_wire_parameter7=reg_debug5;
		

		task GPU_TASK_RESET_DEVICES;
		begin
			reg_dma_fifo_resetn<=1'b0;
			reg_dma_resetn<=1'b0;
		end
		endtask

		task GPU_TASK_RESET_PROCESSER;
		begin
			reg_processer_fsm<=8'h00;
		end
		endtask

		task GPU_TASK_RESET_OPCODE;
		begin
			reg_opcode<=`GPU_OPCODE_NOP;
			reg_parameter1<=32'h00000000;
			reg_parameter2<=32'h00000000;
			reg_parameter3<=32'h00000000;
			reg_parameter4<=32'h00000000;
			reg_parameter5<=32'h00000000;
			reg_parameter6<=32'h00000000;
			reg_parameter7<=32'h00000000;
		end
		endtask

		task GPU_TASK_RESET_FSM;
		begin
			GPU_TASK_RESET_OPCODE;
			reg_state<=`GPU_STATE_IDLE;
			reg_processer_fsm<=0;
			reg_return<=32'h00000000;
		end
		endtask

		task GPU_TASK_PULL_OPCODE;
		begin
			reg_opcode<=r_wire_parameter0;
			reg_parameter1<=r_wire_parameter1;
			reg_parameter2<=r_wire_parameter2;
			reg_parameter3<=r_wire_parameter3;
			reg_parameter4<=r_wire_parameter4;
			reg_parameter5<=r_wire_parameter5;
			reg_parameter6<=r_wire_parameter6;
			reg_parameter7<=r_wire_parameter7;
		end
		endtask

		
		task GPU_TASK_RESET;
		begin
			GPU_TASK_RESET_DEVICES;
			GPU_TASK_RESET_PROCESSER;
			GPU_TASK_RESET_FSM;
		end
		endtask

		task GPU_TASK_STATE_IDLE;
			if(wire_opcode!=`GPU_OPCODE_NOP)
			begin
				GPU_TASK_PULL_OPCODE;
				reg_state<=`GPU_STATE_PROCESSING;
			end
			else
			begin
				reg_state<=reg_state;
			end
		endtask

		task GPU_TASK_OPCODE_GETVESION;
		begin
			reg_return<=32'h20240405;
			reg_state<=`GPU_STATE_DONE;
		end
		endtask

		task GPU_TASK_OPCODE_RESET;
		begin
			GPU_TASK_RESET;
			//reg_state<=`GPU_STATE_IDLE;
		end
		endtask

		reg [31:0] reg_task_memcpy_s_address;
		reg [31:0] reg_task_memcpy_d_address;
		reg [31:0] reg_task_memcpy_reserve_lenght;
		reg [10:0] reg_task_memcpy_block_size;
		
		task GPU_TASK_OPCODE_MEMCPY;
		begin
			case(reg_processer_fsm)
			0:
			begin
				//reset fifo
				reg_dma_fifo_resetn<=0;
				//reset dma
				reg_dma_resetn<=0;

				reg_processer_fsm<=1;
			end
			1:
			begin
				
				//0 source address
				//1 destination address
				//2 data size
				reg_task_memcpy_s_address<=reg_parameter1;
				reg_task_memcpy_d_address<=reg_parameter2;
				reg_task_memcpy_reserve_lenght<=reg_parameter3;
				//open fifo
				reg_dma_fifo_resetn<=1'b1;
				reg_processer_fsm<=2;
			end
			2:
			begin
				//calc reg_task_memcpy_block_size & update reg_task_memcpy_reserve_lenght
				if (reg_task_memcpy_reserve_lenght)
				begin
					if (reg_task_memcpy_reserve_lenght>256)
					begin
						reg_task_memcpy_block_size<=10'd256;
						reg_task_memcpy_reserve_lenght<=reg_task_memcpy_reserve_lenght-256;
						reg_processer_fsm<=3;
					end
					else if(reg_task_memcpy_reserve_lenght>0)
					begin
						reg_task_memcpy_block_size<=reg_task_memcpy_reserve_lenght;
						reg_task_memcpy_reserve_lenght<=0;
						reg_processer_fsm<=3;
					end
					else
					begin
						reg_processer_fsm<=7;
					end
				end
				else
				begin
					reg_processer_fsm<=7;
				end
			end
			3:
			begin
				//read data from source
				reg_gpu_dma_opcode<=1'b0;
				reg_gpu_dma_address<=reg_task_memcpy_s_address;
				reg_task_memcpy_s_address<=reg_task_memcpy_s_address+(reg_task_memcpy_block_size<<2);
				reg_gpu_dma_data_size<=reg_task_memcpy_block_size;
				reg_processer_fsm<=4;
				//trigger dma
				reg_dma_resetn<=1'b1;
			end
			4:
			begin
				//wait done
				if(!wire_dma_error)
				begin
					if(wire_dma_done)
					begin
						//close dma
						reg_dma_resetn<=0;
						reg_processer_fsm<=5;
					end
					else
					begin
						reg_processer_fsm<=reg_processer_fsm;
					end
				end
				else
				begin
					reg_processer_fsm<=8;
				end
				
			end
			5:
			begin
				//write data to destination
				reg_gpu_dma_opcode<=1'b1;
				reg_gpu_dma_address<=reg_task_memcpy_d_address;
				reg_task_memcpy_d_address<=reg_task_memcpy_d_address+(reg_task_memcpy_block_size<<2);
				reg_gpu_dma_data_size<=reg_task_memcpy_block_size;
				//trigger dma
				reg_dma_resetn<=1'b1;
				reg_processer_fsm<=6;
			end
			6:
			begin
				//wait done
				if(!wire_dma_error)
				begin
					if(wire_dma_done)
					begin
						reg_processer_fsm<=2;
						reg_dma_resetn<=0;
					end
					else
					begin
						reg_processer_fsm<=reg_processer_fsm;
					end
				end
				else
				begin
					reg_processer_fsm<=8'h08;
				end
			end
			7:
			begin
				reg_return<=32'h00000001;
				reg_dma_fifo_resetn<=0;
				reg_dma_resetn<=0;
				reg_state<=`GPU_STATE_DONE;
			end
			8:
			begin
				reg_return<=32'h00000000;
				reg_dma_fifo_resetn<=0;
				reg_dma_resetn<=0;
				reg_state<=`GPU_STATE_ERROR;
			end
			default:
			begin
				reg_state<=`GPU_STATE_ERROR;
			end
			endcase
		end
		endtask

	   //gpu vm processing
		task GPU_TASK_PROCESSING;
		begin
			case(reg_opcode)
				`GPU_OPCODE_RESET:
				begin
					GPU_TASK_OPCODE_RESET;
				end
				`GPU_OPCODE_GETVERSION:
				begin
					GPU_TASK_OPCODE_GETVESION;
				end
				`GPU_OPCODE_MEMCPY:
				begin
					GPU_TASK_OPCODE_MEMCPY;
				end
				default: 
					reg_state<=`GPU_STATE_ERROR;
			endcase
		end
		endtask

		task GPU_TASK_ERROR;
			if(wire_opcode==`GPU_OPCODE_RESET)
				GPU_TASK_RESET;
			else
				reg_state<=reg_state;
		endtask

		task GPU_TASK_DONE;
			if(wire_opcode==`GPU_OPCODE_RESET)
				GPU_TASK_RESET;
			else
				reg_state<=reg_state;
		endtask

		//GPU FSM controller
		always@(posedge i_wire_axi_clock or negedge i_wire_resetn)
		begin
			if(!i_wire_resetn)
			begin
				GPU_TASK_RESET;
			end
			else
			begin
				case(reg_state)
					`GPU_STATE_IDLE:
					begin
						GPU_TASK_STATE_IDLE;
					end
					`GPU_STATE_PROCESSING:
					begin
						GPU_TASK_PROCESSING;
					end
					`GPU_STATE_ERROR:
					begin
						GPU_TASK_ERROR;
					end
					`GPU_STATE_DONE:
					begin
						GPU_TASK_DONE;
					end
					default: reg_state<=reg_state;
				endcase
			end
		end

	

	endmodule
