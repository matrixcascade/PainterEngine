`timescale 1ns / 1ns


module painterengine_gpu_fifo_tb();

reg clock;

always  #5 clock = ~clock;

reg resetn;
reg [31:0] data_in;
reg fifo_read;
reg fifo_write;
integer i;
initial begin
    clock = 1;
    resetn = 0;
    data_in = 0;
    fifo_read = 0;
    fifo_write = 0;

    #100 
    resetn =  1;
    fifo_write= 1;
    for (i = 1; i <= 64; i = i + 1) begin
        data_in = i;
        #10;
    end
    
    #1000
    fifo_write= 0;
    fifo_read=1;

    #1000
    fifo_write= 1;
    fifo_read=1;
    for (i = 1; i <= 64; i = i + 1) begin
        data_in = i;
        #10;
    end


end



painterengine_gpu_fifo painterengine_gpu_fifo_inst(
    .i_wire_write_clock(clock),
    .i_wire_read_clock(clock),
    .i_wire_resetn(resetn),
    .i_wire_data_in(data_in),
    .i_wire_write(fifo_write),
    .i_wire_read(fifo_read)
);
endmodule
