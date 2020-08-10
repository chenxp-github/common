// ImgFile.cpp: implementation of the CImgFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImgFile.h"
#include "misc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImgFile::CImgFile()
{
    this->InitBasic();
}
CImgFile::~CImgFile()
{
    this->Destroy();
}
int CImgFile::InitBasic()
{
    this->i_file = NULL;
    return OK;
}
int CImgFile::Init()
{
    this->InitBasic();
    return OK;
}
int  CImgFile::Destroy()
{
    this->InitBasic();
    return OK;
}
int  CImgFile::Print()
{
    PD(this->img_info.header_size);
    PX(this->img_info.data_start);
    PX(this->img_info.data_end);
    PD(this->img_info.data_size);
    PD(this->img_info.data_off);
    PX(this->img_info.const_start);
    PX(this->img_info.const_end);
    PD(this->img_info.const_size);
    PD(this->img_info.const_off);
    PX(this->img_info.bss_start);
    PX(this->img_info.bss_end);
    PD(this->img_info.bss_size);
    PD(this->img_info.bss_off);
    PX(this->img_info.text_start);
    PX(this->img_info.text_end);
    PD(this->img_info.text_size);
    PD(this->img_info.text_off);

    return TRUE;
}

int CImgFile::LoadFile(CFileBase *pfile)
{
    CXmlNode *px;
    CXml *pxml_header,xml_header;
    int header_size;
    char buf[LBUF_SIZE];
    CMem mem_buf;

    ASSERT(this->i_file == NULL);
    ASSERT(pfile);
    
    mem_buf.Init();
    mem_buf.SetP(buf,LBUF_SIZE);

    pxml_header = &xml_header;
    pxml_header->Init();

    this->i_file = pfile;

    header_size = CHeaderFile::LoadHeader(this->i_file,pxml_header);
    ASSERT(header_size > 0);

    this->img_info.header_size = header_size;
    ////////////////////////////////////////////////////
    px = pxml_header->GetNodeByPath("/Img/data");
    ASSERT(px);
    
    px->GetAttrib("start",&mem_buf);
    this->img_info.data_start = CMisc::hex2dec_32(mem_buf.p);
    px->GetAttrib("size",&mem_buf);
    this->img_info.data_size = CMisc::hex2dec_32(mem_buf.p);
    px->GetAttrib("offset",&mem_buf);
    this->img_info.data_off = CMisc::hex2dec_32(mem_buf.p) + header_size;
    ////////////////////////////////////////////////////
    px = pxml_header->GetNodeByPath("/Img/text");
    ASSERT(px);
    
    px->GetAttrib("start",&mem_buf);
    this->img_info.text_start = CMisc::hex2dec_32(mem_buf.p);
    px->GetAttrib("size",&mem_buf);
    this->img_info.text_size = CMisc::hex2dec_32(mem_buf.p);
    px->GetAttrib("offset",&mem_buf);
    this->img_info.text_off = CMisc::hex2dec_32(mem_buf.p) + header_size;
    ////////////////////////////////////////////////////
    px = pxml_header->GetNodeByPath("/Img/const");
    ASSERT(px);
    
    px->GetAttrib("start",&mem_buf);
    this->img_info.const_start = CMisc::hex2dec_32(mem_buf.p);
    px->GetAttrib("size",&mem_buf);
    this->img_info.const_size = CMisc::hex2dec_32(mem_buf.p);
    px->GetAttrib("offset",&mem_buf);
    this->img_info.const_off = CMisc::hex2dec_32(mem_buf.p) + header_size;
    ////////////////////////////////////////////////////
    px = pxml_header->GetNodeByPath("/Img/bss");
    ASSERT(px);

    px->GetAttrib("start",&mem_buf);
    this->img_info.bss_start = CMisc::hex2dec_32(mem_buf.p);
    px->GetAttrib("size",&mem_buf);
    this->img_info.bss_size = CMisc::hex2dec_32(mem_buf.p);
    px->GetAttrib("offset",&mem_buf);
    this->img_info.bss_off = CMisc::hex2dec_32(mem_buf.p) + header_size;
    ////////////////////////////////////////////////////
    px = pxml_header->GetNodeByPath("/Img/xcu");
    ASSERT(px);
    
    px->GetAttrib("start",&mem_buf);
    this->img_info.xcu_start = CMisc::hex2dec_32(mem_buf.p);
    px->GetAttrib("size",&mem_buf);
    this->img_info.xcu_size = CMisc::hex2dec_32(mem_buf.p);
    px->GetAttrib("offset",&mem_buf);
    this->img_info.xcu_off = CMisc::hex2dec_32(mem_buf.p) + header_size;
    ///////////////////////////////////////////////////
    //增加效率而加入少许冗余
    this->img_info.const_end = this->img_info.const_start + this->img_info.const_size -1;
    this->img_info.text_end = this->img_info.text_start + this->img_info.text_size -1;
    this->img_info.bss_end = this->img_info.bss_start + this->img_info.bss_size -1;
    this->img_info.data_end = this->img_info.data_start + this->img_info.data_size -1;
    this->img_info.xcu_end =  this->img_info.xcu_start + this->img_info.xcu_size -1;

    mem_buf.Destroy();

    return OK;
}

int CImgFile::WriteToVMem(CVMem *vm)
{
    ASSERT(vm);

    long i,ret = OK;

    this->i_file->Seek(this->img_info.data_off);
    for(i = this->img_info.data_start; i <= this->img_info.data_end && ret != ERROR; i++)
    {
        ret = vm->Write8Mem_Out(i,this->i_file->Getc());
    }
    
    this->i_file->Seek(this->img_info.text_off);
    for(i = this->img_info.text_start; i <= this->img_info.text_end && ret != ERROR; i++)
    {
        ret = vm->Write8Mem_Out(i,this->i_file->Getc());
    }

    this->i_file->Seek(this->img_info.bss_off);
    for(i = this->img_info.bss_start; i <= this->img_info.bss_end && ret != ERROR; i++)
    {
        ret = vm->Write8Mem_Out(i,this->i_file->Getc());
    }
    
    this->i_file->Seek(this->img_info.xcu_off);
    for(i = this->img_info.xcu_start; i <= this->img_info.xcu_end && ret != ERROR; i++)
    {
        ret = vm->Write8Mem_Out(i,this->i_file->Getc());
    }

    this->i_file->Seek(this->img_info.const_off);
    for(i = this->img_info.const_start; i <= this->img_info.const_end && ret != ERROR; i++)
    {
        ret = vm->Write8Mem_Out(i,this->i_file->Getc());
    }

    if(ret == ERROR)
    {
        LOG("err while load img file to vmem!\n");
    }

    return ret;
}
