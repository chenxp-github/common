#include "taskmd5sum.h"
#include "mem.h"
#include "encoder.h"

#define STEP_MD5_BEGIN      1
#define STEP_MD5_UPDATE     2
#define STEP_MD5_FINAL      3

int CTaskMd5Sum::EVENT_FINISH = 1;
status_t CTaskMd5Sum::ERROR_NONE = 0;

CTaskMd5Sum::CTaskMd5Sum()
{
    this->InitBasic();
}
CTaskMd5Sum::~CTaskMd5Sum()
{
    this->Destroy();
}
status_t CTaskMd5Sum::InitBasic()
{
    CTask::InitBasic();
    this->mCache = NULL;
    this->iSrcFile = NULL;
    this->A = 0;
    this->B = 0;
    this->C = 0;
    this->D = 0;
    this->mStep = 0;
    crt_memset(buf,0,sizeof(buf));
    return OK;
}
status_t CTaskMd5Sum::Init(CTaskMgr *mgr)
{
    this->InitBasic();
    CTask::Init(mgr);
    NEW(this->mCache,CMem);
    this->mCache->Malloc(1024*1024);

    return OK;
}
status_t CTaskMd5Sum::Destroy()
{
    DEL(this->mCache);
    CTask::Destroy();
    this->InitBasic();
    return OK;
}
status_t CTaskMd5Sum::Copy(CTaskMd5Sum *p)
{
    if(this == p)
        return OK;
    CTask::Copy(p);
    //add your code
    return OK;
}
status_t CTaskMd5Sum::Comp(CTaskMd5Sum *p)
{
    return 0;
}
status_t CTaskMd5Sum::Print()
{
    CTask::Print();
    //add your code
    return TRUE;
}
status_t CTaskMd5Sum::OnTimer(int32_t interval)
{
    ASSERT(this->iSrcFile);

    if(this->mStep == STEP_MD5_BEGIN)
    {
        A=0x67452301,B=0xefcdab89,C=0x98badcfe,D=0x10325476;
        this->iSrcFile->Seek(0);
        crt_memset(buf,0,64);
        this->mStep = STEP_MD5_UPDATE;
    }
    else if(this->mStep == STEP_MD5_UPDATE)
    {
        this->mCache->SetSize(0);
        int_ptr_t rs = this->iSrcFile->Read(this->mCache->GetRawBuf(),(int_ptr_t)(this->mCache->GetMaxSize()));
        
        if(rs <= 0)
        {
            this->mStep = STEP_MD5_FINAL;
            return OK;
        }

        this->mCache->SetSize(rs);
        this->mCache->Seek(0);

        fsize_t len = this->mCache->GetSize();
        this->mCache->Read(buf,64);
        for(int32_t i=0;i<(len>>6);i++)
        {      
            CEncoder::md5_core(&A,&B,&C,&D,buf);
            crt_memset(buf,0,64);
            this->mCache->Read(buf,64);
        }
    }
    else if(this->mStep == STEP_MD5_FINAL)
    {
        int_ptr_t len = (int_ptr_t)(this->iSrcFile->GetSize());
        ((uint8_t*)buf)[len&63]=128;
        if((len&63)>55)
        {
            CEncoder::md5_core(&A,&B,&C,&D,buf);
            crt_memset(buf,0,64);
        }

        uint32_t flen[2];
        flen[1]=len >> 29;
        flen[0]= (len&(0x20000000-1))<<3;
        crt_memcpy(buf+14,flen,8);
        CEncoder::md5_core(&A,&B,&C,&D,buf);

        this->Stop(ERROR_NONE);
    }

    return OK;
}
const char * CTaskMd5Sum::ErrorToString(int32_t err)
{
    if(err == ERROR_NONE)
        return "none";
    return "unknown error";
}
status_t CTaskMd5Sum::ReportError(int32_t err)
{
    LOCAL_MEM(mem);

    mem.Puts("CTaskMd5Sum ");
    mem.Printf("(%d) ",this->GetID());

    if(err != ERROR_NONE)
    {
        mem.Puts("exit with error:");
        mem.Puts(this->ErrorToString(err));     
    }
    else 
    {
        mem.Puts("exit successfully!");
    }

    if(err != ERROR_NONE)
    {
        LOG("%s\n",mem.CStr());
    }
    return OK;
}
status_t CTaskMd5Sum::Start()
{
    this->mStep = STEP_MD5_BEGIN;
    this->Resume();
    return OK;
}
status_t CTaskMd5Sum::Stop(status_t err)
{
    this->Quit();
    this->ReportError(err);

    LOCAL_MEM(mem);
    this->GetStringResult(&mem);

    this->SetCallbackParam(1,(void*)err);
    this->SetCallbackParam(2,(void*)mem.CStr());
    this->RunCallback(EVENT_FINISH);
    return OK;
}

status_t CTaskMd5Sum::SetSrcFile(CFileBase *iFile)
{
    ASSERT(iFile);
    this->iSrcFile = iFile;
    return OK;
}

status_t CTaskMd5Sum::GetStringResult(CFileBase *out)
{
    uint32_t v[8];

    v[0] = (A<<24)|((A<<8)&0xff0000)|((A>>8)&0xff00)|(A>>24);
    v[1] = (B<<24)|((B<<8)&0xff0000)|((B>>8)&0xff00)|(B>>24);
    v[2] = (C<<24)|((C<<8)&0xff0000)|((C>>8)&0xff00)|(C>>24);
    v[3] = (D<<24)|((D<<8)&0xff0000)|((D>>8)&0xff00)|(D>>24);

    out->SetSize(0);
    out->Printf("%08x%08x%08x%08x",v[0],v[1],v[2],v[3]);
    return OK;
}
