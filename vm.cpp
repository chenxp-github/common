// Vm.cpp: implementation of the CVm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Vm.h"
#include "misc.h"
#include "enums.h"
#include "reg.h"
#include "asmmisc.h"

//#define _STEP
//#define _DEASM
//#define _PRTREG

#define M_PI 3.14159265358979323846

#define FLAG_BIT_CF     0
#define FLAG_BIT_PF     2
#define FLAG_BIT_AF     4
#define FLAG_BIT_ZF     6
#define FLAG_BIT_SF     7
#define FLAG_BIT_TF     8
#define FLAG_BIT_IF     9
#define FLAG_BIT_DF     10
#define FLAG_BIT_OF     11
#define FLAG_BIT_NF     14
#define FLAG_BIT_RF     16
#define FLAG_BIT_VM     17
#define FLAG_BIT_AC     18

#define FPU_STATUS_BIT_C0 8
#define FPU_STATUS_BIT_C1 9
#define FPU_STATUS_BIT_C2 10
#define FPU_STATUS_BIT_C3 14

int CVm::do_mov()
{
    _RegVal val,opnd2;
    int ret;

    ret = this->GetOpnd_Common(0,&val,1);
    this->GetOpnd2(&opnd2);
    this->SetDesOpnd1Val(ret,&val,&opnd2);

    return OK;
}
int CVm::do_push()
{   
    _RegVal val;
    int bytes;

    bytes = this->GetOpnd1(&val);
    ASSERT(bytes == 4);

    this->PushDWord(val.val_32);

    return OK;
}
int CVm::do_pop()
{
    _RegVal val,r;
    int bytes;
    
    bytes = this->GetOpnd_Common(0,&val,1);
    r.val_64 = this->PopDWord();

    this->SetDesOpnd1Val(bytes,&val,&r);

    return OK;
}
int CVm::do_call()
{
    _RegVal val;    

    this->GetOpnd1(&val);

    this->PushDWord(this->GetEIP());
    this->SetEIP(val.val_32);

    return OK;
}
int CVm::do_lea()
{
    int type;
    _RegVal val,addr2;

    type = this->GetOpnd_Common(0,&val,1);
    this->GetOpnd_Common(1,&addr2,1); //lea 需要传送地址

    this->SetDesOpnd1Val(type,&val,&addr2);
    
    return OK;
}
int CVm::do_ret()
{
    _RegVal opnd1;
    long eip;

    eip = this->PopDWord(); 

    if(this->GetCurOpndNum() == 1)
    {
        this->GetOpnd1(&opnd1);
        //调整堆栈
        this->reg_index[REG_ESP]->val.val_32 += opnd1.val_32;
    }

    this->SetEIP(eip);

    return OK;
}
int CVm::do_test()
{
    _RegVal opnd1,opnd2,result;
    
    this->GetOpnd1(&opnd1);
    this->GetOpnd2(&opnd2);

    result.val_32 = opnd1.val_32 & opnd2.val_32;

    this->SetFlagBit(FLAG_BIT_ZF,result.val_32 == 0);
    this->SetFlagBit(FLAG_BIT_SF,(int)(result.val_32) < 0);
    this->SetFlagBit(FLAG_BIT_CF,0);
    this->SetFlagBit(FLAG_BIT_OF,0);

    return OK;
}
int CVm::do_xor()
{   
    _RegVal opnd1,val,opnd2,result;
    long type,eip;

    eip = this->GetEIP();
    this->GetOpnd1(&opnd1);
    this->SetEIP(eip);

    type = this->GetOpnd_Common(0,&val,1);
    this->GetOpnd2(&opnd2);

    result.val_32 = opnd1.val_32 ^ opnd2.val_32;

    this->SetFlagBit(FLAG_BIT_ZF,result.val_32 == 0);
    this->SetFlagBit(FLAG_BIT_SF,(int)(result.val_32) < 0);
    this->SetFlagBit(FLAG_BIT_CF,0);
    this->SetFlagBit(FLAG_BIT_OF,0);

    this->SetDesOpnd1Val(type,&val,&result);

    return OK;
}
int CVm::do_rep()
{
    DWORD ecx;
    int type;

    ecx = this->reg_index[REG_ECX]->val.val_32;

    type = this->cur_oper[2];
    
    ASSERT(type != REP_TYPE_ERR);

    while(ecx != 0 && !end_flag)
    {
        switch(type)
        {
            case REP_TYPE_MOVSB: do_movsb();break;
            case REP_TYPE_MOVSW: do_movsw();break;
            case REP_TYPE_MOVSD: do_movsd();break;
            case REP_TYPE_STOSB: do_stosb();break;
            case REP_TYPE_STOSW: do_stosw();break;
            case REP_TYPE_STOSD: do_stosd();break;
        }
        ecx --;
    };

    this->reg_index[REG_ECX]->val.val_32 = ecx;

    return OK;
}
int CVm::do_or()
{
    _RegVal opnd1,val,opnd2,result;
    long type,eip;

    eip = this->GetEIP();
    this->GetOpnd1(&opnd1);
    this->SetEIP(eip);

    type = this->GetOpnd_Common(0,&val,1);
    this->GetOpnd2(&opnd2);

    result.val_32 = opnd1.val_32 | opnd2.val_32;

    this->SetFlagBit(FLAG_BIT_ZF,result.val_32 == 0);
    this->SetFlagBit(FLAG_BIT_SF,(int)(result.val_32) < 0);
    this->SetFlagBit(FLAG_BIT_CF,0);
    this->SetFlagBit(FLAG_BIT_OF,0);

    this->SetDesOpnd1Val(type,&val,&result);

    return OK;
}
int CVm::do_add()
{
    _RegVal opnd1,val,opnd2,result;
    long type,eip;

    eip = this->GetEIP();
    this->GetOpnd1(&opnd1);
    this->SetEIP(eip);

    type = this->GetOpnd_Common(0,&val,1);
    this->GetOpnd2(&opnd2);

    result.val_32 = opnd1.val_32 + opnd2.val_32;

    this->SetFlagBit(FLAG_BIT_ZF,result.val_32 == 0);
    this->SetFlagBit(FLAG_BIT_SF,(int)(result.val_32) < 0);
    this->SetFlagBit(FLAG_BIT_CF,CMisc::add_overflow(opnd1.val_32,opnd2.val_32));
    this->SetFlagBit(FLAG_BIT_OF,CMisc::add_overflow(int(opnd1.val_32),int(opnd2.val_32)));

    this->SetDesOpnd1Val(type,&val,&result);

    return OK;
}
int CVm::do_sub()
{
    _RegVal opnd1,val,opnd2,result;
    long type,eip;  

    eip = this->GetEIP();
    this->GetOpnd1(&opnd1);
    this->SetEIP(eip);

    type = this->GetOpnd_Common(0,&val,1);
    this->GetOpnd2(&opnd2);

    result.val_32 = opnd1.val_32 - opnd2.val_32;

    this->SetFlagBit(FLAG_BIT_ZF,result.val_32 == 0);
    this->SetFlagBit(FLAG_BIT_SF,(int)(result.val_32) < 0);
    this->SetFlagBit(FLAG_BIT_CF,CMisc::sub_overflow(opnd1.val_32,opnd2.val_32));
    this->SetFlagBit(FLAG_BIT_OF,CMisc::sub_overflow(int(opnd1.val_32),int(opnd2.val_32)));

    this->SetDesOpnd1Val(type,&val,&result);

    return OK;
}
int CVm::do_mul()
{
    _RegVal opnd1,val,opnd2,result;
    long type,eip,hi,low;
    DWORD mul_num,over_flow;

    eip = this->GetEIP();
    this->GetOpnd1(&opnd1);
    this->SetEIP(eip);
    
    //如果有第二个操作数
    if(this->GetCurOpndNum() == 2)
    {
        type = this->GetOpnd_Common(0,&val,1);
        this->GetOpnd2(&opnd2);
        mul_num = opnd2.val_32;
    }
    else
    {
        mul_num = this->GetReg(REG_EAX)->val.val_32;
    }

    CMisc::mult(mul_num,opnd1.val_32,&hi,&low);
    
    over_flow = CMisc::multi_overflow(mul_num,opnd1.val_32);
    this->SetFlagBit(FLAG_BIT_CF,over_flow);
    this->SetFlagBit(FLAG_BIT_OF,over_flow);

    //如果有第二个操作数
    if(this->GetCurOpndNum() == 2)
    {
        result.val_32 = low;
        this->SetDesOpnd1Val(type,&val,&result);
        //虽然是两个操作数，实际上第一个操作数永远为eax,edx还需要设置
        this->GetReg(REG_EDX)->val.val_32 = hi; 
    }
    else
    {
        this->GetReg(REG_EAX)->val.val_32 = low;
        this->GetReg(REG_EDX)->val.val_32 = hi;
    }

    return OK;
}

int CVm::do_div()
{
    _RegVal div;
    QWORD edx_eax;

    this->GetOpnd1(&div);

    if(div.val_32 == 0)
    {
        this->runtime_error("divided by 0!\n");
        return ERROR;
    }

    edx_eax = this->GetReg(REG_EAX)->val.val_32;    
    *(((DWORD *)&edx_eax) + 1) = this->GetReg(REG_EDX)->val.val_32;
    
    this->GetReg(REG_EAX)->val.val_32 = DWORD(edx_eax / div.val_32);
    this->GetReg(REG_EDX)->val.val_32 = DWORD(edx_eax % div.val_32);

    return OK;
}

