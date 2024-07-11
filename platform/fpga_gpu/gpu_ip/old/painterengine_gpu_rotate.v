	`timescale 1 ns / 1 ns
	
	module painterengine_gpu_rotate #
		(
		parameter integer iteration	= 22
		)
		(
  		input 			i_wire_clock,
  		input 	signed 	[31:0] i_wire_x,
		input 	signed 	[31:0] i_wire_y,
  		input 	signed 	[31:0] i_wire_angle,
  		output 	signed  [31:0] o_wire_x,
		output 	signed  [31:0] o_wire_y
		);
  		wire signed 	[31:0] deg_table [0:iteration-1];
                          
  		assign deg_table[0] = 32'd2949120;
		assign deg_table[1] = 32'd1740967;
		assign deg_table[2] = 32'd919879;
		assign deg_table[3] = 32'd466945;
		assign deg_table[4] = 32'd234378;
		assign deg_table[5] = 32'd117303;
		assign deg_table[6] = 32'd58666;
		assign deg_table[7] = 32'd29334;
		assign deg_table[8] = 32'd14667;
		assign deg_table[9] = 32'd7333;
		assign deg_table[10] = 32'd3666;
		assign deg_table[11] = 32'd1833;
		assign deg_table[12] = 32'd916;
		assign deg_table[13] = 32'd458;
		assign deg_table[14] = 32'd229;
		assign deg_table[15] = 32'd114;
		assign deg_table[16] = 32'd57;
		assign deg_table[17] = 32'd28;
		assign deg_table[18] = 32'd14;
		assign deg_table[19] = 32'd7;
		assign deg_table[20] = 32'd3;
		assign deg_table[21] = 32'd1;


  		reg signed [31:0] x [0:iteration-1];
  		reg signed [31:0] y [0:iteration-1];
  		reg signed [31:0] z [0:iteration-1];

		wire [31:0] aligned_x, aligned_y,aligned_angle;

		assign aligned_x=(i_wire_x>>>10)*622;
		assign aligned_y=(i_wire_y>>>10)*622;
		assign aligned_angle=aligned_angle>0?{i_wire_angle[31:16]%360,16'b0}:{(360+(i_wire_angle[31:16]%360)),16'b0};

  		always @(posedge i_wire_clock)
  		begin 
			if ((aligned_angle>>16)<=90) 
			begin
				x[0] <= aligned_x;
        		y[0] <= aligned_y;
        		z[0] <= aligned_angle;
			end	
			else if ((aligned_angle>>16)<=180)
			begin
				x[0] <= -aligned_y;
				y[0] <= aligned_x;
				z[0] <= aligned_angle-(32'd90*65536);
			end
			else if ((aligned_angle>>16)<=270)
			begin
				x[0] <= -aligned_x;
				y[0] <= -aligned_y;
				z[0] <= aligned_angle-(32'd180*65536);
			end
			else
			begin
				x[0] <= aligned_y;
				y[0] <= -aligned_x;
				z[0] <= aligned_angle-(32'd270*65536);
			end
 		 end

 		genvar i;
 		generate
 		for (i=0; i < (iteration-1); i=i+1)
 		begin: xyz
 		  wire z_sign;
 		  wire signed [iteration-1:0] x_shr, y_shr;

 		  //Xn+1 = Xn-dn(2^(-i)Yn)
		  //Yn+1 = Yn+dn(2^(-i)Xn)
		  //Zn+1=Zn-dn(theta)
 		  assign x_shr = x[i] >>> i;
 		  assign y_shr = y[i] >>> i;
 		  assign z_sign = z[i][31];

 		  always @(posedge i_wire_clock)
 		  begin
 		    x[i+1] <= z_sign ? x[i] + y_shr : x[i] - y_shr;
 		    y[i+1] <= z_sign ? y[i] - x_shr : y[i] + x_shr;
 		    z[i+1] <= z_sign ? z[i] + deg_table[i] : z[i] - deg_table[i];
 		  end
 		end
 		endgenerate

  		assign o_wire_x = x[iteration-1];
  		assign o_wire_y = y[iteration-1];
	endmodule
