`timescale 1 ns / 1 ns

    `define GPUINFO_OPCODE_RESET 	    32'h00000000
    `define GPUINFO_OPCODE_GETVERSION 	32'h00000001
    `define GPUINFO_OPCODE_GETDEBUG 	32'h00000002
    `define GPUINFO_OPCODE_GETTICK 	    32'h00000003

    `define GPUINFO_STATE_IDLE   		32'h00000000
    `define GPUINFO_STATE_PROCESSING	32'h00000001
    `define GPUINFO_STATE_ERROR    		32'h00000002
    `define GPUINFO_STATE_DONE          32'h00000003

    module painterengine_gpu_gpuinfo
        (
                //clock
                input wire          i_wire_clock,
                //enable
                input wire   		i_wire_resetn,
                input wire          i_wire_tick_resetn,

                input wire [31 : 0]  i_wire_opcode,
                output wire [31 : 0] o_wire_state,
                output wire [31 : 0] o_wire_return
        );
        reg[6 : 0] reg_tick_div100;
        reg[31 : 0] reg_tick_us;

        always @(posedge i_wire_clock or negedge i_wire_tick_resetn)
        begin
            if(!i_wire_tick_resetn)
            begin
                reg_tick_div100<=7'd0;
                reg_tick_us<=0;
            end
            else
            begin
                if(reg_tick_div100==7'd100)
                begin
                    reg_tick_div100<=7'd0;
                    reg_tick_us<=reg_tick_us+1;
                end
                else
                begin
                    reg_tick_div100<=reg_tick_div100+1;
                end
            end
        end

        reg[31 : 0] reg_opcode;
        reg[31 : 0] reg_state;
        reg[31 : 0] reg_return;

        

        assign o_wire_state=reg_state;
        assign o_wire_return=reg_return;

        task task_reset;
        begin
            reg_opcode<=32'h00000000;
            reg_state<=`GPUINFO_STATE_IDLE;
            reg_return<=32'h00000000;
        end
        endtask

        task task_idle;
            if(i_wire_opcode!=`GPUINFO_OPCODE_RESET)
            begin
                reg_opcode<=i_wire_opcode;
                reg_state<=`GPUINFO_STATE_PROCESSING;
                reg_return<=32'h00000000;
            end
            else
            begin
                reg_state<=`GPUINFO_STATE_IDLE;
                reg_return<=32'h00000000;
            end
        endtask

        task task_processing;
          case(reg_opcode)
            `GPUINFO_OPCODE_GETVERSION:
            begin
                reg_state<=`GPUINFO_STATE_DONE;
                reg_return<=32'h00000001;
            end
            `GPUINFO_OPCODE_GETDEBUG:
            begin
                reg_state<=`GPUINFO_STATE_DONE;
                reg_return<=32'h20240612;
            end
            `GPUINFO_OPCODE_GETTICK:
            begin
                reg_state<=`GPUINFO_STATE_DONE;
                reg_return<=reg_tick_us;
            end
            default:
            begin
                reg_state<=`GPUINFO_STATE_ERROR;
                reg_return<=32'h00000000;
            end
         endcase
        endtask

        always @(posedge i_wire_clock or negedge i_wire_resetn)
        begin
            if(!i_wire_resetn)
            begin
                task_reset;
            end
            else
            begin
                case(reg_state)
                    `GPUINFO_STATE_IDLE:
                    begin
                       task_idle;
                    end
                    `GPUINFO_STATE_PROCESSING:
                    begin
                        task_processing;
                    end

                    default:
                    begin
                        reg_state<=reg_state;
                        reg_return<=reg_return;
                    end
                endcase
            end
        end
    endmodule