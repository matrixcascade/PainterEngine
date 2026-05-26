#ifndef PX_SYNTAX_BASE_OPCODE_H
#define PX_SYNTAX_BASE_OPCODE_H
#include "PX_Syntax.h"
#include "PX_Syntax_base_operate.h"


px_bool PX_Syntax_ExecuteOpcode(PX_Syntax* pSyntax, px_int opcode_index);
px_bool PX_Syntax_load_base_opcode(PX_Syntax* pSyntax);
#endif