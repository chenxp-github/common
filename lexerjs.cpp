#include "lexerjs.h"
#include "syslog.h"
#include "mem_tool.h"
#include "mem.h"

CLexerJs::CLexerJs()
{
	this->InitBasic();
}
CLexerJs::~CLexerJs()
{
	this->Destroy();
}
status_t CLexerJs::InitBasic()
{
	CLexer::InitBasic();
	m_NewLineEnd = false;
	m_LastNotEmptyType = TOKEN_UNKNOWN;
	m_MayBeRegularExpressionFollow = false;
	return OK;
}
status_t CLexerJs::Init()
{
	this->InitBasic();
	CLexer::Init();
	return OK;
}
status_t CLexerJs::Destroy()
{
	CLexer::Destroy();
	this->InitBasic();
	return OK;
}

#define return_token_type(type) do{\
	ret_type = type;\
	goto end;\
}while(0)\
	

int CLexerJs::NextToken(CFileBase *out)
{
	ASSERT(out);

	int ret_type = TOKEN_UNKNOWN;
	char ch = 0;

	out->SetSize(0);

	if(this->IsEnd())
	{
		return_token_type(TOKEN_UNKNOWN);
	}
	
	ch = this->Getc();
	
	if(ch == '/')
	{
		out->Putc(ch);
		int type = ReadCStyleComments(out);
		if(type == TOKEN_UNKNOWN && m_MayBeRegularExpressionFollow)
		{
			type = ReadRegularExpression(out);
			if(type != TOKEN_UNKNOWN)
				return_token_type(type);
			else
				return_token_type(TOKEN_PUNCTUATION);
		}
		else
		{
			return_token_type(type);
		}
	}
	else if(ch =='\"')
	{
		out->Putc(ch);
		this->ReadQuoteStr(out,'\\','\"');
		return_token_type(TOKEN_STRING);
	}
	else if(ch == '\'')
	{
		out->Putc(ch);
		this->ReadQuoteStr(out,'\\','\'');
		return_token_type(TOKEN_STRING);
	}
	else if(IS_DEC_NUMBER(ch))
	{
		out->Putc(ch);
		this->ReadNumber(out);
		return_token_type(TOKEN_NUMBER);
	}
	else if(ch == '_' || IS_ALPHA(ch))
	{
		out->Putc(ch);
		this->ReadWord(out);
		int key_type = this->IsKeyWord(out);

		if( key_type >= 0)
		{
			m_CurKeyWordType = key_type;
			return_token_type(TOKEN_KEY_WORD);
		}
		else
		{
			m_CurKeyWordType = -1;
			return_token_type(TOKEN_NORMAL_WORD);
		}
	}
	else if(CFileBase::IsEmptyChar(ch))
	{
		out->Putc(ch);
		if(ch == '\n')
		{
			m_NewLineEnd = true;
			return_token_type(TOKEN_EMPTY);
		}
		else
		{
			m_NewLineEnd = false;
			if(this->ReadEmptyNewLine(out) == 2)
				m_NewLineEnd = true;
			return_token_type(TOKEN_EMPTY);
		}
	}
	else if(ch & 0x80)
	{		
		out->Putc(' ');
		m_NewLineEnd = false;
		if(this->ReadEmptyNewLine(out) == 2)
			m_NewLineEnd = true;
		return_token_type(TOKEN_EMPTY);
	}
	else
	{
		out->Putc(ch);
		return_token_type(TOKEN_PUNCTUATION);
	}

end:
	if(ret_type != TOKEN_EMPTY)
	{
		m_MayBeRegularExpressionFollow = false;
		m_LastNotEmptyType = ret_type;
		if(ret_type == TOKEN_PUNCTUATION)
		{
			char ch = out->CharAt(0);
			m_MayBeRegularExpressionFollow = (ch != ')');
		}
	}
	return ret_type;
}

bool CLexerJs::IsEmptyNewLineEnd()
{
	return m_NewLineEnd;
}

