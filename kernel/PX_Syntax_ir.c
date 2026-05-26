#include "PX_Syntax_ir.h"


PX_SYNTAX_FUNCTION(PX_Syntax_IR_error)
{
	PX_Syntax_Terminate(pSyntax, "runtime:error:unknown ir error");
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_ir_skip_newline_spacer)
{
	px_int counter = 0;
	while (PX_TRUE)
	{
		px_char ch = PX_Syntax_PreviewNextChar(pSyntax);
		if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
		{
			PX_Syntax_GetNextChar(pSyntax);
			counter++;
			continue;
		}
		else
		{
			break;
		}
	}
	return counter != 0;
}

static px_int PX_Syntax_IR_GetScanMode(PX_Syntax* pSyntax)
{
	px_abi* pscope = PX_Syntax_GetAbiFromForward(pSyntax, "scope");
	if (!pscope) return -1;
	return PX_AbiGetValue_int(pscope, "scan_mode");
}

static px_bool PX_Syntax_IR_EmitText(PX_Syntax* pSyntax, const px_byte* payload, px_int size)
{
	px_abi* pscope = PX_Syntax_GetAbiFromForward(pSyntax, "scope");
	px_int scan_mode;
	PX_Syntax_bin_map_to_ir mapEntry;
	px_dword bin_size;
	if (!pscope)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:EmitText no scope");
		return PX_FALSE;
	}
	scan_mode = PX_AbiGetValue_int(pscope, "scan_mode");
	if (scan_mode == PX_SYNTAX_IR_SCAN_MODE_LABEL)
	{
		px_int text_length = PX_AbiGetValue_int(pscope, "text_length");
		text_length += size;
		return PX_AbiSet_int(pscope, "text_length", text_length);
	}
	if (scan_mode == PX_SYNTAX_IR_SCAN_MODE_RDATA)
	{
		return PX_TRUE;
	}
	
	if (!PX_AbiGet_data(pscope, "text", &bin_size))
	{
		bin_size = 0;
	}
	mapEntry.ip = bin_size;
	mapEntry.source_index = pSyntax->reg_expr_source_index;
	mapEntry.source_line = pSyntax->reg_expr_begin_line;
	if (!PX_Syntax_AppendData(pSyntax, "scope", "bin_map_to_ir", (px_byte*)&mapEntry, sizeof(mapEntry)))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:EmitText failed to append bin_map_to_ir");
		return PX_FALSE;
	}
	
	return PX_Syntax_AppendData(pSyntax, "scope", "text", payload, size);
}

static px_bool PX_Syntax_IR_EmitRdata(PX_Syntax* pSyntax, const px_byte* payload, px_int size)
{
	px_abi* pscope = PX_Syntax_GetAbiFromForward(pSyntax, "scope");
	px_int scan_mode;
	if (!pscope)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:EmitRdata no scope");
		return PX_FALSE;
	}
	scan_mode = PX_AbiGetValue_int(pscope, "scan_mode");
	if (scan_mode == PX_SYNTAX_IR_SCAN_MODE_LABEL)
	{
		return PX_TRUE;
	}
	if (scan_mode == PX_SYNTAX_IR_SCAN_MODE_RDATA)
	{
		px_int rdata_length = PX_AbiGetValue_int(pscope, "rdata_length");
		rdata_length += size;
		return PX_AbiSet_int(pscope, "rdata_length", rdata_length);
	}
	return PX_Syntax_AppendData(pSyntax, "scope", "rdata", payload, size);
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_rx)
{
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	const px_char* pvalue;
	px_abi* new_rx;
	px_int reg_index;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "identifier"), "IR_rx must follow an identifier abi");
	pvalue = PX_AbiGet_string(plast, "value");
	if (pvalue[0] != 'r' && pvalue[0] !='R')
	{
		return PX_FALSE;
	}
	
	if (PX_strequ2(pvalue, "ip"))
	{
		reg_index = 4;
	}
	else if (PX_strequ2(pvalue, "sp"))
	{
		reg_index = 5;
	}
	else if (PX_strequ2(pvalue, "bp"))
	{
		reg_index = 6;
	}
	else if (PX_charIsNumeric(pvalue[1]) && pvalue[2] == '\0')
	{
		if (pvalue[1] < '0' || pvalue[1]>'7' )
		{
			PX_Syntax_Terminate(pSyntax, "IR_rx target must be r0-r7");
			return PX_FALSE;
		}
		reg_index = pvalue[1] - '0';
	}
	else
	{
		PX_Syntax_Terminate(pSyntax, "IR_rx target must be r0-r7");
		return PX_FALSE;
	}

	PX_Syntax_PopAbi(pSyntax);
	new_rx = PX_Syntax_NewAbi(pSyntax, "ir_rx", pSyntax->reg_lifetime);
	if (!new_rx)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_rx Memory Error1");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(new_rx, "index", reg_index))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_rx Memory Error2");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_fx)
{
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	const px_char* pvalue;
	px_abi* new_fx;
	px_int reg_index;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "identifier"), "IR_fx must follow an identifier abi");
	pvalue = PX_AbiGet_string(plast, "value");
	if (pvalue[0] != 'f' && pvalue[0] != 'F')
	{
		return PX_FALSE;
	}

	if (PX_charIsNumeric(pvalue[1]) && pvalue[2] == '\0')
	{
		if (pvalue[1] < '0' || pvalue[1] > '3')
		{
			PX_Syntax_Terminate(pSyntax, "IR_fx target must be f0-f3");
			return PX_FALSE;
		}
		reg_index = pvalue[1] - '0';
	}
	else
	{
		PX_Syntax_Terminate(pSyntax, "IR_fx target must be f0-f3");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	new_fx = PX_Syntax_NewAbi(pSyntax, "ir_fx", pSyntax->reg_lifetime);
	if (!new_fx)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_fx Memory Error1");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(new_fx, "index", reg_index))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_fx Memory Error2");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_local_addr)
{
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	const px_char* pvalue;
	px_int ivalue;
	px_abi* new_addr;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "const_int"), "IR_local_addr must follow an const_int abi");
	pvalue = PX_AbiGet_string(plast, "value");
	ivalue = PX_atoi(pvalue);
	if (ivalue<0)
	{
		PX_Syntax_Terminate(pSyntax, "IR_local_addr target must be non-negative");
		return PX_FALSE;
	}
	
	PX_Syntax_PopAbi(pSyntax);
	new_addr = PX_Syntax_NewAbi(pSyntax, "ir_addr", pSyntax->reg_lifetime);
	if (!new_addr)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_local_addr Memory Error1");
		return PX_FALSE;
	}
	if (!PX_AbiSet_dword(new_addr, "local_offset", ivalue))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_local_addr Memory Error2");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_global_addr)
{
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	const px_char* pvalue;
	px_int ivalue;
	px_abi* new_addr;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "const_int"), "IR_global_addr must follow an const_int abi");
	pvalue = PX_AbiGet_string(plast, "value");
	ivalue = PX_atoi(pvalue);
	if (ivalue<0)
	{
		PX_Syntax_Terminate(pSyntax, "IR_global_addr target must be non-negative");
		return PX_FALSE;
	}
	
	PX_Syntax_PopAbi(pSyntax);
	new_addr = PX_Syntax_NewAbi(pSyntax, "ir_addr", pSyntax->reg_lifetime);
	if (!new_addr)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_global_addr Memory Error1");
		return PX_FALSE;
	}
	if (!PX_AbiSet_dword(new_addr, "global_offset", ivalue))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_global_addr Memory Error2");
		return PX_FALSE;
	}
	return PX_TRUE;

}

px_bool PX_Syntax_IR_loadxx(PX_Syntax* pSyntax, px_bool signed_instr, px_int x)
{
	//loadu8[r0...r3], [n][bp + n]
	px_byte payload[8] = { 0 };
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* psecondlast = PX_Syntax_GetAbiSecondLast(pSyntax);
	px_int reg_index;
	px_dword offset;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "ir_addr"), "IR_loadxx must follow an addr abi");
	PX_ASSERTIFX(!PX_Syntax_CheckSecondLastAbiName(pSyntax, "ir_rx"), "IR_loadxx must follow an rx abi");
	reg_index = PX_AbiGetValue_int(psecondlast, "index");
	//opcode
	if (signed_instr)
	{
		switch (x)
		{
		case 8:
			payload[0] = PX_SYNTAX_MACHINE_OPCODE_LOADI8;
			break;
		case 16:
			payload[0] = PX_SYNTAX_MACHINE_OPCODE_LOADI16;
			break;
		case 32:
			payload[0] = PX_SYNTAX_MACHINE_OPCODE_LOADI32;
			break;
		default:
			PX_ASSERTX("IR_loadux x error");
			return PX_FALSE;
		}
	}
	else
	{
		switch (x)
		{
		case 8:
			payload[0] = PX_SYNTAX_MACHINE_OPCODE_LOADU8;
			break;
		case 16:
			payload[0] = PX_SYNTAX_MACHINE_OPCODE_LOADU16;
			break;
		case 32:
			payload[0] = PX_SYNTAX_MACHINE_OPCODE_LOADU32;
			break;
		default:
			PX_ASSERTX("IR_loadux x error");
			return PX_FALSE;
		}
	}


	//target register
	payload[1] = reg_index & 0x0f;

	//0 is global 1 is local
	
	if (PX_AbiExist(plast, "global_offset"))
	{
		offset = PX_AbiGetValue_dword(plast, "global_offset");
	}
	else
	{
		offset = PX_AbiGetValue_dword(plast, "local_offset");
		payload[1] |= 0x10;
	}
	*(px_dword*)(payload + 2) = offset;

	if (!PX_Syntax_IR_EmitText(pSyntax,payload, 6))//6bytes
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_loaduxx Memory Error1");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	PX_Syntax_PopAbi(pSyntax);

	return PX_TRUE;
}

px_bool PX_Syntax_IR_storex(PX_Syntax* pSyntax,px_int x)
{
	//loadu8[r0...r3], [n][bp + n]
	px_byte payload[8] = { 0 };
	px_abi* plast =PX_Syntax_GetAbiLast(pSyntax); 
	px_abi* psecondlast = PX_Syntax_GetAbiSecondLast(pSyntax);
	px_int reg_index;
	px_dword offset;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "ir_rx"), "IR_storexx must follow an addr abi");
	PX_ASSERTIFX(!PX_Syntax_CheckSecondLastAbiName(pSyntax, "ir_addr"), "IR_storexx must follow an rx abi");
	reg_index = PX_AbiGetValue_int(plast, "index");
	//opcode
	switch (x)
	{
	case 8:
		payload[0] = PX_SYNTAX_MACHINE_OPCODE_STORE8;
		break;
	case 16:
		payload[0] = PX_SYNTAX_MACHINE_OPCODE_STORE16;
		break;
	case 32:
		payload[0] = PX_SYNTAX_MACHINE_OPCODE_STORE32;
		break;
	default:
		PX_ASSERTX("IR_storex error");
		return PX_FALSE;
	}


	//target register
	payload[1] = reg_index & 0x0f;

	//0 is global 1 is local
	if (PX_AbiExist(psecondlast, "global_offset"))
	{
		offset = PX_AbiGetValue_dword(psecondlast, "global_offset");
	}
	else
	{
		offset = PX_AbiGetValue_dword(psecondlast, "local_offset");
		payload[1] |= 0x10;
	}
	*(px_dword*)(payload + 2) = offset;

	if (!PX_Syntax_IR_EmitText(pSyntax,payload, 6))//6bytes
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_loadu8 Memory Error1");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_opcode_handle_begin)
{
	px_int i,j,begin_source_index,begin,end_source_index,end;

	const px_char* pchar = PX_Syntax_GetCurrentLexeme(pSyntax);
	px_dword vallr=314159;
	px_byte* r = (px_byte*)&vallr;
	j = 0;
	for (i = 0; i < 16; i++)
	{
		if (pchar[i]==0)
		{
			break;
		}
		vallr = PX_rand_lcg(vallr+pchar[i]) +1013904223u;
	}
	begin_source_index = PX_Syntax_GetCurrentLexemeBeginSourceIndex(pSyntax);
	begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
	end_source_index = PX_Syntax_GetCurrentLexemeEndSourceIndex(pSyntax);
	end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);

	pSyntax->reg_expr_begin_line = PX_Syntax_GetCurrentLexemeLine(pSyntax);
	pSyntax->reg_expr_source_index = begin_source_index;

	//color
	PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end, PX_COLOR(255, 127+r[0]/2,127+ r[1]/2, r[2]), 0);
	return PX_TRUE;
}


PX_SYNTAX_FUNCTION(PX_Syntax_IR_db)
{
	//db label_name(identifier):  string/identifier(eg:"ff00ff001020....")
	px_char rdata_label_key[128] = "labels.";
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* psecondlast = PX_Syntax_GetAbiSecondLast(pSyntax);
	const px_char* plabel_value;
	const px_char* plast_value;
	px_byte* payload = PX_NULL;
	px_int payload_length;
	px_int scan_mode;
	px_abi *pscope = PX_Syntax_GetAbiFromForward(pSyntax, "scope");
	if (!pscope)
	{
		PX_Syntax_Terminate(pSyntax, "ir_db: no scope abi found");
		return PX_FALSE;
	}
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "const_string")&& !PX_Syntax_CheckLastAbiName(pSyntax, "identifier"), "IR_db must follow a const_string or identifier abi");
	PX_ASSERTIFX(!PX_Syntax_CheckSecondLastAbiName(pSyntax, "identifier"), "IR_db must follow an identifier abi");
	plabel_value = PX_AbiGetValue_string(psecondlast, "value");
	plast_value = PX_AbiGetValue_string(plast, "value");
	if (PX_strlen(plabel_value) + PX_strlen(rdata_label_key) >= 128)
	{
		PX_Syntax_Terminate(pSyntax, "IR_db label name too long");
		return PX_FALSE;
	}

	if (PX_Syntax_CheckLastAbiName(pSyntax, "const_string"))
	{
		payload_length = (px_int)PX_strlen(plast_value)+1;
	}
	else
	{
		if (PX_strlen(plast_value) & 1)
		{
			PX_Syntax_Terminate(pSyntax, "IR_db identifier value length must be even");
			return PX_FALSE;
		}
		payload_length = (px_int)PX_strlen(plast_value) / 2;
	}

	scan_mode = PX_AbiGetValue_int(pscope, "scan_mode");
	PX_strcat_s(rdata_label_key, 128, plabel_value);

	if (scan_mode == PX_SYNTAX_IR_SCAN_MODE_LABEL)
	{
		PX_Syntax_PopAbi(pSyntax);
		PX_Syntax_PopAbi(pSyntax);
		return PX_TRUE;
	}

	if (scan_mode == PX_SYNTAX_IR_SCAN_MODE_RDATA)
	{
		px_int text_length = PX_AbiGetValue_int(pscope, "text_length");
		px_int rdata_length = PX_AbiGetValue_int(pscope, "rdata_length");
		px_dword rdata_offset = (px_dword)(text_length + rdata_length);
		if (!PX_AbiSet_dword(pscope, rdata_label_key, rdata_offset))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_db Memory Error4");
			return PX_FALSE;
		}
		rdata_length += payload_length;
		PX_AbiSet_int(pscope, "rdata_length", rdata_length);
		PX_Syntax_PopAbi(pSyntax);
		PX_Syntax_PopAbi(pSyntax);
		return PX_TRUE;
	}

	if (PX_Syntax_CheckLastAbiName(pSyntax, "const_string"))
	{
		payload = PX_Malloc(px_byte,pSyntax->mp, payload_length);
		if (!payload)
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_db Memory Error1");
			return PX_FALSE;
		}
		PX_memcpy(payload, plast_value, payload_length);
	}
	else
	{
		payload = PX_Malloc(px_byte, pSyntax->mp, payload_length);
		if (!payload)
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_db Memory Error2");
			return PX_FALSE;
		}
		if(!PX_HexStringToBuffer(plast_value, payload))
		{
			PX_Free(pSyntax->mp, payload);
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_db HexStringToBuffer Error");
			return PX_FALSE;
		}
	}

	if (!PX_Syntax_AppendData(pSyntax,"scope","rdata", payload, payload_length))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_db Memory Error3");
		PX_Free(pSyntax->mp, payload);
		return PX_FALSE;
	}

	PX_Free(pSyntax->mp, payload);
	PX_Syntax_PopAbi(pSyntax);
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}


