#include "PX_Syntax_base_type.h"

PX_SYNTAX_FUNCTION(PX_Syntax_is_type_exec)
{
	px_int begin, end, begin_source_index, end_source_index;
	px_char content[128] = { 0 };
	const px_char* ptype_mnemonic=PX_NULL,*ptype_define_type;
	px_abi * pnewabi;
	PX_SYNTAXLEXER_LEXEME_TYPE type = PX_Syntax_GetNextLexeme(pSyntax);
	if (type!=PX_SYNTAXLEXER_LEXEME_TYPE_TOKEN)
	{
		return PX_FALSE;
	}
	ptype_mnemonic = PX_Syntax_GetCurrentLexeme(pSyntax);
	if (PX_NULL==(ptype_define_type=PX_Syntax_FindTypeDefineTypeByMnemonic(pSyntax, ptype_mnemonic)))
	{
		return PX_FALSE;
	}
	
	pnewabi = PX_Syntax_NewAbi(pSyntax, "type", pSyntax->reg_lifetime);
	if (!pnewabi)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_is_type_exec Memory Error1");
		return PX_FALSE;
	}
	begin = PX_Syntax_GetCurrentLexemeBegin(pSyntax);
	end = PX_Syntax_GetCurrentLexemeEnd(pSyntax);
	begin_source_index = PX_Syntax_GetCurrentLexemeBeginSourceIndex(pSyntax);
	end_source_index = PX_Syntax_GetCurrentLexemeEndSourceIndex(pSyntax);
	if (begin_source_index != end_source_index)
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:cross source define is not allowed");
		return PX_FALSE;
	}

	if (!PX_AbiSet_string(pnewabi, "value", ptype_define_type))
	{
		PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_is_type_exec Memory Error2");
		return PX_FALSE;
	}
	if (PX_Syntax_NewMapToken(pSyntax, begin_source_index, begin, end_source_index, end, PX_COLOR(255, 199, 166, 230), ptype_mnemonic))
	{
		px_abi type_define_abi = PX_Syntax_GetTypeDefineAbiReadOnly(pSyntax, ptype_define_type);
		PX_strcat(content, ptype_mnemonic);
		PX_strcat(content, " size:");
		PX_strcat(content, PX_itos(PX_AbiGetValue_int(&type_define_abi, "size"), 10).data);
		if (!PX_Syntax_LastMapInfo(pSyntax, begin_source_index, content))
		{
			PX_Syntax_Terminate(pSyntax, "runtime:error:PX_Syntax_is_type_exec Memory Error6");
			return PX_FALSE;
		}
	}
	return PX_TRUE;
}




