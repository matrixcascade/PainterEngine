#include "PX_Syntax_base_operate.h"



px_abi* PX_Syntax_PushOprand(PX_Syntax* pSyntax, const px_char type[],  px_int type_size, PX_SYNTAX_OPRAND_FROM datafrom)
{
	px_abi* pnewabi = PX_Syntax_NewAbi(pSyntax, "oprand", pSyntax->reg_lifetime);
	if (!pnewabi)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_PushOprand out of memory1");
		return PX_NULL;
	}
	if (!PX_AbiSet_string(pnewabi, "type", type) || \
		!PX_AbiSet_int(pnewabi, "type_size", type_size) || \
		!PX_AbiSet_int(pnewabi, "from", (px_int)datafrom)
		)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_PushOprand out of memory2");
		PX_Syntax_PopAbi(pSyntax);
		return PX_NULL;
	}
	return pnewabi;
}

px_abi* PX_Syntax_NewOprand(PX_Syntax* pSyntax)
{
	return PX_Syntax_NewAbi(pSyntax, "oprand", pSyntax->reg_lifetime);
}

px_abi* PX_Syntax_PushVariableOprand(PX_Syntax* pSyntax, px_abi* pvariableabi)
{
	const px_char* oprand_type = PX_NULL;
	px_abi* pnewoprand = PX_Syntax_NewOprand(pSyntax);
	const px_char* final_type = "";
	px_abi var_probe_abi;
	if (!pnewoprand)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_PushVariableOprand memory error");
		return PX_NULL;
	}

	//set type
	do
	{
		//---is pointer?
		if (PX_AbiGet_AbiReadOnly(pvariableabi, &var_probe_abi, "declare_token_prefix"))
		{
			if (PX_AbiExist_string(&var_probe_abi, "type", "pointer"))
			{
				px_int pointer_level = PX_AbiGetValue_int(&var_probe_abi, "pointer_level");
				if (!PX_AbiSet_string(pnewoprand, "type", "pointer")) goto _ERROR;
				if (!PX_AbiSet_int(pnewoprand, "pointer_level", pointer_level)) goto _ERROR;
			}
			else
			{
				PX_Syntax_Terminate(pSyntax, "ast:error:unsupport declare_token_prefix");
				return PX_NULL;
			}
			final_type = "pointer";
		}
		else
		{
			const px_char* pvar_type = PX_AbiGet_string(pvariableabi, "type.type_define.type");
			PX_ASSERTIFX(!pvar_type, "ast:error:variable type not found");
			if (!PX_AbiSet_string(pnewoprand, "type", pvar_type)) goto _ERROR;
			final_type = pvar_type;
		}
	} while (0);

	//set mnemonic
	do
	{
		const px_char* pmnemonic = PX_AbiGetValue_string(pvariableabi, "type.type_define.mnemonic");
		if (!PX_AbiSet_string(pnewoprand, "mnemonic", pmnemonic)) goto _ERROR;
	} while (0);

	//set type_size
	do
	{
		px_int type_size = PX_AbiGetValue_int(pvariableabi, "type.type_define.size");
		if (!PX_AbiSet_int(pnewoprand, "type_size", type_size)) goto _ERROR;
	} while (0);

	//count
	do
	{
		px_int count = 1;
		//is array?
		if (PX_AbiGet_AbiReadOnly(pvariableabi, &var_probe_abi, "declare_array"))
		{
			px_int d = PX_AbiGetValue_int(&var_probe_abi, "d");
			px_int i;
			if (!PX_AbiSet_int(pnewoprand, "d", d)) goto _ERROR;
			for (i = 0; i < d; i++)
			{
				count *= PX_AbiGetValue_int(&var_probe_abi, PX_itos(i, 10).data);
				if (!PX_AbiCopy_FromAbiMember(pnewoprand, &var_probe_abi, PX_itos(i, 10).data)) goto _ERROR;
			}
		}
		if (!PX_AbiSet_int(pnewoprand, "count", count)) goto _ERROR;
	} while (0);

	//set lexeme
	do
	{
		const px_char* var_mnemonic = PX_AbiGet_string(pvariableabi, "identifier.value");
		PX_ASSERTIFX(!var_mnemonic, "Error:variable mnemonic not found");
		if (!PX_AbiSet_string(pnewoprand, "lexeme", var_mnemonic)) goto _ERROR;
	} while (0);

	//set from
	do
	{
		const px_char* pvar_scope = PX_AbiGet_string(pvariableabi, "scope");
		if (PX_strequ(pvar_scope, "global"))
		{
			if (!PX_AbiSet_int(pnewoprand, "from", PX_SYNTAX_OPRAND_FROM_GLOBAL)) goto _ERROR;
		}
		else if (PX_strequ(pvar_scope, "local"))
		{
			if (!PX_AbiSet_int(pnewoprand, "from", PX_SYNTAX_OPRAND_FROM_LOCAL)) goto _ERROR;
		}
		else
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:unsupport variable scope");
			return PX_NULL;
		}
	} while (0);

	//set offset
	do
	{
		px_int* poffset = PX_AbiGet_int(pvariableabi, "offset");
		PX_ASSERTIFX(!poffset, "Error:variable offset not found");
		if (poffset)
		{
			if (!PX_AbiSet_int(pnewoprand, "offset", *poffset)) goto _ERROR;
		}
	} while (0);

	return pnewoprand;
_ERROR:
	PX_Syntax_Terminate(pSyntax, "runtime:error:memory error:new oprand");
	return PX_NULL;
}

px_int PX_Syntax_GetOprandStackIndex(PX_Syntax* pSyntax, px_int index)
{
	px_int i;
	for (i = pSyntax->reg_abi_stack.size - 1; i >= 0; i--)
	{
		px_abi* pabi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
		if (PX_Syntax_CheckAbiName(pabi, "oprand"))
		{
			if (!index)
			{
				return i;
			}
			index--;
		}
	}
	return -1;
}

px_abi* PX_Syntax_GetOprandStack(PX_Syntax* pSyntax, px_int index)
{
	px_int i = PX_Syntax_GetOprandStackIndex(pSyntax, index);
	if (i != -1)
	{
		return PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, i);
	}
	return PX_NULL;

}