PX_SYNTAX_FUNCTION(PX_Syntax_IR_loadu8)
{
	return PX_Syntax_IR_loadxx( pSyntax, PX_FALSE, 8);
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_loadu16)
{
	return PX_Syntax_IR_loadxx( pSyntax, PX_FALSE, 16);
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_loadu32)
{
	return PX_Syntax_IR_loadxx( pSyntax, PX_FALSE, 32);
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_loadi8)
{
	return PX_Syntax_IR_loadxx( pSyntax, PX_TRUE, 8);
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_loadi16)
{
	return PX_Syntax_IR_loadxx( pSyntax, PX_TRUE, 16);
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_loadi32)
{
	return PX_Syntax_IR_loadxx( pSyntax, PX_TRUE, 32);
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_store8)
{
	return PX_Syntax_IR_storex( pSyntax, 8);
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_store16)
{
	return PX_Syntax_IR_storex( pSyntax, 16);
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_store32)
{
	return PX_Syntax_IR_storex( pSyntax, 32);
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_push)
{
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "ir_rx"), "IR_push must follow an rx abi");
	px_int reg_index = PX_AbiGetValue_int(plast, "index");
	px_byte payload[2];
	//opcode
	payload[0] = PX_SYNTAX_MACHINE_OPCODE_PUSH;
	//target register
	payload[1] = ((reg_index & 0x0f) << 4) | 0x01; //type 1 is register
	if (!PX_Syntax_IR_EmitText(pSyntax,payload, 2))//2bytes
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_push Memory Error1");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}



PX_SYNTAX_FUNCTION(PX_Syntax_IR_pop)
{
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "ir_rx"), "IR_pop must follow an rx abi");
	px_int reg_index = PX_AbiGetValue_int(plast, "index");
	px_byte payload[2];
	//opcode
	payload[0] = PX_SYNTAX_MACHINE_OPCODE_POP;
	//target register
	payload[1] = ((reg_index & 0x0f) << 4) | 0x01; //type 1 is register
	if (!PX_Syntax_IR_EmitText(pSyntax,payload, 2))//2bytes
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_pop Memory Error1");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_popn)
{
	px_byte payload[6];
	//opcode
	payload[0] = PX_SYNTAX_MACHINE_OPCODE_POPN;
	if (!PX_Syntax_IR_EmitText(pSyntax,payload, 1))//1bytes
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_popn Memory Error1");
		return PX_FALSE;
	}
	return PX_Syntax_IR_opcode_handle_begin(pSyntax, past, userptr);
}


PX_SYNTAX_FUNCTION(PX_Syntax_ir_init)
{
	px_abi* pscope;
	if (pSyntax->reg_abi_stack.size!=0)
	{
		return PX_FALSE;
	}
	if (!PX_Syntax_EnterScope(pSyntax))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_ir_init Memory Error1");
		return PX_FALSE;
	}
	pscope = PX_Syntax_GetAbiFromForward(pSyntax, "scope");
	if (!PX_AbiSet_int(pscope, "scan_mode", PX_SYNTAX_IR_SCAN_MODE_LABEL))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_ir_init Memory Error2");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(pscope, "text_length", 0))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_ir_init Memory Error2");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(pscope, "rdata_length", 0))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_ir_init Memory Error2");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_ir_loc_render_color)
{
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* psecondlast = PX_Syntax_GetAbiSecondLast(pSyntax);
	px_int begin_source_index, begin;
	px_int end_source_index, end;
	begin_source_index = PX_SyntaxLexer_GetLexemeBeginSourceIndex(&pSyntax->reg_syntaxlexer);
	end_source_index = PX_SyntaxLexer_GetLexemeEndSourceIndex(&pSyntax->reg_syntaxlexer);
	begin = PX_SyntaxLexer_GetLexemeBegin(&pSyntax->reg_syntaxlexer);
	end = PX_SyntaxLexer_GetLexemeEnd(&pSyntax->reg_syntaxlexer);
	if(begin_source_index == end_source_index)
	{
		PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end, PX_COLOR(255, 255, 0, 0), 0);
	}

	
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_ir_comment_loc)
{
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* psecondlast = PX_Syntax_GetAbiSecondLast(pSyntax);
	px_abi* pscope;
	px_dword bin_size = 0;
	px_int scan_mode;
	PX_Syntax_bin_map_to_source map_entry;

	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "const_int"), "ir_loc: last abi must be const_int");
	PX_ASSERTIFX(!PX_Syntax_CheckSecondLastAbiName(pSyntax, "const_int"), "ir_loc: second last abi must be const_int");

	pscope = PX_Syntax_GetAbiFromForward(pSyntax, "scope");
	if (!pscope)
	{
		PX_Syntax_Terminate(pSyntax, "ir_loc: no scope abi found");
		return PX_FALSE;
	}

	scan_mode = PX_AbiGetValue_int(pscope, "scan_mode");
	if (scan_mode != PX_SYNTAX_IR_SCAN_MODE_TEXT)
	{
		PX_Syntax_PopAbi(pSyntax);
		PX_Syntax_PopAbi(pSyntax);
		return PX_TRUE;
	}

	// get current ip from bin data size
	if (!PX_AbiGet_data(pscope, "text", &bin_size))
	{
		bin_size = 0;
	}

	map_entry.ip = (px_dword)bin_size;
	map_entry.source_index = (px_dword)PX_atoi(PX_AbiGetValue_string(psecondlast, "value"));
	map_entry.source_line = (px_dword)PX_atoi(PX_AbiGetValue_string(plast, "value"));

	// append map entry to scope "bin_map_to_source" data
	if (!PX_AbiExist_Type(pscope, "bin_map_to_source", PX_ABI_TYPE_DATA))
	{
		if (!PX_AbiSet_data(pscope, "bin_map_to_source", (px_byte*)&map_entry, sizeof(map_entry)))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:ir_loc Memory Error1");
			return PX_FALSE;
		}
	}
	else
	{
		if (!PX_AbiAppend_data(pscope, "bin_map_to_source", (px_byte*)&map_entry, sizeof(map_entry)))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:ir_loc Memory Error2");
			return PX_FALSE;
		}
	}

	PX_Syntax_PopAbi(pSyntax);
	PX_Syntax_PopAbi(pSyntax);

	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_ir_comment)
{
	px_int begin_source_index, begin;
	px_int end_source_index, end;
	PX_SYNTAXLEXER_LEXEME_TYPE type = PX_Syntax_GetNextLexeme(pSyntax);
	if (type == PX_SYNTAXLEXER_LEXEME_TYPE_DELIMITER)
	{
		if (PX_strequ(PX_Syntax_GetCurrentLexeme(pSyntax), ";"))
		{
			begin_source_index = PX_Syntax_GetCurrentLexemeBeginSourceIndex(pSyntax);
			begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
			//skip the line
			while (PX_TRUE)
			{
				px_char ch = PX_Syntax_GetNextChar(pSyntax);
				if (ch == '\0')
				{
					end_source_index = PX_Syntax_GetCurrentLexemeEndSourceIndex(pSyntax);
					end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);
					if (begin_source_index== end_source_index)
					{
						if (!PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end - 1, PX_COLOR(255, 255, 192, 64), 0))
						{
							PX_Syntax_Terminate(pSyntax, "out of memory");
							return PX_FALSE;
						}
					}
					
					return PX_TRUE;
				}
				if (ch == '\n')
				{
					end_source_index = PX_Syntax_GetCurrentLexemeEndSourceIndex(pSyntax);
					end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);
					if (!PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end, PX_COLOR(255, 255, 192, 64), 0))
					{
						PX_Syntax_Terminate(pSyntax, "out of memory");
						return PX_FALSE;
					}
					return PX_TRUE;
				}
			}
		}
		return PX_FALSE;
	}
	return PX_FALSE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_ir_import)
{
	//last abi is const_string
	px_int begin_source_index, end_source_index,begin,end;
	px_int scan_mode;
	px_abi* pscope_abi = PX_Syntax_GetAbiFromForward(pSyntax, "scope");
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	PX_ASSERTIFX(!pscope_abi, "import: no scope abi found");
	PX_ASSERTIFX(!plast, "import: no abi found");
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "const_string"), "import: must follow an const_string abi");
	begin_source_index = PX_Syntax_GetCurrentLexemeBeginSourceIndex(pSyntax);
	end_source_index = PX_Syntax_GetCurrentLexemeEndSourceIndex(pSyntax);
	begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
	end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);

	scan_mode = PX_AbiGetValue_int(pscope_abi, "scan_mode");
	if (scan_mode == PX_SYNTAX_IR_SCAN_MODE_LABEL)
	{
		if (PX_AbiExist_Type(pscope_abi, "imports", PX_ABI_TYPE_STRING))
		{
			if (!PX_AbiAppend_string(pscope_abi, "imports", ","))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:import Memory Error");
				return PX_FALSE;
			}
		}

		if (!PX_AbiAppend_string(pscope_abi, "imports", PX_Syntax_GetCurrentLexeme(pSyntax)))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:import Memory Error");
			return PX_FALSE;
		}
	}

	PX_Syntax_PopAbi(pSyntax);
	if(begin_source_index==end_source_index)
		PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end, PX_COLOR(255, 255, 66, 96), "label");

	return PX_TRUE;
}

static px_bool PX_Syntax_ir_label_exec(struct _PX_Syntax* pSyntax, struct _PX_Syntax_ast* past, px_void* userptr, px_bool export)
{
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* pscope;
	px_char labels_payload[128] = "labels.";
	const px_char* label_name;
	px_dword current_offset;
	px_int scan_mode;
	px_int begin_source_index, begin;
	px_int end_source_index, end;

	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "identifier"), "ir_label: must follow an identifier abi");
	label_name = PX_AbiGet_string(plast, "value");
	if (!label_name || label_name[0] == '\0')
	{
		PX_Syntax_Terminate(pSyntax, "ir_label: empty label name");
		return PX_FALSE;
	}

	begin_source_index = PX_AbiGetValue_int(plast, "source_index");
	begin = PX_AbiGetValue_int(plast, "begin");

	end_source_index = PX_Syntax_GetCurrentLexemeEndSourceIndex(pSyntax);
	end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);

	pscope = PX_Syntax_GetAbiFromForward(pSyntax, "scope");
	if (!pscope)
	{
		PX_Syntax_Terminate(pSyntax, "ir_label: no scope abi found");
		return PX_FALSE;
	}

	scan_mode = PX_AbiGetValue_int(pscope, "scan_mode");

	if (scan_mode == PX_SYNTAX_IR_SCAN_MODE_LABEL)
	{
		current_offset = (px_dword)PX_AbiGetValue_int(pscope, "text_length");
		PX_strcat(labels_payload, label_name);
		if (!PX_AbiSet_dword(pscope, labels_payload, current_offset))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:ir_label Memory Error");
			return PX_FALSE;
		}
	}

	if (export)
	{
		px_char export_labels_payload[128] = "exports.";
		current_offset = (px_dword)PX_AbiGetValue_int(pscope, "text_length");
		PX_strcat(export_labels_payload, label_name);
		if (!PX_AbiSet_dword(pscope, export_labels_payload, current_offset))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:ir_label Memory Error");
			return PX_FALSE;
		}
	}

	PX_Syntax_PopAbi(pSyntax);

	PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end, PX_COLOR(255, 255, 66, 96), "label");

	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_ir_label)
{
	return PX_Syntax_ir_label_exec(pSyntax, past, userptr, PX_FALSE);
}

PX_SYNTAX_FUNCTION(PX_Syntax_export_ir_label)
{
	return PX_Syntax_ir_label_exec(pSyntax, past, userptr, PX_TRUE);
}

px_int PX_Syntax_Disassemble_loadxx(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 6)
	{
		return 0;
	}
	switch (payload[0])
	{
	case PX_SYNTAX_MACHINE_OPCODE_LOADU8:
		PX_strcat(out, "loadu8 ");
		break;
	case PX_SYNTAX_MACHINE_OPCODE_LOADU16:
		PX_strcat(out, "loadu16 ");
		break;
	case PX_SYNTAX_MACHINE_OPCODE_LOADU32:
		PX_strcat(out, "loadu32 ");
		break;
	case PX_SYNTAX_MACHINE_OPCODE_LOADI8:
		PX_strcat(out, "loadi8 ");
		break;
	case PX_SYNTAX_MACHINE_OPCODE_LOADI16:
		PX_strcat(out, "loadi16 ");
		break;
	case PX_SYNTAX_MACHINE_OPCODE_LOADI32:
		PX_strcat(out, "loadi32 ");
		break;
	default:
		PX_ASSERTX("PX_Syntax_Disassemble_loadxx: opcode error");
		return 0;
	}
	//target register
	do
	{
		px_dword target_reg = payload[1] & 0x0f;
		px_bool is_global = (payload[1] & 0x10) ? PX_FALSE:PX_TRUE;
		px_dword offset = *(px_dword*)(payload + 2);
		px_char temp[32] = { 0 };

		PX_strcat(out, "r");
		PX_strcatchar(out, (px_char)('0' + target_reg));
		PX_strcatchar(out, ',');
		if (!is_global)
		{
			PX_sprintf1(temp, sizeof(temp), "bp-%1", PX_STRINGFORMAT_INT(offset));
		}
		else
		{
			PX_sprintf1(temp, sizeof(temp), "%1", PX_STRINGFORMAT_INT(offset));
		}
		PX_strcat(out, temp);
	} while (0);
	return 6;
}

