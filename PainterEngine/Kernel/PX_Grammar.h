#ifndef __PX_GRAMMAR_H
#define __PX_GRAMMAR_H
#include "PX_ScriptCore.h"
//////////////////////////////////////////////////////////////////////////

typedef struct __PX_ASM_GRAMMAR
{
	px_lexer *lexer;
	px_memorypool *mp;
	px_bool   bSkipSpacer;
	px_bool   bSkipNewline;
	px_bool   bLowercase;
	px_vector V_SentenceMatcher;
}PX_ASM_Grammar;


#endif