px_void PX_Syntax_PopLastOprand(PX_Syntax* pSyntax)
{
	px_int i = PX_Syntax_GetOprandStackIndex(pSyntax, 0);
	if (i != -1)
	{
		while (pSyntax->reg_abi_stack.size > i)
		{
			PX_Syntax_PopAbi(pSyntax);
		}
	}
}

px_void PX_Syntax_PopSecondLastOprand(PX_Syntax* pSyntax)
{
	px_int i = PX_Syntax_GetOprandStackIndex(pSyntax, 1);
	if (i != -1)
	{
		while (pSyntax->reg_abi_stack.size > i)
		{
			PX_Syntax_PopAbi(pSyntax);
		}
	}
}


px_bool PX_Syntax_NewOperate(PX_Syntax* pSyntax,px_int opcode_index, const px_char type1[], const px_char type2[], const px_char type3[], PX_Syntax_Operate_Function opfun)
{
	PX_Syntax_opcode* popcode = PX_VECTORAT(PX_Syntax_opcode, &pSyntax->reg_expr_opcode_stack, opcode_index);
	px_int operate_count, new_operate_count = \
		(popcode->type == PX_SYNTAX_OPCODE_TYPE_UNARY_PREFIX || popcode->type == PX_SYNTAX_OPCODE_TYPE_UNARY_SUFFIX ?1 :\
			(popcode->type == PX_SYNTAX_OPCODE_TYPE_BINARY ? 2 : (popcode->type == PX_SYNTAX_OPCODE_TYPE_TERNARY ? 3 : 0)));
	px_string payload;
	px_int i,scope_index;
	px_abi* pscope_abi;

	if (new_operate_count>2)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:unsupport operate count");
		return PX_FALSE;
	}

	PX_StringInitialize(pSyntax->mp, &payload);
	scope_index = PX_Syntax_GetTypeDefineScopeAbiIndex(pSyntax, type1);
	if (scope_index==-1)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_NewOperate type1 scope not found");
		goto _ERROR;
	}
	pscope_abi = PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, scope_index);
	PX_StringFormat1(&payload, "type_defines.%1.operates", PX_STRINGFORMAT_STRING(type1));
	operate_count = PX_AbiGet_PayloadMemberCount(pscope_abi, payload.buffer);
	//find exist?
	for (i = 0; i < operate_count; i++)
	{
		px_abi type_define_operate_abi_readonly;
		px_int opcode_define_index;
		PX_StringFormat2(&payload, "type_defines.%1.operates.[%2]", PX_STRINGFORMAT_STRING(type1),PX_STRINGFORMAT_INT(i));
		type_define_operate_abi_readonly = PX_AbiGetValue_abireadonly(pscope_abi, payload.buffer);
		opcode_define_index = PX_AbiGetValue_int(&type_define_operate_abi_readonly, "opcode_index");
		if (opcode_define_index == opcode_index)
		{
			if (new_operate_count == 1)
			{
				PX_Syntax_Terminate(pSyntax, "ast:error:operate exist");
				return PX_FALSE;
			}
			else if (new_operate_count == 2)
			{
				const px_char* pother_type = PX_AbiGet_string(&type_define_operate_abi_readonly, "other_type");
				if (PX_strequ(type2, pother_type))
				{
					PX_Syntax_Terminate(pSyntax, "ast:error:operate exist");
					return PX_FALSE;
				}
			}
			else
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:unsupport operate count");
				return PX_FALSE;
			}
		}
		
	}
	//new operate type
	PX_StringFormat2(&payload, "type_defines.%1.operates.[%2].opcode_index", PX_STRINGFORMAT_STRING(type1), PX_STRINGFORMAT_INT(operate_count));
	if (!PX_AbiSet_int(pscope_abi, payload.buffer, opcode_index))
	{
		goto _ERROR;
	}

	//function
	PX_StringFormat2(&payload, "type_defines.%1.operates.[%2].function", PX_STRINGFORMAT_STRING(type1), PX_STRINGFORMAT_INT(operate_count));
	if(!PX_AbiSet_ptr(pscope_abi, payload.buffer, opfun))
	{
		goto _ERROR;
	}

	//other type
	if (new_operate_count == 2)
	{
		PX_StringFormat2(&payload, "type_defines.%1.operates.[%2].other_type", PX_STRINGFORMAT_STRING(type1), PX_STRINGFORMAT_INT(operate_count));
		if (!PX_AbiSet_string(pscope_abi, payload.buffer, type2))
		{
			goto _ERROR;
		}
	}
	PX_StringFree(&payload);
	return PX_TRUE;
_ERROR:
	PX_StringFree(&payload);
	return PX_FALSE;
}

px_void PX_Syntax_PopOperate2(PX_Syntax* pSyntax, px_int index1, px_int index2)
{
	px_int max, min;
	max = index1 > index2 ? index1 : index2;
	min = index1 < index2 ? index1 : index2;
	PX_ASSERTIFX(max == min, "Error:pop unary operate oprand and opcode failed");
	PX_ASSERTIFX(max < 0 || max >= pSyntax->reg_abi_stack.size, "Error:pop unary operate oprand and opcode failed");
	PX_ASSERTIFX(min < 0 || min >= pSyntax->reg_abi_stack.size, "Error:pop unary operate oprand and opcode failed");
	PX_Syntax_PopAbiIndex(pSyntax, max);
	PX_Syntax_PopAbiIndex(pSyntax, min);
}


px_void PX_Syntax_PopOperate3(PX_Syntax* pSyntax, px_int index1, px_int index2, px_int index3)
{
	px_int max_to_min[3];
	px_int i, j, temp;
	max_to_min[0] = index1;
	max_to_min[1] = index2;
	max_to_min[2] = index3;
	//sort
	for (i = 0; i < 3 - 1; i++)
	{
		for (j = 0; j < 3 - i - 1; j++)
		{
			if (max_to_min[j] < max_to_min[j + 1])
			{
				temp = max_to_min[j];
				max_to_min[j] = max_to_min[j + 1];
				max_to_min[j + 1] = temp;
			}
		}
	}
	PX_ASSERTIFX(max_to_min[0] == max_to_min[1] || max_to_min[1] == max_to_min[2] || max_to_min[0] == max_to_min[2], "Error:pop binary operate oprand and opcode failed");
	PX_ASSERTIFX(max_to_min[0] < 0 || max_to_min[0] >= pSyntax->reg_abi_stack.size, "Error:pop binary operate oprand and opcode failed");
	PX_ASSERTIFX(max_to_min[1] < 0 || max_to_min[1] >= pSyntax->reg_abi_stack.size, "Error:pop binary operate oprand and opcode failed");
	PX_ASSERTIFX(max_to_min[2] < 0 || max_to_min[2] >= pSyntax->reg_abi_stack.size, "Error:pop binary operate oprand and opcode failed");
	for (i = 0; i < 3; i++)
		PX_Syntax_PopAbiIndex(pSyntax, max_to_min[i]);
}


