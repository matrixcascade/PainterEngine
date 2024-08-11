`timescale 1 ns / 1 ns

module painterengine_gpu_rasterizer(
    input wire       i_wire_clock,
    input wire       i_wire_resetn,
    input wire[31:0] i_wire_test_point,
    input wire       i_wire_valid,
    input wire[31:0] i_wire_point1,
    input wire[31:0] i_wire_point2,
    input wire[31:0] i_wire_point3,
    input wire[31:0] i_wire_yes_color,
    input wire[31:0] i_wire_no_color,

    output wire o_wire_valid,
    output wire[31:0] o_wire_color
);
reg signed [15:0] reg_x;
reg signed [15:0] reg_y;
reg signed [15:0] reg_x1;
reg signed [15:0] reg_y1;
reg signed [15:0] reg_x2;
reg signed [15:0] reg_y2;
reg signed [15:0] reg_x3;
reg signed [15:0] reg_y3;

reg signed [31:0] x2_sub_x1;
reg signed [31:0] y_sub_y1;
reg signed [31:0] y2_sub_y1;
reg signed [31:0] x_sub_x1;
reg signed [31:0] x3_sub_x2;
reg signed [31:0] y_sub_y2;
reg signed [31:0] y3_sub_y2;
reg signed [31:0] x_sub_x2;
reg signed [31:0] x1_sub_x3;
reg signed [31:0] y_sub_y3;
reg signed [31:0] y1_sub_y3;
reg signed [31:0] x_sub_x3;

reg signed [31:0] x2_sub_x1_mul_y_sub_y1;
reg signed [31:0] y2_sub_y1_mul_x_sub_x1;
reg signed [31:0] x3_sub_x2_mul_y_sub_y2;
reg signed [31:0] y3_sub_y2_mul_x_sub_x2;
reg signed [31:0] x1_sub_x3_mul_y_sub_y3;
reg signed [31:0] y1_sub_y3_mul_x_sub_x3;

reg signed [31:0] area1;
reg signed [31:0] area2;
reg signed [31:0] area3;

reg [31:0] reg_yes_color_d0;
reg [31:0] reg_no_color_d0;
reg [31:0] reg_yes_color_d1;
reg [31:0] reg_no_color_d1;
reg [31:0] reg_yes_color_d2;
reg [31:0] reg_no_color_d2;
reg [31:0] reg_yes_color_d3;
reg [31:0] reg_no_color_d3;

reg [31:0] reg_color;

reg reg_valid_d0;
reg reg_valid_d1;
reg reg_valid_d2;
reg reg_valid_d3;
reg reg_valid_d4;

assign o_wire_valid=reg_valid_d4;
assign o_wire_color=reg_valid_d4?reg_color:0;


always @(posedge i_wire_clock or negedge i_wire_resetn)
begin
    if(!i_wire_resetn)
    begin
        reg_x<=0;
        reg_y<=0;
        reg_x1<=0;
        reg_y1<=0;
        reg_x2<=0;
        reg_y2<=0;
        reg_x3<=0;
        reg_y3<=0;
        reg_yes_color_d0<=0;
        reg_no_color_d0<=0;
        reg_valid_d0<=0;
    end
    else
    begin
    reg_x<=i_wire_test_point[15:0];
    reg_y<=i_wire_test_point[31:16];
    reg_x1<=i_wire_point1[15:0];
    reg_y1<=i_wire_point1[31:16];
    reg_x2<=i_wire_point2[15:0];
    reg_y2<=i_wire_point2[31:16];
    reg_x3<=i_wire_point3[15:0];
    reg_y3<=i_wire_point3[31:16];
    reg_yes_color_d0<=i_wire_yes_color;
    reg_no_color_d0<=i_wire_no_color;
    reg_valid_d0<=i_wire_valid;
    end
end

always @(posedge i_wire_clock or negedge i_wire_resetn)
begin
    if(!i_wire_resetn)
    begin
        x2_sub_x1<=0;
        y_sub_y1<=0;
        y2_sub_y1<=0;
        x_sub_x1<=0;
        x3_sub_x2<=0;
        y_sub_y2<=0;
        y3_sub_y2<=0;
        x_sub_x2<=0;
        x1_sub_x3<=0;
        y_sub_y3<=0;
        y1_sub_y3<=0;
        x_sub_x3<=0;
        reg_yes_color_d1<=0;
        reg_no_color_d1<=0;
        reg_valid_d1<=0;
    end
    else
    begin
    x2_sub_x1<=reg_x2-reg_x1;
    y_sub_y1<=reg_y-reg_y1;
    y2_sub_y1<=reg_y2-reg_y1;
    x_sub_x1<=reg_x-reg_x1;
    x3_sub_x2<=reg_x3-reg_x2;
    y_sub_y2<=reg_y-reg_y2;
    y3_sub_y2<=reg_y3-reg_y2;
    x_sub_x2<=reg_x-reg_x2;
    x1_sub_x3<=reg_x1-reg_x3;
    y_sub_y3<=reg_y-reg_y3;
    y1_sub_y3<=reg_y1-reg_y3;
    x_sub_x3<=reg_x-reg_x3;
    reg_yes_color_d1<=reg_yes_color_d0;
    reg_no_color_d1<=reg_no_color_d0;
    reg_valid_d1<=reg_valid_d0;
    end
end

always @(posedge i_wire_clock or negedge i_wire_resetn)
begin
    if(!i_wire_resetn)
    begin
        x2_sub_x1_mul_y_sub_y1<=0;
        y2_sub_y1_mul_x_sub_x1<=0;
        x3_sub_x2_mul_y_sub_y2<=0;
        y3_sub_y2_mul_x_sub_x2<=0;
        x1_sub_x3_mul_y_sub_y3<=0;
        y1_sub_y3_mul_x_sub_x3<=0;
        reg_yes_color_d2<=0;
        reg_no_color_d2<=0;
        reg_valid_d2<=0;
    end
    else
    begin
    x2_sub_x1_mul_y_sub_y1<=x2_sub_x1*y_sub_y1;
    y2_sub_y1_mul_x_sub_x1<=y2_sub_y1*x_sub_x1;
    x3_sub_x2_mul_y_sub_y2<=x3_sub_x2*y_sub_y2;
    y3_sub_y2_mul_x_sub_x2<=y3_sub_y2*x_sub_x2;
    x1_sub_x3_mul_y_sub_y3<=x1_sub_x3*y_sub_y3;
    y1_sub_y3_mul_x_sub_x3<=y1_sub_y3*x_sub_x3;
    reg_yes_color_d2<=reg_yes_color_d1;
    reg_no_color_d2<=reg_no_color_d1;
    reg_valid_d2<=reg_valid_d1;
    end
end

always @(posedge i_wire_clock or negedge i_wire_resetn)
begin
    if(!i_wire_resetn)
    begin
        area1<=0;
        area2<=0;
        area3<=0;
        reg_yes_color_d3<=0;
        reg_no_color_d3<=0;
        reg_valid_d3<=0;
    end
    else
    begin
    area1<=x2_sub_x1_mul_y_sub_y1-y2_sub_y1_mul_x_sub_x1;
    area2<=x3_sub_x2_mul_y_sub_y2-y3_sub_y2_mul_x_sub_x2;
    area3<=x1_sub_x3_mul_y_sub_y3-y1_sub_y3_mul_x_sub_x3;
    reg_yes_color_d3<=reg_yes_color_d2;
    reg_no_color_d3<=reg_no_color_d2;
    reg_valid_d3<=reg_valid_d2;
    end
end

always @(posedge i_wire_clock or negedge i_wire_resetn)
begin
    if(!i_wire_resetn)
    begin
        reg_color<=0;
        reg_valid_d4<=0;
    end
    else
    begin
    if(area1[31]^area2[31]^area3[31]==0)
    begin
        reg_color<=reg_no_color_d3;
    end
    else
    begin
        reg_color<=reg_yes_color_d3;
    end
    reg_valid_d4<=reg_valid_d3;
    end
end

endmodule


