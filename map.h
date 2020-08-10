#ifndef __MAP_H
#define __MAP_H

#include "memstk.h"

class CMap{
public:
    int32_t __weak_ref_id;
    CMemStk *mKeyValues;
public:
    status_t Clear();
    status_t ReadString(const char *key, CMem *out, const char *defValue);
    status_t WriteString(const char *key, const char *str);
    bool ReadBool(const char *key, bool defValue);
    status_t WriteBool(const char *key, bool d);
    float ReadFloat(const char *key, float defValue);
    status_t WriteFloat(const char *key, float d);
    status_t WriteDouble(const char *key, double d);
    double ReadDouble(const char *key, double defValue);
    status_t WriteInt(const char *key, int i);
    int ReadInt(const char *key, int defValue);
    status_t ReadKeyValueRaw(const char *key, void **value, int32_t *size,int32_t *type);
    status_t ReadValueRaw(CMem *mem, void **value, int32_t *size, int32_t *type);
    CMem * SearchKey(const char *key);
    status_t WriteKeyValue(const char *key, void *value, int32_t size, int32_t type);
    status_t SetKeyValue(CMem *out, const char *key, void *value, int32_t size, int32_t type);
    CMap();
    virtual ~CMap();
    status_t InitBasic();
    status_t Init();
    status_t Destroy();
    status_t Copy(CMap *p);
    status_t Comp(CMap *p);
    status_t Print();
};

#endif