px_bool PX_Syntax_NewBinaryOperate(PX_Syntax* pSyntax, px_int opcode_index, const px_char type1[], const px_char type2[], PX_Syntax_Operate_Function opfun)
{
	return PX_Syntax_NewOperate(pSyntax, opcode_index, type1, type2, "", opfun);
}

px_bool PX_Syntax_NewBeginEndOperate(PX_Syntax* pSyntax, px_int opcode_index, PX_Syntax_Operate_Function opfun)
{
	return PX_Syntax_NewOperate(pSyntax, opcode_index,  "", "", "", opfun);
}

px_bool PX_Syntax_NewUnaryPrefixOperate(PX_Syntax* pSyntax, px_int opcode_index, const px_char type1[], PX_Syntax_Operate_Function opfun)
{
	return PX_Syntax_NewOperate(pSyntax, opcode_index,  type1, "", "", opfun);
}

px_bool PX_Syntax_NewUnarySuffixOperate(PX_Syntax* pSyntax, px_int opcode_index, const px_char type1[], PX_Syntax_Operate_Function opfun)
{
	return PX_Syntax_NewOperate(pSyntax, opcode_index,  type1, "", "", opfun);
}
//
// if count>1  const-->list begin address
//             register--> error
//             stack--> error
//             global --> begin address
//             local  --> bp-begin address             

px_bool PX_Syntax_MapOprandToRegister(PX_Syntax* pSyntax, px_int oprand_index, px_int reg_index)
{
	px_char register_name[8] = { 0 };
	px_abi* oprand = PX_Syntax_GetAbiByIndex(pSyntax, oprand_index);
	const px_char* op_type = PX_AbiGet_string(oprand, "type");
	px_int op_type_size = PX_AbiGetValue_int(oprand, "type_size");
	PX_SYNTAX_OPRAND_FROM op_from = (PX_SYNTAX_OPRAND_FROM)(PX_AbiGetValue_int(oprand, "from"));
	PX_sprintf1(register_name, sizeof(register_name), "r%1", PX_STRINGFORMAT_INT(reg_index));
	//load oprand2 to r0 or f0
	switch (op_from)
	{
	case PX_SYNTAX_OPRAND_FROM_CONST:
	{
		const px_char* value = PX_AbiGetValue_string(oprand, "value");
		if(PX_Syntax_TypeMatch(op_type, "fx.const"))
		{
			if (!PX_Syntax_NewIRInstruction2(pSyntax, "movf", register_name, value))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_MapOprandToRegister out of memory err1");
				return PX_FALSE;
			}
		}
		else if(PX_Syntax_TypeMatch3(op_type, "ix.const","array","pointer"))
		{
			if (!PX_Syntax_NewIRInstruction2(pSyntax, "movc", register_name, value))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_MapOprandToRegister out of memory err2");
				return PX_FALSE;
			}
		}
		else
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:unsupport const oprand type");
			return PX_FALSE;
		}
	}
		break;
	case PX_SYNTAX_OPRAND_FROM_REGISTER:
	{
		px_int fromreg = PX_AbiGetValue_int(oprand, "offset");
		if (fromreg != reg_index)
		{
			px_char targetr[] = "rx";
			targetr[1] = '0' + fromreg;
			if (!PX_Syntax_NewIRInstruction2(pSyntax, "movr", register_name, targetr))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory err8");
				return PX_FALSE;
			}
		}
	}
	break;
	case PX_SYNTAX_OPRAND_FROM_STACK:
	{
		if (!PX_Syntax_NewIRInstruction1(pSyntax, "pop", register_name))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory err10");
			return PX_FALSE;
		}
	}
	break;
	case PX_SYNTAX_OPRAND_FROM_GLOBAL:
	case PX_SYNTAX_OPRAND_FROM_LOCAL:
	{
		if(PX_Syntax_TypeMatch2(op_type,"array","pointer"))
		{
			//eg array.ix.i.8, array.pointer.ix.i.32, pointer.ix.i.32
			const px_char* opcode_instr = "";
			px_char oprand2_str[16] = { 0 };
			px_int offset = PX_AbiGetValue_int(oprand, "offset");
			PX_sprintf1(oprand2_str, sizeof(oprand2_str), "%1", PX_STRINGFORMAT_INT(offset));
			switch (op_type_size)
			{
			case 4:
				opcode_instr = "loadu32";
				break;
			default:
				PX_Syntax_Terminate(pSyntax, "ast:error:unsupport type size");
				return PX_FALSE;
			}
			if (!PX_Syntax_NewIRInstruction2(pSyntax, opcode_instr, register_name, oprand2_str))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory err15");
				return PX_FALSE;
			}
		}
		else if(PX_Syntax_TypeMatch2(op_type, "fx", "ix"))
		{
			//eg ix.i.8, ix.u.16, fx.f.32
			const px_char* opcode_instr = "";
			px_char oprand2_str[16] = { 0 };
			px_int offset = PX_AbiGetValue_int(oprand, "offset");
			if (op_from == PX_SYNTAX_OPRAND_FROM_LOCAL)
				PX_sprintf1(oprand2_str, sizeof(oprand2_str), "bp-%1", PX_STRINGFORMAT_INT(offset));
			else
				PX_sprintf1(oprand2_str, sizeof(oprand2_str), "%1", PX_STRINGFORMAT_INT(offset));
			if (PX_Syntax_TypeMatch2(op_type, "ix.u","fx.f"))
			{
				switch (op_type_size)
				{
				case 4:
					opcode_instr = "loadu32";
					break;
				case 2:
					opcode_instr = "loadu16";
					break;
				case 1:
					opcode_instr = "loadu8";
					break;
				default:
					PX_Syntax_Terminate(pSyntax, "ast:error:unsupport type size");
					return PX_FALSE;
				}
			}
			else if (PX_Syntax_TypeMatch(op_type, "ix.i"))
			{
				switch (op_type_size)
				{
				case 4:
					opcode_instr = "loadi32";
					break;
				case 2:
					opcode_instr = "loadi16";
					break;
				case 1:
					opcode_instr = "loadi8";
					break;
				default:
					PX_Syntax_Terminate(pSyntax, "ast:error:unsupport type size");
					return PX_FALSE;
				}
			}
			else 
			{
				PX_Syntax_Terminate(pSyntax, "ast:error:unsupport type size");
				return PX_FALSE;
			}
			if (!PX_Syntax_NewIRInstruction2(pSyntax, opcode_instr, register_name, oprand2_str))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory err15");
				return PX_FALSE;
			}
		}
		else
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:unsupport variable oprand type");
			return PX_FALSE;
		}
	}
	break;
	default:
		PX_Syntax_Terminate(pSyntax, "ast:error:unsupport oprand type");
		return PX_FALSE;
	}
	return PX_TRUE;
}


