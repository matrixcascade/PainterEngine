
#ifndef PX_SYNTAX_FUNCTION_H
#define PX_SYNTAX_FUNCTION_H
#include "PX_Syntax.h"

// define_function --- function_return_type(abi type)
//				  |
//				   --- function_name(identifier abi)
//				  |
//                 --- param_count(int)
// 				  |
//				   --- [](parameters abi variable)

// call_function   --- value(string)
//                |
//                 --- check_param_count(int)
//				  |
//				   --- define_function(abi define_function)

px_bool PX_Syntax_load_function(PX_Syntax* pSyntax);
#endif // !PX_SYNTAX_FUNCTION_H
