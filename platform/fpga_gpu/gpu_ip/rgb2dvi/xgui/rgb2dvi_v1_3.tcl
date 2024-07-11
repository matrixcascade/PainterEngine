# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  ipgui::add_param $IPINST -name "Component_Name"
  #Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Page 0"]
  ipgui::add_param $IPINST -name "kRstActiveHigh" -parent ${Page_0}
  ipgui::add_param $IPINST -name "kGenerateSerialClk" -parent ${Page_0}
  ipgui::add_param $IPINST -name "kClkPrimitive" -parent ${Page_0} -layout horizontal
  ipgui::add_param $IPINST -name "kClkRange" -parent ${Page_0} -layout horizontal
  #Adding Group
  set Invert_TMDS_lanes [ipgui::add_group $IPINST -name "Invert TMDS lanes" -parent ${Page_0} -layout horizontal]
  set_property tooltip {Use the tickboxes below to invert data or clock lanes independently on boards with the P and N sides of the pair swapped.} ${Invert_TMDS_lanes}
  ipgui::add_param $IPINST -name "kD0Swap" -parent ${Invert_TMDS_lanes}
  ipgui::add_param $IPINST -name "kD1Swap" -parent ${Invert_TMDS_lanes}
  ipgui::add_param $IPINST -name "kD2Swap" -parent ${Invert_TMDS_lanes}
  ipgui::add_param $IPINST -name "kClkSwap" -parent ${Invert_TMDS_lanes}



}

proc update_PARAM_VALUE.TMDS_BOARD_INTERFACE { PARAM_VALUE.TMDS_BOARD_INTERFACE } {
	# Procedure called to update TMDS_BOARD_INTERFACE when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.TMDS_BOARD_INTERFACE { PARAM_VALUE.TMDS_BOARD_INTERFACE } {
	# Procedure called to validate TMDS_BOARD_INTERFACE
	return true
}

proc update_PARAM_VALUE.kClkRange { PARAM_VALUE.kClkRange } {
	# Procedure called to update kClkRange when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.kClkRange { PARAM_VALUE.kClkRange } {
	# Procedure called to validate kClkRange
	return true
}

proc update_PARAM_VALUE.kClkSwap { PARAM_VALUE.kClkSwap } {
	# Procedure called to update kClkSwap when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.kClkSwap { PARAM_VALUE.kClkSwap } {
	# Procedure called to validate kClkSwap
	return true
}

proc update_PARAM_VALUE.kD0Swap { PARAM_VALUE.kD0Swap } {
	# Procedure called to update kD0Swap when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.kD0Swap { PARAM_VALUE.kD0Swap } {
	# Procedure called to validate kD0Swap
	return true
}

proc update_PARAM_VALUE.kD1Swap { PARAM_VALUE.kD1Swap } {
	# Procedure called to update kD1Swap when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.kD1Swap { PARAM_VALUE.kD1Swap } {
	# Procedure called to validate kD1Swap
	return true
}

proc update_PARAM_VALUE.kD2Swap { PARAM_VALUE.kD2Swap } {
	# Procedure called to update kD2Swap when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.kD2Swap { PARAM_VALUE.kD2Swap } {
	# Procedure called to validate kD2Swap
	return true
}

proc update_PARAM_VALUE.kClkPrimitive { PARAM_VALUE.kClkPrimitive } {
	# Procedure called to update kClkPrimitive when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.kClkPrimitive { PARAM_VALUE.kClkPrimitive } {
	# Procedure called to validate kClkPrimitive
	return true
}

proc update_PARAM_VALUE.kRstActiveHigh { PARAM_VALUE.kRstActiveHigh } {
	# Procedure called to update kRstActiveHigh when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.kRstActiveHigh { PARAM_VALUE.kRstActiveHigh } {
	# Procedure called to validate kRstActiveHigh
	return true
}

proc update_PARAM_VALUE.kGenerateSerialClk { PARAM_VALUE.kGenerateSerialClk } {
	# Procedure called to update kGenerateSerialClk when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.kGenerateSerialClk { PARAM_VALUE.kGenerateSerialClk } {
	# Procedure called to validate kGenerateSerialClk
	return true
}


proc update_MODELPARAM_VALUE.kGenerateSerialClk { MODELPARAM_VALUE.kGenerateSerialClk PARAM_VALUE.kGenerateSerialClk } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.kGenerateSerialClk}] ${MODELPARAM_VALUE.kGenerateSerialClk}
}

proc update_MODELPARAM_VALUE.kClkPrimitive { MODELPARAM_VALUE.kClkPrimitive PARAM_VALUE.kClkPrimitive } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.kClkPrimitive}] ${MODELPARAM_VALUE.kClkPrimitive}
}

proc update_MODELPARAM_VALUE.kRstActiveHigh { MODELPARAM_VALUE.kRstActiveHigh PARAM_VALUE.kRstActiveHigh } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.kRstActiveHigh}] ${MODELPARAM_VALUE.kRstActiveHigh}
}

proc update_MODELPARAM_VALUE.kClkRange { MODELPARAM_VALUE.kClkRange PARAM_VALUE.kClkRange } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.kClkRange}] ${MODELPARAM_VALUE.kClkRange}
}

proc update_MODELPARAM_VALUE.kD0Swap { MODELPARAM_VALUE.kD0Swap PARAM_VALUE.kD0Swap } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.kD0Swap}] ${MODELPARAM_VALUE.kD0Swap}
}

proc update_MODELPARAM_VALUE.kD1Swap { MODELPARAM_VALUE.kD1Swap PARAM_VALUE.kD1Swap } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.kD1Swap}] ${MODELPARAM_VALUE.kD1Swap}
}

proc update_MODELPARAM_VALUE.kD2Swap { MODELPARAM_VALUE.kD2Swap PARAM_VALUE.kD2Swap } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.kD2Swap}] ${MODELPARAM_VALUE.kD2Swap}
}

proc update_MODELPARAM_VALUE.kClkSwap { MODELPARAM_VALUE.kClkSwap PARAM_VALUE.kClkSwap } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.kClkSwap}] ${MODELPARAM_VALUE.kClkSwap}
}

