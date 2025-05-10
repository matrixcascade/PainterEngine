#ifndef PX_SYNTAX_DECLARE_TOKEN_SUFFIX_H
#define PX_SYNTAX_DECLARE_TOKEN_SUFFIX_H
#include "PX_Syntax.h"
//
//declare_array   --- d(int)
//               |
//                --- [d](int)
//				 |
//				  --- count(int)
px_bool PX_Syntax_load_declare_token_suffix(PX_Syntax* pSyntax);


#endif