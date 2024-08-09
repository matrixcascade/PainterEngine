`timescale 1ns / 1ns


module painterengine_gpu_fifo_tb();


wire[1:0] argb2rgb_i_wire_iargb_mode;
assign argb2rgb_i_wire_iargb_mode = 2'b00;
wire argb2rgb_i_wire_oargb_mode;
assign argb2rgb_i_wire_oargb_mode = 1'b0;
reg[31:0] argb2rgb_i_wire_data;
reg argb2rgb_i_wire_valid;
wire[31:0] argb2rgb_o_wire_rgb;
wire argb2rgb_o_wire_valid;

reg clock;

always  #5 clock = ~clock;

reg resetn;
reg [31:0] data_in;
integer i;
initial begin
    clock = 1;
    resetn = 0;
    data_in = 0;
    argb2rgb_i_wire_data=0;
    argb2rgb_i_wire_valid = 0;
    #100 
    resetn =  1;
    for (i = 1; i <= 8; i = i + 1) begin
        argb2rgb_i_wire_data = 32'h44332211;
        argb2rgb_i_wire_valid =1;
        #10;
    end
    argb2rgb_i_wire_valid=0;
    #10;
     for (i = 1; i <= 8; i = i + 1) begin
        argb2rgb_i_wire_data = 32'h88776655;
        argb2rgb_i_wire_valid =1;
        #10;
    end
    argb2rgb_i_wire_valid=0;
     for (i = 1; i <= 8; i = i + 1) begin
        argb2rgb_i_wire_data = 32'h44332211;
        argb2rgb_i_wire_valid =1;
        #10;
    end
    argb2rgb_i_wire_valid=0;
    #10;
     for (i = 1; i <= 8; i = i + 1) begin
        argb2rgb_i_wire_data = 32'h88776655;
        argb2rgb_i_wire_valid =1;
        #10;
    end
    argb2rgb_i_wire_valid =0;
end


painterengine_gpu_argb2rgb painterengine_gpu_argb2rgb(
    .i_wire_clock(clock),
    .i_wire_resetn(resetn),
    .i_wire_color(argb2rgb_i_wire_data),
    .i_wire_valid(argb2rgb_i_wire_valid),
    .i_wire_iargb_mode(argb2rgb_i_wire_iargb_mode),
    .i_wire_oargb_mode(argb2rgb_i_wire_oargb_mode),//0:RGB, 1:BGR
    .o_wire_rgb(argb2rgb_o_wire_rgb),
    .o_wire_valid(argb2rgb_o_wire_valid)
    );

painterengine_gpu_fifo painterengine_gpu_colorconvert_fifo2(
    .i_wire_write_clock(clock),
    .i_wire_read_clock(clock),
    .i_wire_resetn(resetn),

    .i_wire_write(argb2rgb_o_wire_valid),
    .i_wire_read(1'b0),

    .i_wire_data_in(argb2rgb_o_wire_rgb),
    .o_wire_data_out(),

    .o_wire_almost_full(),
    .o_wire_full(),
    .o_wire_almost_empty(),
    .o_wire_empty()
);

endmodule