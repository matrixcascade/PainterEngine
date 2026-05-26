#ifndef PX_SYNTAX_EXPR_H
#define PX_SYNTAX_EXPR_H
#include "PX_Syntax.h"
#include "PX_Syntax_base_opcode.h"
#include "PX_Syntax_base_operate.h"

px_bool PX_Syntax_load_expr(PX_Syntax* pSyntax);

//oprand
px_abi* PX_Syntax_PushOprand(PX_Syntax* pSyntax, const px_char type[], px_int type_size, PX_SYNTAX_OPRAND_FROM datafrom);
px_abi* PX_Syntax_PushVariableOprand(PX_Syntax* pSyntax, px_abi* pvariableabi);
px_int  PX_Syntax_GetOprandStackIndex(PX_Syntax* pSyntax, px_int index);
px_abi* PX_Syntax_GetOprandStack(PX_Syntax* pSyntax, px_int index);
px_void PX_Syntax_PopLastOprand(PX_Syntax* pSyntax);
px_void PX_Syntax_PopSecondLastOprand(PX_Syntax* pSyntax);



#endif