PX_SYNTAX_OPERATE_FUNCTION(PX_Syntax_operate_positive_negative_ixfx)
{
	const px_char* poprand_type;
	px_abi* poprand_abi = PX_Syntax_GetAbiByIndex(pSyntax, oprand1_abi_index);
	px_abi* popcode_abi = PX_Syntax_GetAbiByIndex(pSyntax, opcode_abi_index);
	px_int index = PX_AbiGetValue_int(popcode_abi, "index");
	PX_Syntax_opcode* pdefine_opcode = PX_Syntax_GetOpcodeDefine(pSyntax, index);

	PX_ASSERTIFX(!pdefine_opcode, "Unknow positive_negative Error");
	poprand_type= PX_AbiGet_string(poprand_abi, "type");
	PX_ASSERTIFX(!poprand_type, "Unknow positive_negative Error");
	if (pdefine_opcode->opcode[0] == '+')
	{
		//remove opcode
		PX_Syntax_PopAbiIndex(pSyntax, opcode_abi_index);

		return PX_TRUE;
	}
	else if (pdefine_opcode->opcode[0] == '-')
	{
		if (PX_Syntax_TypeMatch2(poprand_type, "ix.const", "fx.const"))
		{
			px_char value[64] = { 0 };
			const px_char* pvalue = PX_AbiGet_string(poprand_abi, "value");
			if (pvalue[0] == '-')
			{
				PX_strcpy(value, pvalue + 1, sizeof(value));
			}
			else
			{
				value[0] = '-';
				PX_strcpy(value + 1, pvalue, sizeof(value) - 1);
			}
			if (!PX_AbiSet_string(poprand_abi, "value", value))
			{
				PX_Syntax_Terminate(pSyntax, "runrime:error:out of memory");
				return PX_FALSE;
			}
			PX_Syntax_PopAbiIndex(pSyntax, opcode_abi_index);
			return PX_TRUE;
		}
		else if (PX_Syntax_TypeMatch(poprand_type, "fx"))
		{
			if (!PX_Syntax_MapOprandToRegister(pSyntax, oprand1_abi_index,0))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_positive_negative_ixuxfx Memory Error1");
				return PX_FALSE;
			}
			//r0->f0
			if (!PX_Syntax_NewIRInstruction2(pSyntax, "movr", "f0", "r0"))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_positive_negative_ixuxfx Memory Error2");
				return PX_FALSE;
			}

			//f0=-f0
			if (!PX_Syntax_NewIRInstruction1(pSyntax, "fneg", "f0"))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_positive_negative_ixuxfx Memory Error3");
				return PX_FALSE;
			}

			//f0->r0
			if (!PX_Syntax_NewIRInstruction2(pSyntax, "movr", "r0", "f0"))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_positive_negative_ixuxfx Memory Error4");
				return PX_FALSE;
			}

			if (!PX_Syntax_NewIRInstruction1(pSyntax, "push", "r0"))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_positive_negative_ixuxfx Memory Error3");
				return PX_FALSE;
			}

			//pop oprand and push r0
			
			if (!PX_Syntax_PushOprand(pSyntax, poprand_type,  PX_Syntax_GetTypeSize(pSyntax, poprand_type), PX_SYNTAX_OPRAND_FROM_STACK))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_positive_negative_ixuxfx Memory Error5");
				return PX_FALSE;
			}
		}
		else if (PX_Syntax_TypeMatch(poprand_type, "ix"))
		{
			if (!PX_Syntax_MapOprandToRegister(pSyntax, oprand1_abi_index,0))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_positive_negative_ixuxfx Memory Error6");
				return PX_FALSE;
			}
			//r0=-r0
			if (!PX_Syntax_NewIRInstruction1(pSyntax, "neg", "r0"))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_positive_negative_ixuxfx Memory Error7");
				return PX_FALSE;
			}
			if (!PX_Syntax_NewIRInstruction1(pSyntax, "push", "r0"))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_positive_negative_ixuxfx Memory Error8");
				return PX_FALSE;
			}
			//pop oprand and push r0
			
			if (!PX_Syntax_PushOprand(pSyntax, poprand_type, PX_Syntax_GetTypeSize(pSyntax, poprand_type), PX_SYNTAX_OPRAND_FROM_STACK))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_positive_negative_ixuxfx Memory Error9");
				return PX_FALSE;
			}
		}
		else
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:unsupport type for positive_negative");
			return PX_FALSE;
		}
		//remove opcode
		PX_Syntax_PopOperate2(pSyntax, oprand1_abi_index, opcode_abi_index);
		return PX_TRUE;
	}
	else
	{
		PX_Syntax_Terminate(pSyntax, "ast:error:Unknow positive_negative Error");
		return PX_FALSE;
	}

}

