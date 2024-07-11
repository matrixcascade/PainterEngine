	    
`timescale 1 ns / 1 ns

    module clock_50MHZ(
		input  wire i_wire_50MHZ,
	    output wire o_wire_clock_50MHZ,
	    output wire o_wire_enabled
	    );
		reg enabled=0;
		reg [15:0] counter=0;
		always @(posedge i_wire_50MHZ)
		begin
			if (counter<100)
			begin
				counter<=counter+1;
				enabled<=0;
			end
			else
			begin
				counter<=counter;
				enabled<=1;
			end
		end
		assign o_wire_clock_50MHZ = i_wire_50MHZ;
		assign o_wire_enabled = enabled;
	endmodule
    
	