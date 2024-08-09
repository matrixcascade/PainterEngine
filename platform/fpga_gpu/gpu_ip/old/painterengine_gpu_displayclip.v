	    
	`timescale 1 ns / 1 ns
	`define VIDEO_DISPLAY_MODE_1280_720   3'b000
	`define VIDEO_DISPLAY_MODE_480_272    3'b001
	`define VIDEO_DISPLAY_MODE_640_480    3'b010
	`define VIDEO_DISPLAY_MODE_800_480    3'b011
	`define VIDEO_DISPLAY_MODE_800_600    3'b100
	`define VIDEO_DISPLAY_MODE_1024_768   3'b101
	`define VIDEO_DISPLAY_MODE_1920_1080  3'b110


	module painterengine_gpu_displayclip
		(
		input wire													i_wire_clock,
		input wire  		 										i_wire_resetn,
		output wire 												o_wire_valid,

		input wire[2:0]             								i_wire_display_mode,  
		input wire[15:0]   											i_wire_image_width,         //texture width
		input wire[15:0]   											i_wire_image_height,        //texture height
		output wire[15:0]   										o_wire_clip_width,         //display width
		output wire[15:0]   										o_wire_clip_height       //display height
		);
		reg [15:0]   												reg_clip_width;
		reg [15:0]   												reg_clip_height;
		reg                                                         reg_valid;
		assign o_wire_clip_width=reg_clip_width;
		assign o_wire_clip_height=reg_clip_height;
		assign o_wire_valid=reg_valid;

		always @(posedge i_wire_clock or negedge i_wire_resetn)
		begin
			if(!i_wire_resetn)
			begin
				reg_clip_width<=0;
				reg_clip_height<=0;
				reg_valid<=0;
			end
			else
			begin
				reg_valid<=1;
				case (i_wire_display_mode)
				`VIDEO_DISPLAY_MODE_1280_720:
				begin
					reg_clip_width<=i_wire_image_width>1280?1280:i_wire_image_width;
					reg_clip_height<=i_wire_image_height>720?720:i_wire_image_height;
				end
				`VIDEO_DISPLAY_MODE_480_272:
				begin
					reg_clip_width<=i_wire_image_width>480?480:i_wire_image_width;
					reg_clip_height<=i_wire_image_height>272?272:i_wire_image_height;
				end
				`VIDEO_DISPLAY_MODE_640_480:
				begin
					reg_clip_width<=i_wire_image_width>640?640:i_wire_image_width;
					reg_clip_height<=i_wire_image_height>480?480:i_wire_image_height;
				end
				`VIDEO_DISPLAY_MODE_800_480:
				begin
					reg_clip_width<=i_wire_image_width>800?800:i_wire_image_width;
					reg_clip_height<=i_wire_image_height>480?480:i_wire_image_height;
				end
				`VIDEO_DISPLAY_MODE_800_600:
				begin
					reg_clip_width<=i_wire_image_width>800?800:i_wire_image_width;
					reg_clip_height<=i_wire_image_height>600?600:i_wire_image_height;
				end
				`VIDEO_DISPLAY_MODE_1024_768:
				begin
					reg_clip_width<=i_wire_image_width>1024?1024:i_wire_image_width;
					reg_clip_height<=i_wire_image_height>768?768:i_wire_image_height;
				end
				`VIDEO_DISPLAY_MODE_1920_1080:
				begin
					reg_clip_width<=i_wire_image_width>1920?1920:i_wire_image_width;
					reg_clip_height<=i_wire_image_height>1080?1080:i_wire_image_height;
				end
				default:
				begin
					reg_clip_width<=0;
					reg_clip_height<=0;
				end
				endcase
			end
		end
	endmodule