PX_SYNTAX_OPERATE_FUNCTION(PX_Syntax_operate_ixfx_const_folding_add_sub_mul_div)
{
	px_abi* pabi1, * pabi2, * pabi_result;
	const px_char* type1, * type2;
	const px_char* value1, * value2; 
	px_int opcode_index;
	px_char final_value[64] = { 0 };
	px_abi* pdefine_opcode_abi;
	const px_char* popcode_str;
	PX_SYNTAX_OPCODE_TYPE opcode_type;
	px_int precedence;
	if (oprand1_abi_index < 0 || oprand2_abi_index < 0) return PX_FALSE;
	PX_VECTOR_CHECK_RANGE(&pSyntax->reg_abi_stack, opcode_abi_index);
	opcode_index = PX_AbiGetValue_int(PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, opcode_abi_index), "index");
	pdefine_opcode_abi = PX_Syntax_GetAbiByIndex(pSyntax, opcode_index);
	PX_ASSERTIFX(!PX_Syntax_CheckAbiName(pdefine_opcode_abi, "define_opcode"), "Unknow operate_const_folding_add_sub_mul_div Error");

	popcode_str = PX_AbiGetValue_string(pdefine_opcode_abi, "opcode");
	opcode_type = PX_AbiGetValue_int(pdefine_opcode_abi, "type");
	precedence = PX_AbiGetValue_int(pdefine_opcode_abi, "precedence");

	pabi1 = PX_Syntax_GetAbiByIndex(pSyntax, oprand1_abi_index);
	pabi2 = PX_Syntax_GetAbiByIndex(pSyntax, oprand2_abi_index);
	if (!pabi1 || !pabi2) return PX_FALSE;
	type1 = PX_AbiGetValue_string(pabi1, "type");
	type2 = PX_AbiGetValue_string(pabi2, "type");
	value1 = PX_AbiGetValue_string(pabi1, "value");
	value2 = PX_AbiGetValue_string(pabi2, "value");
	if (PX_strlen(value1)>=32)
	{
		PX_Syntax_Terminate(pSyntax, "ast:error:Const value too long");
		return PX_FALSE;
	}
	if (PX_strlen(value2) >= 32)
	{
		PX_Syntax_Terminate(pSyntax, "ast:error:Const value too long");
		return PX_FALSE;
	}

	PX_ASSERTIFX(!PX_Syntax_TypeMatch(type1, "ix.const") && !PX_Syntax_TypeMatch(type1, "fx.const"), "unsupport type");

	if(popcode_str[0]=='+')
	{
		//add
		if (!PX_StringNumeric_add(value1, value2, final_value, sizeof(final_value)))
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:Const folding add error");
			return PX_FALSE;
		}
	}
	else if (popcode_str[0] == '-')
	{
		//sub
		if (!PX_StringNumeric_sub(value1, value2, final_value, sizeof(final_value)))
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:Const folding sub error");
			return PX_FALSE;
		}
	}
	else if (popcode_str[0] == '*')
	{
		//mul
		if (!PX_StringNumeric_mul(value1, value2, final_value, sizeof(final_value)))
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:Const folding mul error");
			return PX_FALSE;
		}
	}
	else if (popcode_str[0] == '/')
	{
		//div
		if(PX_atof(value2)==0)
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:Const folding div zero error");
			return PX_FALSE;
		}

		if (!PX_StringNumeric_div2(value1, value2, final_value, sizeof(final_value)))
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:Const folding div error");
			return PX_FALSE;
		}
	}
	else
	{
		PX_Syntax_Terminate(pSyntax, "ast:error:Const folding unsupport opcode");
		return PX_FALSE;
	}
	
	if (PX_strequ(type1,"fx.const")|| PX_strequ(type2, "fx.const"))
	{
		pabi_result = PX_Syntax_PushOprand(pSyntax, "fx.const", PX_Syntax_GetTypeSize(pSyntax,"fx.f.32"), PX_SYNTAX_OPRAND_FROM_CONST);
		if (!pabi_result)
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_const_folding_add_sub_mul_div Memory Error1");
			return PX_FALSE;
		}
		// Set the result value
		if (!PX_AbiSet_string(pabi_result, "value", final_value))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_const_folding_add_sub_mul_div Memory Error2");
			return PX_FALSE;
		}
	}
	else 
	{
		pabi_result = PX_Syntax_PushOprand(pSyntax, "ix.const",  PX_Syntax_GetTypeSize(pSyntax, "ix.i.32"), PX_SYNTAX_OPRAND_FROM_CONST);
		if (!pabi_result)
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_const_folding_add_sub_mul_div Memory Error3");
			return PX_FALSE;
		}
		// Set the result value
		if (!PX_AbiSet_string(pabi_result, "value", final_value))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_const_folding_add_sub_mul_div Memory Error4");
			return PX_FALSE;
		}
	}
	PX_Syntax_PopOperate3(pSyntax, oprand1_abi_index, oprand2_abi_index, opcode_abi_index);
	return PX_TRUE;
}