px_int PX_Syntax_Disassemble_storexx(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 6)
	{
		return 0;
	}
	switch (payload[0])
	{
	case PX_SYNTAX_MACHINE_OPCODE_STORE8:
		PX_strcat(out, "store8 ");
		break;
	case PX_SYNTAX_MACHINE_OPCODE_STORE16:
		PX_strcat(out, "store16 ");
		break;
	case PX_SYNTAX_MACHINE_OPCODE_STORE32:
		PX_strcat(out, "store32 ");
		break;
	default:
		PX_ASSERTX("PX_Syntax_Disassemble_storexx: opcode error");
		return 0;
	}
	//target register
	do
	{
		px_dword source_reg = payload[1] & 0x0f;
		px_bool is_global = (payload[1] & 0x10) ? PX_FALSE:PX_TRUE ;
		px_dword offset = *(px_dword*)(payload + 2);
		px_char temp[32] = { 0 };
		
		if (!is_global)
		{
			PX_sprintf1(temp, sizeof(temp), "bp-%1", PX_STRINGFORMAT_INT(offset));
		}
		else
		{
			PX_sprintf1(temp, sizeof(temp), "%1", PX_STRINGFORMAT_INT(offset));
		}
		PX_strcat(out, temp);
		PX_strcat(out, ",r");
		PX_strcatchar(out, (px_char)('0' + source_reg));
	} while (0);
	return 6;
}

px_int PX_Syntax_Disassemble_push(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_int optype;
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 2)
	{
		return 0;
	}
	optype = payload[1] & 0x0f;
	switch (optype)
	{
	case 0:
		break;
	case 1:
	{
		px_int reg_index = (payload[1] & 0xf0) >> 4;
		if (reg_index <= 3)
		{
			PX_strset(out, "push r");
			PX_strcatchar(out, (px_char)('0' + reg_index));
		}
		else if (reg_index == 4)
		{
			PX_strset(out, "push ip");
		}
		else if (reg_index == 5)
		{
			PX_strset(out, "push sp");
		}
		else if (reg_index == 6)
		{
			PX_strset(out, "push bp");
		}
		else if (reg_index == 7)
		{
			PX_strset(out, "push flag");
		}
		else
		{
			PX_strset(out, "unknow");
		}
	}
	break;
	default:
		return 0;
		break;
	}
	return 2;

}

px_int PX_Syntax_Disassemble_pop(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_int optype;
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 2)
	{
		return 0;
	}
	optype = payload[1] & 0x0f;
	switch (optype)
	{
	case 0:
		break;
	case 1:
	{
		px_int reg_index = (payload[1] & 0xf0) >> 4;
		if (reg_index == 4)
		{
			PX_strset(out, "pop ip");
		}
		else if (reg_index == 5)
		{
			PX_strset(out, "pop sp");
		}
		else if (reg_index == 6)
		{
			PX_strset(out, "pop bp");
		}
		else if (reg_index == 7)
		{
			PX_strset(out, "pop flag");
		}
		else
		{
			if (reg_index <= 3)
			{
				PX_strset(out, "pop r");
				PX_strcatchar(out, (px_char)('0' + reg_index));
			}
			else
			{
				PX_strcat(out, "unknow");
				return 0;
			}
		}
	}
	break;
	default:
		return 0;
		break;
	}
	return 2;
}

px_int PX_Syntax_Disassemble_popn(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 1)
	{
		return 0;
	}
	PX_strset(out, "popn");
	return 1;
}

// ============================================================
//  addsp / subsp
// ============================================================

PX_SYNTAX_FUNCTION(PX_Syntax_IR_addsp)
{
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	px_byte payload[5];
	px_dword n;
	const px_char* pvalue;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "const_int"), "IR_addsp must follow a const_int abi");
	pvalue = PX_AbiGet_string(plast, "value");
	n = (px_dword)PX_atoi(pvalue);
	payload[0] = PX_SYNTAX_MACHINE_OPCODE_ADDSP;
	*(px_dword*)(payload + 1) = n;
	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 5))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_addsp Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_subsp)
{
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	px_byte payload[5];
	px_dword n;
	const px_char* pvalue;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "const_int"), "IR_subsp must follow a const_int abi");
	pvalue = PX_AbiGet_string(plast, "value");
	n = (px_dword)PX_atoi(pvalue);
	payload[0] = PX_SYNTAX_MACHINE_OPCODE_SUBSP;
	*(px_dword*)(payload + 1) = n;
	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 5))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_subsp Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

px_int PX_Syntax_Disassemble_addsp_subsp(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_dword n;
	px_char temp[32] = { 0 };
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 5)
		return 0;
	n = *(px_dword*)(payload + 1);
	if (payload[0] == PX_SYNTAX_MACHINE_OPCODE_ADDSP)
		PX_strcat(out, "addsp ");
	else
		PX_strcat(out, "subsp ");
	PX_sprintf1(temp, sizeof(temp), "%1", PX_STRINGFORMAT_INT(n));
	PX_strcat(out, temp);
	return 5;
}

// ============================================================
//  nop
// ============================================================

PX_SYNTAX_FUNCTION(PX_Syntax_IR_nop)
{
	px_byte payload[1];
	payload[0] = PX_SYNTAX_MACHINE_OPCODE_NOP;
	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 1))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_nop Memory Error");
		return PX_FALSE;
	}
	return PX_Syntax_IR_opcode_handle_begin(pSyntax, past, userptr);
}

px_int PX_Syntax_Disassemble_nop(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 1)
		return 0;
	PX_strset(out, "nop");
	return 1;
}


// ============================================================
//  movr[r0...r7,f0...f3], [r0...r7,f0...f3]  (3 bytes)
//  byte0: opcode
//  byte1: (dest_type 0~3) | (dest_index 4~7)
//  byte2: (src_type  0~3) | (src_index  4~7)
//  type: 0=common register  1=float register
// ============================================================

PX_SYNTAX_FUNCTION(PX_Syntax_IR_movr)
{
	px_abi* plast        = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* psecondlast  = PX_Syntax_GetAbiSecondLast(pSyntax);
	px_byte payload[3];
	px_int dest_type, dest_index, src_type, src_index;

	// last is src, second-last is dest
	if (PX_Syntax_CheckLastAbiName(pSyntax, "ir_rx"))
	{
		src_type  = 0;
		src_index = PX_AbiGetValue_int(plast, "index");
	}
	else if (PX_Syntax_CheckLastAbiName(pSyntax, "ir_fx"))
	{
		src_type  = 1;
		src_index = PX_AbiGetValue_int(plast, "index");
	}
	else
	{
		PX_Syntax_Terminate(pSyntax, "IR_movr: src must be ir_rx or ir_fx");
		return PX_FALSE;
	}

	if (PX_Syntax_CheckSecondLastAbiName(pSyntax, "ir_rx"))
	{
		dest_type  = 0;
		dest_index = PX_AbiGetValue_int(psecondlast, "index");
	}
	else if (PX_Syntax_CheckSecondLastAbiName(pSyntax, "ir_fx"))
	{
		dest_type  = 1;
		dest_index = PX_AbiGetValue_int(psecondlast, "index");
	}
	else
	{
		PX_Syntax_Terminate(pSyntax, "IR_movr: dest must be ir_rx or ir_fx");
		return PX_FALSE;
	}

	payload[0] = PX_SYNTAX_MACHINE_OPCODE_MOVR;
	payload[1] = (px_byte)((dest_type & 0x0f) | ((dest_index & 0x0f) << 4));
	payload[2] = (px_byte)((src_type  & 0x0f) | ((src_index  & 0x0f) << 4));

	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 3))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_movr Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

// ============================================================
//  movc  (6 bytes)
//  byte0: opcode
//  byte1: (reg_index 0~2) | reserved(3~7)
//  bytes2-5: 32-bit const int
// ============================================================

PX_SYNTAX_FUNCTION(PX_Syntax_IR_movc)
{
	px_abi* plast       = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* psecondlast = PX_Syntax_GetAbiSecondLast(pSyntax);
	px_byte payload[6];
	px_int  reg_index;
	px_dword cval;
	const px_char* pvalue;

	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "const_int"),   "IR_movc: last must be const_int");
	PX_ASSERTIFX(!PX_Syntax_CheckSecondLastAbiName(pSyntax, "ir_rx"), "IR_movc: second-last must be ir_rx");

	reg_index = PX_AbiGetValue_int(psecondlast, "index");
	pvalue = PX_AbiGet_string(plast, "value");
	cval   = (px_dword)PX_atoi(pvalue);

	payload[0] = PX_SYNTAX_MACHINE_OPCODE_MOVC;
	payload[1] = (px_byte)(reg_index & 0x07);
	*(px_dword*)(payload + 2) = cval;

	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 6))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_movc Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

// ============================================================
//  movf  (6 bytes)
//  byte0: opcode
//  byte1: (reg_index 0~2) | reserved(3~7)
//  bytes2-5: 32-bit float const (IEEE 754)
// ============================================================

PX_SYNTAX_FUNCTION(PX_Syntax_IR_movf)
{
	px_abi* plast       = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* psecondlast = PX_Syntax_GetAbiSecondLast(pSyntax);
	px_byte payload[6];
	px_int  reg_index;
	px_byte reg_field;
	px_float32 fval;
	const px_char* pvalue;

	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "const_float"), "IR_movf: last must be const_float");

	if (PX_Syntax_CheckSecondLastAbiName(pSyntax, "ir_rx"))
	{
		reg_index = PX_AbiGetValue_int(psecondlast, "index");
		reg_field = (px_byte)(reg_index & 0x07); /* bit3=0: rx */
	}
	else if (PX_Syntax_CheckSecondLastAbiName(pSyntax, "ir_fx"))
	{
		reg_index = PX_AbiGetValue_int(psecondlast, "index");
		reg_field = (px_byte)((reg_index & 0x07) | 0x08); /* bit3=1: fx */
	}
	else
	{
		PX_ASSERTIFX(PX_TRUE, "IR_movf: second-last must be ir_rx or ir_fx");
		return PX_FALSE;
	}

	pvalue = PX_AbiGet_string(plast, "value");
	fval   = (px_float32)PX_atof(pvalue);

	payload[0] = PX_SYNTAX_MACHINE_OPCODE_MOVF;
	payload[1] = reg_field;
	*(px_float32*)(payload + 2) = fval;

	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 6))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_movf Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

// ============================================================
//  movn  (1 byte) -- r0=src, r1=dst, r2=size (implicit)
// ============================================================

PX_SYNTAX_FUNCTION(PX_Syntax_IR_movn)
{
	px_byte payload[1];
	payload[0] = PX_SYNTAX_MACHINE_OPCODE_MOVN;
	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 1))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_movn Memory Error");
		return PX_FALSE;
	}
	return PX_Syntax_IR_opcode_handle_begin(pSyntax, past, userptr);
}

px_int PX_Syntax_Disassemble_movr(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_int dest_type, dest_index, src_type, src_index;
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 3)
		return 0;
	dest_type  = payload[1] & 0x0f;
	dest_index = (payload[1] >> 4) & 0x0f;
	src_type   = payload[2] & 0x0f;
	src_index  = (payload[2] >> 4) & 0x0f;
	PX_strset(out, "movr ");
	// dest
	if (dest_type == 1)
	{
		PX_strcat(out, "f");
		PX_strcatchar(out, (px_char)('0' + dest_index));
	}
	else
	{
		if (dest_index <= 3) { PX_strcat(out, "r"); PX_strcatchar(out, (px_char)('0' + dest_index)); }
		else if (dest_index == 4) PX_strcat(out, "ip");
		else if (dest_index == 5) PX_strcat(out, "sp");
		else if (dest_index == 6) PX_strcat(out, "bp");
		else if (dest_index == 7) PX_strcat(out, "flag");
	}
	PX_strcat(out, ",");
	// src
	if (src_type == 1)
	{
		PX_strcat(out, "f");
		PX_strcatchar(out, (px_char)('0' + src_index));
	}
	else
	{
		if (src_index <= 3) { PX_strcat(out, "r"); PX_strcatchar(out, (px_char)('0' + src_index)); }
		else if (src_index == 4) PX_strcat(out, "ip");
		else if (src_index == 5) PX_strcat(out, "sp");
		else if (src_index == 6) PX_strcat(out, "bp");
		else if (src_index == 7) PX_strcat(out, "flag");
	}
	return 3;
}

px_int PX_Syntax_Disassemble_movc(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_int reg_index;
	px_dword cval;
	px_char temp[32] = { 0 };
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 6)
		return 0;
	reg_index = payload[1] & 0x07;
	cval = *(px_dword*)(payload + 2);
	PX_strset(out, "movc r");
	PX_strcatchar(out, (px_char)('0' + reg_index));
	PX_strcat(out, ",");
	PX_sprintf1(temp, sizeof(temp), "%1", PX_STRINGFORMAT_INT(cval));
	PX_strcat(out, temp);
	return 6;
}

px_int PX_Syntax_Disassemble_movf(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_int reg_index;
	px_float fval;
	px_char temp[32] = { 0 };
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 6)
		return 0;
	reg_index = payload[1] & 0x07;
	fval = *(px_float*)(payload + 2);
	PX_strset(out, "movf r");
	PX_strcatchar(out, (px_char)('0' + reg_index));
	PX_strcat(out, ",");
	PX_sprintf1(temp, sizeof(temp), "%1", PX_STRINGFORMAT_FLOAT(fval));
	PX_strcat(out, temp);
	return 6;
}

// ============================================================
//  f2i / f2u / i2f / u2f  (2 bytes)
//  byte0: opcode
//  byte1: (0~3) 4bit rx index | (4~7) 4bit fx index
//  f2i: fx->rx   byte1: rx_index(0~3) | fx_index(4~7)
//  f2u: fx->rx   byte1: rx_index(0~3) | fx_index(4~7)
//  i2f: rx->fx   byte1: fx_index(0~3) | rx_index(4~7)
//  u2f: rx->fx   byte1: fx_index(0~3) | rx_index(4~7)
// ============================================================

