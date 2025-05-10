#ifndef PX_SYNTAX_DEFINE_STRUCT_H
#define PX_SYNTAX_DEFINE_STRUCT_H
#include "PX_Syntax.h"
//define_struct  -- struct_name -- value(string)
//				|
//				|-- member_count(int)
//				|
//				|-- size(int)
//				|
//				|-- [](variable abi)

px_bool PX_Syntax_load_define_struct(PX_Syntax* pSyntax);


#endif