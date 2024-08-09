`timescale 1 ns / 1 ns

	module painterengine_gpu_argb2rgb
        (
            input wire i_wire_clock,
            input wire i_wire_resetn,
            input wire [31:0] i_wire_color,
            input wire i_wire_valid,
            input wire[1:0] i_wire_iargb_mode,//0:ARGB, 1:ABGR, 2:RGBA, 3:BGRA
            input wire  i_wire_oargb_mode,//0:RGB, 1:BGR
            output wire [31:0] o_wire_rgb,
            output wire o_wire_valid
        );

        reg [31:0] reg_o_data;
        reg [7:0]  reg_i_fifo[7:0];
        reg [2:0]  reg_wr;
        reg [2:0]  reg_rd;
        reg reg_o_valid;

        wire[3:0] wire_fifo_count;

        assign o_wire_rgb = reg_o_data;
        assign o_wire_valid = reg_o_valid;
        assign wire_fifo_count = reg_wr - reg_rd;

        always @(posedge i_wire_clock or negedge i_wire_resetn)
        begin
            if(!i_wire_resetn)
            begin
                reg_wr <= 3'd0;
            end
            else
            begin
                if(i_wire_valid)
                begin
                    reg_wr <= reg_wr + 3'd3;
                end
                else
                begin
                    reg_wr <= reg_wr;
                end
            end
        end

        always @(posedge i_wire_clock or negedge i_wire_resetn)
        begin
            if(!i_wire_resetn)
            begin
                reg_i_fifo[0] <= 8'h00;
                reg_i_fifo[1] <= 8'h00;
                reg_i_fifo[2] <= 8'h00;
                reg_i_fifo[3] <= 8'h00;
                reg_i_fifo[4] <= 8'h00;
                reg_i_fifo[5] <= 8'h00;
                reg_i_fifo[6] <= 8'h00;
                reg_i_fifo[7] <= 8'h00;
            end
            else
            begin
                if(i_wire_valid)
                begin
                    case (i_wire_iargb_mode)
                        2'b00: 
                        begin
                            reg_i_fifo[reg_wr] <= i_wire_color[16+:8];
                            reg_i_fifo[reg_wr+3'd1] <= i_wire_color[8+:8];
                            reg_i_fifo[reg_wr+3'd2] <= i_wire_color[0+:8];
                        end
                        2'b01:
                        begin
                            reg_i_fifo[reg_wr] <= i_wire_color[0+:8];
                            reg_i_fifo[reg_wr+3'd1] <= i_wire_color[8+:8];
                            reg_i_fifo[reg_wr+3'd2] <= i_wire_color[16+:8];
                        end
                        2'b10:
                        begin
                            reg_i_fifo[reg_wr] <= i_wire_color[24+:8];
                            reg_i_fifo[reg_wr+3'd1] <= i_wire_color[16+:8];
                            reg_i_fifo[reg_wr+3'd2] <= i_wire_color[8+:8];
                        end
                        2'b11:
                        begin
                            reg_i_fifo[reg_wr] <= i_wire_color[8+:8];
                            reg_i_fifo[reg_wr+3'd1] <= i_wire_color[0+:8];
                            reg_i_fifo[reg_wr+3'd2] <= i_wire_color[16+:8];
                        end
                    endcase
                end
            end
        end

        always @(posedge i_wire_clock or negedge i_wire_resetn)
        begin
            if(!i_wire_resetn)
            begin
                reg_rd <= 3'd0;
            end
            else
            begin
                if(wire_fifo_count>=4'd4)
                begin
                    reg_rd <= reg_rd + 3'd4;
                end
                else
                begin
                    reg_rd <= reg_rd;
                end
            end
        end

        always @(posedge i_wire_clock or negedge i_wire_resetn)
        begin
            if(!i_wire_resetn)
            begin
                reg_o_valid <= 3'd0;
            end
            else
            begin
                if(wire_fifo_count>=4'd4)
                begin
                    reg_o_valid<=1'b1;
                end
                else
                begin
                    reg_o_valid<=1'b0;
                end
            end
        end

        always @(posedge i_wire_clock or negedge i_wire_resetn)
        begin
            if(!i_wire_resetn)
            begin
                reg_o_data <= 32'h00000000;
            end
            else
            begin
                if(wire_fifo_count>=4'd4)
                begin
                    case (i_wire_oargb_mode)
                        1'b0:
                        begin
                            reg_o_data[0+:8] <= reg_i_fifo[reg_rd];
                            reg_o_data[8+:8] <= reg_i_fifo[reg_rd+3'd1];
                            reg_o_data[16+:8] <= reg_i_fifo[reg_rd+3'd2];
                            reg_o_data[24+:8] <= reg_i_fifo[reg_rd+3'd3];
                        end
                        1'b1:
                        begin
                            reg_o_data[0+:8] <= reg_i_fifo[reg_rd+3'd2];
                            reg_o_data[8+:8] <= reg_i_fifo[reg_rd+3'd1];
                            reg_o_data[16+:8] <= reg_i_fifo[reg_rd];
                            reg_o_data[24+:8] <= reg_i_fifo[reg_rd+3'd3];
                        end
                    endcase
                end
                else
                begin
                    reg_o_data <= 0;
                end
            end
        end
  

    endmodule