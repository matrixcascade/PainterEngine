#ifndef PX_SYNTAX_DECLARE_TYPE_H
#define PX_SYNTAX_DECLARE_TYPE_H
#include "PX_Syntax.h"
// variable --- declare_array(abi)
//         |
//          --- declare_token_prefix(abi)
//         |
//          --- declare_prefix(abi)
//         |
//          --- size(int)
//         |
//          --- offset(int)
//         |
//          --- identifier(abi)
px_bool PX_Syntax_load_declare_variable(PX_Syntax* pSyntax);

#endif