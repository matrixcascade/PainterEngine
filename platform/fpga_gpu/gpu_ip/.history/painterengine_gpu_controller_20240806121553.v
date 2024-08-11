`timescale 1 ns / 1 ns

	//gpu controller
	`define GPU_CONTROLLER_OPCODE_RESET 		32'h00000000
	`define GPU_CONTROLLER_OPCODE_GPUINFO  		32'h00000001
	`define GPU_CONTROLLER_OPCODE_MEMCPY   		32'h00000002
	`define GPU_CONTROLLER_OPCODE_RENDERER 		32'h00000003
	`define GPU_CONTROLLER_OPCODE_CONVERT 		32'h00000004

	`define GPU_CONTROLLER_STATE_IDLE   				32'h00000000
	`define GPU_CONTROLLER_STATE_GPUINFO_PROCESSING		32'h00000001
	`define GPU_CONTROLLER_STATE_MEMCPY_PROCESSING		32'h00000002
	`define GPU_CONTROLLER_STATE_RENDERER_PROCESSING	32'h00000003

	`define GPU_CONTROLLER_STATE_ERROR    		32'h00000005
	`define GPU_CONTROLLER_STATE_DONE    		32'h00000006

	//gpuinfo

	//gpuinfo opcodes
    `define GPUINFO_OPCODE_RESET 	    32'h00000000
    `define GPUINFO_OPCODE_GETVERSION 	32'h00000001
    `define GPUINFO_OPCODE_GETDEBUG 	32'h00000002
    `define GPUINFO_OPCODE_GETTICK 	    32'h00000003

    `define GPUINFO_STATE_IDLE   		32'h00000000
    `define GPUINFO_STATE_PROCESSING	32'h00000001
    `define GPUINFO_STATE_ERROR    		32'h00000002
    `define GPUINFO_STATE_DONE          32'h00000003

	//memcpy
	`define GPU_MEMCPY_STATE_INIT						8'h00
	`define GPU_MEMCPY_STATE_PUSH_PARAM					8'h01
	`define GPU_MEMCPY_STATE_CALC_PROCESS				8'h02
	`define GPU_MEMCPY_STATE_READ						8'h03
	`define GPU_MEMCPY_STATE_READ_WAIT					8'h04
	`define GPU_MEMCPY_STATE_WRITE						8'h05
	`define GPU_MEMCPY_STATE_WRITE_WAIT					8'h06
	`define GPU_MEMCPY_STATE_CHECKSIZE					8'h07
	`define GPU_MEMCPY_STATE_DONE						8'h08
	`define GPU_MEMCPY_STATE_LENGTH_ERROR				8'h09
	`define GPU_MEMCPY_STATE_DMA_READER_ERROR			8'h0A
	`define GPU_MEMCPY_STATE_DMA_WRITER_ERROR			8'h0B

	//renderer
	`define GPU_RENDERER_STATE_INIT 8'h00
	`define GPU_RENDERER_STATE_CHECKX 8'h01
	`define GPU_RENDERER_STATE_CHECKY 8'h02
	`define GPU_RENDERER_STATE_CALC 8'h03
	`define GPU_RENDERER_STATE_CALC2 8'h04
	`define GPU_RENDERER_STATE_CALC3 8'h05
	`define GPU_RENDERER_STATE_CALC_ADDRESS 8'h06
	`define GPU_RENDERER_STATE_1_READING 8'h07
	`define GPU_RENDERER_STATE_2_READING 8'h08
	`define GPU_RENDERER_STATE_WRITING 8'h09
	`define GPU_RENDERER_STATE_INC 8'h0A
	`define GPU_RENDERER_STATE_DONE 8'h0B
	`define GPU_RENDERER_STATE_READER1_ERROR 8'h0C
	`define GPU_RENDERER_STATE_READER2_ERROR 8'h0D
	`define GPU_RENDERER_STATE_WRITER_ERROR 8'h0E
	`define GPU_RENDERER_BLOCK_PIXELS_COUNT 32'd64

	module painterengine_gpu_controller #
	(
		parameter integer C_S_AXI_DATA_WIDTH	= 32,
		parameter integer C_S_AXI_ADDR_WIDTH	= 32
	)
	(
		//axi4lite signals
		input wire  S_AXI_ACLK,
		input wire  S_AXI_ARESETN,
		input wire [C_S_AXI_ADDR_WIDTH-1 : 0] S_AXI_AWADDR,
		input wire [2 : 0] S_AXI_AWPROT,
		input wire  S_AXI_AWVALID,
		output wire  S_AXI_AWREADY,
		input wire [C_S_AXI_DATA_WIDTH-1 : 0] S_AXI_WDATA,
		input wire [(C_S_AXI_DATA_WIDTH/8)-1 : 0] S_AXI_WSTRB,
		input wire  S_AXI_WVALID,
		output wire  S_AXI_WREADY,
		output wire [1 : 0] S_AXI_BRESP,
		output wire  S_AXI_BVALID,
		input wire  S_AXI_BREADY,
		input wire [C_S_AXI_ADDR_WIDTH-1 : 0] S_AXI_ARADDR,
		input wire [2 : 0] S_AXI_ARPROT,
		input wire  S_AXI_ARVALID,
		output wire  S_AXI_ARREADY,
		output wire [C_S_AXI_DATA_WIDTH-1 : 0] S_AXI_RDATA,
		output wire [1 : 0] S_AXI_RRESP,
		output wire  S_AXI_RVALID,
		input wire  S_AXI_RREADY,

		//gpuinfo
		output wire o_wire_gpuinfo_resetn,
		output wire[31:0] o_wire_gpuinfo_opcode,
		input  wire[31:0] i_wire_gpuinfo_state,
		input  wire[31:0] i_wire_gpuinfo_return,


		//memcpy
		output wire o_wire_memcpy_resetn,
		output wire[31:0] o_wire_memcpy_src_address,
		output wire[31:0] o_wire_memcpy_dst_address,
		output wire[31:0] o_wire_memcpy_data_length,
		input  wire[31:0] i_wire_memcpy_state,

		//renderer
		output wire 	  o_wire_renderer_resetn,
		output wire[31:0] o_wire_renderer_src_address,
		output wire[31:0] o_wire_renderer_src_width,
		output wire[31:0] o_wire_renderer_src_x_count,
		output wire[31:0] o_wire_renderer_src_y_count,

		output wire[31:0] o_wire_renderer_dst_address,
		output wire[31:0] o_wire_renderer_dst_width,
		
		output wire[31:0] o_wire_renderer_color_format,
		output wire[31:0] o_wire_renderer_blend,
		input  wire[31:0] i_wire_renderer_state,

		//color convert
		output wire o_wire_colorconvert_resetn,
		output wire[31:0] o_wire_colorconvert_src_address,
		output wire[31:0] o_wire_colorconvert_dst_address,
		output wire[31:0] o_wire_colorconvert_data_length,
		input  wire[31:0] i_wire_colorconvert_state,
	);
		reg [7:0] reg_controller_state;

		//gpuinfo
		reg  reg_gpuinfo_resetn;
		reg [31:0] reg_gpuinfo_opcode;


		//memcpy
		reg reg_memcpy_resetn;
		reg [31:0] reg_memcpy_src_address;
		reg [31:0] reg_memcpy_dst_address;
		reg [31:0] reg_memcpy_data_length;

		//memcpy
		reg reg_colorconvert_resetn;
		reg [31:0] reg_colorconvert_src_address;
		reg [31:0] reg_colorconvert_dst_address;
		reg [31:0] reg_colorconvert_data_length;

		//renderer
		reg reg_renderer_resetn;
		reg [31:0] reg_renderer_src_address;
		reg [31:0] reg_renderer_src_width;
		reg [31:0] reg_renderer_src_x_count;
		reg [31:0] reg_renderer_src_y_count;
		reg [31:0] reg_renderer_dst_address;
		reg [31:0] reg_renderer_dst_width;
		reg [31:0] reg_renderer_color_format;
		reg [31:0] reg_renderer_blend;

		assign o_wire_gpuinfo_resetn=reg_gpuinfo_resetn;
		assign o_wire_gpuinfo_opcode=reg_gpuinfo_opcode;

		assign o_wire_memcpy_resetn=reg_memcpy_resetn;
		assign o_wire_memcpy_src_address=reg_memcpy_src_address;
		assign o_wire_memcpy_dst_address=reg_memcpy_dst_address;
		assign o_wire_memcpy_data_length=reg_memcpy_data_length;

		assign o_wire_colorconvert_resetn=reg_colorconvert_resetn;
		assign o_wire_colorconvert_src_address=reg_colorconvert_src_address;
		assign o_wire_colorconvert_dst_address=reg_colorconvert_dst_address;
		assign o_wire_colorconvert_data_length=reg_colorconvert_data_length;

		assign o_wire_renderer_resetn=reg_renderer_resetn;
		assign o_wire_renderer_src_address=reg_renderer_src_address;
		assign o_wire_renderer_src_width=reg_renderer_src_width;
		assign o_wire_renderer_src_x_count=reg_renderer_src_x_count;
		assign o_wire_renderer_src_y_count=reg_renderer_src_y_count;
		assign o_wire_renderer_dst_address=reg_renderer_dst_address;
		assign o_wire_renderer_dst_width=reg_renderer_dst_width;
		assign o_wire_renderer_color_format=reg_renderer_color_format;
		assign o_wire_renderer_blend=reg_renderer_blend;


		/////////////////////////////////////////////////////
		//gpu controller/registers
		/////////////////////////////////////////////////////
	
		/////////////////////////////////////////////////////
		//interface
		wire [31:0] r_wire_opcode;
		wire [31:0] r_wire_parameter1;
		wire [31:0] r_wire_parameter2;
		wire [31:0] r_wire_parameter3;
		wire [31:0] r_wire_parameter4;
		wire [31:0] r_wire_parameter5;
		wire [31:0] r_wire_parameter6;
		wire [31:0] r_wire_parameter7;
		wire [31:0] r_wire_parameter8;
		wire [31:0] r_wire_parameter9;
		wire [31:0] r_wire_parameter10;
		wire [31:0] r_wire_parameter11;
		wire [31:0] r_wire_parameter12;
		wire [31:0] r_wire_parameter13;
		wire [31:0] r_wire_parameter14;
		wire [31:0] r_wire_parameter15;

		/////////////////////////////////////////////////////
		//gpu vm distributor
		
		reg [31:0] out_parameter0;
		reg [31:0] out_parameter1;
		reg [31:0] out_parameter2;
		reg [31:0] out_parameter3;
		reg [31:0] out_parameter4;
		reg [31:0] out_parameter5;
		reg [31:0] out_parameter6;
		reg [31:0] out_parameter7;
		reg [31:0] out_parameter8;
		reg [31:0] out_parameter9;
		reg [31:0] out_parameter10;
		reg [31:0] out_parameter11;
		reg [31:0] out_parameter12;
		reg [31:0] out_parameter13;
		reg [31:0] out_parameter14;
		reg [31:0] out_parameter15;
		
		/////////////////////////////////////////////////////
		//instance
		painterengine_gpu_registers gpu_registers(
			.S_AXI_ACLK(S_AXI_ACLK),
			.S_AXI_ARESETN(S_AXI_ARESETN),
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
			.o_wire_parameter0(r_wire_opcode),
			.o_wire_parameter1(r_wire_parameter1),
			.o_wire_parameter2(r_wire_parameter2),
			.o_wire_parameter3(r_wire_parameter3),
			.o_wire_parameter4(r_wire_parameter4),
			.o_wire_parameter5(r_wire_parameter5),
			.o_wire_parameter6(r_wire_parameter6),
			.o_wire_parameter7(r_wire_parameter7),
			.o_wire_parameter8(r_wire_parameter8),
			.o_wire_parameter9(r_wire_parameter9),
			.o_wire_parameter10(r_wire_parameter10),
			.o_wire_parameter11(r_wire_parameter11),
			.o_wire_parameter12(r_wire_parameter12),
			.o_wire_parameter13(r_wire_parameter13),
			.o_wire_parameter14(r_wire_parameter14),
			.o_wire_parameter15(r_wire_parameter15),
			.i_wire_parameter0(out_parameter0),
			.i_wire_parameter1(out_parameter1),
			.i_wire_parameter2(out_parameter2),
			.i_wire_parameter3(out_parameter3),
			.i_wire_parameter4(out_parameter4),
			.i_wire_parameter5(out_parameter5),
			.i_wire_parameter6(out_parameter6),
			.i_wire_parameter7(out_parameter7),
			.i_wire_parameter8(out_parameter8),
			.i_wire_parameter9(out_parameter9),
			.i_wire_parameter10(out_parameter10),
			.i_wire_parameter11(out_parameter11),
			.i_wire_parameter12(out_parameter12),
			.i_wire_parameter13(out_parameter13),
			.i_wire_parameter14(out_parameter14),
			.i_wire_parameter15(out_parameter15)
		);




		/////////////////////////////////////////////////////
		//gpu info
		task GPU_TASK_RESET;
		begin
			reg_controller_state<=`GPU_CONTROLLER_STATE_IDLE;
			
			reg_gpuinfo_resetn<=0;
			reg_gpuinfo_opcode<=0;

			reg_memcpy_resetn<=0;
			reg_memcpy_src_address<=0;
			reg_memcpy_dst_address<=0;
			reg_memcpy_data_length<=0;

			reg_colorconvert_resetn<=0;
			reg_colorconvert_src_address<=0;
			reg_colorconvert_dst_address<=0;
			reg_colorconvert_data_length<=0;

			reg_renderer_resetn<=0;
			reg_renderer_src_address<=0;
			reg_renderer_src_width<=0;
			reg_renderer_src_x_count<=0;
			reg_renderer_src_y_count<=0;

			reg_renderer_dst_address<=0;
			reg_renderer_dst_width<=0;

			reg_renderer_color_format<=0;
			reg_renderer_blend<=0;
			
		end
		endtask
		

		task GPU_TASK_STATE_IDLE;
			case(r_wire_opcode)
				`GPU_CONTROLLER_OPCODE_RESET:
				begin
					GPU_TASK_RESET;
				end
				`GPU_CONTROLLER_OPCODE_GPUINFO:
				begin
					reg_gpuinfo_resetn<=1;
					reg_gpuinfo_opcode<=r_wire_parameter1;

					reg_controller_state<=`GPU_CONTROLLER_STATE_GPUINFO_PROCESSING;
				end
				`GPU_CONTROLLER_OPCODE_MEMCPY:
				begin
					reg_memcpy_resetn<=1;
					reg_memcpy_src_address<=r_wire_parameter1;
					reg_memcpy_dst_address<=r_wire_parameter2;
					reg_memcpy_data_length<=r_wire_parameter3;

					reg_controller_state<=`GPU_CONTROLLER_STATE_MEMCPY_PROCESSING;
				end
				`GPU_CONTROLLER_OPCODE_RENDERER:
				begin
					reg_renderer_resetn<=1;
					reg_renderer_src_address<=r_wire_parameter1;
					reg_renderer_src_width<=r_wire_parameter2;
					reg_renderer_src_x_count<=r_wire_parameter3;
					reg_renderer_src_y_count<=r_wire_parameter4;
					reg_renderer_dst_address<=r_wire_parameter5;
					reg_renderer_dst_width<=r_wire_parameter6;
					reg_renderer_color_format<=r_wire_parameter7;
					reg_renderer_blend<=r_wire_parameter8;

			
					reg_controller_state<=`GPU_CONTROLLER_STATE_RENDERER_PROCESSING;
				end
			endcase
		endtask

		task GPU_TASK_GPUINFO_PROCESSING;
			case(i_wire_gpuinfo_state)
			   `GPUINFO_STATE_IDLE:
			   begin
				//wait for gpuinfo processing
				reg_gpuinfo_resetn<=1;
				reg_gpuinfo_opcode<=reg_gpuinfo_opcode;
			   end
			   `GPUINFO_STATE_PROCESSING:
			   begin
				//wait for gpuinfo processing
				reg_gpuinfo_resetn<=1;
				reg_gpuinfo_opcode<=reg_gpuinfo_opcode;
			   end
			   `GPUINFO_STATE_DONE:
			   begin
				reg_gpuinfo_resetn<=1;
				reg_gpuinfo_opcode<=reg_gpuinfo_opcode;
				reg_controller_state<=`GPU_CONTROLLER_STATE_DONE;
			   end
			   `GPUINFO_STATE_ERROR:
			   begin
				//gpuinfo error
				reg_gpuinfo_resetn<=1;
				reg_gpuinfo_opcode<=reg_gpuinfo_opcode;
				reg_controller_state<=`GPU_CONTROLLER_STATE_ERROR;
			   end
			endcase
		endtask

		task GPU_TASK_MEMCPY_PROCESSING;
			case(i_wire_memcpy_state[7:0])
			   `GPU_MEMCPY_STATE_DONE:
			   begin
				reg_controller_state<=`GPU_CONTROLLER_STATE_DONE;
			   end
			   `GPU_MEMCPY_STATE_DMA_READER_ERROR:
			   begin
				reg_controller_state<=`GPU_CONTROLLER_STATE_ERROR;
			   end
			   `GPU_MEMCPY_STATE_DMA_WRITER_ERROR:
			   begin
				reg_controller_state<=`GPU_CONTROLLER_STATE_ERROR;
			   end
			   default:
			   begin
				reg_controller_state<=reg_controller_state;
			   end
			endcase
		endtask
		task GPU_TASK_RENDERER_PROCESSING;
		begin
			case(i_wire_renderer_state)
			`GPU_RENDERER_STATE_READER1_ERROR:
			begin
				reg_controller_state<=`GPU_CONTROLLER_STATE_ERROR;
			end
			`GPU_RENDERER_STATE_READER2_ERROR:
			begin
				reg_controller_state<=`GPU_CONTROLLER_STATE_ERROR;
			end
			`GPU_RENDERER_STATE_WRITER_ERROR:
			begin
				reg_controller_state<=`GPU_CONTROLLER_STATE_ERROR;
			end
			`GPU_RENDERER_STATE_DONE:
			begin
				reg_controller_state<=`GPU_CONTROLLER_STATE_DONE;
			end
			default:
			begin
				reg_controller_state<=reg_controller_state;
			end
			endcase
		end
		endtask


		task GPU_TASK_ERROR;
		begin
			reg_controller_state<=reg_controller_state;
			reg_renderer_resetn<=reg_renderer_resetn;
			reg_renderer_src_address<=reg_renderer_src_address;
			reg_renderer_src_width<=reg_renderer_src_width;
			reg_renderer_src_x_count<=reg_renderer_src_x_count;
			reg_renderer_src_y_count<=reg_renderer_src_y_count;
			reg_renderer_dst_address<=reg_renderer_dst_address;
			reg_renderer_dst_width<=reg_renderer_dst_width;
			reg_renderer_color_format<=reg_renderer_color_format;
			reg_renderer_blend<=reg_renderer_blend;
			reg_memcpy_resetn<=reg_memcpy_resetn;
			reg_memcpy_src_address<=reg_memcpy_src_address;
			reg_memcpy_dst_address<=reg_memcpy_dst_address;
			reg_memcpy_data_length<=reg_memcpy_data_length;
			reg_gpuinfo_resetn<=reg_gpuinfo_resetn;
			reg_gpuinfo_opcode<=reg_gpuinfo_opcode;
		end
		endtask

		task GPU_TASK_DONE;
		begin
			reg_controller_state<=reg_controller_state;
			reg_renderer_resetn<=reg_renderer_resetn;
			reg_renderer_src_address<=reg_renderer_src_address;
			reg_renderer_src_width<=reg_renderer_src_width;
			reg_renderer_src_x_count<=reg_renderer_src_x_count;
			reg_renderer_src_y_count<=reg_renderer_src_y_count;
			reg_renderer_dst_address<=reg_renderer_dst_address;
			reg_renderer_dst_width<=reg_renderer_dst_width;
			reg_renderer_color_format<=reg_renderer_color_format;
			reg_renderer_blend<=reg_renderer_blend;
			reg_memcpy_resetn<=reg_memcpy_resetn;
			reg_memcpy_src_address<=reg_memcpy_src_address;
			reg_memcpy_dst_address<=reg_memcpy_dst_address;
			reg_memcpy_data_length<=reg_memcpy_data_length;
			reg_gpuinfo_resetn<=reg_gpuinfo_resetn;
			reg_gpuinfo_opcode<=reg_gpuinfo_opcode;
		end
		endtask


		//GPU STATE controller
		always@(posedge S_AXI_ACLK or negedge S_AXI_ARESETN)
		begin
			if(!S_AXI_ARESETN)
			begin
				GPU_TASK_RESET;
			end
			else
			begin
				if (r_wire_opcode==`GPU_CONTROLLER_OPCODE_RESET)
				begin
					GPU_TASK_RESET;
				end
				else
				begin
					case(reg_controller_state)
					`GPU_CONTROLLER_STATE_IDLE:
					begin
						GPU_TASK_STATE_IDLE;
					end
					`GPU_CONTROLLER_STATE_GPUINFO_PROCESSING:
					begin
						GPU_TASK_GPUINFO_PROCESSING;
					end
					`GPU_CONTROLLER_STATE_MEMCPY_PROCESSING:
					begin
						GPU_TASK_MEMCPY_PROCESSING;
					end
					`GPU_CONTROLLER_STATE_RENDERER_PROCESSING:
					begin
						GPU_TASK_RENDERER_PROCESSING;
					end
					`GPU_CONTROLLER_STATE_ERROR:
					begin
						GPU_TASK_ERROR;
					end
					`GPU_CONTROLLER_STATE_DONE:
					begin
						GPU_TASK_DONE;
					end
					default: reg_controller_state<=reg_controller_state;
				endcase
				end
			end
		end


		//output parameters
		always@(posedge S_AXI_ACLK or negedge S_AXI_ARESETN)
		begin
			if(!S_AXI_ARESETN)
			begin
				out_parameter0<=0;
				out_parameter1<=0;
				out_parameter2<=0;
				out_parameter3<=0;
				out_parameter4<=0;
				out_parameter5<=0;
				out_parameter6<=0;
				out_parameter7<=0;
				out_parameter8<=0;
				out_parameter9<=0;
				out_parameter10<=0;
				out_parameter11<=0;
				out_parameter12<=0;
				out_parameter13<=0;
				out_parameter14<=0;
				out_parameter15<=0;
			end
			else
			begin
				out_parameter0<=32'h31415930;
				out_parameter1[7:0]<=reg_controller_state;
				out_parameter1[8]<=reg_gpuinfo_resetn;
				out_parameter1[9]<=reg_memcpy_resetn;
				out_parameter1[10]<=reg_renderer_resetn;
				out_parameter1[31:11]<=0;

				out_parameter2<=i_wire_gpuinfo_return;
				out_parameter3<=i_wire_gpuinfo_state;
				out_parameter4<=i_wire_memcpy_state;
				out_parameter5<=i_wire_renderer_state;
			end

		end
	endmodule