int CVm::do_jle()
{
    _RegVal val;

    this->GetOpnd1(&val);

    if(     this->GetFlagBit(FLAG_BIT_ZF)
        ||  (this->GetFlagBit(FLAG_BIT_SF) != this->GetFlagBit(FLAG_BIT_OF)))
    {
        this->SetEIP(val.val_32);   
    }

    return OK;
}
int CVm::do_jne()
{
    _RegVal val;

    this->GetOpnd1(&val);
        
    if( this->GetFlagBit(FLAG_BIT_ZF) == 0)
    {
        this->SetEIP(val.val_32);   
    }

    return OK;
}
int CVm::do_jmp()
{
    _RegVal val;

    this->GetOpnd1(&val);

    this->SetEIP(val.val_32);

    return OK;
}
int CVm::do_je()
{   
    _RegVal val;

    this->GetOpnd1(&val);

    if( this->GetFlagBit(FLAG_BIT_ZF) )
    {
        this->SetEIP(val.val_32);   
    }

    return OK;
}
int CVm::do_jz()
{
    return this->do_je();
}
int CVm::do_jge()
{
    _RegVal val;

    this->GetOpnd1(&val);

    if(this->GetFlagBit(FLAG_BIT_SF) == this->GetFlagBit(FLAG_BIT_OF))
    {
        this->SetEIP(val.val_32);   
    }

    return OK;
}
int CVm::do_cmp()
{
    _RegVal opnd1,opnd2,result;

    this->GetOpnd1(&opnd1);
    this->GetOpnd2(&opnd2);

    result.val_32 = opnd1.val_32 - opnd2.val_32;

    this->SetFlagBit(FLAG_BIT_ZF,result.val_32 == 0);
    this->SetFlagBit(FLAG_BIT_SF,(int)(result.val_32) < 0);
    this->SetFlagBit(FLAG_BIT_CF,CMisc::sub_overflow(opnd1.val_32,opnd2.val_32));
    this->SetFlagBit(FLAG_BIT_OF,CMisc::sub_overflow(int(opnd1.val_32),int(opnd2.val_32)));
    
    return OK;
}
int CVm::do_jg()
{
    _RegVal val;

    this->GetOpnd1(&val);

    if(     this->GetFlagBit(FLAG_BIT_ZF) == 0 
        &&  (this->GetFlagBit(FLAG_BIT_SF) == this->GetFlagBit(FLAG_BIT_OF)))
    {
        this->SetEIP(val.val_32);   
    }

    return OK;
}
int CVm::do_inc()
{
    _RegVal opnd1,val;
    int type,eip;
    
    eip = this->GetEIP();
    this->GetOpnd1(&opnd1);
    this->SetEIP(eip);

    type = this->GetOpnd_Common(0,&val,1);

    opnd1.val_32 ++;

    this->SetDesOpnd1Val(type,&val,&opnd1);

    this->SetFlagBit(FLAG_BIT_ZF,opnd1.val_32 == 0);
    this->SetFlagBit(FLAG_BIT_SF,(int)(opnd1.val_32) < 0);

    return OK;
}
int CVm::do_jl()
{
    _RegVal val;

    this->GetOpnd1(&val);

    if(this->GetFlagBit(FLAG_BIT_SF) != this->GetFlagBit(FLAG_BIT_OF))
    {
        this->SetEIP(val.val_32);   
    }

    return OK;
}
int CVm::do_cdq()
{
    if((this->GetReg(REG_EAX)->val.val_32) & 0x80000000)
        this->GetReg(REG_EDX)->val.val_32 = 0xffffffff;
    else
        this->GetReg(REG_EDX)->val.val_32 = 0x00000000;
    
    return OK;
}
int CVm::do_idiv()
{
    _RegVal div;
    __int64 edx_eax;

    this->GetOpnd1(&div);

    if(div.val_32 == 0)
    {
        this->runtime_error("divided by 0!\n");
        return ERROR;
    }

    edx_eax = this->GetReg(REG_EAX)->val.val_32;
    *(((DWORD *)&edx_eax) + 1) = this->GetReg(REG_EDX)->val.val_32;

    this->GetReg(REG_EAX)->val.val_32 = DWORD(edx_eax / (int)div.val_32);
    this->GetReg(REG_EDX)->val.val_32 = DWORD(edx_eax % (int)div.val_32);

    return OK;
}
int CVm::do_shr()
{
    _RegVal addr,val1,val2,result;
    int type,t,old_eip;
    
    old_eip = this->GetEIP();
    type = this->GetOpnd_Common(0,&addr,1);
    this->SetEIP(old_eip);

    t = this->GetOpnd1(&val1);

    if(t == 1) val1.val_32 &= 0x000000ff;
    if(t == 2) val1.val_32 &= 0x0000ffff;

    t = this->GetOpnd2(&val2);

    if(t == 1) val2.val_32 &= 0x000000ff;
    if(t == 2) val2.val_32 &= 0x0000ffff;

    result.val_32 = (val1.val_32 >> val2.val_32);

    this->SetDesOpnd1Val(type,&addr,&result);

    this->SetFlagBit(FLAG_BIT_OF,(result.val_32 & 0x80000000) != (val1.val_32 & 0x80000000));
    this->SetFlagBit(FLAG_BIT_ZF,result.val_32 == 0);
    this->SetFlagBit(FLAG_BIT_SF,(int)(result.val_32) < 0);
    this->SetFlagBit(FLAG_BIT_CF,val1.val_32 & 0x01);

    return OK;
}
int CVm::do_and()
{
    _RegVal opnd1,val,opnd2,result;
    long type,eip;

    eip = this->GetEIP();
    this->GetOpnd1(&opnd1);
    this->SetEIP(eip);

    type = this->GetOpnd_Common(0,&val,1);
    this->GetOpnd2(&opnd2);

    result.val_32 = opnd1.val_32 & opnd2.val_32;

    this->SetFlagBit(FLAG_BIT_ZF,result.val_32 == 0);
    this->SetFlagBit(FLAG_BIT_SF,(int)(result.val_32) < 0);
    this->SetFlagBit(FLAG_BIT_CF,0);
    this->SetFlagBit(FLAG_BIT_OF,0);

    this->SetDesOpnd1Val(type,&val,&result);

    return OK;
}
int CVm::do_movsx()
{
    return this->do_mov();
}
int CVm::do_ja()
{
    _RegVal val;

    this->GetOpnd1(&val);

    if(     this->GetFlagBit(FLAG_BIT_CF) == 0
        &&  this->GetFlagBit(FLAG_BIT_ZF) == 0)
    {
        this->SetEIP(val.val_32);   
    }

    return OK;
}
int CVm::do_jae()
{
    _RegVal val;

    this->GetOpnd1(&val);

    if( this->GetFlagBit(FLAG_BIT_CF) == 0 )        
    {
        this->SetEIP(val.val_32);   
    }

    return OK;
}
int CVm::do_npad()
{
    _RegVal val;

    this->GetOpnd1(&val);
    //no operation, but have one opnd

    return OK;
}
int CVm::do_dec()
{
    _RegVal opnd1,val;
    int type,eip;
    
    eip = this->GetEIP();
    this->GetOpnd1(&opnd1);
    this->SetEIP(eip);

    type = this->GetOpnd_Common(0,&val,1);

    opnd1.val_32 --;

    this->SetDesOpnd1Val(type,&val,&opnd1);

    this->SetFlagBit(FLAG_BIT_ZF,opnd1.val_32 == 0);
    this->SetFlagBit(FLAG_BIT_SF,(int)(opnd1.val_32) < 0);

    return OK;
}
int CVm::do_shl()
{
    _RegVal addr,val1,val2,result;
    int type,old_eip;
    
    old_eip = this->GetEIP();
    type = this->GetOpnd_Common(0,&addr,1);
    this->SetEIP(old_eip);

    this->GetOpnd1(&val1);
    this->GetOpnd2(&val2);

    result.val_32 = (val1.val_32 << val2.val_32);

    this->SetDesOpnd1Val(type,&addr,&result);

    this->SetFlagBit(FLAG_BIT_OF,(result.val_32 & 0x80000000) != (val1.val_32 & 0x80000000));
    this->SetFlagBit(FLAG_BIT_ZF,result.val_32 == 0);
    this->SetFlagBit(FLAG_BIT_SF,(int)(result.val_32) < 0);
    this->SetFlagBit(FLAG_BIT_CF,val1.val_32 & 0x80000000);

    return OK;
}
int CVm::do_sar()
{
    //算术右移指令,最高位保持不变
    //最低位移入CF中
    _RegVal addr,val1,val2,result;
    int type,old_eip;
    
    old_eip = this->GetEIP();
    type = this->GetOpnd_Common(0,&addr,1);
    this->SetEIP(old_eip);

    this->GetOpnd1(&val1);
    this->GetOpnd2(&val2);

    result.val_32 = (val1.val_32 >> val2.val_32);

    result.val_32 &= 0x7fffffff;
    result.val_32 |= (val1.val_32 & 0x80000000); 

    this->SetDesOpnd1Val(type,&addr,&result);

    this->SetFlagBit(FLAG_BIT_OF,(result.val_32 & 0x80000000) != (val1.val_32 & 0x80000000));
    this->SetFlagBit(FLAG_BIT_ZF,result.val_32 == 0);
    this->SetFlagBit(FLAG_BIT_SF,(int)(result.val_32) < 0);
    this->SetFlagBit(FLAG_BIT_CF,val1.val_32 & 0x01);

    return OK;
}
int CVm::do_imul()
{
    _RegVal opnd1,val,opnd2,result;
    long type,eip,hi,low;
    int mul_num,over_flow;  

    eip = this->GetEIP();
    this->GetOpnd1(&opnd1);
    this->SetEIP(eip);
    
    //如果有第二个操作数
    if(this->GetCurOpndNum() == 2)
    {
        type = this->GetOpnd_Common(0,&val,1);
        this->GetOpnd2(&opnd2);
        mul_num = opnd2.val_32;
    }
    else
    {
        mul_num = this->GetReg(REG_EAX)->val.val_32;
    }

    CMisc::imult(mul_num,opnd1.val_32,&hi,&low);

    over_flow = CMisc::multi_overflow(mul_num,(int)opnd1.val_32);

    this->SetFlagBit(FLAG_BIT_CF,over_flow);
    this->SetFlagBit(FLAG_BIT_OF,over_flow);

    //如果有第二个操作数
    if(this->GetCurOpndNum() == 2)
    {
        result.val_32 = low;
        this->SetDesOpnd1Val(type,&val,&result);
    }
    else
    {
        this->GetReg(REG_EAX)->val.val_32 = low;
        this->GetReg(REG_EDX)->val.val_32 = hi;
    }

    return OK;
}
int CVm::do_not()
{
    _RegVal addr,val;
    int type;

    DWORD old_eip;
    
    old_eip = this->GetEIP();
    type = this->GetOpnd_Common(0,&addr,1);
    this->SetEIP(old_eip);

    this->GetOpnd1(&val);

    val.val_32 = ~(val.val_32); 

    this->SetDesOpnd1Val(type,&addr,&val);

    return OK;
}
int CVm::do_neg()
{   
    _RegVal addr,val;
    int type;
    
    DWORD old_eip;
    
    old_eip = this->GetEIP();
    type = this->GetOpnd_Common(0,&addr,1);
    this->SetEIP(old_eip);
    
    this->GetOpnd1(&val);

    this->SetFlagBit(FLAG_BIT_CF,CMisc::add_overflow(DWORD(0),val.val_32));
    this->SetFlagBit(FLAG_BIT_OF,CMisc::add_overflow(int(0),int(val.val_32)));

    val.val_32 = 0 - val.val_32;
    
    this->SetFlagBit(FLAG_BIT_ZF,val.val_32 == 0);
    this->SetFlagBit(FLAG_BIT_SF,(int)(val.val_32) < 0);
    
    this->SetDesOpnd1Val(type,&addr,&val);
    
    return OK;
}
int CVm::do_sete()
{
    _RegVal addr,val;
    int type;

    type = this->GetOpnd_Common(0,&addr,1);

    val.val_32 = this->GetFlagBit(FLAG_BIT_ZF);

    this->SetDesOpnd1Val(type,&addr,&val);

    return OK;
}
int CVm::do_repne()
{
    DWORD ecx;
    int type;

    ecx = this->reg_index[REG_ECX]->val.val_32;

    type = this->cur_oper[2];
    
    ASSERT(type != REP_TYPE_ERR);

    while(ecx != 0 && !end_flag)
    {   
        switch(type)
        {
            case REP_TYPE_CMPSB: do_cmpsb();break;
            case REP_TYPE_CMPSW: do_cmpsw();break;
            case REP_TYPE_CMPSD: do_cmpsd();break;
            case REP_TYPE_SCASB: do_scasb();break;
            case REP_TYPE_SCASW: do_scasw();break;
            case REP_TYPE_SCASD: do_scasd();break;
        }
        ecx --; 
        if(this->GetFlagBit(FLAG_BIT_ZF))
            break;      
    }

    this->reg_index[REG_ECX]->val.val_32 = ecx;

    return OK;
}
int CVm::do_repe()
{
    DWORD ecx;
    int type;

    ecx = this->reg_index[REG_ECX]->val.val_32;

    type = this->cur_oper[2];
    
    ASSERT(type != REP_TYPE_ERR);

    while(ecx != 0 && !end_flag)
    {   
        switch(type)
        {
            case REP_TYPE_CMPSB: do_cmpsb();break;
            case REP_TYPE_CMPSW: do_cmpsw();break;
            case REP_TYPE_CMPSD: do_cmpsd();break;
            case REP_TYPE_SCASB: do_scasb();break;
            case REP_TYPE_SCASW: do_scasw();break;
            case REP_TYPE_SCASD: do_scasd();break;
        }
        ecx --; 
        if(this->GetFlagBit(FLAG_BIT_ZF) == 0)
            break;
    }

    this->reg_index[REG_ECX]->val.val_32 = ecx;

    return OK;
}
int CVm::do_sbb()
{
    _RegVal opnd1,val,opnd2,result;
    long type,eip;  
    int cf,of;

    eip = this->GetEIP();
    this->GetOpnd1(&opnd1);
    this->SetEIP(eip);

    type = this->GetOpnd_Common(0,&val,1);
    this->GetOpnd2(&opnd2);

    result.val_32 = opnd1.val_32 - opnd2.val_32 - this->GetFlagBit(FLAG_BIT_CF);

    this->SetFlagBit(FLAG_BIT_ZF,result.val_32 == 0);
    this->SetFlagBit(FLAG_BIT_SF,(int)(result.val_32) < 0);

    of = CMisc::sub_overflow(int(opnd1.val_32),int(opnd2.val_32));
    if(!of)
    {
        int mid_r = (int)opnd1.val_32 - (int)opnd2.val_32;
        of = CMisc::sub_overflow(mid_r,this->GetFlagBit(FLAG_BIT_CF));
    }
    cf = CMisc::sub_overflow(opnd1.val_32,opnd2.val_32);
    if(!cf)
    {
        DWORD mid_r = opnd1.val_32 - opnd2.val_32;
        cf = CMisc::sub_overflow(mid_r,DWORD(this->GetFlagBit(FLAG_BIT_CF)));
    }
    this->SetFlagBit(FLAG_BIT_CF,cf);
    this->SetFlagBit(FLAG_BIT_OF,of);

    this->SetDesOpnd1Val(type,&val,&result);

    return OK;
}
int CVm::do_jbe()
{
    _RegVal val;

    this->GetOpnd1(&val);

    if(     this->GetFlagBit(FLAG_BIT_CF)
        ||  this->GetFlagBit(FLAG_BIT_ZF))
    {
        this->SetEIP(val.val_32);   
    }

    return OK;
}
int CVm::do_jb()
{
    _RegVal val;

    this->GetOpnd1(&val);

    if(this->GetFlagBit(FLAG_BIT_CF))
    {
        this->SetEIP(val.val_32);   
    }

    return OK;
}
int CVm::do_fild()
{
    _RegVal val;

    this->GetOpnd1(&val);

    //convert integer to double
    double v = (double)((int)(val.val_32));
    this->PushFPU(*((QWORD*)(&v)));

    return OK;
}
int CVm::do_fmulp()
{
    this->do_fmul();
    this->PopFPU();

    return OK;
}
int CVm::do_fimul()
{
    _RegVal val;
    double *p1;

    ASSERT(this->GetCurOpndNum() == 1);

    this->GetOpnd1(&val);

    p1 = (double*)&(this->GetFPU(0)->val.val_64);

    *p1 = *p1 * (double)((int)val.val_32);

    return OK;
}
int CVm::do_fidivr()
{
    _RegVal val;
    double *p1;

    ASSERT(this->GetCurOpndNum() == 1);

    this->GetOpnd1(&val);

    p1 = (double*)&(this->GetFPU(0)->val.val_64);

    if(*p1 == 0.0)
    {
        this->runtime_error("divided by 0\n");
    }
    else
    {
        *p1 = (double)((int)val.val_32) / *p1;
    }

    return OK;
}
int CVm::do_fdivrp()
{
    this->do_fdivr();
    this->PopFPU();

    return OK;
}
int CVm::do_fidiv()
{
    _RegVal val;
    double *p1;

    ASSERT(this->GetCurOpndNum() == 1);

    this->GetOpnd1(&val);

    p1 = (double*)&(this->GetFPU(0)->val.val_64);

    if(val.val_32 == 0)
    {
        this->runtime_error("divided by 0\n");
    }
    else
    {
        *p1 = *p1 / (double)((int)val.val_32);
    }

    return OK;
}
int CVm::do_fmul()
{   
    int ret = OK,opnd_num;
    _RegVal val;
    double *p1,*p2;

    opnd_num = this->GetCurOpndNum();

    if(opnd_num == 0)
    {
        //操作数为0个时比较特殊
        p1 = (double*)&(this->GetFPU(0)->val.val_64);
        p2 = (double*)&(this->GetFPU(1)->val.val_64);

        *p2 = (*p1) * (*p2);

        this->PopFPU();
    }
    else if(opnd_num == 1)
    {
        this->GetOpnd1(&val);
        
        p1 = (double*)&(this->GetFPU(0)->val.val_64);
        p2 = (double*)&(val.val_64);

        *p1 = (*p1) * (*p2);

    }
    else if(opnd_num == 2)
    {
        _RegVal addr;

        this->GetOpnd_Common(0,&addr,1);
        this->GetOpnd2(&val);
        
        p1 = (double*)&(this->GetFpuRegByRegNum(addr.val_32)->val.val_64);
        p2 = (double*)&(val.val_64);
        
        *p1 = (*p1) * (*p2);
    }

    return OK;
}
int CVm::do_fdivr()
{
    int ret = OK,opnd_num;
    _RegVal val;
    double *p1,*p2;

    opnd_num = this->GetCurOpndNum();

    if(opnd_num == 0)
    {
        p1 = (double*)&(this->GetFPU(0)->val.val_64);
        p2 = (double*)&(this->GetFPU(1)->val.val_64);
        
        if(*p2 == 0.0)
        {
            this->runtime_error("divided by 0 in fdivr\n");
            ret = ERROR;
        }
        else
        {
            *p2 = *p1 / *p2;
        }
        this->PopFPU();
    }
    else if(opnd_num == 1)
    {   
        p1 = (double*)&(this->GetFPU(0)->val.val_64);
        this->GetOpnd1(&val);
        p2 = (double*)&(val.val_64);

        if(*p1 == 0.0)
        {
            this->runtime_error("divided by 0 in fdivr\n");
            ret = ERROR;
        }
        else
        {
            *p1 = *p2 / *p1;
        }
    }
    else if(opnd_num == 2)
    {
        _RegVal addr;

        this->GetOpnd_Common(0,&addr,1);
        this->GetOpnd2(&val);
        
        p1 = (double*)&(this->GetFpuRegByRegNum(addr.val_32)->val.val_64);
        p2 = (double*)&(val.val_64);
        
        if( *p1 == 0.0)
        {
            this->runtime_error("divided by 0 in fdivr\n");
            return ERROR;
        }
        else
        {
            *p1 = *p2 / *p1;
        }
    }
    return ret;
}