PX_SYNTAX_FUNCTION(PX_Syntax_IR_f2i)
{
	// f2i rx, fx  -- last=ir_fx, second-last=ir_rx
	px_abi* plast       = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* psecondlast = PX_Syntax_GetAbiSecondLast(pSyntax);
	px_byte payload[2];
	px_int rx_index, fx_index;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "ir_fx"),       "IR_f2i: last must be ir_fx");
	PX_ASSERTIFX(!PX_Syntax_CheckSecondLastAbiName(pSyntax, "ir_rx"), "IR_f2i: second-last must be ir_rx");
	rx_index = PX_AbiGetValue_int(psecondlast, "index");
	fx_index = PX_AbiGetValue_int(plast,       "index");
	payload[0] = PX_SYNTAX_MACHINE_OPCODE_F2I;
	payload[1] = (px_byte)((rx_index & 0x0f) | ((fx_index & 0x0f) << 4));
	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 2))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_f2i Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_f2u)
{
	// f2u rx, fx  -- last=ir_fx, second-last=ir_rx
	px_abi* plast       = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* psecondlast = PX_Syntax_GetAbiSecondLast(pSyntax);
	px_byte payload[2];
	px_int rx_index, fx_index;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "ir_fx"),       "IR_f2u: last must be ir_fx");
	PX_ASSERTIFX(!PX_Syntax_CheckSecondLastAbiName(pSyntax, "ir_rx"), "IR_f2u: second-last must be ir_rx");
	rx_index = PX_AbiGetValue_int(psecondlast, "index");
	fx_index = PX_AbiGetValue_int(plast,       "index");
	payload[0] = PX_SYNTAX_MACHINE_OPCODE_F2U;
	payload[1] = (px_byte)((rx_index & 0x0f) | ((fx_index & 0x0f) << 4));
	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 2))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_f2u Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_i2f)
{
	// i2f fx, rx  -- last=ir_rx, second-last=ir_fx
	px_abi* plast       = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* psecondlast = PX_Syntax_GetAbiSecondLast(pSyntax);
	px_byte payload[2];
	px_int rx_index, fx_index;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "ir_rx"),       "IR_i2f: last must be ir_rx");
	PX_ASSERTIFX(!PX_Syntax_CheckSecondLastAbiName(pSyntax, "ir_fx"), "IR_i2f: second-last must be ir_fx");
	fx_index = PX_AbiGetValue_int(psecondlast, "index");
	rx_index = PX_AbiGetValue_int(plast,       "index");
	payload[0] = PX_SYNTAX_MACHINE_OPCODE_I2F;
	payload[1] = (px_byte)((fx_index & 0x0f) | ((rx_index & 0x0f) << 4));
	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 2))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_i2f Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_u2f)
{
	// u2f fx, rx  -- last=ir_rx, second-last=ir_fx
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* psecondlast = PX_Syntax_GetAbiSecondLast(pSyntax);
	px_byte payload[2];
	px_int rx_index, fx_index;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "ir_rx"),       "IR_u2f: last must be ir_rx");
	PX_ASSERTIFX(!PX_Syntax_CheckSecondLastAbiName(pSyntax, "ir_fx"), "IR_u2f: second-last must be ir_fx");
	fx_index = PX_AbiGetValue_int(psecondlast, "index");
	rx_index = PX_AbiGetValue_int(plast,       "index");
	payload[0] = PX_SYNTAX_MACHINE_OPCODE_U2F;
	payload[1] = (px_byte)((fx_index & 0x0f) | ((rx_index & 0x0f) << 4));
	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 2))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_u2f Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_ff2f)
{
	px_byte payload[1];
	payload[0] = PX_SYNTAX_MACHINE_OPCODE_FF2F;
	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 1))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_ff2f Memory Error");
		return PX_FALSE;
	}
	return PX_Syntax_IR_opcode_handle_begin(pSyntax, past, userptr);
}

px_int PX_Syntax_Disassemble_f2i(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_int rx_index, fx_index;
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 2) return 0;
	rx_index = payload[1] & 0x0f;
	fx_index = (payload[1] >> 4) & 0x0f;
	PX_strset(out, "f2i r");
	PX_strcatchar(out, (px_char)('0' + rx_index));
	PX_strcat(out, ",f");
	PX_strcatchar(out, (px_char)('0' + fx_index));
	return 2;
}

px_int PX_Syntax_Disassemble_f2u(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_int rx_index, fx_index;
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 2) return 0;
	rx_index = payload[1] & 0x0f;
	fx_index = (payload[1] >> 4) & 0x0f;
	PX_strset(out, "f2u r");
	PX_strcatchar(out, (px_char)('0' + rx_index));
	PX_strcat(out, ",f");
	PX_strcatchar(out, (px_char)('0' + fx_index));
	return 2;
}

px_int PX_Syntax_Disassemble_i2f(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_int fx_index, rx_index;
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 2) return 0;
	fx_index = payload[1] & 0x0f;
	rx_index = (payload[1] >> 4) & 0x0f;
	PX_strset(out, "i2f f");
	PX_strcatchar(out, (px_char)('0' + fx_index));
	PX_strcat(out, ",r");
	PX_strcatchar(out, (px_char)('0' + rx_index));
	return 2;
}

px_int PX_Syntax_Disassemble_u2f(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_int fx_index, rx_index;
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 2) return 0;
	fx_index = payload[1] & 0x0f;
	rx_index = (payload[1] >> 4) & 0x0f;
	PX_strset(out, "u2f f");
	PX_strcatchar(out, (px_char)('0' + fx_index));
	PX_strcat(out, ",r");
	PX_strcatchar(out, (px_char)('0' + rx_index));
	return 2;
}

// ============================================================
//  Integer ALU: neg (2 bytes), add/sub/mul/div/idiv/mod (3 bytes)
//  neg:  opcode | (0~3)4bit reg_index
//  3-op: opcode | (dest 0~3)|(src1 4~7) | (src2 0~3)|(padding 4~7)
// ============================================================

PX_SYNTAX_FUNCTION(PX_Syntax_IR_neg)
{
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	px_byte payload[2];
	px_int reg_index;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "ir_rx"), "IR_neg must follow an ir_rx abi");
	reg_index  = PX_AbiGetValue_int(plast, "index");
	payload[0] = PX_SYNTAX_MACHINE_OPCODE_NEG;
	payload[1] = (px_byte)(reg_index & 0x0f);
	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 2))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_neg Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

// Helper: emit 3-byte ALU instruction dst=src1 op src2
// The abi stack order after parsing "op rx_dst, rx_src1, rx_src2" is:
//   last = src2, second-last = src1, third-last = dst  -- not yet available
// So we use a dedicated ir_rx3 helper or parse differently.
// The grammar will push dest first, then src1, then src2.
// At call time: abi[-1]=src2, abi[-2]=src1, abi[-3]=dst
// We walk back using GetAbiLast helpers.

static px_bool PX_Syntax_IR_alu3(PX_Syntax* pSyntax, px_byte opcode)
{
	// Stack (from top): src2, src1, dest
	px_abi* psrc2 = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* psrc1 = PX_Syntax_GetAbiSecondLast(pSyntax);
	// We need the third-last; use GetAbiByIndex via count
	px_int  count = PX_Syntax_GetAbiCount(pSyntax);
	px_abi* pdst  = PX_Syntax_GetAbiByIndex(pSyntax, count - 3);
	px_byte payload[3];
	px_int  dest_index, src1_index, src2_index;

	if (!psrc2 || !psrc1 || !pdst)
	{
		PX_Syntax_Terminate(pSyntax, "IR_alu3: not enough abi on stack");
		return PX_FALSE;
	}
	if (!PX_Syntax_CheckAbiName(pdst,  "ir_rx") ||
	    !PX_Syntax_CheckAbiName(psrc1, "ir_rx") ||
	    !PX_Syntax_CheckAbiName(psrc2, "ir_rx"))
	{
		PX_Syntax_Terminate(pSyntax, "IR_alu3: operands must be ir_rx");
		return PX_FALSE;
	}
	dest_index = PX_AbiGetValue_int(pdst,  "index");
	src1_index = PX_AbiGetValue_int(psrc1, "index");
	src2_index = PX_AbiGetValue_int(psrc2, "index");

	payload[0] = opcode;
	payload[1] = (px_byte)((dest_index & 0x0f) | ((src1_index & 0x0f) << 4));
	payload[2] = (px_byte)(src2_index & 0x0f);

	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 3))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_alu3 Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	PX_Syntax_PopAbi(pSyntax);
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_add)  { return PX_Syntax_IR_alu3(pSyntax, PX_SYNTAX_MACHINE_OPCODE_ADD);  }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_sub)  { return PX_Syntax_IR_alu3(pSyntax, PX_SYNTAX_MACHINE_OPCODE_SUB);  }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_mul)  { return PX_Syntax_IR_alu3(pSyntax, PX_SYNTAX_MACHINE_OPCODE_MUL);  }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_div)  { return PX_Syntax_IR_alu3(pSyntax, PX_SYNTAX_MACHINE_OPCODE_DIV);  }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_idiv) { return PX_Syntax_IR_alu3(pSyntax, PX_SYNTAX_MACHINE_OPCODE_IDIV); }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_mod)  { return PX_Syntax_IR_alu3(pSyntax, PX_SYNTAX_MACHINE_OPCODE_MOD);  }

px_int PX_Syntax_Disassemble_neg(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_int reg_index;
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 2) return 0;
	reg_index = payload[1] & 0x0f;
	PX_strset(out, "neg r");
	PX_strcatchar(out, (px_char)('0' + reg_index));
	return 2;
}

px_int PX_Syntax_Disassemble_alu3(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_int dest_index, src1_index, src2_index;
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 3) return 0;
	dest_index = payload[1] & 0x0f;
	src1_index = (payload[1] >> 4) & 0x0f;
	src2_index = payload[2] & 0x0f;
	switch (payload[0])
	{
	case PX_SYNTAX_MACHINE_OPCODE_ADD:  PX_strset(out, "add ");  break;
	case PX_SYNTAX_MACHINE_OPCODE_SUB:  PX_strset(out, "sub ");  break;
	case PX_SYNTAX_MACHINE_OPCODE_MUL:  PX_strset(out, "mul ");  break;
	case PX_SYNTAX_MACHINE_OPCODE_DIV:  PX_strset(out, "div ");  break;
	case PX_SYNTAX_MACHINE_OPCODE_IDIV: PX_strset(out, "idiv "); break;
	case PX_SYNTAX_MACHINE_OPCODE_MOD:  PX_strset(out, "mod ");  break;
	default: PX_strset(out, "unknow"); return 0;
	}
	PX_strcat(out, "r"); PX_strcatchar(out, (px_char)('0' + dest_index));
	PX_strcat(out, ",r"); PX_strcatchar(out, (px_char)('0' + src1_index));
	PX_strcat(out, ",r"); PX_strcatchar(out, (px_char)('0' + src2_index));
	return 3;
}

// ============================================================
//  FPU: fneg (2 bytes), fadd/fsub/fmul/fdiv (3 bytes)
//  fcmp / fcomi (2 bytes):  opcode | (0~3)f0 index | (4~7)f1 index
//  fneg: opcode | (0~3) fx_index
//  fadd/fsub/fmul/fdiv: opcode | (dest 0~3)|(src1 4~7) | (src2 0~3)|(pad 4~7)
// ============================================================

PX_SYNTAX_FUNCTION(PX_Syntax_IR_fneg)
{
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	px_byte payload[2];
	px_int fx_index;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "ir_fx"), "IR_fneg must follow an ir_fx abi");
	fx_index   = PX_AbiGetValue_int(plast, "index");
	payload[0] = PX_SYNTAX_MACHINE_OPCODE_FNEG;
	payload[1] = (px_byte)(fx_index & 0x0f);
	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 2))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_fneg Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

static px_bool PX_Syntax_IR_fpu3(PX_Syntax* pSyntax, px_byte opcode)
{
	px_abi* psrc2 = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* psrc1 = PX_Syntax_GetAbiSecondLast(pSyntax);
	px_int  count = PX_Syntax_GetAbiCount(pSyntax);
	px_abi* pdst  = PX_Syntax_GetAbiByIndex(pSyntax, count - 3);
	px_byte payload[3];
	px_int  dest_index, src1_index, src2_index;

	if (!psrc2 || !psrc1 || !pdst)
	{
		PX_Syntax_Terminate(pSyntax, "IR_fpu3: not enough abi on stack");
		return PX_FALSE;
	}
	if (!PX_Syntax_CheckAbiName(pdst,  "ir_fx") ||
	    !PX_Syntax_CheckAbiName(psrc1, "ir_fx") ||
	    !PX_Syntax_CheckAbiName(psrc2, "ir_fx"))
	{
		PX_Syntax_Terminate(pSyntax, "IR_fpu3: operands must be ir_fx");
		return PX_FALSE;
	}
	dest_index = PX_AbiGetValue_int(pdst,  "index");
	src1_index = PX_AbiGetValue_int(psrc1, "index");
	src2_index = PX_AbiGetValue_int(psrc2, "index");

	payload[0] = opcode;
	payload[1] = (px_byte)((dest_index & 0x0f) | ((src1_index & 0x0f) << 4));
	payload[2] = (px_byte)(src2_index & 0x0f);

	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 3))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_fpu3 Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	PX_Syntax_PopAbi(pSyntax);
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_fadd) { return PX_Syntax_IR_fpu3(pSyntax, PX_SYNTAX_MACHINE_OPCODE_FADD); }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_fsub) { return PX_Syntax_IR_fpu3(pSyntax, PX_SYNTAX_MACHINE_OPCODE_FSUB); }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_fmul) { return PX_Syntax_IR_fpu3(pSyntax, PX_SYNTAX_MACHINE_OPCODE_FMUL); }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_fdiv) { return PX_Syntax_IR_fpu3(pSyntax, PX_SYNTAX_MACHINE_OPCODE_FDIV); }

// fcmp / fcomi: 2 bytes -- opcode | (0~3) f0_index | (4~7) f1_index
PX_SYNTAX_FUNCTION(PX_Syntax_IR_fcmp)
{
	px_abi* plast       = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* psecondlast = PX_Syntax_GetAbiSecondLast(pSyntax);
	px_byte payload[2];
	px_int  f0_index, f1_index;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "ir_fx"),       "IR_fcmp: last must be ir_fx");
	PX_ASSERTIFX(!PX_Syntax_CheckSecondLastAbiName(pSyntax, "ir_fx"), "IR_fcmp: second-last must be ir_fx");
	f0_index   = PX_AbiGetValue_int(psecondlast, "index");
	f1_index   = PX_AbiGetValue_int(plast,       "index");
	payload[0] = PX_SYNTAX_MACHINE_OPCODE_FCMP;
	payload[1] = (px_byte)((f0_index & 0x0f) | ((f1_index & 0x0f) << 4));
	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 2))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_fcmp Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_fcomi)
{
	px_abi* plast       = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* psecondlast = PX_Syntax_GetAbiSecondLast(pSyntax);
	px_byte payload[2];
	px_int  f0_index, f1_index;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "ir_fx"),       "IR_fcomi: last must be ir_fx");
	PX_ASSERTIFX(!PX_Syntax_CheckSecondLastAbiName(pSyntax, "ir_fx"), "IR_fcomi: second-last must be ir_fx");
	f0_index   = PX_AbiGetValue_int(psecondlast, "index");
	f1_index   = PX_AbiGetValue_int(plast,       "index");
	payload[0] = PX_SYNTAX_MACHINE_OPCODE_FCOMI;
	payload[1] = (px_byte)((f0_index & 0x0f) | ((f1_index & 0x0f) << 4));
	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 2))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_fcomi Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