PX_SYNTAX_OPERATE_FUNCTION(PX_Syntax_opcode_pointer_assign)
{
	px_abi* oprand1 = PX_Syntax_GetAbiByIndex(pSyntax, oprand1_abi_index);
	px_abi* oprand2 = PX_Syntax_GetAbiByIndex(pSyntax, oprand2_abi_index);
	const px_char* op1_type = PX_AbiGet_string(oprand1, "pointer_type");
	const px_char* op2_type = PX_AbiGet_string(oprand2, "pointer_type");
	px_int op1_pointer_level = PX_AbiGetValue_int(oprand1, "pointer_level");
	px_int op2_pointer_level = PX_AbiGetValue_int(oprand2, "pointer_level");
	PX_SYNTAX_OPRAND_FROM op1_from = (PX_SYNTAX_OPRAND_FROM)(PX_AbiGetValue_int(oprand1, "from"));
	PX_SYNTAX_OPRAND_FROM op2_from = (PX_SYNTAX_OPRAND_FROM)(PX_AbiGetValue_int(oprand2, "from"));
	if (op1_pointer_level != op2_pointer_level)
	{
		PX_Syntax_Terminate(pSyntax, "ast:error:Pointer level not match");
		return PX_FALSE;
	}

	if (!PX_strequ(op1_type, op2_type))
	{
		PX_Syntax_Message(pSyntax, "warning:pointer type not match\n");
	}
	//map oprand2 to register r0
	switch (op2_from)
	{
	case PX_SYNTAX_OPRAND_FROM_CONST:
	{
		const px_char* value = PX_AbiGet_string(oprand2, "value");
		if (!PX_Syntax_NewIRInstruction2(pSyntax,"movc","r0",value))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory err1");
			return PX_FALSE;
		}
	}
	break;
	case PX_SYNTAX_OPRAND_FROM_REGISTER:
	{
		//should not b here
		px_int fromreg = PX_AbiGetValue_int(oprand2, "offset");
		if (fromreg != 0)
		{
			if (!PX_Syntax_NewIRInstruction2(pSyntax, "movr", "r0", PX_itos(fromreg, 10).data))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory err2");
				return PX_FALSE;
			}
		}
	}
	break;
	case PX_SYNTAX_OPRAND_FROM_STACK:
	{
		if (!PX_Syntax_NewIRInstruction1(pSyntax, "pop", "r0"))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory err3");
			return PX_FALSE;
		}
	}
	break;
	case PX_SYNTAX_OPRAND_FROM_LOCAL:
	{
		px_char content[32] = "[bp-";
		px_int offset = PX_AbiGetValue_int(oprand2, "offset");
		PX_strcat(content, PX_itos(offset, 10).data);
		PX_strcat(content, "]");
		if (!PX_Syntax_NewIRInstruction2(pSyntax, "loadu32", "r0", content))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory err4");
			return PX_FALSE;
		}
	}
	break;
	case PX_SYNTAX_OPRAND_FROM_GLOBAL:
	{
		px_char content[32] = "[";
		px_int offset = PX_AbiGetValue_int(oprand2, "offset");
		PX_strcat(content, PX_itos(offset, 10).data);
		PX_strcat(content, "]");
		if (!PX_Syntax_NewIRInstruction2(pSyntax, "loadu32", "r0", content))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory err5");
			return PX_FALSE;
		}
	}
	break;
	default:
		PX_Syntax_Terminate(pSyntax, "ast:error:unsupport oprand type");
		return PX_FALSE;
	}

	//store r0 to oprand1
	switch (op1_from)
	{
	case PX_SYNTAX_OPRAND_FROM_CONST:
		PX_Syntax_Terminate(pSyntax, "ast:error:can not assign to constant");
		return PX_FALSE;
	case PX_SYNTAX_OPRAND_FROM_GLOBAL:
	{
		px_int offset = PX_AbiGetValue_int(oprand1, "offset");
		px_char content[32] = "[";
		PX_strcat(content, PX_itos(offset, 10).data);
		PX_strcat(content, "]");
		if (!PX_Syntax_NewIRInstruction2(pSyntax, "store32", "r0", content))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory err6");
			return PX_FALSE;
		}

	}
	break;
	case PX_SYNTAX_OPRAND_FROM_LOCAL:
	{
		px_int offset = PX_AbiGetValue_int(oprand1, "offset");
		px_char content[32] = "[bp-";
		PX_strcat(content, PX_itos(offset, 10).data);
		PX_strcat(content, "]");
		if (!PX_Syntax_NewIRInstruction2(pSyntax, "store32", "r0", content))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory err7");
			return PX_FALSE;
		}
	}
	break;
	case PX_SYNTAX_OPRAND_FROM_REGISTER:
	{
		px_int toreg = PX_AbiGetValue_int(oprand1, "offset");
		px_char content[32] = "r";
		PX_strcat(content, PX_itos(toreg, 10).data);
		if (!PX_Syntax_NewIRInstruction2(pSyntax, "movr",  content, "r0"))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory err8");
			return PX_FALSE;
		}
	}
	break;
	case PX_SYNTAX_OPRAND_FROM_STACK:
	{
		if (!PX_Syntax_NewIRInstruction1(pSyntax, "pop", "r0"))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory err9");
			return PX_FALSE;
		}
	}
	break;

	default:
		PX_Syntax_Terminate(pSyntax, "ast:error:unsupport oprand type");
		return PX_FALSE;
	}

	do
	{
		px_abi* pnewoprand;
		px_char op1_type[32] = "pointer:";
		PX_strcat(op1_type, op1_type);
		if (!PX_Syntax_NewIRInstruction1(pSyntax, "push", "r0"))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory err9");
			return PX_FALSE;
		}
		PX_Syntax_PopOperate3(pSyntax, oprand1_abi_index, oprand2_abi_index, opcode_abi_index);
		if (PX_NULL == (pnewoprand = PX_Syntax_PushOprand(pSyntax, op1_type, PX_Syntax_GetTypeSize(pSyntax, "pointer"), PX_SYNTAX_OPRAND_FROM_STACK)))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:out of memory");
			return PX_FALSE;
		}
	} while (0);

	return PX_TRUE;
}


