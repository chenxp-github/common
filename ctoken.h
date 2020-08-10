// CToken.h: interface for the CCToken class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CTOKEN_H__C053DE38_C57E_477C_BC61_3CB76A8440DA__INCLUDED_)
#define AFX_CTOKEN_H__C053DE38_C57E_477C_BC61_3CB76A8440DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "memfile.h"
#include "mem.h"
#include "memstk.h"

enum _ctype{
    CTYPE_ERR,
    CTYPE_NORMAL,
    CTYPE_STRING,
    CTYPE_STRING_SINGLE,
    CTYPE_SINGLE_COMMENT,
    CTYPE_MULTI_COMMENT,
    CTYPE_PUNC,
    CTYPE_PREPROCESS,
    CTYPE_WHITE,
};

class CCToken{
public:
    CFileBase *i_file_src;
    int32_t line;
    CMemFile *mf_value;
    const char **ignore_words;
    int32_t ignore_words_size;
public: 
    status_t SetIgnoreWords(const char **words, int32_t size);
    status_t SetIgnoreTypeJava();
    status_t SetIgnoreTypeC();
    status_t GetStdType(const char *type, CFileBase *out);
    status_t GetStdType(CFileBase *type, CFileBase *out);
    status_t SimplifyTypeName(CFileBase *type, CFileBase *out);
    bool IsIgnoreWord(CFileBase *word);
    int32_t Split_IF_Expression(CFileBase *source, CFileBase *part1, CFileBase *part2);
    char Getc();
    status_t GetNextTag();
    status_t IsEnd();
    char UnGetc();
    status_t SplitToBlocks(CMemStk *stk);
    status_t ReadExpression(CFileBase *file,status_t *has_bracket);
    static int32_t ParseFuncBlock(CFileBase *block, CFileBase *name,CFileBase *ret_type,CFileBase *param);
    static int32_t IsPreProcessBlock(CFileBase *file);
    static int32_t IsCommentBlock(CFileBase *file);
    static int32_t IsKeyWord(const char *str);
    static int32_t StripBracket(CFileBase *src,CFileBase *dst);
    static int32_t IsFuncBlock(CFileBase *block);
    static int32_t IsBracketBlock(CFileBase *block);
    static bool IsIgnoreWord(const char *tab[],int tabSize,CFileBase *word);
    static bool IsStaticType(CFileBase *type);
    static bool IsVoidType(CFileBase *type);
    static bool IsArrayType(CFileBase *type);
    static bool IsVoidRetType(CFileBase *ret);
    static status_t ParseParams(CFileBase *line, CMemStk *params);
    CCToken();
    ~CCToken();
    status_t InitBasic();
    status_t Init();
    status_t  Destroy();
};

#endif // !defined(AFX_CTOKEN_H__C053DE38_C57E_477C_BC61_3CB76A8440DA__INCLUDED_)
