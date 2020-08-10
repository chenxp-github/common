#ifndef __JSONREADER_H
#define __JSONREADER_H

#include "memstk.h"

#define JSON_TYPE_ERROR             0
#define JSON_TYPE_BRACE_ENTER       1
#define JSON_TYPE_BRACE_LEAVE       2
#define JSON_TYPE_KEY               5
#define JSON_TYPE_VALUE             6

class CJsonReader{
public:
    CFileBase *iSrcFile;
    CMem *mValue;
    CMemStk  *mNameStack;
public:
    int GetValueType(CFileBase *value);
    const char *ValueTypeToString(int valueType);
    int GetNextValueType(CFileBase *file);
    bool IsPreTopBrace();
    static status_t JsonToXml(CFileBase *json,CFileBase *xml);
    CMem * GetValue();
    bool IsTopBrace();
    status_t PopName();
    CMem *GetTopName();
    status_t Parse();
    CJsonReader();
    virtual ~CJsonReader();
    status_t Init();
    status_t Destroy();
    status_t InitBasic();
    status_t Skip(const char *str);
    status_t GetCurPath(CFileBase *file);
    status_t SetSrcFile(CFileBase *iFile);
    char Getc_NotWhite(CFileBase *file);

};

#endif
