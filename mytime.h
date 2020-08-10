#ifndef __MY_TIME_H
#define __MY_TIME_H

#include "cruntime.h"
#include "filebase.h"

class CTime{
private:
    int mYear;
    int mMonth;
    int mDay;
    int mHour;
    int mMinute;
    int mSecond;
    int mMilisec;
public:
    status_t ParseString(const char *str);
    status_t ClearTime();
    status_t ClearDate();
    status_t Clear();
    status_t ParseDate(uint32_t date);
    time_t ToLong();
    status_t GetTime();
    status_t GetTime(time_t _lt);
    CTime();
    virtual ~CTime();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CTime *_p);
    int Comp(CTime *_p);
    status_t Print(CFileBase *_buf);
    int GetYear();
    int GetMonth();
    int GetDay();
    int GetHour();
    int GetMinute();
    int GetSecond();
    int GetMilisec();
    status_t SetYear(int _year);
    status_t SetMonth(int _month);
    status_t SetDay(int _day);
    status_t SetHour(int _hour);
    status_t SetMinute(int _minute);
    status_t SetSecond(int _second);
    status_t SetMilisec(int _milisec);
};

#endif
