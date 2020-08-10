#include "mytime.h"
#include "syslog.h"
#include "mem_tool.h"
#include "mem.h"

#define YEAR0 1900

CTime::CTime()
{
    this->InitBasic();  
}
CTime::~CTime()
{
    this->Destroy();
}
int CTime::Comp(CTime *_p)
{
    ASSERT(_p);
    int c;
    c = mYear - _p->mYear;
    if(c!=0)return c;
    c = mMonth - _p->mMonth;
    if(c!=0)return c;
    c=mDay - _p->mDay;
    if(c!=0)return c;
    c = this->mHour - _p->mHour;
    if(c!=0)return c;
    c = this->mMinute - _p->mMinute;
    if(c!=0)return c;
    c = this->mSecond - _p->mSecond;
    if(c!=0)return c;
    c = this->mMilisec - _p->mMilisec;
    if(c!=0)return c;
    return 0;
}
status_t CTime::InitBasic()
{
    this->mYear = 0;
    this->mMonth = 0;
    this->mDay = 0;
    this->mHour = 0;
    this->mMinute = 0;
    this->mSecond = 0;
    this->mMilisec = 0;
    return OK;
}
status_t CTime::Init()
{
    this->Destroy();
    return OK;
}
status_t CTime::Destroy()
{
    this->InitBasic();
    return OK;
}
status_t CTime::Copy(CTime *_p)
{
    ASSERT(_p);
    if(_p == this)return OK;
    this->mYear = _p->mYear;
    this->mMonth = _p->mMonth;
    this->mDay = _p->mDay;
    this->mHour = _p->mHour;
    this->mMinute = _p->mMinute;
    this->mSecond = _p->mSecond;
    this->mMilisec = _p->mMilisec;
    return OK;
}
status_t CTime::Print(CFileBase *_buf)
{
    _buf->Log("%04d-%02d-%02d %02d:%02d:%02d",
        mYear,mMonth,mDay,mHour,mMinute,mSecond);
    return OK;
}
status_t CTime::Clear()
{
    return this->InitBasic();
}

int CTime::GetYear()
{
    return this->mYear;
}
int CTime::GetMonth()
{
    return this->mMonth;
}
int CTime::GetDay()
{
    return this->mDay;
}
int CTime::GetHour()
{
    return this->mHour;
}
int CTime::GetMinute()
{
    return this->mMinute;
}
int CTime::GetSecond()
{
    return this->mSecond;
}
int CTime::GetMilisec()
{
    return this->mMilisec;
}
status_t CTime::SetYear(int _year)
{
    this->mYear = _year;
    return OK;
}
status_t CTime::SetMonth(int _month)
{
    this->mMonth = _month;
    return OK;
}
status_t CTime::SetDay(int _day)
{
    this->mDay = _day;
    return OK;
}
status_t CTime::SetHour(int _hour)
{
    this->mHour = _hour;
    return OK;
}
status_t CTime::SetMinute(int _minute)
{
    this->mMinute = _minute;
    return OK;
}
status_t CTime::SetSecond(int _second)
{
    this->mSecond = _second;
    return OK;
}
status_t CTime::SetMilisec(int _milisec)
{
    this->mMilisec = _milisec;
    return OK;
}

status_t CTime::GetTime(time_t _lt)
{
    struct tm *local;
    local=localtime(&_lt);
    if(local == NULL)
        return ERROR;
    this->mYear=local->tm_year+YEAR0;
    this->mMonth=local->tm_mon+1;
    this->mDay=local->tm_mday;
    this->mHour=local->tm_hour;
    this->mMinute=local->tm_min;
    this->mSecond=local->tm_sec;
    this->mMilisec=0;
    return OK;
}

status_t CTime::GetTime()
{
#if HAVE_WINDOWS_H
    SYSTEMTIME st;
    GetLocalTime(&st);
    this->mYear=st.wYear;
    this->mMonth=st.wMonth;
    this->mDay=st.wDay;
    this->mHour=st.wHour;
    this->mMinute=st.wMinute;
    this->mSecond=st.wSecond;
    this->mMilisec = st.wMilliseconds;
#else
    return this->GetTime(time(NULL));
#endif
    return OK;
}

time_t CTime::ToLong()
{
    struct tm local;
    local.tm_year=mYear-YEAR0;
    local.tm_mon=mMonth-1;
    local.tm_mday=mDay;
    local.tm_hour=mHour;
    local.tm_min=mMinute;
    local.tm_sec=mSecond;
    return mktime(&local);
}

//format like 20140823
status_t CTime::ParseDate(uint32_t date)
{
    this->Clear();
    
    this->mYear = date/10000;
    this->mMonth = date/100 - mYear*100;
    this->mDay = date - mMonth*100 - mYear*10000;
    
    return OK;
}
status_t CTime::ClearDate()
{
    this->mYear = 0;
    this->mMonth = 0;
    this->mDay = 0;
    return OK;
}
status_t CTime::ClearTime()
{
    this->mHour = 0;
    this->mMinute = 0;
    this->mSecond = 0;
    this->mMilisec = 0;
    return OK;
}

#define FORMAT_NONE 0
#define FORMAT_DATA 1
#define FORMAT_TIME 2

static int string_format(CMem *mem)
{
    int len = mem->StrLen();
    int d = 0,t=0;
    const char *str = mem->CStr();

    for(int i = 0; i< len; i++)
    {
        if(str[i]=='-')
        {
            d++;
            continue;
        }
        if(str[i] == ':')
        {
            t++;
            continue;
        }
        
        if(str[i] < '0'|| str[i] >'9')
            return FORMAT_NONE; 
    }
    if(d >= 1)return FORMAT_DATA;
    if(t >= 1)return FORMAT_TIME;
    return FORMAT_NONE;
}

//2014-10-23 10:24:22
status_t CTime::ParseString(const char *_str)
{
    ASSERT(_str);
    CMem str(_str);

    LOCAL_MEM(mem);
    LOCAL_MEM(buf);

    str.SetSplitChars(" \t");
    str.Seek(0);

    for(int i = 0; i < 2; i++)
    {
        str.ReadString(&buf);
        buf.Seek(0);
        if(string_format(&buf) == FORMAT_DATA)
        {
            buf.SetSplitChars("- \t");
            buf.ReadString(&mem);
            this->mYear = atoi(mem.CStr());
            buf.ReadString(&mem);
            this->mMonth = atoi(mem.CStr());
            buf.ReadString(&mem);
            this->mDay = atoi(mem.CStr());
        }
        else if(string_format(&buf) == FORMAT_TIME)
        {
            buf.SetSplitChars(": \t");
            buf.ReadString(&mem);
            this->mHour = atoi(mem.CStr());
            buf.ReadString(&mem);
            this->mMinute = atoi(mem.CStr());
            buf.ReadString(&mem);
            this->mSecond = atoi(mem.CStr());
        }
    }

    return OK;
}

