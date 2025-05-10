#ifndef PX_SYNTAX_BASE_TYPE_H
#define PX_SYNTAX_BASE_TYPE_H
#include "PX_Syntax.h"

// type -- type_define --- mnemonic(string)
//			          |
//					   --- type(string) ["struct","base"]
//					  |
//					   --- size(int)
//                    |
//					   --- define_struct  -- struct_name(string)
//					  				     |
//					  				     |-- member_count(int)
//					  				     |
//					  				     |-- size(int)
//					  				     |
//					  				     |-- [](variable abi)


px_bool PX_Syntax_load_base_type(PX_Syntax* pSyntax);


#endif