px_bool PX_Syntax_init_base_type(PX_Syntax* pSyntax)
{
	px_int operate_index;

	if (!PX_Syntax_NewTypeDefine(pSyntax, "ix.u.8", "u8", 1, pSyntax->reg_lifetime)) goto _ERROR;
	if (!PX_Syntax_NewTypeDefine(pSyntax, "ix.u.16", "u16", 2, pSyntax->reg_lifetime)) goto _ERROR;
	if (!PX_Syntax_NewTypeDefine(pSyntax, "ix.u.32", "u32", 4, pSyntax->reg_lifetime)) goto _ERROR;

	if (!PX_Syntax_NewTypeDefine(pSyntax, "ix.i.8", "i8",  1, pSyntax->reg_lifetime)) goto _ERROR;
	if (!PX_Syntax_NewTypeDefine(pSyntax, "ix.i.16", "i16",  2, pSyntax->reg_lifetime)) goto _ERROR;
	if (!PX_Syntax_NewTypeDefine(pSyntax, "ix.i.32", "i32",  4, pSyntax->reg_lifetime)) goto _ERROR;

	if (!PX_Syntax_NewTypeDefine(pSyntax, "fx.f.32", "f32",  4, pSyntax->reg_lifetime)) goto _ERROR;

	if (!PX_Syntax_NewTypeDefine(pSyntax, "void", "void", 0, pSyntax->reg_lifetime)) goto _ERROR;
	if (!PX_Syntax_NewTypeDefine(pSyntax, "pointer", "pointer", 4, pSyntax->reg_lifetime)) goto _ERROR;

	if (!PX_Syntax_NewTypedef(pSyntax, "ix.u.32", "bool")) goto _ERROR;
	if (!PX_Syntax_NewTypedef(pSyntax, "ix.i.32", "int")) goto _ERROR;
	if (!PX_Syntax_NewTypedef(pSyntax, "fx.f.32", "float")) goto _ERROR;

	if (!PX_Syntax_Parse_PEBNF(pSyntax, "type = *", 0, PX_Syntax_is_type_exec, 0))
		goto _ERROR;

	if (!PX_Syntax_load_base_opcode(pSyntax))goto _ERROR;

	//+ ix
	//- ix
	operate_index = PX_Syntax_GetOpcodeDefineIndex(pSyntax, "+", PX_SYNTAX_OPCODE_TYPE_UNARY_PREFIX);
	PX_ASSERTIFX(operate_index == -1, "Error: opcode + not found");
	if(!PX_Syntax_NewUnaryPrefixOperate(pSyntax, operate_index, "ix", PX_Syntax_operate_positive_negative_ixfx))goto _ERROR;
	if (!PX_Syntax_NewUnaryPrefixOperate(pSyntax, operate_index, "fx", PX_Syntax_operate_positive_negative_ixfx))goto _ERROR;
	operate_index = PX_Syntax_GetOpcodeDefineIndex(pSyntax, "-", PX_SYNTAX_OPCODE_TYPE_UNARY_PREFIX);
	PX_ASSERTIFX(operate_index == -1, "Error: opcode - not found");
    if (!PX_Syntax_NewUnaryPrefixOperate(pSyntax, operate_index, "ix", PX_Syntax_operate_positive_negative_ixfx)) goto _ERROR;
	if (!PX_Syntax_NewUnaryPrefixOperate(pSyntax, operate_index, "fx", PX_Syntax_operate_positive_negative_ixfx)) goto _ERROR;


	//ix = ix
	//ix = fx
	//fx = ix
	//fx = fx
	operate_index = PX_Syntax_GetOpcodeDefineIndex(pSyntax, "=", PX_SYNTAX_OPCODE_TYPE_BINARY);
	PX_ASSERTIFX(operate_index == -1, "Error: opcode = not found");
	PX_Syntax_NewBinaryOperate(pSyntax, operate_index, "ix", "ix", PX_Syntax_opcode_ixfx_assign_ixfx);
	PX_Syntax_NewBinaryOperate(pSyntax, operate_index, "ix", "fx", PX_Syntax_opcode_ixfx_assign_ixfx);
	PX_Syntax_NewBinaryOperate(pSyntax, operate_index, "fx", "ix", PX_Syntax_opcode_ixfx_assign_ixfx);
	PX_Syntax_NewBinaryOperate(pSyntax, operate_index, "fx", "fx", PX_Syntax_opcode_ixfx_assign_ixfx);

	//ix + ix
	//ix + fx
	//fx + ix
	//fx + fx
	operate_index = PX_Syntax_GetOpcodeDefineIndex(pSyntax, "+", PX_SYNTAX_OPCODE_TYPE_BINARY);
	PX_ASSERTIFX(operate_index == -1, "Error: opcode + not found");
	//runtime: ix/fx + ix/fx
	PX_Syntax_NewBinaryOperate(pSyntax, operate_index, "ix", "ix", PX_Syntax_operate_ixfx_add_sub_mul_div);
	PX_Syntax_NewBinaryOperate(pSyntax, operate_index, "ix", "fx", PX_Syntax_operate_ixfx_add_sub_mul_div);
	PX_Syntax_NewBinaryOperate(pSyntax, operate_index, "fx", "ix", PX_Syntax_operate_ixfx_add_sub_mul_div);
	PX_Syntax_NewBinaryOperate(pSyntax, operate_index, "fx", "fx", PX_Syntax_operate_ixfx_add_sub_mul_div);
	
	//ix - ix
	//ix - fx
	//fx - ix
	//fx - fx
	operate_index = PX_Syntax_GetOpcodeDefineIndex(pSyntax, "-", PX_SYNTAX_OPCODE_TYPE_BINARY);
	PX_ASSERTIFX(operate_index == -1, "Error: opcode - not found");
	PX_Syntax_NewBinaryOperate(pSyntax, operate_index, "ix", "ix", PX_Syntax_operate_ixfx_add_sub_mul_div);
	PX_Syntax_NewBinaryOperate(pSyntax, operate_index, "ix", "fx", PX_Syntax_operate_ixfx_add_sub_mul_div);
	PX_Syntax_NewBinaryOperate(pSyntax, operate_index, "fx", "ix", PX_Syntax_operate_ixfx_add_sub_mul_div);
	PX_Syntax_NewBinaryOperate(pSyntax, operate_index, "fx", "fx", PX_Syntax_operate_ixfx_add_sub_mul_div);

	//ix * ix
	//ix * fx
	//fx * ix
	//fx * fx
	operate_index = PX_Syntax_GetOpcodeDefineIndex(pSyntax, "*", PX_SYNTAX_OPCODE_TYPE_BINARY);
	PX_ASSERTIFX(operate_index == -1, "Error: opcode * not found");
	PX_Syntax_NewBinaryOperate(pSyntax, operate_index, "ix", "ix", PX_Syntax_operate_ixfx_add_sub_mul_div);
	PX_Syntax_NewBinaryOperate(pSyntax, operate_index, "ix", "fx", PX_Syntax_operate_ixfx_add_sub_mul_div);
	PX_Syntax_NewBinaryOperate(pSyntax, operate_index, "fx", "ix", PX_Syntax_operate_ixfx_add_sub_mul_div);
	PX_Syntax_NewBinaryOperate(pSyntax, operate_index, "fx", "fx", PX_Syntax_operate_ixfx_add_sub_mul_div);

	//ix / ix
	//ix / fx
	//fx / ix
	//fx / fx
	operate_index = PX_Syntax_GetOpcodeDefineIndex(pSyntax, "/", PX_SYNTAX_OPCODE_TYPE_BINARY);
	PX_ASSERTIFX(operate_index == -1, "Error: opcode / not found");
	PX_Syntax_NewBinaryOperate(pSyntax, operate_index, "ix", "ix", PX_Syntax_operate_ixfx_add_sub_mul_div);
	PX_Syntax_NewBinaryOperate(pSyntax, operate_index, "ix", "fx", PX_Syntax_operate_ixfx_add_sub_mul_div);
	PX_Syntax_NewBinaryOperate(pSyntax, operate_index, "fx", "ix", PX_Syntax_operate_ixfx_add_sub_mul_div);
	PX_Syntax_NewBinaryOperate(pSyntax, operate_index, "fx", "fx", PX_Syntax_operate_ixfx_add_sub_mul_div);

	
	return PX_TRUE;
_ERROR:
	return PX_FALSE;
}