px_int PX_Syntax_Disassemble_fneg(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_int fx_index;
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 2) return 0;
	fx_index = payload[1] & 0x0f;
	PX_strset(out, "fneg f");
	PX_strcatchar(out, (px_char)('0' + fx_index));
	return 2;
}

px_int PX_Syntax_Disassemble_fpu3(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_int dest_index, src1_index, src2_index;
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 3) return 0;
	dest_index = payload[1] & 0x0f;
	src1_index = (payload[1] >> 4) & 0x0f;
	src2_index = payload[2] & 0x0f;
	switch (payload[0])
	{
	case PX_SYNTAX_MACHINE_OPCODE_FADD: PX_strset(out, "fadd "); break;
	case PX_SYNTAX_MACHINE_OPCODE_FSUB: PX_strset(out, "fsub "); break;
	case PX_SYNTAX_MACHINE_OPCODE_FMUL: PX_strset(out, "fmul "); break;
	case PX_SYNTAX_MACHINE_OPCODE_FDIV: PX_strset(out, "fdiv "); break;
	default: PX_strset(out, "unknow"); return 0;
	}
	PX_strcat(out, "f"); PX_strcatchar(out, (px_char)('0' + dest_index));
	PX_strcat(out, ",f"); PX_strcatchar(out, (px_char)('0' + src1_index));
	PX_strcat(out, ",f"); PX_strcatchar(out, (px_char)('0' + src2_index));
	return 3;
}

px_int PX_Syntax_Disassemble_fcmp(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_int f0_index, f1_index;
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 2) return 0;
	f0_index = payload[1] & 0x0f;
	f1_index = (payload[1] >> 4) & 0x0f;
	if (payload[0] == PX_SYNTAX_MACHINE_OPCODE_FCMP)
		PX_strset(out, "fcmp f");
	else
		PX_strset(out, "fcomi f");
	PX_strcatchar(out, (px_char)('0' + f0_index));
	PX_strcat(out, ",f");
	PX_strcatchar(out, (px_char)('0' + f1_index));
	return 2;
}

// ============================================================
//  Logical: not (2 bytes), and/or/xor/shl/shr (3 bytes)
// ============================================================

PX_SYNTAX_FUNCTION(PX_Syntax_IR_not)
{
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	px_byte payload[2];
	px_int reg_index;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "ir_rx"), "IR_not must follow an ir_rx abi");
	reg_index  = PX_AbiGetValue_int(plast, "index");
	payload[0] = PX_SYNTAX_MACHINE_OPCODE_NOT;
	payload[1] = (px_byte)(reg_index & 0x0f);
	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 2))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_not Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_and)  { return PX_Syntax_IR_alu3(pSyntax, PX_SYNTAX_MACHINE_OPCODE_AND); }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_or)   { return PX_Syntax_IR_alu3(pSyntax, PX_SYNTAX_MACHINE_OPCODE_OR);  }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_xor)  { return PX_Syntax_IR_alu3(pSyntax, PX_SYNTAX_MACHINE_OPCODE_XOR); }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_shl)  { return PX_Syntax_IR_alu3(pSyntax, PX_SYNTAX_MACHINE_OPCODE_SHL); }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_shr)  { return PX_Syntax_IR_alu3(pSyntax, PX_SYNTAX_MACHINE_OPCODE_SHR); }

px_int PX_Syntax_Disassemble_not(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_int reg_index;
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 2) return 0;
	reg_index = payload[1] & 0x0f;
	PX_strset(out, "not r");
	PX_strcatchar(out, (px_char)('0' + reg_index));
	return 2;
}

px_int PX_Syntax_Disassemble_logical3(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_int dest_index, src1_index, src2_index;
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 3) return 0;
	dest_index = payload[1] & 0x0f;
	src1_index = (payload[1] >> 4) & 0x0f;
	src2_index = payload[2] & 0x0f;
	switch (payload[0])
	{
	case PX_SYNTAX_MACHINE_OPCODE_AND: PX_strset(out, "and "); break;
	case PX_SYNTAX_MACHINE_OPCODE_OR:  PX_strset(out, "or ");  break;
	case PX_SYNTAX_MACHINE_OPCODE_XOR: PX_strset(out, "xor "); break;
	case PX_SYNTAX_MACHINE_OPCODE_SHL: PX_strset(out, "shl "); break;
	case PX_SYNTAX_MACHINE_OPCODE_SHR: PX_strset(out, "shr "); break;
	default: PX_strset(out, "unknow"); return 0;
	}
	PX_strcat(out, "r"); PX_strcatchar(out, (px_char)('0' + dest_index));
	PX_strcat(out, ",r"); PX_strcatchar(out, (px_char)('0' + src1_index));
	PX_strcat(out, ",r"); PX_strcatchar(out, (px_char)('0' + src2_index));
	return 3;
}

// ============================================================
//  cmp (2 bytes): opcode | (0~3) r0_index | (4~7) r1_index
// ============================================================

PX_SYNTAX_FUNCTION(PX_Syntax_IR_cmp)
{
	px_abi* plast       = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* psecondlast = PX_Syntax_GetAbiSecondLast(pSyntax);
	px_byte payload[2];
	px_int  r0_index, r1_index;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "ir_rx"),       "IR_cmp: last must be ir_rx");
	PX_ASSERTIFX(!PX_Syntax_CheckSecondLastAbiName(pSyntax, "ir_rx"), "IR_cmp: second-last must be ir_rx");
	r0_index   = PX_AbiGetValue_int(psecondlast, "index");
	r1_index   = PX_AbiGetValue_int(plast,       "index");
	payload[0] = PX_SYNTAX_MACHINE_OPCODE_CMP;
	payload[1] = (px_byte)((r0_index & 0x0f) | ((r1_index & 0x0f) << 4));
	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 2))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_cmp Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

px_int PX_Syntax_Disassemble_cmp(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_int r0_index, r1_index;
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 2) return 0;
	r0_index = payload[1] & 0x0f;
	r1_index = (payload[1] >> 4) & 0x0f;
	PX_strset(out, "cmp r");
	PX_strcatchar(out, (px_char)('0' + r0_index));
	PX_strcat(out, ",r");
	PX_strcatchar(out, (px_char)('0' + r1_index));
	return 2;
}

// ============================================================
//  jmp / je / jne / jg / jl / jge / jle  (5 bytes)
//  opcode + 32-bit label address
// ============================================================

static px_bool PX_Syntax_IR_jxx(PX_Syntax* pSyntax, px_byte opcode)
{
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	px_byte payload[5];
	px_dword addr;
	const px_char* pvalue;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "const_int"), "IR_jxx must follow a const_int abi");
	pvalue     = PX_AbiGet_string(plast, "value");
	addr       = (px_dword)PX_atoi(pvalue);
	payload[0] = opcode;
	*(px_dword*)(payload + 1) = addr;
	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 5))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_jxx Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_jmp_addr) { return PX_Syntax_IR_jxx(pSyntax, PX_SYNTAX_MACHINE_OPCODE_JMP); }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_je_addr)  { return PX_Syntax_IR_jxx(pSyntax, PX_SYNTAX_MACHINE_OPCODE_JE);  }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_jne_addr) { return PX_Syntax_IR_jxx(pSyntax, PX_SYNTAX_MACHINE_OPCODE_JNE); }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_jg_addr)  { return PX_Syntax_IR_jxx(pSyntax, PX_SYNTAX_MACHINE_OPCODE_JG);  }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_jl_addr)  { return PX_Syntax_IR_jxx(pSyntax, PX_SYNTAX_MACHINE_OPCODE_JL);  }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_jge_addr) { return PX_Syntax_IR_jxx(pSyntax, PX_SYNTAX_MACHINE_OPCODE_JGE); }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_jle_addr) { return PX_Syntax_IR_jxx(pSyntax, PX_SYNTAX_MACHINE_OPCODE_JLE); }

// ============================================================
//  jmp/je/jne/jg/jl/jge/jle  label-name variant (5 bytes)
//  scan 1: emit opcode + 0 (placeholder)
//  scan 2: emit opcode + resolved label address
// ============================================================

static px_bool PX_Syntax_IR_jxx_label(PX_Syntax* pSyntax, px_byte opcode)
{
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* pscope;
	px_byte payload[5];
	px_dword addr = 0;
	px_char label_key[128] = "labels.";
	const px_char* label_name;
	px_int scan_mode;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "identifier"), "IR_jxx_label must follow an identifier abi");
	label_name = PX_AbiGet_string(plast, "value");
	pscope = PX_Syntax_GetAbiFromForward(pSyntax, "scope");
	PX_ASSERTIFX(!pscope, "IR_jxx_label: no scope abi found");
	scan_mode = PX_AbiGetValue_int(pscope, "scan_mode");
	if (scan_mode == PX_SYNTAX_IR_SCAN_MODE_TEXT)
	{
		px_dword* paddr;
		PX_strcat(label_key, label_name);
		paddr = PX_AbiGet_dword(pscope, label_key);
		if (!paddr)
		{
			px_char errmsg[256];
			PX_sprintf1(errmsg, sizeof(errmsg), "runtime:error:IR_jxx_label label '%1' not found", PX_STRINGFORMAT_STRING(label_name));
			PX_Syntax_Terminate(pSyntax, errmsg);
			return PX_FALSE;
		}
		addr = *paddr;
	}
	payload[0] = opcode;
	*(px_dword*)(payload + 1) = addr;
	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 5))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_jxx_label Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_jmp_label) { return PX_Syntax_IR_jxx_label(pSyntax, PX_SYNTAX_MACHINE_OPCODE_JMP); }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_je_label)  { return PX_Syntax_IR_jxx_label(pSyntax, PX_SYNTAX_MACHINE_OPCODE_JE);  }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_jne_label) { return PX_Syntax_IR_jxx_label(pSyntax, PX_SYNTAX_MACHINE_OPCODE_JNE); }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_jg_label)  { return PX_Syntax_IR_jxx_label(pSyntax, PX_SYNTAX_MACHINE_OPCODE_JG);  }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_jl_label)  { return PX_Syntax_IR_jxx_label(pSyntax, PX_SYNTAX_MACHINE_OPCODE_JL);  }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_jge_label) { return PX_Syntax_IR_jxx_label(pSyntax, PX_SYNTAX_MACHINE_OPCODE_JGE); }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_jle_label) { return PX_Syntax_IR_jxx_label(pSyntax, PX_SYNTAX_MACHINE_OPCODE_JLE); }

px_int PX_Syntax_Disassemble_jxx(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_dword addr;
	px_char temp[32] = { 0 };
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 5) return 0;
	addr = *(px_dword*)(payload + 1);
	switch (payload[0])
	{
	case PX_SYNTAX_MACHINE_OPCODE_JMP: PX_strset(out, "jmp "); break;
	case PX_SYNTAX_MACHINE_OPCODE_JE:  PX_strset(out, "je ");  break;
	case PX_SYNTAX_MACHINE_OPCODE_JNE: PX_strset(out, "jne "); break;
	case PX_SYNTAX_MACHINE_OPCODE_JG:  PX_strset(out, "jg ");  break;
	case PX_SYNTAX_MACHINE_OPCODE_JL:  PX_strset(out, "jl ");  break;
	case PX_SYNTAX_MACHINE_OPCODE_JGE: PX_strset(out, "jge "); break;
	case PX_SYNTAX_MACHINE_OPCODE_JLE: PX_strset(out, "jle "); break;
	default: PX_strset(out, "unknow"); return 0;
	}
	PX_sprintf1(temp, sizeof(temp), "%1", PX_STRINGFORMAT_INT(addr));
	PX_strcat(out, temp);
	return 5;
}

// ============================================================
//  call (5 bytes)
//  byte0: opcode
//  bytes1-4: 32-bit address
// ============================================================

PX_SYNTAX_FUNCTION(PX_Syntax_IR_call_addr)
{
	// call <const_int>  -- direct call to address
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	px_byte payload[5];
	px_dword addr;
	const px_char* pvalue;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "const_int"), "IR_call_label must follow a const_int abi");
	pvalue     = PX_AbiGet_string(plast, "value");
	addr       = (px_dword)PX_atoi(pvalue);
	payload[0] = PX_SYNTAX_MACHINE_OPCODE_CALL;
	*(px_dword*)(payload + 1) = addr;
	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 5))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_call_label Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_callr)
{
	// callr <ir_rx>  -- indirect call via register (2 bytes)
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	px_byte payload[2];
	px_int reg_index;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "ir_rx"), "IR_callr must follow an ir_rx abi");
	reg_index  = PX_AbiGetValue_int(plast, "index");
	payload[0] = PX_SYNTAX_MACHINE_OPCODE_CALLR;
	payload[1] = (px_byte)(reg_index & 0xff);
	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 2))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_callr Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}


PX_SYNTAX_FUNCTION(PX_Syntax_IR_jmpr)
{
	// jmpr <ir_rx>  -- indirect jump via register (2 bytes)
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	px_byte payload[2];
	px_int reg_index;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "ir_rx"), "IR_jmpr must follow an ir_rx abi");
	reg_index  = PX_AbiGetValue_int(plast, "index");
	payload[0] = PX_SYNTAX_MACHINE_OPCODE_JMPR;
	payload[1] = (px_byte)(reg_index & 0xff);
	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 2))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_jmpr Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_call_label)
{
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	px_abi* pscope;
	px_byte payload[5];
	px_dword addr = 0;
	px_char label_key[128] = "labels.";
	const px_char* label_name;
	px_int scan_mode;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "identifier"), "IR_call_label_name must follow an identifier abi");
	label_name = PX_AbiGet_string(plast, "value");
	pscope = PX_Syntax_GetAbiFromForward(pSyntax, "scope");
	PX_ASSERTIFX(!pscope, "IR_call_label: no scope abi found");
	scan_mode = PX_AbiGetValue_int(pscope, "scan_mode");
	if (scan_mode == PX_SYNTAX_IR_SCAN_MODE_TEXT)
	{
		px_dword* paddr;
		PX_strcat(label_key, label_name);
		paddr = PX_AbiGet_dword(pscope, label_key);
		if (!paddr)
		{
			px_char errmsg[256];
			PX_sprintf1(errmsg, sizeof(errmsg), "runtime:error:IR_call_label label '%1' not found", PX_STRINGFORMAT_STRING(label_name));
			PX_Syntax_Terminate(pSyntax, errmsg);
			return PX_FALSE;
		}
		addr = *paddr;
	}
	payload[0] = PX_SYNTAX_MACHINE_OPCODE_CALL;
	*(px_dword*)(payload + 1) = addr;
	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 5))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_call_label_name Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

px_int PX_Syntax_Disassemble_call(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_dword value;
	px_char temp[32] = { 0 };
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 5) return 0;
	value = *(px_dword*)(payload + 1);
	PX_strset(out, "call ");
	PX_sprintf1(temp, sizeof(temp), "%1", PX_STRINGFORMAT_INT(value));
	PX_strcat(out, temp);
	return 5;
}

