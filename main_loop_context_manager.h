#ifndef __MAIN_LOOP_CONTEXT_MANAGER_H
#define __MAIN_LOOP_CONTEXT_MANAGER_H

#include "filebase.h"
#include "closure.h"
#include "main_loop_context.h"



class MainLoopContextManager_HashEntry{

public:
    CMainLoopContext *m_RawPtr;
    MainLoopContextManager_HashEntry *next;
public:
    MainLoopContextManager_HashEntry();
    ~MainLoopContextManager_HashEntry();
    CMainLoopContext *get();
    status_t set(CMainLoopContext *node);
};

class CMainLoopContextManager{

private:
    MainLoopContextManager_HashEntry **m_Data;
    int m_Capacity;
    int m_Size;
public:    
    status_t Clear();
    status_t DiscardAll();
    bool IsEmpty();
    int GetSize();
    int GetCapacity();
    status_t Del(CMainLoopContext *key);
    MainLoopContextManager_HashEntry* Remove(CMainLoopContext *key);
    status_t EnumAll(CClosure *closure);        
    CMainLoopContext* Get(CMainLoopContext *key);    
    status_t PutPtr(CMainLoopContext *ptr);
    int HashCode(CMainLoopContext *hashentry);
    status_t ToArray(CMainLoopContext *arr[], int *len);

    static int HashCode(CMainLoopContext *hashentry,int capacity);
    static bool Equals(CMainLoopContext *hashentry1, CMainLoopContext *hashentry2);    

    CMainLoopContextManager();
    virtual ~CMainLoopContextManager();
    status_t InitBasic();
    status_t Init(int capacity=1024);
    status_t Destroy();

    status_t Del(JNIEnv *env);
    CMainLoopContext *Get(JNIEnv *env);
};



#endif