int CVm::do_fdivp()
{
    this->do_fdiv();
    this->PopFPU();

    return OK;
}
int CVm::do_fdiv()
{
    int ret = OK,opnd_num;
    _RegVal val;
    double *p1,*p2;

    opnd_num = this->GetCurOpndNum();

    if(opnd_num == 0)
    {
        p1 = (double*)&(this->GetFPU(0)->val.val_64);
        p2 = (double*)&(this->GetFPU(1)->val.val_64);
        
        if(*p1 == 0.0)
        {
            this->runtime_error("divided by 0 in fdiv\n");
            ret = ERROR;
        }
        else
        {
            *p2 = *p2 / *p1;
        }

        this->PopFPU();
    }
    else if(opnd_num == 1)
    {   
        p1 = (double*)&(this->GetFPU(0)->val.val_64);
        this->GetOpnd1(&val);
        p2 = (double*)&(val.val_64);

        if(*p2 == 0.0)
        {
            this->runtime_error("divided by 0 fdiv\n");
            ret = ERROR;
        }
        else
        {
            *p1 = *p1 / *p2;
        }
    }
    else if(opnd_num == 2)
    {
        _RegVal addr;

        this->GetOpnd_Common(0,&addr,1);
        this->GetOpnd2(&val);
        
        p1 = (double*)&(this->GetFpuRegByRegNum(addr.val_32)->val.val_64);
        p2 = (double*)&(val.val_64);
        
        if( *p2 == 0.0)
        {
            this->runtime_error("divided by 0 fdiv\n");
            return ERROR;
        }
        else
        {
            *p1 = *p1 / *p2;
        }
    }

    return OK;
}
int CVm::do_fstp()
{
    this->do_fst();
    this->PopFPU();

    return OK;
}