px_int PX_Syntax_Disassemble_callr(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_int reg_index;
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 2) return 0;
	reg_index = payload[1] & 0xff;
	PX_strset(out, "callr ");
	if (reg_index <= 3) { PX_strcat(out, "r"); PX_strcatchar(out, (px_char)('0' + reg_index)); }
	else if (reg_index == 4) PX_strcat(out, "ip");
	else if (reg_index == 5) PX_strcat(out, "sp");
	else if (reg_index == 6) PX_strcat(out, "bp");
	else if (reg_index == 7) PX_strcat(out, "flag");
	else PX_strcat(out, "unknow");
	return 2;
}

px_int PX_Syntax_Disassemble_jmpr(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_int reg_index;
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 2) return 0;
	reg_index = payload[1] & 0xff;
	PX_strset(out, "jmpr ");
	if (reg_index <= 3) { PX_strcat(out, "r"); PX_strcatchar(out, (px_char)('0' + reg_index)); }
	else if (reg_index == 4) PX_strcat(out, "ip");
	else if (reg_index == 5) PX_strcat(out, "sp");
	else if (reg_index == 6) PX_strcat(out, "bp");
	else if (reg_index == 7) PX_strcat(out, "flag");
	else PX_strcat(out, "unknow");
	return 2;
}

// ============================================================
//  ret (1 byte)
// ============================================================

PX_SYNTAX_FUNCTION(PX_Syntax_IR_ret)
{
	px_byte payload[1];
	payload[0] = PX_SYNTAX_MACHINE_OPCODE_RET;
	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 1))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_ret Memory Error");
		return PX_FALSE;
	}
	return PX_Syntax_IR_opcode_handle_begin(pSyntax, past, userptr);
}

px_int PX_Syntax_Disassemble_ret(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 1) return 0;
	PX_strset(out, "ret");
	return 1;
}


// ============================================================
//  ee disassemble helpers
// ============================================================
static const px_char* PX_Syntax_IR_ee_type_name(px_byte type, px_dword value, px_char buf[32])
{
	switch (type)
	{
	case 0: PX_sprintf1(buf, 32, "%1", PX_STRINGFORMAT_INT(value)); break;
	case 1: { px_float fv = *(px_float*)&value; PX_sprintf1(buf, 32, "%1f", PX_STRINGFORMAT_FLOAT(fv)); } break;
	case 2: PX_sprintf1(buf, 32, "r%1", PX_STRINGFORMAT_INT(value)); break;
	case 3: PX_sprintf1(buf, 32, "f%1", PX_STRINGFORMAT_INT(value)); break;
	case 4: PX_sprintf1(buf, 32, "%1", PX_STRINGFORMAT_INT(value)); break;
	case 5: PX_sprintf1(buf, 32, "bp-%1", PX_STRINGFORMAT_INT(value)); break;
	default: PX_strset(buf, "?"); break;
	}
	return buf;
}

static px_int PX_Syntax_Disassemble_ee_unary(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_byte  type;
	px_dword value;
	px_char  tmp[32] = { 0 };
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 6) return 0;
	type = payload[1];
	value = *(px_dword*)(payload + 2);
	switch (payload[0])
	{
	case PX_SYNTAX_MACHINE_OPCODE_EE_PUSH: PX_strset(out, "ee_push "); break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_POP:  PX_strset(out, "ee_pop ");  break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_NEG:  PX_strset(out, "ee_neg ");  break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_FNEG: PX_strset(out, "ee_fneg "); break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_NOT:  PX_strset(out, "ee_not ");  break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_CALL: PX_strset(out, "ee_call "); break;
	default: PX_strset(out, "ee_? "); break;
	}
	PX_Syntax_IR_ee_type_name(type, value, tmp);
	PX_strcat(out, tmp);
	return 6;
}

static px_int PX_Syntax_Disassemble_ee_ternary(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_byte  type_dst, type_src1, type_src2;
	px_dword val_dst, val_src1, val_src2;
	px_char  tmp[32] = { 0 };
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 16) return 0;
	type_dst = payload[1];
	type_src1 = payload[2];
	type_src2 = payload[3];
	val_dst = *(px_dword*)(payload + 4);
	val_src1 = *(px_dword*)(payload + 8);
	val_src2 = *(px_dword*)(payload + 12);
	switch (payload[0])
	{
	case PX_SYNTAX_MACHINE_OPCODE_EE_ADD:  PX_strset(out, "ee_add ");  break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_SUB:  PX_strset(out, "ee_sub ");  break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_MUL:  PX_strset(out, "ee_mul ");  break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_DIV:  PX_strset(out, "ee_div ");  break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_IDIV: PX_strset(out, "ee_idiv "); break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_MOD:  PX_strset(out, "ee_mod ");  break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_FADD: PX_strset(out, "ee_fadd "); break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_FSUB: PX_strset(out, "ee_fsub "); break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_FMUL: PX_strset(out, "ee_fmul "); break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_FDIV: PX_strset(out, "ee_fdiv "); break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_AND:  PX_strset(out, "ee_and ");  break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_OR:   PX_strset(out, "ee_or ");   break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_XOR:  PX_strset(out, "ee_xor ");  break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_SHL:  PX_strset(out, "ee_shl ");  break;
	case PX_SYNTAX_MACHINE_OPCODE_EE_SHR:  PX_strset(out, "ee_shr ");  break;
	default: PX_strset(out, "ee_? "); break;
	}
	PX_Syntax_IR_ee_type_name(type_dst, val_dst, tmp);   PX_strcat(out, tmp); PX_strcat(out, ",");
	PX_Syntax_IR_ee_type_name(type_src1, val_src1, tmp); PX_strcat(out, tmp); PX_strcat(out, ",");
	PX_Syntax_IR_ee_type_name(type_src2, val_src2, tmp); PX_strcat(out, tmp);
	return 16;
}

static px_int PX_Syntax_Disassemble_ee_cmp(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_byte  type_src1, type_src2;
	px_dword val_src1, val_src2;
	px_char  tmp[32] = { 0 };
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 12) return 0;
	type_src1 = payload[1];
	type_src2 = payload[2];
	val_src1 = *(px_dword*)(payload + 4);
	val_src2 = *(px_dword*)(payload + 8);
	PX_strset(out, "ee_cmp ");
	PX_Syntax_IR_ee_type_name(type_src1, val_src1, tmp); PX_strcat(out, tmp); PX_strcat(out, ",");
	PX_Syntax_IR_ee_type_name(type_src2, val_src2, tmp); PX_strcat(out, tmp);
	return 12;
}

static px_int PX_Syntax_Disassemble_ee_fcmp(const px_byte* payload, px_int reserved_size, px_char out[32])
{
	px_byte  type_src1, type_src2;
	px_dword val_src1, val_src2;
	px_char  tmp[32] = { 0 };
	PX_memset(out, 0, sizeof(px_char) * 32);
	if (reserved_size < 12) return 0;
	type_src1 = payload[1];
	type_src2 = payload[2];
	val_src1 = *(px_dword*)(payload + 4);
	val_src2 = *(px_dword*)(payload + 8);
	PX_strset(out, "ee_fcmp ");
	PX_Syntax_IR_ee_type_name(type_src1, val_src1, tmp); PX_strcat(out, tmp); PX_strcat(out, ",");
	PX_Syntax_IR_ee_type_name(type_src2, val_src2, tmp); PX_strcat(out, tmp);
	return 12;
}

