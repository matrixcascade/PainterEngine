`timescale 1 ns / 1 ns

module painterengine_gpu_alphablend_tb();
reg clock=0;
reg resetn=0;

initial begin
    $dumpfile("painterengine_gpu_alphablend_tb.vcd");
    $dumpvars(0, painterengine_gpu_alphablend_tb);
    $display("Start of simulation");
    resetn=0;
    #1000;
    resetn=1;
end

always #5 clock = ~clock;

painterengine_gpu_alphablend tb_alphablend(
    .i_wire_clock(clock),
    .i_wire_resetn(resetn),
    .i_wire_valid(1),
    .o_wire_valid(),
    .a1(8'd128),
    .r1(8'd192),
    .g1(8'd128),
    .b1(8'd28),
    .a2(8'd128),
    .r2(8'd87),
    .g2(8'd68),
    .b2(8'd55),
    .ba(8'd64),
    .br(8'd64),
    .bg(8'd64),
    .bb(8'd64),
    .a(),
    .r(),
    .g(),
    .b()
);
endmodule