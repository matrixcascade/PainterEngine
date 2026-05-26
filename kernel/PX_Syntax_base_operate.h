#ifndef PX_SYNTAX_BASE_OPERATE_H
#define PX_SYNTAX_BASE_OPERATE_H
#include "PX_Syntax.h"


px_bool PX_Syntax_NewBeginEndOperate(PX_Syntax* pSyntax, px_int opcode_index, PX_Syntax_Operate_Function opfun);
px_bool PX_Syntax_NewUnaryPrefixOperate(PX_Syntax* pSyntax, px_int opcode_index, const px_char type1[], PX_Syntax_Operate_Function opfun);
px_bool PX_Syntax_NewUnarySuffixOperate(PX_Syntax* pSyntax, px_int opcode_index, const px_char type1[], PX_Syntax_Operate_Function opfun);
px_bool PX_Syntax_NewBinaryOperate(PX_Syntax* pSyntax, px_int opcode_index, const px_char type1[], const px_char type2[], PX_Syntax_Operate_Function opfun);
px_abi* PX_Syntax_GetOprandStack(PX_Syntax* pSyntax, px_int index);
px_int PX_Syntax_GetOprandStackIndex(PX_Syntax* pSyntax, px_int index);
px_void PX_Syntax_PopLastOprand(PX_Syntax* pSyntax);
px_void PX_Syntax_PopSecondLastOprand(PX_Syntax* pSyntax);
px_bool PX_Syntax_MapOprandToRegister(PX_Syntax* pSyntax, px_int oprand_index, px_int reg_index);

PX_SYNTAX_OPERATE_FUNCTION(PX_Syntax_operate_positive_negative_ixfx);
PX_SYNTAX_OPERATE_FUNCTION(PX_Syntax_opcode_ixfx_assign_ixfx);
PX_SYNTAX_OPERATE_FUNCTION(PX_Syntax_operate_ixfx_add_sub_mul_div);

#endif