px_int PX_Syntax_Disassemble(const px_byte* payload, px_int reserved_size, px_char out_asm[32])
{
	if (reserved_size < 1)
	{
		return 0;
	}
	switch (payload[0])
	{
	case PX_SYNTAX_MACHINE_OPCODE_NOP:
		return PX_Syntax_Disassemble_nop(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_MOVR:
		return PX_Syntax_Disassemble_movr(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_MOVC:
		return PX_Syntax_Disassemble_movc(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_MOVF:
		return PX_Syntax_Disassemble_movf(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_MOVN:
		PX_memset(out_asm, 0, sizeof(px_char) * 32);
		PX_strset(out_asm, "movn");
		return 1;
	case PX_SYNTAX_MACHINE_OPCODE_LOADU8:
	case PX_SYNTAX_MACHINE_OPCODE_LOADU16:
	case PX_SYNTAX_MACHINE_OPCODE_LOADU32:
	case PX_SYNTAX_MACHINE_OPCODE_LOADI8:
	case PX_SYNTAX_MACHINE_OPCODE_LOADI16:
	case PX_SYNTAX_MACHINE_OPCODE_LOADI32:
		return PX_Syntax_Disassemble_loadxx(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_STORE8:
	case PX_SYNTAX_MACHINE_OPCODE_STORE16:
	case PX_SYNTAX_MACHINE_OPCODE_STORE32:
		return PX_Syntax_Disassemble_storexx(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_F2I:
		return PX_Syntax_Disassemble_f2i(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_F2U:
		return PX_Syntax_Disassemble_f2u(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_I2F:
		return PX_Syntax_Disassemble_i2f(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_U2F:
		return PX_Syntax_Disassemble_u2f(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_FF2F:
		PX_memset(out_asm, 0, sizeof(px_char) * 32);
		PX_strset(out_asm, "ff2f");
		return 1;
	case PX_SYNTAX_MACHINE_OPCODE_PUSH:
		return PX_Syntax_Disassemble_push(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_POP:
		return PX_Syntax_Disassemble_pop(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_POPN:
		return PX_Syntax_Disassemble_popn(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_ADDSP:
	case PX_SYNTAX_MACHINE_OPCODE_SUBSP:
		return PX_Syntax_Disassemble_addsp_subsp(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_NEG:
		return PX_Syntax_Disassemble_neg(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_ADD:
	case PX_SYNTAX_MACHINE_OPCODE_SUB:
	case PX_SYNTAX_MACHINE_OPCODE_MUL:
	case PX_SYNTAX_MACHINE_OPCODE_DIV:
	case PX_SYNTAX_MACHINE_OPCODE_IDIV:
	case PX_SYNTAX_MACHINE_OPCODE_MOD:
		return PX_Syntax_Disassemble_alu3(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_FNEG:
		return PX_Syntax_Disassemble_fneg(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_FADD:
	case PX_SYNTAX_MACHINE_OPCODE_FSUB:
	case PX_SYNTAX_MACHINE_OPCODE_FMUL:
	case PX_SYNTAX_MACHINE_OPCODE_FDIV:
		return PX_Syntax_Disassemble_fpu3(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_FCMP:
	case PX_SYNTAX_MACHINE_OPCODE_FCOMI:
		return PX_Syntax_Disassemble_fcmp(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_NOT:
		return PX_Syntax_Disassemble_not(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_AND:
	case PX_SYNTAX_MACHINE_OPCODE_OR:
	case PX_SYNTAX_MACHINE_OPCODE_XOR:
	case PX_SYNTAX_MACHINE_OPCODE_SHL:
	case PX_SYNTAX_MACHINE_OPCODE_SHR:
		return PX_Syntax_Disassemble_logical3(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_CMP:
		return PX_Syntax_Disassemble_cmp(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_JMP:
	case PX_SYNTAX_MACHINE_OPCODE_JE:
	case PX_SYNTAX_MACHINE_OPCODE_JNE:
	case PX_SYNTAX_MACHINE_OPCODE_JG:
	case PX_SYNTAX_MACHINE_OPCODE_JL:
	case PX_SYNTAX_MACHINE_OPCODE_JGE:
	case PX_SYNTAX_MACHINE_OPCODE_JLE:
		return PX_Syntax_Disassemble_jxx(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_CALL:
		return PX_Syntax_Disassemble_call(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_CALLR:
		return PX_Syntax_Disassemble_callr(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_JMPR:
		return PX_Syntax_Disassemble_jmpr(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_RET:
		return PX_Syntax_Disassemble_ret(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_EE_PUSH:
	case PX_SYNTAX_MACHINE_OPCODE_EE_POP:
	case PX_SYNTAX_MACHINE_OPCODE_EE_NEG:
	case PX_SYNTAX_MACHINE_OPCODE_EE_FNEG:
	case PX_SYNTAX_MACHINE_OPCODE_EE_NOT:
	case PX_SYNTAX_MACHINE_OPCODE_EE_CALL:
		return PX_Syntax_Disassemble_ee_unary(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_EE_ADD:
	case PX_SYNTAX_MACHINE_OPCODE_EE_SUB:
	case PX_SYNTAX_MACHINE_OPCODE_EE_MUL:
	case PX_SYNTAX_MACHINE_OPCODE_EE_DIV:
	case PX_SYNTAX_MACHINE_OPCODE_EE_IDIV:
	case PX_SYNTAX_MACHINE_OPCODE_EE_MOD:
	case PX_SYNTAX_MACHINE_OPCODE_EE_FADD:
	case PX_SYNTAX_MACHINE_OPCODE_EE_FSUB:
	case PX_SYNTAX_MACHINE_OPCODE_EE_FMUL:
	case PX_SYNTAX_MACHINE_OPCODE_EE_FDIV:
	case PX_SYNTAX_MACHINE_OPCODE_EE_AND:
	case PX_SYNTAX_MACHINE_OPCODE_EE_OR:
	case PX_SYNTAX_MACHINE_OPCODE_EE_XOR:
	case PX_SYNTAX_MACHINE_OPCODE_EE_SHL:
	case PX_SYNTAX_MACHINE_OPCODE_EE_SHR:
		return PX_Syntax_Disassemble_ee_ternary(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_EE_CMP:
		return PX_Syntax_Disassemble_ee_cmp(payload, reserved_size, out_asm);
	case PX_SYNTAX_MACHINE_OPCODE_EE_FCMP:
		return PX_Syntax_Disassemble_ee_fcmp(payload, reserved_size, out_asm);
	default:
		break;
	}
	return PX_FALSE;
}

//#include "platform/modules/px_file.h"
PX_SYNTAX_FUNCTION(PX_Syntax_IR_next_scan)
{
	px_int scan_mode;
	if (PX_Syntax_IsEndOfSource(pSyntax))
	{
		px_abi* pscope = PX_Syntax_GetAbiFromForward(pSyntax, "scope");
		PX_ASSERTIFX(!pscope, "IR_next_scan: no scope abi found");
		scan_mode = PX_AbiGetValue_int(pscope, "scan_mode");
		if (scan_mode == PX_SYNTAX_IR_SCAN_MODE_LABEL)
		{
			PX_AbiSet_int(pscope, "scan_mode", PX_SYNTAX_IR_SCAN_MODE_RDATA);
		}
		else if (scan_mode == PX_SYNTAX_IR_SCAN_MODE_RDATA)
		{
			PX_AbiSet_int(pscope, "scan_mode", PX_SYNTAX_IR_SCAN_MODE_TEXT);
		}
		else
		{
			return PX_FALSE;
		}
		PX_SyntaxLexer_Reset(&pSyntax->reg_syntaxlexer);
		PX_Syntax_ClearSourceDescription(pSyntax);
		PX_StringClear(&pSyntax->message);
		if (!PX_Syntax_CallSourceIndex(pSyntax, 0, "_"))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_next_scan CallSourceIndex failed");
		}
		return PX_TRUE;
	}
	return PX_FALSE;
}

// ============================================================
//  ir_ee_oprand_int   - integer operand (per document):
//    type 0=const_int  2=rx  3=fx  4=addr(global)  5=bp-addr(local)
//  ir_ee_oprand_float - float operand (per document):
//    type 0=const_int  1=const_float  2=rx  3=fx  4=addr(global)  5=bp-addr(local)
//  Both produce an abi named "ir_ee_oprand" with fields: type(int), value(dword)
// ============================================================

typedef enum
{
	PX_SYNTAX_IR_EE_OPRAND_TYPE_CONST_INT,
	PX_SYNTAX_IR_EE_OPRAND_TYPE_CONST_FLOAT,
	PX_SYNTAX_IR_EE_OPRAND_TYPE_RX,
	PX_SYNTAX_IR_EE_OPRAND_TYPE_FX,
	PX_SYNTAX_IR_EE_OPRAND_TYPE_ADDR_GLOBAL,
	PX_SYNTAX_IR_EE_OPRAND_TYPE_ADDR_LOCAL,
}PX_SYNTAX_IR_EE_OPRAND_TYPE;

static px_bool PX_Syntax_IR_ee_make_oprand(PX_Syntax* pSyntax, PX_SYNTAX_IR_EE_OPRAND_TYPE type, px_dword value)
{
	px_abi* pop;
	PX_Syntax_PopAbi(pSyntax); // remove the source abi
	pop = PX_Syntax_NewAbi(pSyntax, "ir_ee_oprand", pSyntax->reg_lifetime);
	if (!pop)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:ir_ee_oprand Memory Error1");
		return PX_FALSE;
	}
	if (!PX_AbiSet_int(pop, "type", (px_int)type))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:ir_ee_oprand Memory Error2");
		return PX_FALSE;
	}
	if (!PX_AbiSet_dword(pop, "value", value))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:ir_ee_oprand Memory Error3");
		return PX_FALSE;
	}
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_op_rx)
{
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "ir_rx"), "ir_ee_op_rx: must follow ir_rx");
	return PX_Syntax_IR_ee_make_oprand(pSyntax, PX_SYNTAX_IR_EE_OPRAND_TYPE_RX, (px_dword)PX_AbiGetValue_int(plast, "index"));
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_op_fx)
{
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "ir_fx"), "ir_ee_op_fx: must follow ir_fx");
	return PX_Syntax_IR_ee_make_oprand(pSyntax, PX_SYNTAX_IR_EE_OPRAND_TYPE_FX, (px_dword)PX_AbiGetValue_int(plast, "index"));
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_op_addr)
{
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "ir_addr"), "ir_ee_op_addr: must follow ir_addr");
	if (PX_AbiExist(plast, "global_offset"))
		return PX_Syntax_IR_ee_make_oprand(pSyntax, PX_SYNTAX_IR_EE_OPRAND_TYPE_ADDR_GLOBAL, PX_AbiGetValue_dword(plast, "global_offset"));
	else
		return PX_Syntax_IR_ee_make_oprand(pSyntax, PX_SYNTAX_IR_EE_OPRAND_TYPE_ADDR_LOCAL, PX_AbiGetValue_dword(plast, "local_offset"));
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_op_const_int)
{
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	const px_char* pvalue;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "const_int"), "ir_ee_op_const: must follow const_int");
	pvalue = PX_AbiGet_string(plast, "value");
	return PX_Syntax_IR_ee_make_oprand(pSyntax, PX_SYNTAX_IR_EE_OPRAND_TYPE_CONST_INT, (px_dword)PX_atoi(pvalue));
}


PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_op_const_float)
{
	px_abi* plast = PX_Syntax_GetAbiLast(pSyntax);
	px_float32 fvalue;
	const px_char* pvalue;
	PX_ASSERTIFX(!PX_Syntax_CheckLastAbiName(pSyntax, "const_float"), "ir_ee_op_const: must follow const_float");
	pvalue = PX_AbiGet_string(plast, "value");
	fvalue = (px_float32)PX_atof(pvalue);
	return PX_Syntax_IR_ee_make_oprand(pSyntax, PX_SYNTAX_IR_EE_OPRAND_TYPE_CONST_FLOAT, *((px_dword*)&fvalue));
}

// ============================================================
//  ee helper: extract (type, value) from ir_ee_oprand abi
// ============================================================
static px_bool PX_Syntax_IR_ee_get_operand(PX_Syntax* pSyntax, px_abi* pabi, px_byte* out_type, px_dword* out_value)
{
	if (!PX_Syntax_CheckAbiName(pabi, "ir_ee_oprand"))
	{
		PX_Syntax_Terminate(pSyntax, "ee_get_operand: operand must be ir_ee_oprand");
		return PX_FALSE;
	}
	*out_type  = (px_byte)PX_AbiGetValue_int(pabi, "type");
	*out_value = PX_AbiGetValue_dword(pabi, "value");
	return PX_TRUE;
}

// ============================================================
//  ee unary instructions (6 bytes):
//  [opcode][type][32bit value]
//  ee_push, ee_pop, ee_neg, ee_fneg, ee_not, ee_call
// ============================================================
static px_bool PX_Syntax_IR_ee_unary(PX_Syntax* pSyntax, px_byte opcode)
{
	px_abi*  plast = PX_Syntax_GetAbiLast(pSyntax);
	px_byte  payload[6];
	px_byte  type;
	px_dword value;
	if (!PX_Syntax_IR_ee_get_operand(pSyntax, plast, &type, &value))
		return PX_FALSE;
	payload[0] = opcode;
	payload[1] = type;
	*(px_dword*)(payload + 2) = value;
	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 6))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_ee_unary Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_push)  { return PX_Syntax_IR_ee_unary(pSyntax, PX_SYNTAX_MACHINE_OPCODE_EE_PUSH);  }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_pop)   { return PX_Syntax_IR_ee_unary(pSyntax, PX_SYNTAX_MACHINE_OPCODE_EE_POP);   }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_neg)   { return PX_Syntax_IR_ee_unary(pSyntax, PX_SYNTAX_MACHINE_OPCODE_EE_NEG);   }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_fneg)  { return PX_Syntax_IR_ee_unary(pSyntax, PX_SYNTAX_MACHINE_OPCODE_EE_FNEG);  }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_not)   { return PX_Syntax_IR_ee_unary(pSyntax, PX_SYNTAX_MACHINE_OPCODE_EE_NOT);   }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_call)  { return PX_Syntax_IR_ee_unary(pSyntax, PX_SYNTAX_MACHINE_OPCODE_EE_CALL);  }

// ============================================================
//  ee ternary instructions (16 bytes):
//  [opcode][type_dst][type_src1][type_src2][dst 4B][src1 4B][src2 4B]
//  abi stack (top to bottom): src2, src1, dst
// ============================================================
static px_bool PX_Syntax_IR_ee_ternary(PX_Syntax* pSyntax, px_byte opcode)
{
	px_abi*  psrc2  = PX_Syntax_GetAbiByIndex(pSyntax,-1);
	px_abi*  psrc1  = PX_Syntax_GetAbiByIndex(pSyntax, -2);
	px_abi*  pdst   = PX_Syntax_GetAbiByIndex(pSyntax, -3);
	px_byte  payload[16];
	px_byte  type_dst, type_src1, type_src2;
	px_dword val_dst, val_src1, val_src2;

	if (!psrc2 || !psrc1 || !pdst)
	{
		PX_Syntax_Terminate(pSyntax, "ee_ternary: not enough operands on abi stack");
		return PX_FALSE;
	}
	if (!PX_Syntax_IR_ee_get_operand(pSyntax, pdst,  &type_dst,  &val_dst))  return PX_FALSE;
	if (!PX_Syntax_IR_ee_get_operand(pSyntax, psrc1, &type_src1, &val_src1)) return PX_FALSE;
	if (!PX_Syntax_IR_ee_get_operand(pSyntax, psrc2, &type_src2, &val_src2)) return PX_FALSE;

	payload[0]  = opcode;
	payload[1]  = type_dst;
	payload[2]  = type_src1;
	payload[3]  = type_src2;
	*(px_dword*)(payload + 4)  = val_dst;
	*(px_dword*)(payload + 8)  = val_src1;
	*(px_dword*)(payload + 12) = val_src2;

	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 16))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_ee_ternary Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	PX_Syntax_PopAbi(pSyntax);
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_add)  { return PX_Syntax_IR_ee_ternary(pSyntax, PX_SYNTAX_MACHINE_OPCODE_EE_ADD);  }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_sub)  { return PX_Syntax_IR_ee_ternary(pSyntax, PX_SYNTAX_MACHINE_OPCODE_EE_SUB);  }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_mul)  { return PX_Syntax_IR_ee_ternary(pSyntax, PX_SYNTAX_MACHINE_OPCODE_EE_MUL);  }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_div)  { return PX_Syntax_IR_ee_ternary(pSyntax, PX_SYNTAX_MACHINE_OPCODE_EE_DIV);  }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_idiv) { return PX_Syntax_IR_ee_ternary(pSyntax, PX_SYNTAX_MACHINE_OPCODE_EE_IDIV); }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_mod)  { return PX_Syntax_IR_ee_ternary(pSyntax, PX_SYNTAX_MACHINE_OPCODE_EE_MOD);  }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_fadd) { return PX_Syntax_IR_ee_ternary(pSyntax, PX_SYNTAX_MACHINE_OPCODE_EE_FADD); }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_fsub) { return PX_Syntax_IR_ee_ternary(pSyntax, PX_SYNTAX_MACHINE_OPCODE_EE_FSUB); }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_fmul) { return PX_Syntax_IR_ee_ternary(pSyntax, PX_SYNTAX_MACHINE_OPCODE_EE_FMUL); }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_fdiv) { return PX_Syntax_IR_ee_ternary(pSyntax, PX_SYNTAX_MACHINE_OPCODE_EE_FDIV); }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_and)  { return PX_Syntax_IR_ee_ternary(pSyntax, PX_SYNTAX_MACHINE_OPCODE_EE_AND);  }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_or)   { return PX_Syntax_IR_ee_ternary(pSyntax, PX_SYNTAX_MACHINE_OPCODE_EE_OR);   }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_xor)  { return PX_Syntax_IR_ee_ternary(pSyntax, PX_SYNTAX_MACHINE_OPCODE_EE_XOR);  }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_shl)  { return PX_Syntax_IR_ee_ternary(pSyntax, PX_SYNTAX_MACHINE_OPCODE_EE_SHL);  }
PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_shr)  { return PX_Syntax_IR_ee_ternary(pSyntax, PX_SYNTAX_MACHINE_OPCODE_EE_SHR);  }

// ============================================================
//  ee_cmp (12 bytes):
//  [opcode][type_src1][type_src2][pad(1B)][src1 4B][src2 4B]
//  abi stack (top to bottom): src2, src1
// ============================================================
PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_cmp)
{
	px_abi*  psrc2  = PX_Syntax_GetAbiLast(pSyntax);
	px_abi*  psrc1  = PX_Syntax_GetAbiSecondLast(pSyntax);
	px_byte  payload[12];
	px_byte  type_src1, type_src2;
	px_dword val_src1, val_src2;

	if (!psrc2 || !psrc1)
	{
		PX_Syntax_Terminate(pSyntax, "ee_cmp: not enough operands on abi stack");
		return PX_FALSE;
	}
	if (!PX_Syntax_IR_ee_get_operand(pSyntax, psrc1, &type_src1, &val_src1)) return PX_FALSE;
	if (!PX_Syntax_IR_ee_get_operand(pSyntax, psrc2, &type_src2, &val_src2)) return PX_FALSE;

	payload[0] = PX_SYNTAX_MACHINE_OPCODE_EE_CMP;
	payload[1] = type_src1;
	payload[2] = type_src2;
	payload[3] = 0;
	*(px_dword*)(payload + 4) = val_src1;
	*(px_dword*)(payload + 8) = val_src2;

	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 12))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_ee_cmp Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}

// ============================================================
//  ee_fcmp (12 bytes):
//  [opcode][type_src1][type_src2][pad(1B)][src1 4B][src2 4B]
//  abi stack (top to bottom): src2, src1
// ============================================================
PX_SYNTAX_FUNCTION(PX_Syntax_IR_ee_fcmp)
{
	px_abi*  psrc2  = PX_Syntax_GetAbiLast(pSyntax);
	px_abi*  psrc1  = PX_Syntax_GetAbiSecondLast(pSyntax);
	px_byte  payload[12];
	px_byte  type_src1, type_src2;
	px_dword val_src1, val_src2;

	if (!psrc2 || !psrc1)
	{
		PX_Syntax_Terminate(pSyntax, "ee_fcmp: not enough operands on abi stack");
		return PX_FALSE;
	}
	if (!PX_Syntax_IR_ee_get_operand(pSyntax, psrc1, &type_src1, &val_src1)) return PX_FALSE;
	if (!PX_Syntax_IR_ee_get_operand(pSyntax, psrc2, &type_src2, &val_src2)) return PX_FALSE;

	payload[0] = PX_SYNTAX_MACHINE_OPCODE_EE_FCMP;
	payload[1] = type_src1;
	payload[2] = type_src2;
	payload[3] = 0;
	*(px_dword*)(payload + 4) = val_src1;
	*(px_dword*)(payload + 8) = val_src2;

	if (!PX_Syntax_IR_EmitText(pSyntax, payload, 12))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_IR_ee_fcmp Memory Error");
		return PX_FALSE;
	}
	PX_Syntax_PopAbi(pSyntax);
	PX_Syntax_PopAbi(pSyntax);
	return PX_TRUE;
}


