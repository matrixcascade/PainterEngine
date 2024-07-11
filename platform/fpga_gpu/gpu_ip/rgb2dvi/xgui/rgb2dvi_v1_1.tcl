# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  set Component_Name  [  ipgui::add_param $IPINST -name "Component_Name" -display_name {Component Name}]
  set_property tooltip {Component Name} ${Component_Name}
  #Adding Page
  set Page_0  [  ipgui::add_page $IPINST -name "Page 0" -display_name {Page 0}]
  set_property tooltip {Page 0} ${Page_0}
  set kRstActiveHigh  [  ipgui::add_param $IPINST -name "kRstActiveHigh" -parent ${Page_0} -display_name {Reset active high}]
  set_property tooltip {Reset active high} ${kRstActiveHigh}
  set kGenerateSerialClk  [  ipgui::add_param $IPINST -name "kGenerateSerialClk" -parent ${Page_0} -display_name {Generate SerialClk internally from pixel clock.}]
  set_property tooltip {Generate SerialClk internally from pixel clock.} ${kGenerateSerialClk}
  set kClkPrimitive  [  ipgui::add_param $IPINST -name "kClkPrimitive" -parent ${Page_0} -display_name {MMCM/PLL}]
  set_property tooltip {MMCM/PLL} ${kClkPrimitive}


}

proc update_PARAM_VALUE.kRstActiveHigh { PARAM_VALUE.kRstActiveHigh } {
	# Procedure called to update kRstActiveHigh when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.kRstActiveHigh { PARAM_VALUE.kRstActiveHigh } {
	# Procedure called to validate kRstActiveHigh
	return true
}

proc update_PARAM_VALUE.kClkPrimitive { PARAM_VALUE.kClkPrimitive } {
	# Procedure called to update kClkPrimitive when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.kClkPrimitive { PARAM_VALUE.kClkPrimitive } {
	# Procedure called to validate kClkPrimitive
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

