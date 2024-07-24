`timescale 1 ns / 1 ns

`define GPU_RENDERER_STATE_INIT 8'h00
`define GPU_RENDERER_STATE_CALC 8'h01
`define GPU_RENDERER_STATE_CALC2 8'h02
`define GPU_RENDERER_STATE_1_READING 8'h03
`define GPU_RENDERER_STATE_2_READING 8'h04
`define GPU_RENDERER_STATE_WRITING 8'h05
`define GPU_RENDERER_STATE_DONE 8'h06
`define GPU_RENDERER_STATE_READER1_ERROR 8'h07
`define GPU_RENDERER_STATE_READER2_ERROR 8'h08
`define GPU_RENDERER_STATE_WRITER_ERROR 8'h09
`define GPU_RENDERER_BLOCK_PIXELS_COUNT 32'd64


module painterengine_gpu_renderer(
    input wire i_wire_clock,
    input wire i_wire_resetn,

    input wire [31:0] i_wire_src_frame_buffer_address,
    input wire [31:0] i_wire_dst_frame_buffer_address,
    input wire [31:0] i_wire_src_frame_buffer_width,
    input wire [31:0] i_wire_dst_frame_buffer_width,
    input wire [31:0] i_wire_render_frame_buffer_xcount,
    input wire [31:0] i_wire_render_frame_buffer_ycount,

    //reader controller
    output wire [31:0] o_wire_reader_address,
    output wire [31:0] o_wire_reader_length,
    output wire o_wire_reader1_resetn,
    output wire o_wire_reader2_resetn,
    input wire i_wire_reader_done,
    input wire i_wire_reader_error,

    //writer controller
    output wire [31:0] o_wire_writer_address,
    output wire [31:0] o_wire_writer_length,
    output wire o_wire_writer_resetn,
    input wire i_wire_writer_done,
    input wire i_wire_writer_error,

    output wire o_wire_fifo1_resetn,
    output wire o_wire_fifo2_resetn,

    //state
    output wire [31:0] o_wire_state
);
reg [31:0] reg_state;
reg [15:0] reg_x;
reg [15:0] reg_y;

reg [31:0] reg_reader_address;
reg [31:0] reg_reader_size;
reg reg_reader1_resetn;
reg reg_reader2_resetn;

reg [31:0] reg_writer_address;
reg [31:0] reg_writer_size;

reg [31:0] reg_src_frame_buffer_address;
reg [31:0] reg_dst_frame_buffer_address;

reg [15:0] reg_src_frame_buffer_width;
reg [15:0] reg_dst_frame_buffer_width;

reg reg_writer_resetn;

reg reg_fifo1_resetn;
reg reg_fifo2_resetn;

assign o_wire_reader_address = reg_reader_address;
assign o_wire_reader_length = reg_reader_size;
assign o_wire_reader1_resetn = reg_reader1_resetn;
assign o_wire_reader2_resetn = reg_reader2_resetn;

assign o_wire_writer_address = reg_writer_address;
assign o_wire_writer_length = reg_writer_size;
assign o_wire_writer_resetn = reg_writer_resetn;
assign o_wire_fifo1_resetn = reg_fifo1_resetn;
assign o_wire_fifo2_resetn = reg_fifo2_resetn;


wire [31:0] wire_reserved_x_size=i_wire_render_frame_buffer_xcount-reg_x;
wire [31:0] wire_current_read_size=wire_reserved_x_size>`GPU_RENDERER_BLOCK_PIXELS_COUNT?`GPU_RENDERER_BLOCK_PIXELS_COUNT:wire_reserved_x_size;
wire [31:0] wire_reserved_y_size=i_wire_render_frame_buffer_ycount-reg_y;

reg  [31:0] reg_y_reg_src_frame_buffer_width;
reg  [31:0] reg_y_reg_dst_frame_buffer_width;

//wire [31:0] wire_current_read1_address=reg_src_frame_buffer_address+4*reg_y*reg_src_frame_buffer_width+reg_x*4;
//wire [31:0] wire_current_read2_address=reg_dst_frame_buffer_address+4*reg_y*reg_src_frame_buffer_width+reg_x*4;
//wire [31:0] wire_current_write_address=reg_dst_frame_buffer_address+4*reg_y*reg_dst_frame_buffer_width+reg_x*4;