px_bool PX_Syntax_load_ir(PX_Syntax* pSyntax)
{
	PX_Syntax_load_const_int(pSyntax);
	PX_Syntax_load_const_int_list(pSyntax);
	PX_Syntax_load_const_float(pSyntax);
	PX_Syntax_load_const_string(pSyntax);
	PX_Syntax_load_const_string_list(pSyntax);
	PX_Syntax_load_keyword(pSyntax);
	PX_Syntax_load_identifier(pSyntax);
	PX_Syntax_load_eof(pSyntax);

	PX_Syntax_Parse_PEBNF(pSyntax, "ir_init = *", 0, PX_Syntax_ir_init, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir_skip_newline_spacer = *", 0, PX_Syntax_ir_skip_newline_spacer, PX_NULL);
	
	PX_Syntax_Parse_PEBNF(pSyntax, "ir_loc = ';' '@loc'", 0, PX_Syntax_ir_loc_render_color, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir_loc = ';' '@loc' const_int const_int", 0, PX_Syntax_ir_comment_loc, PX_NULL);//;@loc source_index source offset
	PX_Syntax_Parse_PEBNF(pSyntax, "comment = ir_loc", 0, PX_NULL, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "comment = *", 0, PX_Syntax_ir_comment, PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "ir_label = 'import' const_string", 0, PX_Syntax_ir_import, PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "ir_label = 'export' identifier ':'", 0, PX_Syntax_export_ir_label, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir_label = identifier ':'", 0, PX_Syntax_ir_label, PX_NULL);


	PX_Syntax_Parse_PEBNF(pSyntax, "ir_rx = identifier", 0,PX_Syntax_IR_rx, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir_fx = identifier", 0, PX_Syntax_IR_fx, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir_addr = 'bp' '-' const_int", 0, PX_Syntax_IR_local_addr,  PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir_addr = const_int", 0, PX_Syntax_IR_global_addr, PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "ir = ir_skip_newline_spacer", 0, 0, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = comment", 0, 0, PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "ir = ir_label", 0, 0, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'db' identifier ':' identifier", 0, PX_Syntax_IR_db, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'dd' identifier ':' const_string", 0, PX_Syntax_IR_db, PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'loadu8'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'loadu16'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'loadu32'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'loadi8'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'loadi16'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'loadi32'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'store8'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'store16'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'store32'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'push'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'pop'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	//PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'popn'", 0, PX_Syntax_IR_opcode_render_color, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'addsp'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'subsp'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	//PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'nop'", 0, PX_Syntax_IR_opcode_render_color, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'movr'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'movc'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'movf'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	//PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'movn'", 0, PX_Syntax_IR_opcode_render_color, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'f2i'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'f2u'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'i2f'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'u2f'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	//PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ff2f'", 0, PX_Syntax_IR_opcode_render_color, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'neg'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'add'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'sub'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'mul'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'div'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'idiv'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'mod'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'fneg'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'fadd'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'fsub'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'fmul'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'fdiv'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'fcmp'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'fcomi'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'not'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'and'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'or'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'xor'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'shl'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'shr'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'cmp'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'jmp'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'je'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'jne'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'jg'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'jl'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'jge'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'jle'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'call'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'callr'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'jmpr'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	//PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ret'", 0, PX_Syntax_IR_opcode_render_color, PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'push' ir_rx ", 0, PX_Syntax_IR_push, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'pop' ir_rx ", 0, PX_Syntax_IR_pop, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'popn' ", 0, PX_Syntax_IR_popn,  PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'addsp' const_int ", 0, PX_Syntax_IR_addsp, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'subsp' const_int ", 0, PX_Syntax_IR_subsp, PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'nop' ", 0, PX_Syntax_IR_nop, PX_NULL);

	/* movr: dest and src can each be ir_rx or ir_fx -- match both variants */
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'movr' ir_rx ',' ir_rx ", 0, PX_Syntax_IR_movr, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'movr' ir_rx ',' ir_fx ", 0, PX_Syntax_IR_movr, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'movr' ir_fx ',' ir_rx ", 0, PX_Syntax_IR_movr, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'movr' ir_fx ',' ir_fx ", 0, PX_Syntax_IR_movr, PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'movc' ir_rx ',' const_int ", 0, PX_Syntax_IR_movc, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'movf' ir_rx ',' const_float ", 0, PX_Syntax_IR_movf, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'movf' ir_fx ',' const_float ", 0, PX_Syntax_IR_movf, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'movn' ", 0, PX_Syntax_IR_movn, PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'f2i' ir_rx ',' ir_fx ", 0, PX_Syntax_IR_f2i, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'f2u' ir_rx ',' ir_fx ", 0, PX_Syntax_IR_f2u, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'i2f' ir_fx ',' ir_rx ", 0, PX_Syntax_IR_i2f, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'u2f' ir_fx ',' ir_rx ", 0, PX_Syntax_IR_u2f, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ff2f' ", 0, PX_Syntax_IR_ff2f, PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'loadu8' ir_rx ',' ir_addr ", 0, PX_Syntax_IR_loadu8, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'loadu16' ir_rx ',' ir_addr ", 0, PX_Syntax_IR_loadu16, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'loadu32' ir_rx ',' ir_addr ", 0, PX_Syntax_IR_loadu32, PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'loadi8' ir_rx ',' ir_addr ", 0, PX_Syntax_IR_loadi8, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'loadi16' ir_rx ',' ir_addr ", 0, PX_Syntax_IR_loadi16, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'loadi32' ir_rx ',' ir_addr ", 0, PX_Syntax_IR_loadi32, PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'store8' ir_addr ',' ir_rx ", 0, PX_Syntax_IR_store8, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'store16' ir_addr ',' ir_rx ", 0, PX_Syntax_IR_store16, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'store32' ir_addr ',' ir_rx ", 0, PX_Syntax_IR_store32, PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'neg' ir_rx ", 0, PX_Syntax_IR_neg, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'add' ir_rx ',' ir_rx ',' ir_rx ", 0, PX_Syntax_IR_add, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'sub' ir_rx ',' ir_rx ',' ir_rx ", 0, PX_Syntax_IR_sub, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'mul' ir_rx ',' ir_rx ',' ir_rx ", 0, PX_Syntax_IR_mul, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'div' ir_rx ',' ir_rx ',' ir_rx ", 0, PX_Syntax_IR_div, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'idiv' ir_rx ',' ir_rx ',' ir_rx ", 0, PX_Syntax_IR_idiv, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'mod' ir_rx ',' ir_rx ',' ir_rx ", 0, PX_Syntax_IR_mod, PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'fneg' ir_fx ", 0, PX_Syntax_IR_fneg, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'fadd' ir_fx ',' ir_fx ',' ir_fx ", 0, PX_Syntax_IR_fadd, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'fsub' ir_fx ',' ir_fx ',' ir_fx ", 0, PX_Syntax_IR_fsub, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'fmul' ir_fx ',' ir_fx ',' ir_fx ", 0, PX_Syntax_IR_fmul, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'fdiv' ir_fx ',' ir_fx ',' ir_fx ", 0, PX_Syntax_IR_fdiv, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'fcmp' ir_fx ',' ir_fx ", 0, PX_Syntax_IR_fcmp, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'fcomi' ir_fx ',' ir_fx ", 0, PX_Syntax_IR_fcomi, PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'not' ir_rx ", 0, PX_Syntax_IR_not, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'and' ir_rx ',' ir_rx ',' ir_rx ", 0, PX_Syntax_IR_and, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'or' ir_rx ',' ir_rx ',' ir_rx ",  0, PX_Syntax_IR_or,  PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'xor' ir_rx ',' ir_rx ',' ir_rx ", 0, PX_Syntax_IR_xor, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'shl' ir_rx ',' ir_rx ',' ir_rx ", 0, PX_Syntax_IR_shl, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'shr' ir_rx ',' ir_rx ',' ir_rx ", 0, PX_Syntax_IR_shr, PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'cmp' ir_rx ',' ir_rx ", 0, PX_Syntax_IR_cmp, PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'jmp' identifier ", 0, PX_Syntax_IR_jmp_label, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'je'  identifier ", 0, PX_Syntax_IR_je_label,  PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'jne' identifier ", 0, PX_Syntax_IR_jne_label, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'jg'  identifier ", 0, PX_Syntax_IR_jg_label,  PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'jl'  identifier ", 0, PX_Syntax_IR_jl_label,  PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'jge' identifier ", 0, PX_Syntax_IR_jge_label, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'jle' identifier ", 0, PX_Syntax_IR_jle_label, PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'jmp' const_int ", 0, PX_Syntax_IR_jmp_addr, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'je' const_int ",  0, PX_Syntax_IR_je_addr,  PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'jne' const_int ", 0, PX_Syntax_IR_jne_addr, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'jg' const_int ",  0, PX_Syntax_IR_jg_addr,  PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'jl' const_int ",  0, PX_Syntax_IR_jl_addr,  PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'jge' const_int ", 0, PX_Syntax_IR_jge_addr, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'jle' const_int ", 0, PX_Syntax_IR_jle_addr, PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'call' identifier ", 0, PX_Syntax_IR_call_label, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'call' const_int ", 0, PX_Syntax_IR_call_addr,  PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'callr' ir_rx",     0, PX_Syntax_IR_callr,   PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'jmpr' ir_rx", 0, PX_Syntax_IR_jmpr, PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ret' ", 0, PX_Syntax_IR_ret, PX_NULL);

	// --------------------------------------------------------
	// ir_ee_oprand_int: integer operand (const_int, rx, fx, addr, bp-addr)
	// ir_ee_oprand_float: float operand  (const_float, const_int, rx, fx, addr, bp-addr)
	// Both produce "ir_ee_oprand" abi (type + value)
	// --------------------------------------------------------
	PX_Syntax_Parse_PEBNF(pSyntax, "ir_ee_oprand_int = ir_rx",     0, PX_Syntax_IR_ee_op_rx,          PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir_ee_oprand_int = ir_fx",     0, PX_Syntax_IR_ee_op_fx,          PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir_ee_oprand_int = ir_addr",   0, PX_Syntax_IR_ee_op_addr,        PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir_ee_oprand_int = const_int", 0, PX_Syntax_IR_ee_op_const_int,   PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "ir_ee_oprand_float = ir_rx",       0, PX_Syntax_IR_ee_op_rx,          PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir_ee_oprand_float = ir_fx",       0, PX_Syntax_IR_ee_op_fx,          PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir_ee_oprand_float = ir_addr",     0, PX_Syntax_IR_ee_op_addr,        PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir_ee_oprand_float = const_float", 0, PX_Syntax_IR_ee_op_const_float, PX_NULL);
	//PX_Syntax_Parse_PEBNF(pSyntax, "ir_ee_oprand_float = const_int",   0, PX_Syntax_IR_ee_op_const_int,   PX_NULL);

	// --------------------------------------------------------
	// ee opcode keyword highlighting
	// --------------------------------------------------------
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_push'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_pop'",  0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_neg'",  0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_fneg'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_not'",  0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_call'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_add'",  0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_sub'",  0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_mul'",  0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_div'",  0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_idiv'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_mod'",  0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_fadd'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_fsub'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_fmul'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_fdiv'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_and'",  0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_or'",   0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_xor'",  0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_shl'",  0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_shr'",  0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_cmp'",  0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_fcmp'", 0, PX_Syntax_IR_opcode_handle_begin, PX_NULL);

	// --------------------------------------------------------
	// ee unary instructions (6 bytes):  op  operand
	//   integer unary: ee_push, ee_pop, ee_neg, ee_not, ee_call use ir_ee_oprand_int
	//   float unary:   ee_fneg uses ir_ee_oprand_float
	// --------------------------------------------------------
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_push' ir_ee_oprand_int ", 0, PX_Syntax_IR_ee_push, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_pop'  ir_ee_oprand_int ", 0, PX_Syntax_IR_ee_pop,  PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_neg'  ir_ee_oprand_int ", 0, PX_Syntax_IR_ee_neg,  PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_fneg' ir_ee_oprand_float ", 0, PX_Syntax_IR_ee_fneg, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_not'  ir_ee_oprand_int ", 0, PX_Syntax_IR_ee_not,  PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_call' ir_ee_oprand_int ", 0, PX_Syntax_IR_ee_call, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_call' identifier ", 0, PX_Syntax_IR_call_label, PX_NULL);
	// --------------------------------------------------------
	// ee ternary instructions (16 bytes):  op  dst, src1, src2
	//   integer ternary use ir_ee_oprand_int
	//   float ternary   use ir_ee_oprand_float
	// --------------------------------------------------------
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_add'  ir_ee_oprand_int ',' ir_ee_oprand_int ',' ir_ee_oprand_int ", 0, PX_Syntax_IR_ee_add,  PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_sub'  ir_ee_oprand_int ',' ir_ee_oprand_int ',' ir_ee_oprand_int ", 0, PX_Syntax_IR_ee_sub,  PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_mul'  ir_ee_oprand_int ',' ir_ee_oprand_int ',' ir_ee_oprand_int ", 0, PX_Syntax_IR_ee_mul,  PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_div'  ir_ee_oprand_int ',' ir_ee_oprand_int ',' ir_ee_oprand_int ", 0, PX_Syntax_IR_ee_div,  PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_idiv' ir_ee_oprand_int ',' ir_ee_oprand_int ',' ir_ee_oprand_int ", 0, PX_Syntax_IR_ee_idiv, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_mod'  ir_ee_oprand_int ',' ir_ee_oprand_int ',' ir_ee_oprand_int ", 0, PX_Syntax_IR_ee_mod,  PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_fadd' ir_ee_oprand_float ',' ir_ee_oprand_float ',' ir_ee_oprand_float ", 0, PX_Syntax_IR_ee_fadd, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_fsub' ir_ee_oprand_float ',' ir_ee_oprand_float ',' ir_ee_oprand_float ", 0, PX_Syntax_IR_ee_fsub, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_fmul' ir_ee_oprand_float ',' ir_ee_oprand_float ',' ir_ee_oprand_float ", 0, PX_Syntax_IR_ee_fmul, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_fdiv' ir_ee_oprand_float ',' ir_ee_oprand_float ',' ir_ee_oprand_float ", 0, PX_Syntax_IR_ee_fdiv, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_and'  ir_ee_oprand_int ',' ir_ee_oprand_int ',' ir_ee_oprand_int ", 0, PX_Syntax_IR_ee_and,  PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_or'   ir_ee_oprand_int ',' ir_ee_oprand_int ',' ir_ee_oprand_int ", 0, PX_Syntax_IR_ee_or,   PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_xor'  ir_ee_oprand_int ',' ir_ee_oprand_int ',' ir_ee_oprand_int ", 0, PX_Syntax_IR_ee_xor,  PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_shl'  ir_ee_oprand_int ',' ir_ee_oprand_int ',' ir_ee_oprand_int ", 0, PX_Syntax_IR_ee_shl,  PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_shr'  ir_ee_oprand_int ',' ir_ee_oprand_int ',' ir_ee_oprand_int ", 0, PX_Syntax_IR_ee_shr,  PX_NULL);

	// --------------------------------------------------------
	// ee_cmp  (12 bytes): integer compare,  src1, src2
	// ee_fcmp (12 bytes): float compare,    src1, src2
	// --------------------------------------------------------
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_cmp'  ir_ee_oprand_int   ',' ir_ee_oprand_int   ", 0, PX_Syntax_IR_ee_cmp,  PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "ir = 'ee_fcmp' ir_ee_oprand_float ',' ir_ee_oprand_float ", 0, PX_Syntax_IR_ee_fcmp, PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "next_scan = *", 0, PX_Syntax_IR_next_scan, PX_NULL);

	PX_Syntax_Parse_PEBNF(pSyntax, "_ = ir_init ...", 0, 0, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "_ = ir ...", 0, 0, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "_ = next_scan ...", 0, 0, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "_ = eof ", 0, 0, PX_NULL);
	PX_Syntax_Parse_PEBNF(pSyntax, "_ = * ", 0, PX_Syntax_IR_error, PX_NULL);
	return PX_TRUE;
}
