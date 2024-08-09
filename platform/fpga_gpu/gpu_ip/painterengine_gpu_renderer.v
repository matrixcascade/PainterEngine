`timescale 1 ns / 1 ns

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
reg reg_writer_resetn;

reg [31:0] reg_src_frame_buffer_address;
reg [31:0] reg_dst_frame_buffer_address;

reg [15:0] reg_render_frame_buffer_xcount;
reg [15:0] reg_render_frame_buffer_ycount;

reg [15:0] reg_src_frame_buffer_width;
reg [15:0] reg_dst_frame_buffer_width;

reg reg_fifo1_resetn;
reg reg_fifo2_resetn;

reg [15:0] reg_reserved_x_size;
reg [15:0] reg_current_size;

reg  [31:0] reg_src_address_op1;
reg  [31:0] reg_src_address_op2;
reg  [31:0] reg_src_address_op3;

reg  [31:0] reg_dst_address_op1;
reg  [31:0] reg_dst_address_op2;
reg  [31:0] reg_dst_address_op3;

reg  [31:0] reg_src_reader_address;
reg  [31:0] reg_dst_rw_address;

assign o_wire_reader_address = reg_reader_address;
assign o_wire_reader_length = reg_reader_size;
assign o_wire_reader1_resetn = reg_reader1_resetn;
assign o_wire_reader2_resetn = reg_reader2_resetn;

assign o_wire_writer_address = reg_writer_address;
assign o_wire_writer_length = reg_writer_size;
assign o_wire_writer_resetn = reg_writer_resetn;
assign o_wire_fifo1_resetn = reg_fifo1_resetn;
assign o_wire_fifo2_resetn = reg_fifo2_resetn;




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

        reg_src_frame_buffer_address <= 32'h00000000;
        reg_dst_frame_buffer_address <= 32'h00000000;

        reg_render_frame_buffer_xcount <= 16'h0000;
        reg_render_frame_buffer_ycount <= 16'h0000;

         reg_src_frame_buffer_width <= 16'h0000;
        reg_dst_frame_buffer_width <= 16'h0000;
       
        reg_fifo1_resetn <= 1'b0;
        reg_fifo2_resetn <= 1'b0;

        reg_reserved_x_size <= 16'h0000;
        reg_current_size <= 16'h0000;

        reg_src_address_op1 <= 32'h00000000;
        reg_src_address_op2 <= 32'h00000000;
        reg_src_address_op3 <= 32'h00000000;

        reg_dst_address_op1 <= 32'h00000000;
        reg_dst_address_op2 <= 32'h00000000;
        reg_dst_address_op3 <= 32'h00000000;

        reg_src_reader_address <= 32'h00000000;
        reg_dst_rw_address <= 32'h00000000;

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
                reg_render_frame_buffer_xcount <= i_wire_render_frame_buffer_xcount[15:0];
                reg_render_frame_buffer_ycount <= i_wire_render_frame_buffer_ycount[15:0];
                reg_x<=0;
                reg_y<=0;
                reg_state <= `GPU_RENDERER_STATE_CHECKX;
            end
        `GPU_RENDERER_STATE_CHECKX:
            begin
                if(reg_x==reg_render_frame_buffer_xcount)
                begin
                    reg_x<=0;
                    reg_y<=reg_y+1;
                    reg_state <= `GPU_RENDERER_STATE_CHECKY;
                end
                else
                begin
                    reg_state <= `GPU_RENDERER_STATE_CALC;
                end
            end
        `GPU_RENDERER_STATE_CHECKY:
            begin
                if(reg_y==reg_render_frame_buffer_ycount)
                begin
                    reg_state <= `GPU_RENDERER_STATE_DONE;
                end
                else
                begin
                    reg_state <= `GPU_RENDERER_STATE_CALC;
                end
            end
        `GPU_RENDERER_STATE_CALC:
            begin
                reg_reserved_x_size<=reg_render_frame_buffer_xcount-reg_x;
                reg_src_address_op1<=reg_y*reg_src_frame_buffer_width;
                reg_dst_address_op1<=reg_y*reg_dst_frame_buffer_width;
                reg_state <= `GPU_RENDERER_STATE_CALC2;
            end
        `GPU_RENDERER_STATE_CALC2:
        begin
            reg_src_address_op2<=reg_src_address_op1*4;
            reg_dst_address_op2<=reg_dst_address_op1*4;
            if(reg_reserved_x_size<`GPU_RENDERER_BLOCK_PIXELS_COUNT)
            begin
                reg_current_size<=reg_reserved_x_size;
            end
            else
            begin
                reg_current_size<=`GPU_RENDERER_BLOCK_PIXELS_COUNT;
            end
            reg_state <= `GPU_RENDERER_STATE_CALC3;
        end
        `GPU_RENDERER_STATE_CALC3:
        begin
            reg_src_address_op3<=reg_src_address_op2+reg_x*4;
            reg_dst_address_op3<=reg_dst_address_op2+reg_x*4;
            if(reg_current_size==0)
            begin
                reg_state <= `GPU_RENDERER_STATE_CHECKX;
            end
            else
            begin
                reg_state <= `GPU_RENDERER_STATE_CALC_ADDRESS;
            end
        end
        `GPU_RENDERER_STATE_CALC_ADDRESS:
        begin
            reg_src_reader_address <= reg_src_frame_buffer_address+reg_src_address_op3;
            reg_reader_size <= reg_current_size;
            reg_dst_rw_address <= reg_dst_frame_buffer_address+reg_dst_address_op3;
            reg_writer_size <= reg_current_size;
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
                    reg_state <= `GPU_RENDERER_STATE_2_READING;
                end
                else
                begin
                    reg_reader_address <= reg_src_reader_address;
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
                    reg_state <= `GPU_RENDERER_STATE_WRITING;
                end
                else
                begin
                    reg_reader_address <= reg_dst_rw_address;
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
                    reg_state <= `GPU_RENDERER_STATE_INC;
                end
                else
                begin
                    reg_writer_address <= reg_dst_rw_address;
                    reg_writer_resetn <= 1'b1;
                end
            end
        `GPU_RENDERER_STATE_INC:
            begin
                reg_x<=reg_x+reg_current_size;
                reg_state <= `GPU_RENDERER_STATE_CHECKX;
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
