//****************************************Copyright (c)***********************************//
//技术支持：www.openedv.com
//淘宝店铺：http://openedv.taobao.com 
//关注微信公众平台微信号："正点原子"，免费获取FPGA & STM32资料。
//版权所有，盗版必究。
//Copyright(C) 正点原子 2018-2028
//All rights reserved
//----------------------------------------------------------------------------------------
// File name:           serializer_10_to_1
// Last modified Date:  2019/7/1 9:30:00
// Last Version:        V1.1
// Descriptions:        用于实现10:1并串转换
//----------------------------------------------------------------------------------------
// Created by:          正点原子
// Created date:        2019/7/1 9:30:00
// Version:             V1.0
// Descriptions:        The original version
//
//----------------------------------------------------------------------------------------
//****************************************************************************************//

`timescale 1ns / 1ps

module serializer_10_to_1(
    input           reset,              // 复位,高有效
    input           paralell_clk,       // 输入并行数据时钟
    input           serial_clk_5x,      // 输入串行数据时钟
    input   [9:0]   paralell_data,      // 输入并行数据

    output 			serial_data_out     // 输出串行数据
    );
    
//wire define
wire		cascade1;     //用于两个OSERDESE2级联的信号
wire		cascade2;
  
//*****************************************************
//**                    main code
//***************************************************** 
    
//例化OSERDESE2原语，实现并串转换,Master模式
OSERDESE2 #(
    .DATA_RATE_OQ   ("DDR"),       // 设置双倍数据速率
    .DATA_RATE_TQ   ("SDR"),       // DDR, BUF, SDR
    .DATA_WIDTH     (10),           // 输入的并行数据宽度为10bit
    .SERDES_MODE    ("MASTER"),    // 设置为Master，用于10bit宽度扩展
    .TBYTE_CTL      ("FALSE"),     // Enable tristate byte operation (FALSE, TRUE)
    .TBYTE_SRC      ("FALSE"),     // Tristate byte source (FALSE, TRUE)
    .TRISTATE_WIDTH (1)             // 3-state converter width (1,4)
)
OSERDESE2_Master (
    .CLK        (serial_clk_5x),    // 串行数据时钟,5倍时钟频率
    .CLKDIV     (paralell_clk),     // 并行数据时钟
    .RST        (reset),            // 1-bit input: Reset
    .OCE        (1'b1),             // 1-bit input: Output data clock enable
    
    .OQ         (serial_data_out),  // 串行输出数据
    
    .D1         (paralell_data[0]), // D1 - D8: 并行数据输入
    .D2         (paralell_data[1]),
    .D3         (paralell_data[2]),
    .D4         (paralell_data[3]),
    .D5         (paralell_data[4]),
    .D6         (paralell_data[5]),
    .D7         (paralell_data[6]),
    .D8         (paralell_data[7]),
   
    .SHIFTIN1   (cascade1),         // SHIFTIN1 用于位宽扩展
    .SHIFTIN2   (cascade2),         // SHIFTIN2
    .SHIFTOUT1  (),                 // SHIFTOUT1: 用于位宽扩展
    .SHIFTOUT2  (),                 // SHIFTOUT2
        
    .OFB        (),                 // 以下是未使用信号
    .T1         (1'b0),             
    .T2         (1'b0),
    .T3         (1'b0),
    .T4         (1'b0),
    .TBYTEIN    (1'b0),             
    .TCE        (1'b0),             
    .TBYTEOUT   (),                 
    .TFB        (),                 
    .TQ         ()                  
);
   
//例化OSERDESE2原语，实现并串转换,Slave模式
OSERDESE2 #(
    .DATA_RATE_OQ   ("DDR"),       // 设置双倍数据速率
    .DATA_RATE_TQ   ("SDR"),       // DDR, BUF, SDR
    .DATA_WIDTH     (10),           // 输入的并行数据宽度为10bit
    .SERDES_MODE    ("SLAVE"),     // 设置为Slave，用于10bit宽度扩展
    .TBYTE_CTL      ("FALSE"),     // Enable tristate byte operation (FALSE, TRUE)
    .TBYTE_SRC      ("FALSE"),     // Tristate byte source (FALSE, TRUE)
    .TRISTATE_WIDTH (1)             // 3-state converter width (1,4)
)
OSERDESE2_Slave (
    .CLK        (serial_clk_5x),    // 串行数据时钟,5倍时钟频率
    .CLKDIV     (paralell_clk),     // 并行数据时钟
    .RST        (reset),            // 1-bit input: Reset
    .OCE        (1'b1),             // 1-bit input: Output data clock enable
    
    .OQ         (),                 // 串行输出数据
    
    .D1         (1'b0),             // D1 - D8: 并行数据输入
    .D2         (1'b0),
    .D3         (paralell_data[8]),
    .D4         (paralell_data[9]),
    .D5         (1'b0),
    .D6         (1'b0),
    .D7         (1'b0),
    .D8         (1'b0),
   
    .SHIFTIN1   (),                 // SHIFTIN1 用于位宽扩展
    .SHIFTIN2   (),                 // SHIFTIN2
    .SHIFTOUT1  (cascade1),         // SHIFTOUT1: 用于位宽扩展
    .SHIFTOUT2  (cascade2),         // SHIFTOUT2
        
    .OFB        (),                 // 以下是未使用信号
    .T1         (1'b0),             
    .T2         (1'b0),
    .T3         (1'b0),
    .T4         (1'b0),
    .TBYTEIN    (1'b0),             
    .TCE        (1'b0),             
    .TBYTEOUT   (),                 
    .TFB        (),                 
    .TQ         ()                  
);  
        
endmodule