int CVm::do_fist()
{
    _RegVal addr,val;
    double *p;

    int type = this->GetOpnd_Common(0,&addr,1);

    p = (double*)&(this->GetFPU(0)->val.val_64);

    val.val_32 = (int)(*p);

    this->SetDesOpnd1Val(type,&addr,&val);
    
    return OK;
}

int CVm::do_fistp()
{
    this->do_fist();
    this->PopFPU();

    return OK;
}
int CVm::do_fprem()
{
    double *p1,*p2;
    int q;

    p1 = (double*)&(this->GetFPU(0)->val.val_64);
    p2 = (double*)&(this->GetFPU(1)->val.val_64);

    if(*p2 == 0.0)
    {
        this->runtime_error("divided by 0 in fprem\n");
    }
    else
    {
        //转换为整数
        q = (int)(*p1 / *p2); 
        *p1 = *p1 - *p2 * q;

        //always complete reduction     
        this->SetFpuStatusBit(FPU_STATUS_BIT_C2,0);
    }

    return OK;
}
int CVm::do_fld()
{
    _RegVal val;

    this->GetOpnd1(&val);
    
    this->PushFPU(val.val_64);

    return OK;
}
int CVm::do_fld1()
{
    QWORD q;
    double *p = (double *)&q;

    *p = 1.0;

    this->PushFPU(q);
    
    return OK;
}
int CVm::do_fldz()
{
    QWORD q;
    double *p = (double *)&q;

    *p = 0.0;

    this->PushFPU(q);
    
    return OK;
}
int CVm::do_fldpi()
{
    QWORD q;
    double *p = (double *)&q;

    *p = M_PI;

    this->PushFPU(q);
    
    return OK;
}
int CVm::do_fldl2e()
{
    QWORD q;
    double *p = (double *)&q;

    *p = 1.44269504088896340736;

    this->PushFPU(q);
    
    return OK;
}

int CVm::do_fldl2t()
{
    QWORD q;
    double *p = (double *)&q;

    *p = 3.32192809488736234787;

    this->PushFPU(q);
    
    return OK;
}

int CVm::do_fldlg2()
{
    QWORD q;
    double *p = (double *)&q;

    *p = 0.30102999566398119521;

    this->PushFPU(q);
    
    return OK;
}

int CVm::do_fldln2()
{
    QWORD q;
    double *p = (double *)&q;

    *p = 0.693147180559945309417;

    this->PushFPU(q);
    
    return OK;
}
int CVm::do_fyl2x()
{
    double *p1 = (double*)&(this->GetFPU(0)->val.val_64);
    double *p2 = (double*)&(this->GetFPU(1)->val.val_64);

    *p2 = *p2 * log(*p1)/0.693147180559945309417;

    this->PopFPU();

    return OK;
}
int CVm::do_fsin()
{
    double *p = (double*)&(this->GetFPU(0)->val.val_64);

    *p = sin(*p);

    return OK;
}
int CVm::do_fiadd()
{
    _RegVal val;
    double *p1;

    ASSERT(this->GetCurOpndNum() == 1);

    this->GetOpnd1(&val);

    p1 = (double*)&(this->GetFPU(0)->val.val_64);

    *p1 = *p1 + (double)((int)val.val_32);

    return OK;
}
int CVm::do_fadd()
{
    _RegVal val;
    double *p1,*p2;

    if(this->GetCurOpndNum() == 0)
    {
        p1 = (double*)&(this->GetFPU(0)->val.val_64);
        p2 = (double*)&(this->GetFPU(1)->val.val_64);
    
        *p2 += *p1;     
        this->PopFPU();
    }
    else if(this->GetCurOpndNum() == 1)
    {
        this->GetOpnd1(&val);

        p1 = (double*)&(this->GetFPU(0)->val.val_64);
        p2 = (double*)&(val.val_64);

        *p1 += *p2;     
    }
    else if(this->GetCurOpndNum() == 2)
    {
        _RegVal addr;

        this->GetOpnd_Common(0,&addr,1);
        this->GetOpnd2(&val);
        
        p1 = (double*)&(this->GetFpuRegByRegNum(addr.val_32)->val.val_64);
        p2 = (double*)&(val.val_64);
        
        *p1 += *p2;
    }
    
    return OK;
}

int CVm::do_faddp()
{
    this->do_fadd();
    this->PopFPU();

    return OK;
}
int CVm::do_fsubp()
{
    this->do_fsub();
    this->PopFPU();

    return OK;
}
int CVm::do_fsubrp()
{
    this->do_fsubr();
    this->PopFPU();

    return OK;
}
int CVm::do_fsubr()
{
    int ret = OK,opnd_num;
    _RegVal val;
    double *p1,*p2;

    opnd_num = this->GetCurOpndNum();

    if(opnd_num == 0)
    {
        p1 = (double*)&(this->GetFPU(0)->val.val_64);
        p2 = (double*)&(this->GetFPU(1)->val.val_64);
    
        *p2 = (*p1) - (*p2);
        this->PopFPU();
    }
    else if(opnd_num == 1)
    {   
        p1 = (double*)&(this->GetFPU(0)->val.val_64);
        this->GetOpnd1(&val);
        p2 = (double*)&(val.val_64);
        *p1 = (*p2) - (*p1);
    }
    else if(opnd_num == 2)
    {
        _RegVal addr;

        this->GetOpnd_Common(0,&addr,1);
        this->GetOpnd2(&val);
        
        p1 = (double*)&(this->GetFpuRegByRegNum(addr.val_32)->val.val_64);
        p2 = (double*)&(val.val_64);
        
        *p1 = (*p2) - (*p1);
    }
    return ret;
}
int CVm::do_fisub()
{
    _RegVal val;
    double *p1;

    ASSERT(this->GetCurOpndNum() == 1);

    this->GetOpnd1(&val);

    p1 = (double*)&(this->GetFPU(0)->val.val_64);

    *p1 = *p1 - (double)((int)val.val_32);

    return OK;
}

int CVm::do_fisubr()
{
    _RegVal val;
    double *p1;

    ASSERT(this->GetCurOpndNum() == 1);

    this->GetOpnd1(&val);

    p1 = (double*)&(this->GetFPU(0)->val.val_64);

    *p1 = (double)((int)val.val_32) - *p1;

    return OK;
}

int CVm::do_fsub()
{
    _RegVal val;
    double *p1,*p2;

    if(this->GetCurOpndNum() == 0)
    {
        p1 = (double*)&(this->GetFPU(0)->val.val_64);
        p2 = (double*)&(this->GetFPU(1)->val.val_64);
    
        *p2 -= *p1;     
        this->PopFPU();
    }
    else if(this->GetCurOpndNum() == 1)
    {
        this->GetOpnd1(&val);

        p1 = (double*)&(this->GetFPU(0)->val.val_64);
        p2 = (double*)&(val.val_64);

        *p1 -= *p2;     
    }
    else if(this->GetCurOpndNum() == 2)
    {
        _RegVal addr;

        this->GetOpnd_Common(0,&addr,1);
        this->GetOpnd2(&val);
        
        p1 = (double*)&(this->GetFpuRegByRegNum(addr.val_32)->val.val_64);
        p2 = (double*)&(val.val_64);

        *p1 -= *p2;
    }
    
    return OK;
}
int CVm::do_fchs()
{
    double *p = (double*)&(this->GetFPU(0)->val.val_64);

    *p = - (*p);

    return OK;
}
int CVm::do_fscale()
{
    double *p1,*p2;

    p1 = (double*)&(this->GetFPU(0)->val.val_64);
    p2 = (double*)&(this->GetFPU(1)->val.val_64);

    *p1 = (*p1) * pow(2.0,*p2);

    return OK;
}
int CVm::do_f2xm1()
{
    double *p = (double*)&(this->GetFPU(0)->val.val_64);

    *p = pow(2.0,*p) - 1.0;

    return OK;
}
int CVm::do_fxch()
{
    QWORD *p1 = NULL,*p2 = NULL,t;
    _RegVal addr;

    p1 = &(this->GetFPU(0)->val.val_64);

    if(this->GetCurOpndNum() == 1)
    {
        this->GetOpnd_Common(0,&addr,1);
        p2 = &(this->GetFpuRegByRegNum(addr.val_32)->val.val_64);
    }
    else if(this->GetCurOpndNum() == 0)
    {
        p2 = &(this->GetFPU(1)->val.val_64);
    }
    
    ASSERT(p1 && p2);

    t = *p1;
    *p1 = *p2;
    *p2 = t;

    return OK;
}
int CVm::do_frndint()
{
    double *p = (double*)&(this->GetFPU(0)->val.val_64);
    int rc;

    rc = this->GetFPUControlRC();
    
    if(rc == 1)
    {
        if(*p >= 0.0)
        {
            *p = (int)(*p);
        }
        else
        {
            if(*p - (int)(*p) != 0.0)
                *p = (int)(*p) - 1.0;
        }
    }
    else
    {
        
        if(*p >= 0.0)
        {
            if( *p - (int)(*p) != 0.0)
                *p = (int)(*p) + 1.0;
        }
        else
        {
            *p = (int)(*p);
        }
        
    }

    return OK;
}
int CVm::do_fabs()
{
    double *p = (double*)&(this->GetFPU(0)->val.val_64);

    *p = fabs(*p);

    return OK;
}
int CVm::do_fsqrt()
{
    double *p = (double*)&(this->GetFPU(0)->val.val_64);

    if(*p < 0.0)
        this->runtime_error("minus opnd in fsqrt\n");
    else
        *p = sqrt(*p);

    return OK;
}
int CVm::do_fcos()
{
    double *p = (double*)&(this->GetFPU(0)->val.val_64);

    *p = cos(*p);

    return OK;
}
int CVm::do_fptan()
{
    double *p1 = (double*)&(this->GetFPU(0)->val.val_64);   

    *p1 = tan(*p1);
    this->PushFPU(1);

    return OK;
}
int CVm::do_fpatan()
{
    double *p1 = (double*)&(this->GetFPU(0)->val.val_64);
    double *p2 = (double*)&(this->GetFPU(1)->val.val_64);   

    if(*p1 == 0.0)
    {
        if(*p2 > 0.0) *p2 = M_PI/2.0;
        else if(*p2 == 0.0) *p2 = 0.0;
        else if(*p2 < 0.0) *p2 = -M_PI/2.0;         
    }
    else if(*p1 < 0.0)
    {
        if(*p2 > 0.0)*p2 = M_PI + atan(*p2 / *p1);
        else if(*p2 == 0.0) *p2 = M_PI;
        else if(*p2 < 0.0) *p2 = atan(*p2 / *p1) - M_PI;
    }
    else if(*p1 > 0.0)
    {
        if(*p2 > 0.0) *p2 = atan(*p2 / *p1);
        else if(*p2 == 0.0) *p2 = 0;
        else if(*p2 < 0.0) *p2 = atan(*p2 / *p1);
    }

    this->PopFPU(); 
    
    return OK;
}
int CVm::do_fxam()
{
    double *p = (double *)&(this->GetFPU(0)->val.val_64);

    this->SetFpuStatusBit(FPU_STATUS_BIT_C0,0);
    this->SetFpuStatusBit(FPU_STATUS_BIT_C1,0);
    this->SetFpuStatusBit(FPU_STATUS_BIT_C2,0);
    this->SetFpuStatusBit(FPU_STATUS_BIT_C3,0);

    if(*p > 0.0)
        this->SetFpuStatusBit(FPU_STATUS_BIT_C2,1);
    
    if(*p == 0.0)
        this->SetFpuStatusBit(FPU_STATUS_BIT_C3,1);

    if(*p < 0.0)
    {
        this->SetFpuStatusBit(FPU_STATUS_BIT_C1,1);
        this->SetFpuStatusBit(FPU_STATUS_BIT_C2,1);
    }

    return OK;
}
int CVm::do_fcom()
{
    _RegVal val;
    double *p1,*p2;

    if(this->GetCurOpndNum() == 1)
    {
        this->GetOpnd1(&val);
        p2 = (double*)&(val.val_64);
    }
    else if(this->GetCurOpndNum() == 0)
    {
        p2 = (double*)&(this->GetFPU(1)->val.val_64);
    }

    p1 = (double*)&(this->GetFPU(0)->val.val_64);
        
    this->SetFpuStatusBit(FPU_STATUS_BIT_C0,0);
    this->SetFpuStatusBit(FPU_STATUS_BIT_C2,0);
    this->SetFpuStatusBit(FPU_STATUS_BIT_C3,0);
    
    if(*p1 < *p2)
        this->SetFpuStatusBit(FPU_STATUS_BIT_C0,1);
    else if(*p1 == *p2)
        this->SetFpuStatusBit(FPU_STATUS_BIT_C3,1);

    return OK;
}

