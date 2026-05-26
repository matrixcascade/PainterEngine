#ifndef PX_SYNTAX_IR_H
#include "PX_Syntax.h"
#include "PX_Syntax_const_float.h"
#include "PX_Syntax_const_int.h"
#include "PX_Syntax_const_string.h"
#include "PX_Syntax_const_float_list.h"
#include "PX_Syntax_const_int_list.h"
#include "PX_Syntax_const_string_list.h"
#include "PX_Syntax_eof.h"
#include "PX_Syntax_keyword.h"
#include "PX_Syntax_identifier.h"

//;@loc source_index source_offset
typedef struct
{
	px_dword ip;
	px_dword source_index;
	px_dword source_line;
}PX_Syntax_bin_map_to_source;

typedef struct
{
	px_dword ip;
	px_dword source_index;
	px_dword source_line;
}PX_Syntax_bin_map_to_ir;

typedef enum
{
	PX_SYNTAX_IR_SCAN_MODE_LABEL = 0,
	PX_SYNTAX_IR_SCAN_MODE_RDATA,
	PX_SYNTAX_IR_SCAN_MODE_TEXT
}PX_SYNTAX_IR_SCAN_MODE;


px_bool PX_Syntax_load_ir(PX_Syntax* pSyntax);
px_int PX_Syntax_Disassemble(const px_byte* payload, px_int reserved_size, px_char out_asm[32]);
#endif // !PX_SYNTAX_IR_H
