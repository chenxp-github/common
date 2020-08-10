#ifndef __S2_LEXERJS_H
#define __S2_LEXERJS_H

#include "cruntime.h"
#include "filebase.h"
#include "lexer.h"

class CLexerJs:public CLexer{
public:
	bool m_NewLineEnd;
	int m_LastNotEmptyType;
	bool m_MayBeRegularExpressionFollow;
public:
	bool IsEmptyNewLineEnd();
	int NextToken(CFileBase *out);
	CLexerJs();
	virtual ~CLexerJs();
	status_t InitBasic();
	status_t Init();
	status_t Destroy();
};

#endif