int CVm::do_fcomp()
{
    this->do_fcom();
    this->PopFPU();

    return OK;
}

int CVm::do_ftst()
{
    double *p = (double *)&(this->GetFPU(0)->val.val_64);

    this->SetFpuStatusBit(FPU_STATUS_BIT_C0,0);
    this->SetFpuStatusBit(FPU_STATUS_BIT_C2,0);
    this->SetFpuStatusBit(FPU_STATUS_BIT_C3,0);
    
    if(*p < 0.0)
        this->SetFpuStatusBit(FPU_STATUS_BIT_C0,1);
    else if(*p == 0.0)
        this->SetFpuStatusBit(FPU_STATUS_BIT_C3,1);

    return OK;
}
int CVm::do_stosw()
{
    DWORD edi;
    _RegVal val;

    this->GetReg(REG_AX)->GetVal(&val);

    edi = this->GetReg(REG_EDI)->val.val_32;
    this->i_vmem->Write16Mem(edi,WORD(val.val_32));

    if(this->GetFlagBit(FLAG_BIT_DF))
    {
        this->GetReg(REG_EDI)->val.val_32 -= 2;
    }
    else
    {
        this->GetReg(REG_EDI)->val.val_32 += 2;
    }

    return OK;
}

int CVm::do_stosd()
{
    DWORD edi;
    _RegVal val;

    this->GetReg(REG_EAX)->GetVal(&val);

    edi = this->GetReg(REG_EDI)->val.val_32;
    this->i_vmem->Write32Mem(edi,val.val_32);

    if(this->GetFlagBit(FLAG_BIT_DF))
    {
        this->GetReg(REG_EDI)->val.val_32 -= 4;
    }
    else
    {
        this->GetReg(REG_EDI)->val.val_32 += 4;
    }

    return OK;
}
int CVm::do_stosb()
{
    DWORD edi;
    _RegVal val;

    this->GetReg(REG_AL)->GetVal(&val);

    edi = this->GetReg(REG_EDI)->val.val_32;
    this->i_vmem->Write8Mem(edi,BYTE(val.val_32));

    if(this->GetFlagBit(FLAG_BIT_DF))
    {
        this->GetReg(REG_EDI)->val.val_32 --;
    }
    else
    {
        this->GetReg(REG_EDI)->val.val_32 ++;
    }

    return OK;
}
int CVm::do_movsw()
{
    DWORD edi,esi;
    WORD w;

    edi = this->GetReg(REG_EDI)->val.val_32;
    esi = this->GetReg(REG_ESI)->val.val_32;

    w = this->i_vmem->Read16Mem(esi);
    this->i_vmem->Write16Mem(edi,w);

    if(this->GetFlagBit(FLAG_BIT_DF))
    {
        this->GetReg(REG_EDI)->val.val_32 -= 2;
        this->GetReg(REG_ESI)->val.val_32 -= 2;
    }
    else
    {
        this->GetReg(REG_EDI)->val.val_32 += 2;
        this->GetReg(REG_ESI)->val.val_32 += 2;
    }

    return OK;
}
int CVm::do_movsd()
{
    DWORD edi,esi;
    DWORD dw;

    edi = this->GetReg(REG_EDI)->val.val_32;
    esi = this->GetReg(REG_ESI)->val.val_32;

    dw = this->i_vmem->Read32Mem(esi);
    this->i_vmem->Write32Mem(edi,dw);

    if(this->GetFlagBit(FLAG_BIT_DF))
    {
        this->GetReg(REG_EDI)->val.val_32 -= 4;
        this->GetReg(REG_ESI)->val.val_32 -= 4;
    }
    else
    {
        this->GetReg(REG_EDI)->val.val_32 += 4;
        this->GetReg(REG_ESI)->val.val_32 += 4;
    }

    return OK;
}
int CVm::do_setne()
{
    _RegVal addr,val;
    int type;

    type = this->GetOpnd_Common(0,&addr,1);

    val.val_32 = (this->GetFlagBit(FLAG_BIT_ZF) == 0);          

    this->SetDesOpnd1Val(type,&addr,&val);

    return OK;
}
int CVm::do_jns()
{
    _RegVal val;

    this->GetOpnd1(&val);
        
    if( this->GetFlagBit(FLAG_BIT_SF) == 0)
    {
        this->SetEIP(val.val_32);   
    }

    return OK;
}
int CVm::do_jnz()
{
    return this->do_jne();
}
int CVm::do_setge()
{
    _RegVal addr,val;
    int type;

    type = this->GetOpnd_Common(0,&addr,1);

    val.val_32 = (this->GetFlagBit(FLAG_BIT_SF) == this->GetFlagBit(FLAG_BIT_OF));

    this->SetDesOpnd1Val(type,&addr,&val);

    return OK;
}
int CVm::do_js()
{
    _RegVal val;

    this->GetOpnd1(&val);
        
    if( this->GetFlagBit(FLAG_BIT_SF))
    {
        this->SetEIP(val.val_32);   
    }

    return OK;
}
int CVm::do_setg()
{
    _RegVal addr,val;
    int type;

    type = this->GetOpnd_Common(0,&addr,1);

    val.val_32 =    (this->GetFlagBit(FLAG_BIT_ZF) == 0) 
                &&  (this->GetFlagBit(FLAG_BIT_SF) == this->GetFlagBit(FLAG_BIT_OF));

    this->SetDesOpnd1Val(type,&addr,&val);

    return OK;
}
int CVm::do_setl()
{
    _RegVal addr,val;
    int type;

    type = this->GetOpnd_Common(0,&addr,1);

    val.val_32 = (this->GetFlagBit(FLAG_BIT_SF) != this->GetFlagBit(FLAG_BIT_OF));

    this->SetDesOpnd1Val(type,&addr,&val);

    return OK;
}
int CVm::do_fst()
{
    _RegVal addr,val;

    ASSERT(this->GetCurOpndNum() == 1);

    int type = this->GetOpnd_Common(0,&addr,1);

    val.val_64 = this->GetFPU(0)->val.val_64;
    
    this->SetDesOpndFloatVal(type,&addr,&val);
    
    return OK;
}
int CVm::do_setle()
{
    _RegVal addr,val;
    int type;

    type = this->GetOpnd_Common(0,&addr,1);

    val.val_32 =    this->GetFlagBit(FLAG_BIT_ZF) 
                ||  (this->GetFlagBit(FLAG_BIT_SF) != this->GetFlagBit(FLAG_BIT_OF));

    this->SetDesOpnd1Val(type,&addr,&val);

    return OK;
}
int CVm::do_movsb()
{
    DWORD edi,esi;
    BYTE b;

    edi = this->GetReg(REG_EDI)->val.val_32;
    esi = this->GetReg(REG_ESI)->val.val_32;

    b = this->i_vmem->Read8Mem(esi);
    this->i_vmem->Write8Mem(edi,b);

    if(this->GetFlagBit(FLAG_BIT_DF))
    {
        this->GetReg(REG_EDI)->val.val_32 --;
        this->GetReg(REG_ESI)->val.val_32 --;
    }
    else
    {
        this->GetReg(REG_EDI)->val.val_32 ++;
        this->GetReg(REG_ESI)->val.val_32 ++;
    }

    return OK;
}
int CVm::do_adc()
{
    _RegVal opnd1,val,opnd2,result;
    long type,eip;
    BOOL cf,of;
    DWORD cf_bit;

    eip = this->GetEIP();
    this->GetOpnd1(&opnd1);
    this->SetEIP(eip);

    type = this->GetOpnd_Common(0,&val,1);
    this->GetOpnd2(&opnd2);

    cf_bit = this->GetFlagBit(FLAG_BIT_CF);
    //带进位位的加法
    result.val_32 = opnd1.val_32 + opnd2.val_32 + cf_bit;

    this->SetFlagBit(FLAG_BIT_ZF,result.val_32 == 0);
    this->SetFlagBit(FLAG_BIT_SF,(int)(result.val_32) < 0);
        
    cf = CMisc::add_overflow(opnd1.val_32 , opnd2.val_32);
    if(!cf)
    {
        DWORD mid_result = opnd1.val_32 + opnd2.val_32;
        cf = CMisc::add_overflow(mid_result,cf_bit);
    }

    of = CMisc::add_overflow(int(opnd1.val_32),int(opnd2.val_32));
    if(!of)
    {
        int mid_result = int(opnd1.val_32) + int(opnd2.val_32);
        of = CMisc::add_overflow(mid_result,int(cf_bit));
    }

    this->SetFlagBit(FLAG_BIT_CF,cf);
    this->SetFlagBit(FLAG_BIT_OF,of);

    this->SetDesOpnd1Val(type,&val,&result);

    return OK;
}

