#ifndef __S2_LEXER_H
#define __S2_LEXER_H

#include "cruntime.h"
#include "filebase.h"
#include "memfile.h"

#define IS_DEC_NUMBER(ch) (ch>='0' && ch<='9')
#define IS_ALPHA(ch) ((ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z'))
#define IS_WHITE(ch) CFileBase::IsEmptyChar(ch)
#define IS_PUNCTUATION(ch) ((!IS_DEC_NUMBER(ch)) && (!IS_ALPHA(ch)) && (!IS_WHITE(ch)))

typedef int (*FUNC_IS_USER_KEY_WORD)(CFileBase *name);

typedef struct{
	const char *name;
	int type;
}KeyWord;

typedef struct{
	fsize_t file_pos;
	int line_number;
}LexContext;

typedef struct{
	fsize_t offset;
	fsize_t size;
}FileContext;

class CLexer{
public:
	enum{
		TOKEN_UNKNOWN = 0,
		TOKEN_MULTI_LINE_COMMENTS,
		TOKEN_SINGLE_LINE_COMMENTS,
		TOKEN_PUNCTUATION,
		TOKEN_PREPROCESS,
		TOKEN_STRING,
		TOKEN_NUMBER,
		TOKEN_NORMAL_WORD,
		TOKEN_EMPTY,
		TOKEN_KEY_WORD,
		TOKEN_XML_TAG,
		TOKEN_XML_ATTRIBUTE,
		TOKEN_XML_NORMAL_TEXT,
		TOKEN_EXEC_STRING,
		TOKEN_REGULAR_EXPRESSION,
	};

	CMemFile m_TmpFile;
	CFileBase *i_SrcFile;
	int m_CurLine;
	KeyWord *i_KeyWords;
	int m_CurState;
	int m_CurKeyWordType;
	FUNC_IS_USER_KEY_WORD m_FuncIsUserKeyWord;
public:
	static status_t RestoreFileContext(CFileBase *file, FileContext *ctx);
	static status_t SaveFileContext(CFileBase *file, FileContext *ctx);
	status_t ReadRegularExpression(CFileBase *out);
	static status_t CorrectInvalidChar(CFileBase *str);
	status_t ReadEmptyNewLine(CFileBase *out);
	status_t GetCurKeyWordType();
	status_t ReportError();
	static bool IsUpper(CFileBase *name);
	status_t Reset();
	status_t SetUserKeyWordFunc(FUNC_IS_USER_KEY_WORD func);
	int IsUserKeyWord(CFileBase *name);
	status_t ReadWord(CFileBase *out);
	status_t ReadEmpty(CFileBase *out);
	status_t RestoreContext(LexContext *context);
	status_t SaveContext(LexContext *context);
	status_t ReadNumber(CFileBase *out);
	status_t ReadQuoteStr(CFileBase *file,char escape_char,char end_char);
	int ReadCStyleComments(CFileBase *out);
	bool IsEnd();
	int GetCurrentLine();
	status_t SetCurrentState(int state);
	int CurrentState();
	int IsKeyWord(CFileBase *name); //return type
	status_t SetKeyWordTable(const KeyWord *table);
	status_t LoadFile(CFileBase *file);
	status_t LoadFile(const char *filename);
	char UnGetc();
	char Getc();
	CLexer();
	virtual ~CLexer();
	status_t InitBasic();
	status_t Init();
	status_t Destroy();
};

#endif