wire [31:0] wire_current_read1_address=reg_src_frame_buffer_address+reg_y_reg_src_frame_buffer_width+{2'b0,reg_x}<<2;
wire [31:0] wire_current_read2_address=reg_dst_frame_buffer_address+reg_y_reg_src_frame_buffer_width+{2'b0,reg_x}<<2;
wire [31:0] wire_current_write_address=reg_dst_frame_buffer_address+reg_y_reg_dst_frame_buffer_width+{2'b0,reg_x}<<2;

always @(posedge i_wire_clock or negedge i_wire_resetn)
begin
    if(!i_wire_resetn)
    begin
        reg_state <= `GPU_RENDERER_STATE_INIT;
        reg_x <= 0;
        reg_y <= 0;
        reg_reader_address <= 32'h00000000;
        reg_reader_size <= 32'h00000000;
        reg_reader1_resetn <= 1'b0;
        reg_reader2_resetn <= 1'b0;

        reg_writer_address <= 32'h00000000;
        reg_writer_size <= 32'h00000000;
        reg_writer_resetn <= 1'b0;
       
        reg_fifo1_resetn <= 1'b0;
        reg_fifo2_resetn <= 1'b0;

        reg_src_frame_buffer_address <= 32'h00000000;
        reg_dst_frame_buffer_address <= 32'h00000000;
        reg_src_frame_buffer_width <= 16'h0000;
        reg_dst_frame_buffer_width <= 16'h0000;
        
        reg_y_reg_src_frame_buffer_width <= 32'h00000000;
        reg_y_reg_dst_frame_buffer_width <= 32'h00000000;
    end
    else
    begin
        case(reg_state)
        `GPU_RENDERER_STATE_INIT:
            begin
                reg_fifo1_resetn <= 1'b1;
                reg_fifo2_resetn <= 1'b1;
                reg_src_frame_buffer_address <= i_wire_src_frame_buffer_address;
                reg_dst_frame_buffer_address <= i_wire_dst_frame_buffer_address;
                reg_src_frame_buffer_width <= i_wire_src_frame_buffer_width[15:0];
                reg_dst_frame_buffer_width <= i_wire_dst_frame_buffer_width[15:0];
                reg_x<=0;
                reg_y<=0;
                reg_state <= `GPU_RENDERER_STATE_CALC;
            end
        `GPU_RENDERER_STATE_CALC:
            begin
                if((reg_x==i_wire_render_frame_buffer_xcount))
                begin
                    if(reg_y+1>=i_wire_render_frame_buffer_ycount)
                    begin
                        reg_state <= `GPU_RENDERER_STATE_DONE;
                    end
                    else
                    begin
                        reg_x <= 0;
                        reg_y <= reg_y+1;
                    end
                end
                else
                begin
                    reg_y_reg_src_frame_buffer_width<=reg_y*reg_src_frame_buffer_width*4;
                    reg_y_reg_dst_frame_buffer_width<=reg_y*reg_dst_frame_buffer_width*4;
                    reg_state <= `GPU_RENDERER_STATE_CALC2;
                end
            end
        `GPU_RENDERER_STATE_CALC2:
        begin
            reg_reader_address <= wire_current_read1_address;
            reg_reader_size <= wire_current_read_size;
            reg_state <= `GPU_RENDERER_STATE_1_READING;
        end
        `GPU_RENDERER_STATE_1_READING:
            begin
                if (i_wire_reader_error)
                begin
                    reg_state <= `GPU_RENDERER_STATE_READER1_ERROR;
                end
                else if(i_wire_reader_done)
                begin
                    reg_reader1_resetn <= 1'b0;
                    reg_reader2_resetn <= 1'b0;
                    reg_reader_address <= wire_current_read2_address;
                    reg_reader_size <= wire_current_read_size;
                    reg_state <= `GPU_RENDERER_STATE_2_READING;
                end
                else
                begin
                    reg_reader1_resetn <= 1'b1;
                    reg_reader2_resetn <= 1'b0;
                end
            end
        `GPU_RENDERER_STATE_2_READING:
            begin
                if (i_wire_reader_error)
                begin
                    reg_state <= `GPU_RENDERER_STATE_READER2_ERROR;
                end
                else if(i_wire_reader_done)
                begin
                    reg_reader1_resetn <= 1'b0;
                    reg_reader2_resetn <= 1'b0;
                    reg_writer_address <= wire_current_write_address;
                    reg_writer_size <= wire_current_read_size;
                    reg_x<=reg_x+wire_current_read_size;
                    reg_state <= `GPU_RENDERER_STATE_WRITING;
                end
                else
                begin
                    reg_reader1_resetn <= 1'b0;
                    reg_reader2_resetn <= 1'b1;
                end
            end
        `GPU_RENDERER_STATE_WRITING:
            begin
                if (i_wire_writer_error)
                begin
                    reg_state <= `GPU_RENDERER_STATE_WRITER_ERROR;
                end
                else if(i_wire_writer_done)
                begin
                    reg_writer_resetn <= 1'b0;
                    reg_state <= `GPU_RENDERER_STATE_CALC;
                end
                else
                begin
                    reg_writer_resetn <= 1'b1;
                end
            end
        `GPU_RENDERER_STATE_DONE:
            begin
                reg_state <= `GPU_RENDERER_STATE_DONE;
            end
        default:
            begin
                reg_state <= reg_state;
            end
        endcase
    end

end

assign o_wire_state={24'd0,reg_state};


endmodule