int CVm::do_myadd()
{
    _RegVal opnd1,val,opnd2,result;
    long type,eip;

    eip = this->GetEIP();
    this->GetOpnd1(&opnd1);
    this->SetEIP(eip);

    type = this->GetOpnd_Common(0,&val,1);
    this->GetOpnd2(&opnd2);

    result.val_32 = opnd1.val_32 + opnd2.val_32;

    this->SetDesOpnd1Val(type,&val,&result);

    return OK;
}
int CVm::do_mysub()
{
    _RegVal opnd1,val,opnd2,result;
    long type,eip;  

    eip = this->GetEIP();
    this->GetOpnd1(&opnd1);
    this->SetEIP(eip);

    type = this->GetOpnd_Common(0,&val,1);
    this->GetOpnd2(&opnd2);

    result.val_32 = opnd1.val_32 - opnd2.val_32;

    this->SetDesOpnd1Val(type,&val,&result);

    return OK;
}
int CVm::do_mymul()
{
    _RegVal opnd1,val,opnd2,result;
    long type,eip;
    
    eip = this->GetEIP();
    this->GetOpnd1(&opnd1);
    this->SetEIP(eip);

    type = this->GetOpnd_Common(0,&val,1);
    this->GetOpnd2(&opnd2);

    result.val_32 = opnd1.val_32 * opnd2.val_32;

    this->SetDesOpnd1Val(type,&val,&result);

    return OK;
}
int CVm::do_mydiv()
{
    return do_div();
}
int CVm::do_scasb()
{
    DWORD edi;
    BYTE b1,b2,r;
    _RegVal val;

    edi = this->GetReg(REG_EDI)->val.val_32;
    
    this->GetReg(REG_AL)->GetVal(&val);

    b1 = this->i_vmem->Read8Mem(edi);
    b2 = (BYTE)(val.val_32);
    
    r = b1 - b2;

    this->SetFlagBit(FLAG_BIT_ZF,r == 0);
    this->SetFlagBit(FLAG_BIT_SF,r & 0x80);
    this->SetFlagBit(FLAG_BIT_OF,CMisc::sub_overflow((char)b1,(char)b2));
    this->SetFlagBit(FLAG_BIT_CF,CMisc::sub_overflow(b1,b2));
    
    if(this->GetFlagBit(FLAG_BIT_DF))
    {
        this->GetReg(REG_EDI)->val.val_32 --;
    }
    else
    {
        this->GetReg(REG_EDI)->val.val_32 ++;
    }

    return OK;
}
int CVm::do_scasw()
{
    DWORD edi;
    WORD w1,w2,r;
    _RegVal val;

    edi = this->GetReg(REG_EDI)->val.val_32;
    
    this->GetReg(REG_AX)->GetVal(&val);

    w1 = this->i_vmem->Read16Mem(edi);
    w2 = (WORD)(val.val_32);
    
    r = w1 - w2;

    this->SetFlagBit(FLAG_BIT_ZF,r == 0);
    this->SetFlagBit(FLAG_BIT_SF,r & 0x8000);
    this->SetFlagBit(FLAG_BIT_OF,CMisc::sub_overflow((short)w1,(short)w2));
    this->SetFlagBit(FLAG_BIT_CF,CMisc::sub_overflow(w1,w2));
    
    if(this->GetFlagBit(FLAG_BIT_DF))
    {
        this->GetReg(REG_EDI)->val.val_32 -= 2;
    }
    else
    {
        this->GetReg(REG_EDI)->val.val_32 += 2;
    }

    return OK;
}
int CVm::do_scasd()
{
    DWORD edi;
    DWORD dw1,dw2,r;
    _RegVal val;

    edi = this->GetReg(REG_EDI)->val.val_32;
    
    this->GetReg(REG_EAX)->GetVal(&val);

    dw1 = this->i_vmem->Read32Mem(edi);
    dw2 = val.val_32;
    
    r = dw1 - dw2;

    this->SetFlagBit(FLAG_BIT_ZF,r == 0);
    this->SetFlagBit(FLAG_BIT_SF,r & 0x80000000);
    this->SetFlagBit(FLAG_BIT_OF,CMisc::sub_overflow((int)dw1,(int)dw2));
    this->SetFlagBit(FLAG_BIT_CF,CMisc::sub_overflow(dw1,dw2));
    
    if(this->GetFlagBit(FLAG_BIT_DF))
    {
        this->GetReg(REG_EDI)->val.val_32 -= 4;
    }
    else
    {
        this->GetReg(REG_EDI)->val.val_32 += 4;
    }

    return OK;
}
int CVm::do_cmpsb()
{
    DWORD edi,esi;
    BYTE b1,b2,r;

    edi = this->GetReg(REG_EDI)->val.val_32;
    esi = this->GetReg(REG_ESI)->val.val_32;

    b1 = this->i_vmem->Read8Mem(edi);
    b2 = this->i_vmem->Read8Mem(esi);

    r = b1 - b2;

    this->SetFlagBit(FLAG_BIT_ZF,r == 0);
    this->SetFlagBit(FLAG_BIT_SF,r & 0x80);
    this->SetFlagBit(FLAG_BIT_OF,CMisc::sub_overflow((char)b1,(char)b2));
    this->SetFlagBit(FLAG_BIT_CF,CMisc::sub_overflow(b1,b2));
    
    if(this->GetFlagBit(FLAG_BIT_DF))
    {
        this->GetReg(REG_EDI)->val.val_32 --;
        this->GetReg(REG_ESI)->val.val_32 --;
    }
    else
    {
        this->GetReg(REG_EDI)->val.val_32 ++;
        this->GetReg(REG_ESI)->val.val_32 ++;
    }

    return OK;
}
int CVm::do_cmpsw()
{
    DWORD edi,esi;
    WORD w1,w2,r;

    edi = this->GetReg(REG_EDI)->val.val_32;
    esi = this->GetReg(REG_ESI)->val.val_32;

    w1 = this->i_vmem->Read16Mem(edi);
    w2 = this->i_vmem->Read16Mem(esi);

    r = w1 - w2;

    this->SetFlagBit(FLAG_BIT_ZF,r == 0);
    this->SetFlagBit(FLAG_BIT_SF,r & 0x8000);
    this->SetFlagBit(FLAG_BIT_OF,CMisc::sub_overflow((short)w1,(short)w2));
    this->SetFlagBit(FLAG_BIT_CF,CMisc::sub_overflow(w1,w2));
    
    if(this->GetFlagBit(FLAG_BIT_DF))
    {
        this->GetReg(REG_EDI)->val.val_32 -= 2;
        this->GetReg(REG_ESI)->val.val_32 -= 2;
    }
    else
    {
        this->GetReg(REG_EDI)->val.val_32 += 2;
        this->GetReg(REG_ESI)->val.val_32 += 2;
    }

    return OK;
}
int CVm::do_cmpsd()
{
    DWORD edi,esi;
    DWORD dw1,dw2,r;

    edi = this->GetReg(REG_EDI)->val.val_32;
    esi = this->GetReg(REG_ESI)->val.val_32;

    dw1 = this->i_vmem->Read32Mem(edi);
    dw2 = this->i_vmem->Read32Mem(esi);

    r = dw1 - dw2;

    this->SetFlagBit(FLAG_BIT_ZF,r == 0);
    this->SetFlagBit(FLAG_BIT_SF,r & 0x80000000);
    this->SetFlagBit(FLAG_BIT_OF,CMisc::sub_overflow((int)dw1,(int)dw2));
    this->SetFlagBit(FLAG_BIT_CF,CMisc::sub_overflow(dw1,dw2));
    
    if(this->GetFlagBit(FLAG_BIT_DF))
    {
        this->GetReg(REG_EDI)->val.val_32 -= 4;
        this->GetReg(REG_ESI)->val.val_32 -= 4;
    }
    else
    {
        this->GetReg(REG_EDI)->val.val_32 += 4;
        this->GetReg(REG_ESI)->val.val_32 += 4;
    }
    return OK;
}
int CVm::do_lahf()
{
    BYTE *p = this->GetReg(REG_AH)->val.ptr_8;
    
    ASSERT(p);

    *p = (BYTE)(this->GetReg(REG_FLAG)->val.val_32);

    return OK;
}
int CVm::do_sahf()
{
    BYTE *p = this->GetReg(REG_AH)->val.ptr_8;
    
    ASSERT(p);

    char *pf = (char*)&(this->GetReg(REG_FLAG)->val.val_32);
    
    *pf = *p;

    return OK;
}
int CVm::do_wait()
{
    //do nothing
    return OK;
}
int CVm::do_leave()
{
    this->GetReg(REG_ESP)->val.val_32 = this->GetReg(REG_EBP)->val.val_32;
    this->GetReg(REG_EBP)->val.val_32 = this->PopDWord();

    return OK;
}
int CVm::do_fnstcw()
{
    _RegVal addr,val;

    int type = this->GetOpnd_Common(0,&addr,1);
    
    if(type >= 0)type = 2; //only word size.

    val.val_32 = this->fpu_control;
    
    this->SetDesOpnd1Val(type,&addr,&val);

    return OK;
}
int CVm::do_fstcw()
{
    return this->do_fnstcw();
}
int CVm::do_fldcw()
{
    _RegVal val;

    this->GetOpnd1(&val);

    this->fpu_control = val.val_32;

    return OK;
}
int CVm::do_fstsw()
{
    _RegVal addr,val;

    int type = this->GetOpnd_Common(0,&addr,1);
    
    if(type >= 0) type = 2; //only word size

    val.val_32 = this->fpu_status;

    this->SetDesOpnd1Val(type,&addr,&val);
    
    return OK;
}
int CVm::do_fnstsw()
{
    this->do_fstsw();
    return OK;
}
int CVm::do_fclex()
{
    this->fpu_status &= 0xffffff00;

    return OK;
}
int CVm::do_fxtract()
{
    double *p,s,e;
    int n;
    QWORD *qword;

    p = (double *)&(this->GetFPU(0)->val.val_64);

    s = frexp(*p,&n) * 2.0;
    e = n - 1;  //convert int to double

    *p = e;
    
    qword = (QWORD*)&s;

    this->PushFPU(*qword);

    return OK;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVm::CVm()
{
    this->InitBasic();
}
CVm::~CVm()
{
    this->Destroy();
}
int CVm::InitBasic()
{
    this->i_vmem = NULL;
    this->reg_tab = NULL;
    this->reg_index = NULL;
    this->end_flag = FALSE;
    this->asm_tab = NULL;
    
    this->fpu_top = 7;
    this->fpu_control = 0;
    this->fpu_status = 0;
    this->fpu_tag = 0;

    return OK;
}
int CVm::Init()
{
    this->InitBasic();
    
    NEW(this->reg_tab,CRegTab);
    this->reg_tab->InitDefault();
    
    NEW(this->asm_tab,CAsmTab);
    this->asm_tab->InitDefault();

    MALLOC(this->reg_index,CReg *,this->reg_tab->GetLen());
    for(int i = 0; i < this->reg_tab->GetLen(); i++)
    {
        this->reg_index[i] = NULL;
    }
    this->MapAllRegs();
    
    return OK;
}
int  CVm::Destroy()
{
    FREE(this->reg_index);
    DEL(this->reg_tab);

    DEL(this->asm_tab);

    this->InitBasic();
    
    return OK;
}
int  CVm::Print()
{
    return TRUE;
}

int CVm::MapAllRegs()
{
    CReg *p;
    
    for(int i = 0; i < this->reg_tab->GetLen(); i++)
    {
        p = this->reg_tab->GetElem(i);
        ASSERT(p);
        
        p = this->reg_tab->BSearch(p->name);
        ASSERT(p);
        
        this->reg_index[p->index] = p;
    }
    return OK;
}

inline DWORD CVm::GetEIP()
{   
    return this->reg_index[REG_EIP]->val.val_32;
}

int CVm::SetEIP(DWORD v)
{
    this->reg_index[REG_EIP]->val.val_32 = v;   
    return OK;
}

DWORD CVm::GetNextCode()
{
    DWORD eip,ret;
    
    eip = this->GetEIP();
    
    ret = this->i_vmem->Read32Mem(eip);
    
    eip += sizeof(DWORD);
    
    this->SetEIP(eip);
    
    return ret; 
}
////////////////////////////////////////////////////////
int CVm::GetNextOper()
{
    *((DWORD*)(this->cur_oper)) = this->GetNextCode();
    *((DWORD*)(this->cur_oper) + 1) = this->GetNextCode();

    return OK;
}

int CVm::Execute()
{
    while(!end_flag)
    {
        this->GetNextOper();
#ifdef _PRTREG
        this->reg_tab->Print();
#endif
#ifdef _DEASM
        this->Bin2Asm(0);
#endif
        this->ExeOper();        
#ifdef _STEP
        if(getch()== 27)
            break;
#endif
    }

    return OK;
}

int CVm::Bin2Asm(BOOL change_eip)
{
    BYTE *byte;
    WORD oper;
    CAsmStmt *pstmt;
    int opnd_num;
    int data_type1,data_type2;
    BOOL bracket1,bracket2;
    BOOL opnd_type1,opnd_type2;
    DWORD old_eip;
    CMem mem_buf;

    if(!change_eip)
        old_eip = this->GetEIP();

    LOCAL_MEM(mem_buf);
    byte = this->cur_oper;

    oper = *((WORD*)byte);

    pstmt = this->asm_tab->GetElemByOperNum(oper);
    ASSERT(pstmt);

    LOG("%s ",pstmt->name); //打印操作符

    if(CAsmMisc::IsRepStmt(pstmt->name))
    {
        CAsmMisc::RepTypeToStr(byte[2],&mem_buf);
        LOG("%s\n",mem_buf.p);
        return OK;
    }

    opnd_num = this->GetCurOpndNum();
    
    data_type1 = byte[3] & 0x07;
    data_type2 = (byte[3]>>3) & 0x07;
    
    bracket1 = (byte[3]>>6) & 1;
    bracket2 = (byte[3]>>7) & 1;
    
    opnd_type1 = byte[4] & 0x03;
    opnd_type2 = (byte[4]>>2) & 0x03;
    
    if(opnd_num >= 1)
    {
        
        CAsmMisc::OpndDataType2Str(data_type1,&mem_buf);
        LOG("%s",mem_buf.p);

        if(bracket1) LOG("[");

        if(opnd_type1 == 1)
        {
            CReg *preg = this->reg_tab->GetRegFromNum(byte[5]);
            ASSERT(preg);

            LOG("%s",preg->name);
        }
        else if(opnd_type1 == 2)
        {
            int opnd1;

            opnd1 = this->GetNextCode();

            LOG("%x",opnd1);
        }

        if(bracket1) LOG("]");
    }
    if(opnd_num >= 2)
    {
        LOG(",");
    
        CAsmMisc::OpndDataType2Str(data_type2,&mem_buf);
        LOG("%s",mem_buf.p);
        if(bracket2) LOG("[");

        if(opnd_type2 == 1)
        {
            CReg *preg = this->reg_tab->GetRegFromNum(byte[6]);
            ASSERT(preg);

            LOG("%s",preg->name);
        }
        else if(opnd_type2 == 2)
        {
            int opnd2;

            opnd2 = this->GetNextCode();

            LOG("%x",opnd2);
        }
        if(bracket2) LOG("]");
    }
    LOG("\r\n");
    
    if(!change_eip)
        this->SetEIP(old_eip);

    return OK;
}

int CVm::GetOpnd1(_RegVal *val)
{
    int type = this->GetOpnd_Common(0,val,0);
    
    if(type == 1)
        val->val_32 = (char)(val->val_32);
    else if(type == 2)
        val->val_32 = (short)(val->val_32);

    return type;
}
int CVm::GetOpnd2(_RegVal *val)
{
    int type = this->GetOpnd_Common(1,val,0);
    
    if(type == 1)
        val->val_32 = (char)(val->val_32);
    else if(type == 2)
        val->val_32 = (short)(val->val_32);

    return type;

}

//返回1,2,4,8 表示数据的byte数
//is_des_opnd 表示是否是目的操作数，如果是的话，那么
//返回-1表示val->val_32中存放寄存器的编号
//返回其他的数，表示val->val_32中存放的是内存的地址
//对于浮点寄存器也,如果是得到其中的值，那么已经自动转换好了
//如果要得到编号，则得到的是在所有寄存器中的编号，而不是
//在浮点堆栈中的编号
int CVm::GetOpnd_Common(int option, _RegVal *val,int is_des_opnd)
{
    BYTE *byte,bt;
    int ret;

    byte = this->cur_oper;

    if(option == 0)
        bt = byte[3] & 0x07;
    else 
        bt = (byte[3] & 0x38)>>3;

    switch(bt)
    {
        case 0: ret = 4; break;
        case 1: ret = 1; break;
        case 2: ret = 2; break;
        case 3: ret = 4; break;
        case 4: ret = 8; break;
        default : ret = 4; break;
    }

    if(option == 0)
        bt = byte[4] & 0x03;
    else
        bt = (byte[4]>>2) & 0x03;

    //如果是寄存器
    if( bt == 1 )
    {
        if(option == 0)
            bt = BYTE(byte[3] & (1<<6));
        else
            bt = BYTE(byte[3] & (1<<7));
        //没有[]，单个寄存器的情况
        if( bt == 0x00 )
        {
            if(option == 0)
                bt = byte[5];
            else
                bt = byte[6];
            
            if(is_des_opnd)
            {
                ret = -1;  //表示返回寄存器
                val->val_32 = bt; //返回寄存器的编号
            }
            else
            {
                //浮点寄存器需要特殊处理，不能仅仅根据编号得到浮点寄存器
                //需要经过转换
                int fpu_reg = this->RegNumToFpuNum(bt);
                if( fpu_reg >= 0)
                {
                    ret = this->GetFPU(fpu_reg)->bytes;
                    this->GetFPU(fpu_reg)->GetVal(val);
                }
                else
                {
                    ret = this->GetReg(bt)->bytes; 
                    this->GetReg(bt)->GetVal(val);
                }
            }
            goto end;
        }
        else //比如 DWORD PTR [eax]的情况
        {
            if(option == 0)
                bt = byte[5];
            else
                bt = byte[6];
            
            if(is_des_opnd)
            {
                this->GetReg(bt)->GetVal(val);
            }
            else
            {
                _RegVal addr;

                this->GetReg(bt)->GetVal(&addr);
                val->val_64 = 0;
                this->i_vmem->ReadMem(addr.val_32,val,ret);
            }
            goto end;
        }
    }
    else if(bt == 2) //如果不是寄存器
    {
        if(option == 0)
            bt = (BYTE)(byte[3] & (1<<6));
        else
            bt = (BYTE)(byte[3] & (1<<7));
        //没有[]
        if( bt == 0x00)
        {
            val->val_32 = this->GetNextCode();
            goto end;
        }
        else
        {
            long addr = this->GetNextCode();
            
            if(is_des_opnd)
            {
                val->val_32 = addr;
            }
            else
            {
                val->val_64 = 0;                                
                this->i_vmem->ReadMem(addr,val,ret);
            }
            goto end;
        }
    }
end:
    return ret;
}

int CVm::PushDWord(DWORD dw)
{
    long ss;

    ss = this->reg_index[REG_ESP]->val.val_32;
    
    ss -= 4;

    this->i_vmem->Write32Mem(ss,dw);

    this->reg_index[REG_ESP]->val.val_32 = ss;

    return OK;
}

DWORD CVm::PopDWord()
{
    long ss;
    DWORD dw;

    ss = this->reg_index[REG_ESP]->val.val_32;  

    dw = this->i_vmem->Read32Mem(ss);

    this->reg_index[REG_ESP]->val.val_32 = ss + 4;

    return dw;
}

int CVm::SetDesOpnd1Val(int bytes, _RegVal *addr, _RegVal *val)
{
    if(bytes == -1) //寄存器
    {
        this->GetReg(addr->val_32)->SetVal(val);
    }
    else
    {
        this->i_vmem->WriteMem(addr->val_32,val,bytes); 
    }

    return OK;
}

int CVm::SetDesOpndFloatVal(int bytes, _RegVal *addr, _RegVal *val)
{
    if(bytes < 0)
    {
        CReg *p = this->GetFpuRegByRegNum(addr->val_32);
        ASSERT(p);

        p->val.val_64 = val->val_64;
    }
    else
    {
        this->SetDesOpnd1Val(bytes,addr,val);
    }
    
    return OK;
}


inline CReg * CVm::GetReg(long index)
{
    ASSERT(index >= 0 && index < this->reg_tab->GetLen());

    return this->reg_index[index];
}

inline int CVm::SetFlagBit(int bit_index,BOOL onebit)
{
    if(onebit)
        this->reg_index[REG_FLAG]->val.val_32 |= (1<<bit_index);
    else
        this->reg_index[REG_FLAG]->val.val_32 &= (~(1<<bit_index));

    return OK;
}

inline int CVm::GetFpuStatusBit(int bit_index)
{
    return (this->fpu_status >> bit_index) & 0x01;
}
inline int CVm::SetFpuStatusBit(int bit_index,BOOL onebit)
{
    if(onebit)
        this->fpu_status |= (1<<bit_index);
    else
        this->fpu_status &= (~(1<<bit_index));

    return OK;
}

inline int CVm::GetFlagBit(int bit_index)
{
    return ((this->reg_index[REG_FLAG]->val.val_32) >> bit_index) & 0x01;
}
int CVm::ExeOper()
{
    int oper = *((WORD*)(this->cur_oper));

    switch(oper)
    {       
        case OPER_ADC:do_adc();break;
        case OPER_ADD:do_add();break;
        case OPER_AND:do_and();break;
        case OPER_CALL:do_call();break;
        case OPER_CDQ:do_cdq();break;
        case OPER_CMP:do_cmp();break;
        case OPER_CMPSB:do_cmpsb();break;
        case OPER_CMPSD:do_cmpsd();break;
        case OPER_CMPSW:do_cmpsw();break;
        case OPER_DEC:do_dec();break;
        case OPER_DIV:do_div();break;
        case OPER_FADD:do_fadd();break;
        case OPER_FCOS:do_fcos();break;
        case OPER_FILD:do_fild();break;
        case OPER_FLD:do_fld();break;
        case OPER_FMUL:do_fmul();break;
        case OPER_FSIN:do_fsin();break;
        case OPER_FST:do_fst();break;
        case OPER_FSTP:do_fstp();break;
        case OPER_FSUB:do_fsub();break;
        case OPER_FXCH:do_fxch();break;
        case OPER_IDIV:do_idiv();break;
        case OPER_IMUL:do_imul();break;
        case OPER_INC:do_inc();break;
        case OPER_INT:do_int();break;
        case OPER_JA:do_ja();break;
        case OPER_JB:do_jb();break;
        case OPER_JBE:do_jbe();break;
        case OPER_JE:do_je();break;
        case OPER_JG:do_jg();break;
        case OPER_JGE:do_jge();break;
        case OPER_JL:do_jl();break;
        case OPER_JLE:do_jle();break;
        case OPER_JMP:do_jmp();break;
        case OPER_JNE:do_jne();break;
        case OPER_JNS:do_jns();break;
        case OPER_JS:do_js();break;
        case OPER_LEA:do_lea();break;
        case OPER_MOV:do_mov();break;
        case OPER_MOVSB:do_movsb();break;
        case OPER_MOVSD:do_movsd();break;
        case OPER_MOVSW:do_movsw();break;
        case OPER_MOVSX:do_movsx();break;
        case OPER_MUL:do_mul();break;
        case OPER_MYADD:do_myadd();break;
        case OPER_MYDIV:do_mydiv();break;
        case OPER_MYMUL:do_mymul();break;
        case OPER_MYSUB:do_mysub();break;
        case OPER_NEG:do_neg();break;
        case OPER_NOT:do_not();break;
        case OPER_NPAD:do_npad();break;
        case OPER_OR:do_or();break;
        case OPER_POP:do_pop();break;
        case OPER_PUSH:do_push();break;
        case OPER_REP:do_rep();break;
        case OPER_REPNE:do_repne();break;
        case OPER_RET:do_ret();break;
        case OPER_SAR:do_sar();break;
        case OPER_SBB:do_sbb();break;
        case OPER_SCASB:do_scasb();break;
        case OPER_SCASD:do_scasd();break;
        case OPER_SCASW:do_scasw();break;
        case OPER_SETE:do_sete();break;
        case OPER_SETG:do_setg();break;
        case OPER_SETGE:do_setge();break;
        case OPER_SETL:do_setl();break;
        case OPER_SETLE:do_setle();break;
        case OPER_SETNE:do_setne();break;
        case OPER_SHL:do_shl();break;
        case OPER_SHR:do_shr();break;
        case OPER_STOSB:do_stosb();break;
        case OPER_STOSD:do_stosd();break;
        case OPER_STOSW:do_stosw();break;
        case OPER_SUB:do_sub();break;
        case OPER_TEST:do_test();break;
        case OPER_XOR:do_xor();break;
        case OPER_JAE:do_jae();break;
        case OPER_FSUBR:do_fsubr();break;
        case OPER_FDIV:do_fdiv();break;
        case OPER_FADDP:do_faddp();break;
        case OPER_FDIVR:do_fdivr();break;
        case OPER_FIADD:do_fiadd();break;
        case OPER_FMULP:do_fmulp();break;
        case OPER_FSUBP:do_fsubp();break;
        case OPER_FISUB:do_fisub();break;
        case OPER_FISUBR:do_fisubr();break;
        case OPER_FIMUL:do_fimul();break;
        case OPER_FSUBRP:do_fsubrp();break;
        case OPER_FCHS:do_fchs();break;
        case OPER_FABS:do_fabs();break;
        case OPER_FDIVP:do_fdivp();break;
        case OPER_FIDIV:do_fidiv();break;
        case OPER_FIDIVR:do_fidivr();break;
        case OPER_FDIVRP:do_fdivrp();break;
        case OPER_WAIT:do_wait();break;
        case OPER_LEAVE:do_leave();break;
        case OPER_FNSTCW:do_fnstcw();break;
        case OPER_FLDCW:do_fldcw();break;
        case OPER_FISTP:do_fistp();break;
        case OPER_FCOMP:do_fcomp();break;
        case OPER_FCOM:do_fcom();break;
        case OPER_FIST:do_fist();break;
        case OPER_FNSTSW:do_fnstsw();break;
        case OPER_FSTSW:do_fstsw();break;
        case OPER_FPTAN:do_fptan();break;
        case OPER_FPATAN:do_fpatan();break;     
        case OPER_FSQRT:do_fsqrt();break;
        case OPER_FLD1:do_fld1();break;
        case OPER_FLDZ:do_fldz();break;
        case OPER_FLDPI:do_fldpi();break;
        case OPER_FLDL2E:do_fldl2e();break;
        case OPER_FLDL2T:do_fldl2t();break;
        case OPER_FLDLG2:do_fldlg2();break;
        case OPER_FLDLN2:do_fldln2();break;
        case OPER_FRNDINT:do_frndint();break;
        case OPER_FSCALE:do_fscale();break;
        case OPER_F2XM1:do_f2xm1();break;
        case OPER_LAHF:do_lahf();break;
        case OPER_SAHF:do_sahf();break;
        case OPER_FYL2X:do_fyl2x();break;
        case OPER_FXAM:do_fxam();break;
        case OPER_FCLEX:do_fclex();break;
        case OPER_JZ:do_jz();break;
        case OPER_FPREM:do_fprem();break;
        case OPER_JNZ:do_jnz();break;
        case OPER_FSTCW:do_fstcw();break;
        case OPER_FTST:do_ftst();break;
        case OPER_FXTRACT:do_fxtract();break;
        case OPER_REPE:do_repe();break;
        default: this->runtime_error("error oper %d\n",oper);break;
    }

    return OK;
}

CReg * CVm::GetFPU(int index)
{
    ASSERT(index >= 0 && index <= 7);

    switch((this->fpu_top + index) % 8)
    {
        case 0: return this->GetReg(REG_ST0);break;
        case 1: return this->GetReg(REG_ST1);break;
        case 2: return this->GetReg(REG_ST2);break;
        case 3: return this->GetReg(REG_ST3);break;
        case 4: return this->GetReg(REG_ST4);break;
        case 5: return this->GetReg(REG_ST5);break;
        case 6: return this->GetReg(REG_ST6);break;
        case 7: return this->GetReg(REG_ST7);break;
    }

    this->runtime_error("error index in GetFPU\n");

    return NULL;
}

QWORD CVm::PopFPU()
{
    CReg *p = this->GetFPU(0);
    ASSERT(p);
    
    this->fpu_top  = (this->fpu_top + 1) % 8;

    return p->val.val_64;
}

int CVm::PushFPU(QWORD v)
{
    this->fpu_top  = (this->fpu_top - 1 + 8) % 8;

    CReg *p = this->GetFPU(0);
    ASSERT(p);

    p->val.val_64 = v;

    return OK;
}

int CVm::GetCurOpndNum()
{
    int s = 0;

    if(this->cur_oper[4] & 0x03) s++;
    if((this->cur_oper[4] >> 2) & 0x03) s++;

    return s;
}

int CVm::RegNumToFpuNum(int reg_num)
{
    switch(reg_num)
    {
        case REG_ST0:return 0;break;
        case REG_ST1:return 1;break;
        case REG_ST2:return 2;break;
        case REG_ST3:return 3;break;
        case REG_ST4:return 4;break;
        case REG_ST5:return 5;break;
        case REG_ST6:return 6;break;
        case REG_ST7:return 7;break;
    }

    return -1;
}

CReg * CVm::GetFpuRegByRegNum(int reg_num)
{
    CReg *p = this->GetFPU(this->RegNumToFpuNum(reg_num));
    
    ASSERT(p);

    return p;
}

int CVm::runtime_error(char *szFormat, ...)
{
    char szBuffer [2048] ;

    va_list pArgList ;
    va_start (pArgList, szFormat) ;
    vsprintf (szBuffer, szFormat, pArgList) ;
    va_end (pArgList) ;
    
    LOG("runtime error: %s",szBuffer);
    
    this->end_flag = true;
    return OK;  
}

int CVm::GetFPUControlRC()
{
    return (this->fpu_control & 0x00000C00) >> 10;
}
