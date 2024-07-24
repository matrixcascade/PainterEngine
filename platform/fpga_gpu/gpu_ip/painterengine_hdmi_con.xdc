set_property PACKAGE_PIN L21 [get_ports {TMDS_DATA_p_0[1]}]
set_property PACKAGE_PIN J21 [get_ports {TMDS_DATA_p_0[2]}]
set_property PACKAGE_PIN M21 [get_ports {TMDS_DATA_p_0[0]}]
set_property PACKAGE_PIN N22 [get_ports TMDS_CLK_p_0]

create_clock -period 10 [ get_ports TMDS_CLK_p_0]
create_clock -period 10 [ get_ports {TMDS_DATA_p_0[0]}] 
create_clock -period 10 [ get_ports {TMDS_DATA_p_0[1]}] 
create_clock -period 10 [ get_ports {TMDS_DATA_p_0[2]}] 
