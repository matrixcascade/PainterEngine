
	
`timescale 1 ns / 1 ns
    
    module painterengine_gpu_pixel_clock
	(
		input wire i_wire_5x_pixel_clock,
		input wire i_wire_resetn,
		output wire o_wire_pixel_clock
	);
		reg 	[2:0]reg_count;
		reg 	reg_pixel_clock;
		assign 	o_wire_pixel_clock=reg_pixel_clock;
		
    	always@(posedge i_wire_5x_pixel_clock or negedge i_wire_resetn)
		begin
			if(!i_wire_resetn)
			begin
				reg_count<=3'd0;
				reg_pixel_clock<=1'b0;
			end
			else
			begin
				if(reg_count>=3'd4)
				begin 
					reg_pixel_clock<=1'b1;
					reg_count<=3'd0;
				end
       		else 
				begin  
					reg_pixel_clock<=1'b0; 
					reg_count<=reg_count+1'b1;
				end
			end
    	end
	endmodule