PX_SYNTAX_OPERATE_FUNCTION(PX_Syntax_opcode_ixfx_assign_ixfx)
{
	px_abi* oprand1 = PX_Syntax_GetAbiByIndex(pSyntax, oprand1_abi_index);
	px_abi* oprand2 = PX_Syntax_GetAbiByIndex(pSyntax, oprand2_abi_index);
	const px_char* op1_type = PX_AbiGet_string(oprand1, "type");
	const px_char* op2_type = PX_AbiGet_string(oprand2, "type");
	px_int op1_type_size = PX_AbiGetValue_int(oprand1, "type_size");
	px_int op2_type_size = PX_AbiGetValue_int(oprand2, "type_size");
	PX_SYNTAX_OPRAND_FROM op1_from = (PX_SYNTAX_OPRAND_FROM)(PX_AbiGetValue_int(oprand1, "from"));
	PX_SYNTAX_OPRAND_FROM op2_from = (PX_SYNTAX_OPRAND_FROM)(PX_AbiGetValue_int(oprand2, "from"));


	if (!PX_Syntax_MapOprandToRegister(pSyntax, oprand2_abi_index, 0))
	{
		PX_Syntax_Terminate(pSyntax, "ast:error:PX_Syntax_opcode_ixfx_assign_ixfx map oprand2 to register failed");
		return PX_FALSE;
	}
	
	switch (op1_from)
	{
	case PX_SYNTAX_OPRAND_FROM_LOCAL:
	case PX_SYNTAX_OPRAND_FROM_GLOBAL:
	{
		px_int offset = PX_AbiGetValue_int(oprand1, "offset");
		px_char content[32] = {0};
		if (op1_from== PX_SYNTAX_OPRAND_FROM_LOCAL)
		{
			PX_strcat(content, "bp-");
		}
		PX_strcat(content, PX_itos(offset, 10).data);

		if (PX_Syntax_TypeMatch(op1_type,"fx"))
		{
			if (PX_Syntax_TypeMatch(op2_type, "ix"))
			{
				//transfer ix->fx
				if (PX_Syntax_TypeMatch(op2_type, "ix.i"))
				{
					if (!PX_Syntax_NewIRInstruction2(pSyntax, "i2f", "f0", "r0"))
					{
						PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory err1");
						return PX_FALSE;
					}
				}
				else
				{
					if (!PX_Syntax_NewIRInstruction2(pSyntax, "u2f", "f0", "r0"))
					{
						PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory err2");
						return PX_FALSE;
					}
				}
				if (!PX_Syntax_NewIRInstruction2(pSyntax, "movr", "r0", "f0"))
				{
					PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory err3");
					return PX_FALSE;
				}
			}
		}
		else
		{
			//transfer fx->ix
			if (PX_Syntax_TypeMatch(op2_type, "fx"))
			{
				if (!PX_Syntax_NewIRInstruction2(pSyntax, "movr", "f0", "r0"))
				{
					PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory err4");
					return PX_FALSE;
				}

				if (PX_Syntax_TypeMatch(op1_type, "ix.i"))
				{
					if (!PX_Syntax_NewIRInstruction2(pSyntax, "f2i", "r0", "f0"))
					{
						PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory err4");
						return PX_FALSE;
					}
				}
				else
				{
					if (!PX_Syntax_NewIRInstruction2(pSyntax, "f2u", "r0", "f0"))
					{
						PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory err5");
						return PX_FALSE;
					}
				}

			}
		}

		switch (op1_type_size)
		{
		case 4:
			if (!PX_Syntax_NewIRInstruction2(pSyntax, "store32", content, "r0"))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory err4");
				return PX_FALSE;
			}
			break;
		case 2:
			if (!PX_Syntax_NewIRInstruction2(pSyntax, "store16", content, "r0"))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory err4");
				return PX_FALSE;
			}
			break;
		case 1:
			if (!PX_Syntax_NewIRInstruction2(pSyntax, "store8", content, "r0"))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory err4");
				return PX_FALSE;
			}
			break;
		default:
			PX_Syntax_Terminate(pSyntax, "ast:error:unsupport type size");
			return PX_FALSE;
		}


	}
	break;
	default:
		PX_Syntax_Terminate(pSyntax, "ast:error:can not assign to this oprand type");
		return PX_FALSE;
	}

	if (!PX_Syntax_NewIRInstruction1(pSyntax, "push","r0"))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory err5");
		return PX_FALSE;
	}
	//pop operate stack
	PX_Syntax_PopOperate3(pSyntax, oprand1_abi_index, oprand2_abi_index, opcode_abi_index);
	//push result oprand
	if (!PX_Syntax_PushOprand(pSyntax, op1_type,op1_type_size,PX_SYNTAX_OPRAND_FROM_STACK))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_Opcode_pointer_equ out of memory");
		return PX_FALSE;
	}

	return PX_TRUE;
}


