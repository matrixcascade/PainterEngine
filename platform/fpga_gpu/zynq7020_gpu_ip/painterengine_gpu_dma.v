`timescale 1 ns / 1 ns

`define FIFO_DEPTH 64
`define DATA_WIDTH 32
`define ADDRESS_WIDTH 32
`define ADDRESS_ALIGN 64
`define RW_CACHE_SIZE 256

module painterengine_gpu_dma #
	(
	parameter integer PARAM_DATA_ALIGN	= `ADDRESS_ALIGN,
	parameter integer PARAM_ADDRESS_WIDTH	= `ADDRESS_WIDTH,
	parameter integer PARAM_DATA_WIDTH	= `DATA_WIDTH,
	parameter integer PARAM_CACHE_MAX_SIZE	= `RW_CACHE_SIZE
	)
	(
		input wire                          i_wire_clock,
        input wire                          i_wire_resetn,

        input wire                          i_wire_opcode_0r_1w,
        input wire[31:0] 					i_wire_address,
        input wire[31:0]                    i_wire_data_size,

        input wire[31:0]    				i_wire_data,
        input wire                          i_wire_data_valid,
        output wire                         o_wire_data_next,

        output wire[31:0]   				o_wire_data,
        output wire                         o_wire_data_valid,
		input wire                          i_wire_data_next,

        output wire                         o_wire_error,
        output wire                         o_wire_done,

	

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


    reg  reg_reader_enable;
    reg  reg_writer_enable;
	wire m00_axi_aclk;
	assign m00_axi_aclk=i_wire_clock;
	wire wire_writer_error;
	wire wire_reader_error;
	wire wire_writer_done;
	wire wire_reader_done;

	always @(posedge i_wire_clock or negedge i_wire_resetn)
	begin
		if (!i_wire_resetn)
		begin
			reg_reader_enable<=0;
			reg_writer_enable<=0;
		end
		else
		begin
			if(i_wire_opcode_0r_1w)
			begin
				reg_reader_enable<=0;
				reg_writer_enable<=1;
			end
			else
			begin
				reg_reader_enable<=1;
				reg_writer_enable<=0;
			end
		end
	end

    painterengine_gpu_dma_reader gpu_dma_reader(
			.i_wire_clock(m00_axi_aclk),
			.i_wire_resetn(reg_reader_enable),
			.o_wire_done(wire_reader_done),
			.o_wire_error(wire_reader_error),

			.i_wire_address(i_wire_address),
			.i_wire_length(i_wire_data_size),

			.o_wire_data(o_wire_data),
			.o_wire_data_valid(o_wire_data_valid),
			.i_wire_data_next(i_wire_data_next),

			.o_wire_M_AXI_ARID(m00_axi_arid),
			.o_wire_M_AXI_ARADDR(m00_axi_araddr),
			.o_wire_M_AXI_ARLEN(m00_axi_arlen),
			.o_wire_M_AXI_ARSIZE(m00_axi_arsize),
			.o_wire_M_AXI_ARBURST(m00_axi_arburst),
			.o_wire_M_AXI_ARLOCK(m00_axi_arlock),
			.o_wire_M_AXI_ARCACHE(m00_axi_arcache),
			.o_wire_M_AXI_ARPROT(m00_axi_arprot),
			.o_wire_M_AXI_ARQOS(m00_axi_arqos),
			.o_wire_M_AXI_ARVALID(m00_axi_arvalid),
			.i_wire_M_AXI_ARREADY(m00_axi_arready),
			.i_wire_M_AXI_RID(m00_axi_rid),
			.i_wire_M_AXI_RDATA(m00_axi_rdata),
			.i_wire_M_AXI_RRESP(m00_axi_rresp),
			.i_wire_M_AXI_RLAST(m00_axi_rlast),
			.i_wire_M_AXI_RVALID(m00_axi_rvalid),
			.o_wire_M_AXI_RREADY(m00_axi_rready)
		);

   painterengine_gpu_dma_writer gpu_dma_writer(
			.i_wire_clock(m00_axi_aclk),
			.i_wire_resetn(reg_writer_enable),
            .o_wire_done(wire_writer_done),
			.o_wire_error(wire_writer_error),
			
			.i_wire_address(i_wire_address),
			.i_wire_length(i_wire_data_size),

            .i_wire_data(i_wire_data),
            .i_wire_data_valid(i_wire_data_valid),
            .o_wire_data_next(o_wire_data_next),

			.o_wire_M_AXI_AWID(m00_axi_awid),
			.o_wire_M_AXI_AWADDR(m00_axi_awaddr),
			.o_wire_M_AXI_AWLEN(m00_axi_awlen),
			.o_wire_M_AXI_AWSIZE(m00_axi_awsize),
			.o_wire_M_AXI_AWBURST(m00_axi_awburst),
			.o_wire_M_AXI_AWLOCK(m00_axi_awlock),
			.o_wire_M_AXI_AWCACHE(m00_axi_awcache),
			.o_wire_M_AXI_AWPROT(m00_axi_awprot),
			.o_wire_M_AXI_AWQOS(m00_axi_awqos),
			.o_wire_M_AXI_AWVALID(m00_axi_awvalid),
			.i_wire_M_AXI_AWREADY(m00_axi_awready),
			.o_wire_M_AXI_WDATA(m00_axi_wdata),
			.o_wire_M_AXI_WSTRB(m00_axi_wstrb),
			.o_wire_M_AXI_WLAST(m00_axi_wlast),
			.o_wire_M_AXI_WVALID(m00_axi_wvalid),
			.i_wire_M_AXI_WREADY(m00_axi_wready),
			.i_wire_M_AXI_BID(m00_axi_bid),
			.i_wire_M_AXI_BRESP(m00_axi_bresp),
			.i_wire_M_AXI_BVALID(m00_axi_bvalid),
			.o_wire_M_AXI_BREADY(m00_axi_bready)
		);
	assign o_wire_error=wire_writer_error|wire_reader_error;
	assign o_wire_done=wire_writer_done|wire_reader_done;
		
 endmodule