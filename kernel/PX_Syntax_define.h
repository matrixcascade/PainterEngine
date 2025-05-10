// define -- define_identifier --- value
//       |
//        -- define_parameters --- identifier[] --- identifier[] --- value
//       |                    |
//       |                     --- count --- value
//        -- define_content --- value
#ifndef PX_SYNTAX_DEFINE_H
#define PX_SYNTAX_DEFINE_H
#include "PX_Syntax.h"

px_bool PX_Syntax_load_define(PX_Syntax* pSyntax);
#endif // !PX_SYNTAX_IDENTIFIER