PX_SYNTAX_OPERATE_FUNCTION(PX_Syntax_operate_runtime_ixfx_add_sub_mul_div)
{
	px_abi* pabi1, *pabi2;
	const px_char* type1, *type2;
	px_int type_size1, type_size2;
	px_int count1, count2;
	px_int opcode_index;
	px_abi* pdefine_opcode_abi;
	const px_char* popcode_str;
	const px_char* result_type;
	px_int result_type_size;

	if (oprand1_abi_index < 0 || oprand2_abi_index < 0) return PX_FALSE;

	opcode_index = PX_AbiGetValue_int(PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, opcode_abi_index), "index");
	pdefine_opcode_abi = PX_Syntax_GetAbiByIndex(pSyntax, opcode_index);
	PX_ASSERTIFX(!PX_Syntax_CheckAbiName(pdefine_opcode_abi, "define_opcode"), "Unknow runtime_add_sub_mul_div Error");
	popcode_str = PX_AbiGetValue_string(pdefine_opcode_abi, "opcode");

	pabi1 = PX_Syntax_GetAbiByIndex(pSyntax, oprand1_abi_index);
	pabi2 = PX_Syntax_GetAbiByIndex(pSyntax, oprand2_abi_index);
	if (!pabi1 || !pabi2) return PX_FALSE;

	type1 = PX_AbiGetValue_string(pabi1, "type");
	type2 = PX_AbiGetValue_string(pabi2, "type");
	type_size1 = PX_AbiGetValue_int(pabi1, "type_size");
	type_size2 = PX_AbiGetValue_int(pabi2, "type_size");
	count1 = PX_AbiGetValue_int(pabi1, "count");
	count2 = PX_AbiGetValue_int(pabi2, "count");

	if (count1 != 1 || count2 != 1)
	{
		PX_Syntax_Terminate(pSyntax, "ast:error:runtime add_sub_mul_div oprand count error");
		return PX_FALSE;
	}

	// map oprand1 to r0, push to save
	if (!PX_Syntax_MapOprandToRegister(pSyntax, oprand1_abi_index, 0)) return PX_FALSE;
	if (!PX_Syntax_NewIRInstruction1(pSyntax, "push", "r0"))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_runtime_add_sub_mul_div out of memory err1");
		return PX_FALSE;
	}

	// map oprand2 to r0
	if (!PX_Syntax_MapOprandToRegister(pSyntax, oprand2_abi_index, 0)) return PX_FALSE;

	// pop oprand1 into r1
	if (!PX_Syntax_NewIRInstruction1(pSyntax, "pop", "r1"))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_runtime_add_sub_mul_div out of memory err2");
		return PX_FALSE;
	}

	// now r1=oprand1, r0=oprand2
	if (PX_Syntax_TypeMatch(type1, "fx") || PX_Syntax_TypeMatch(type2, "fx"))
	{
		// float operation: convert both to float, use f0/f1
		if (PX_Syntax_TypeMatch(type1, "fx"))
		{
			if (!PX_Syntax_NewIRInstruction2(pSyntax, "movr", "f1", "r1"))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_runtime_add_sub_mul_div out of memory err3");
				return PX_FALSE;
			}
		}
		else if (PX_Syntax_TypeMatch(type1, "ix.i"))
		{
			if (!PX_Syntax_NewIRInstruction2(pSyntax, "i2f", "f1", "r1"))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_runtime_add_sub_mul_div out of memory err4");
				return PX_FALSE;
			}
		}
		else if (PX_Syntax_TypeMatch(type1, "ix.u"))
		{
			if (!PX_Syntax_NewIRInstruction2(pSyntax, "u2f", "f1", "r1"))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_runtime_add_sub_mul_div out of memory err5");
				return PX_FALSE;
			}
		}
		else
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:unsupport type for runtime add_sub_mul_div");
			return PX_FALSE;
		}

		if (PX_Syntax_TypeMatch(type2, "fx"))
		{
			if (!PX_Syntax_NewIRInstruction2(pSyntax, "movr", "f0", "r0"))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_runtime_add_sub_mul_div out of memory err6");
				return PX_FALSE;
			}
		}
		else if (PX_strequ(type2, "ix.i"))
		{
			if (!PX_Syntax_NewIRInstruction2(pSyntax, "i2f", "f0", "r0"))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_runtime_add_sub_mul_div out of memory err7");
				return PX_FALSE;
			}
		}
		else
		{
			if (!PX_Syntax_NewIRInstruction2(pSyntax, "u2f", "f0", "r0"))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_runtime_add_sub_mul_div out of memory err8");
				return PX_FALSE;
			}
		}

		// f1 op f0 -> f0
		if (popcode_str[0] == '+')
		{
			if (!PX_Syntax_NewIRInstruction2(pSyntax, "fadd", "f0", "f1"))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_runtime_add_sub_mul_div out of memory err9");
				return PX_FALSE;
			}
		}
		else if (popcode_str[0] == '-')
		{
			if (!PX_Syntax_NewIRInstruction2(pSyntax, "fsub", "f0", "f1"))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_runtime_add_sub_mul_div out of memory err10");
				return PX_FALSE;
			}
		}
		else if (popcode_str[0] == '*')
		{
			if (!PX_Syntax_NewIRInstruction2(pSyntax, "fmul", "f0", "f1"))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_runtime_add_sub_mul_div out of memory err11");
				return PX_FALSE;
			}
		}
		else if (popcode_str[0] == '/')
		{
			if (!PX_Syntax_NewIRInstruction2(pSyntax, "fdiv", "f0", "f1"))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_runtime_add_sub_mul_div out of memory err12");
				return PX_FALSE;
			}
		}
		else
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:unsupport float opcode");
			return PX_FALSE;
		}

		if (!PX_Syntax_NewIRInstruction2(pSyntax, "movr", "r0", "f0"))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_runtime_add_sub_mul_div out of memory err13");
			return PX_FALSE;
		}

		result_type = "fx:f:32";
		result_type_size = PX_Syntax_GetTypeSize(pSyntax, "fx:f:32");
	}
	else
	{
		// integer operation: r1 op r0 -> r0
		if (popcode_str[0] == '+')
		{
			if (!PX_Syntax_NewIRInstruction2(pSyntax, "add", "r0", "r1"))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_runtime_add_sub_mul_div out of memory err14");
				return PX_FALSE;
			}
		}
		else if (popcode_str[0] == '-')
		{
			if (!PX_Syntax_NewIRInstruction2(pSyntax, "sub", "r0", "r1"))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_runtime_add_sub_mul_div out of memory err15");
				return PX_FALSE;
			}
		}
		else if (popcode_str[0] == '*')
		{
			if (!PX_Syntax_NewIRInstruction2(pSyntax, "mul", "r0", "r1"))
			{
				PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_runtime_add_sub_mul_div out of memory err16");
				return PX_FALSE;
			}
		}
		else if (popcode_str[0] == '/')
		{
			if (PX_strequ(type1, "ux") && PX_strequ(type2, "ux"))
			{
				if (!PX_Syntax_NewIRInstruction2(pSyntax, "udiv", "r0", "r1"))
				{
					PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_runtime_add_sub_mul_div out of memory err17");
					return PX_FALSE;
				}
			}
			else
			{
				if (!PX_Syntax_NewIRInstruction2(pSyntax, "div", "r0", "r1"))
				{
					PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_runtime_add_sub_mul_div out of memory err18");
					return PX_FALSE;
				}
			}
		}
		else
		{
			PX_Syntax_Terminate(pSyntax, "ast:error:unsupport integer opcode");
			return PX_FALSE;
		}

		result_type = type1;
		result_type_size = type_size1;
	}

	if (!PX_Syntax_NewIRInstruction1(pSyntax, "push", "r0"))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_runtime_add_sub_mul_div out of memory err19");
		return PX_FALSE;
	}

	PX_Syntax_PopOperate3(pSyntax, oprand1_abi_index, oprand2_abi_index, opcode_abi_index);

	if (!PX_Syntax_PushOprand(pSyntax, result_type, result_type_size, PX_SYNTAX_OPRAND_FROM_STACK))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_operate_runtime_add_sub_mul_div out of memory err20");
		return PX_FALSE;
	}

	return PX_TRUE;
}

PX_SYNTAX_OPERATE_FUNCTION(PX_Syntax_operate_ixfx_add_sub_mul_div)
{
	px_abi* pabi1, *pabi2;
	const px_char* type1, *type2;
	px_int opcode_index;
	px_abi* pdefine_opcode_abi;
	const px_char* popcode_str;
	if (oprand1_abi_index < 0 || oprand2_abi_index < 0) return PX_FALSE;
	opcode_index = PX_AbiGetValue_int(PX_VECTORAT(px_abi, &pSyntax->reg_abi_stack, opcode_abi_index), "index");
	pdefine_opcode_abi = PX_Syntax_GetAbiByIndex(pSyntax, opcode_index);
	PX_ASSERTIFX(!PX_Syntax_CheckAbiName(pdefine_opcode_abi, "define_opcode"), "Unknow operate_router_add_sub_mul_div Error");
	popcode_str = PX_AbiGetValue_string(pdefine_opcode_abi, "opcode");
	pabi1 = PX_Syntax_GetAbiByIndex(pSyntax, oprand1_abi_index);
	pabi2 = PX_Syntax_GetAbiByIndex(pSyntax, oprand2_abi_index);
	if (!pabi1 || !pabi2) return PX_FALSE;
	type1 = PX_AbiGetValue_string(pabi1, "type");
	type2 = PX_AbiGetValue_string(pabi2, "type");

	if (PX_Syntax_TypeMatch2(type1,"fx.const", "ix.const") && PX_Syntax_TypeMatch2(type2, "fx.const", "ix.const"))
	{
		return PX_Syntax_operate_ixfx_const_folding_add_sub_mul_div(pSyntax, oprand1_abi_index, oprand2_abi_index, opcode_abi_index);
	}
	else
	{
		return PX_Syntax_operate_runtime_ixfx_add_sub_mul_div(pSyntax, oprand1_abi_index, oprand2_abi_index, opcode_abi_index);
	}
}