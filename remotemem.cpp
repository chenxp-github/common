#include "RemoteMem.h"
#include "syslog.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRemoteMem::CRemoteMem()
{
    this->InitBasic();
}
CRemoteMem::~CRemoteMem()
{
    this->Destroy();
}
status_t CRemoteMem::InitBasic()
{
    this->hProcess = NULL;
    this->mSize = 0;
    return OK;
}
status_t CRemoteMem::Init()
{
    this->InitBasic();
    this->mPtr = NULL;
    return OK;
}
status_t CRemoteMem::Destroy()
{
    if(this->mPtr)
    {
        VirtualFreeEx(hProcess, this->mPtr, 0, MEM_RELEASE);
        this->mPtr = NULL;
    }

    if(this->hProcess)
    {
        CloseHandle(this->hProcess);
        this->hProcess = NULL;
    }

    this->InitBasic();
    return OK;
}

status_t CRemoteMem::Alloc(uint32_t size)
{
    ASSERT(this->mPtr == NULL);
    ASSERT(this->hProcess);
    this->mPtr = VirtualAllocEx(hProcess,NULL,size,MEM_COMMIT,PAGE_READWRITE);
    ASSERT(this->mPtr);
    this->mSize = size;
    return OK;
}

int_ptr_t CRemoteMem::Write(void *buf, uint32_t size)
{
    ASSERT(buf);
    ASSERT(this->hProcess && this->mPtr);
    return WriteProcessMemory(hProcess,this->mPtr,buf,size,NULL);
}

int_ptr_t CRemoteMem::Read(void *buf, uint32_t size)
{
    ASSERT(buf);
    ASSERT(this->hProcess && this->mPtr);
    return ReadProcessMemory(hProcess, this->mPtr, buf, size, NULL);
}

int CRemoteMem::OpenProcess(uint32_t pid)
{
    ASSERT(this->hProcess == NULL);
    hProcess=::OpenProcess(PROCESS_ALL_ACCESS,false,pid);
    ASSERT(hProcess);
    return OK;
}

status_t CRemoteMem::OpenProcessByHwnd(HWND hwnd)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    ASSERT(pid);
    return this->OpenProcess(pid);
}

char * CRemoteMem::GetRemotePtr()
{
    return (char*)this->mPtr;
}

int_ptr_t CRemoteMem::GetSize()
{
    return this->mSize;
}
