// Vm.h: interface for the CVm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VM_H__AB27D9BE_8A7B_400C_9E88_DE4308C30EE9__INCLUDED_)
#define AFX_VM_H__AB27D9BE_8A7B_400C_9E88_DE4308C30EE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "vmem.h"
#include "regtab.h"
#include "common.h" // Added by ClassView
#include "asmtab.h"

class CVm{
public:
    int end_flag;
    CVMem *i_vmem;
    CRegTab *reg_tab;
    CReg **reg_index;
    CAsmTab *asm_tab;
    BYTE cur_oper[8];
    int step;
    ///////FPU Registers/////////
    DWORD fpu_top;
    DWORD fpu_status;
    DWORD fpu_control;
    DWORD fpu_tag;
public:
    int GetFPUControlRC();
    int runtime_error(char *szFormat, ...);
    int SetDesOpndFloatVal(int bytes, _RegVal *addr, _RegVal *val);
    int GetFpuStatusBit(int bit_index);
    int SetFpuStatusBit(int bit_index,BOOL onebit);
    CReg * GetFpuRegByRegNum(int reg_num);
    int RegNumToFpuNum(int reg_num);
    int GetCurOpndNum();
    QWORD PopFPU();
    CReg * GetFPU(int index);
    int PushFPU(QWORD v);
    int ExeOper();
    DWORD PopDWord();
    int GetFlagBit(int bit_index);
    int SetFlagBit(int bit_index,BOOL onebit);
    virtual CReg * GetReg(long index);
    int SetDesOpnd1Val(int bytes,_RegVal *addr,_RegVal *val);
    int PushDWord(DWORD dw);
    int GetOpnd2(_RegVal *val);
    int GetOpnd_Common(int option,_RegVal *val,int is_des_opnd);
    int GetOpnd1(_RegVal *val);
    int Bin2Asm(BOOL change_eip);
    int Execute();
    int GetNextOper();
    DWORD GetNextCode();
    int SetEIP(DWORD v);
    DWORD GetEIP();
    int MapAllRegs();
    CVm();
    ~CVm();
    int Init();
    int Destroy();
    int Print();
    int InitBasic();
    /////////////////////////////
    int do_repe();
    int do_fxtract();
    int do_ftst();
    int do_fstcw();
    int do_jnz();
    int do_fprem();
    int do_jz();
    int do_fclex();
    int do_fxam();
    int do_fyl2x();
    int do_lahf();
    int do_sahf();
    int do_fscale();
    int do_f2xm1();
    int do_frndint();
    int do_fldz();
    int do_fldpi();
    int do_fldl2t();
    int do_fldlg2();
    int do_fldln2();
    int do_fsqrt();
    int do_fld1();
    int do_fptan();
    int do_fpatan();
    int do_fnstsw();
    int do_fstsw();
    int do_fcom();
    int do_fcomp();
    int do_fist();
    int do_fistp();
    int do_fldcw();
    int do_fnstcw();
    int do_leave();
    int do_wait();
    int do_mov();
    int do_push();
    int do_pop();
    int do_call();
    int do_lea();
    int do_ret();
    int do_test();
    int do_xor();
    int do_rep();
    int do_or();
    int do_add();
    int do_sub();
    int do_mul();
    int do_div();
    int do_jle();
    int do_jne();
    int do_jmp();
    int do_je();
    int do_jge();
    int do_cmp();
    int do_jg();
    int do_inc();
    int do_jl();
    int do_cdq();
    int do_idiv();
    int do_shr();
    int do_and();
    int do_movsx();
    int do_ja();
    int do_jae();
    int do_npad();
    int do_dec();
    int do_shl();
    int do_sar();
    int do_imul();
    int do_not();
    int do_neg();
    int do_sete();
    int do_repne();
    int do_sbb();
    int do_jbe();
    int do_jb();
    int do_fabs();
    int do_fldl2e();
    int do_fdivrp();
    int do_fidivr();
    int do_fiadd();
    int do_fild();
    int do_fmulp();
    int do_fmul();
    int do_fidiv();
    int do_fdiv();
    int do_fdivp();
    int do_fstp();
    int do_fld();
    int do_fsin();
    int do_fadd();
    int do_fsubrp();
    int do_fsub();
    int do_fimul();
    int do_fisub();
    int do_fisubr();
    int do_fxch();
    int do_fcos();
    int do_fdivr();
    int do_stosw();
    int do_stosb();
    int do_fsubr();
    int do_fsubp();
    int do_movsw();
    int do_setne();
    int do_jns();
    int do_fchs();
    int do_setge();
    int do_js();
    int do_setg();
    int do_setl();
    int do_fst();
    int do_setle();
    int do_movsb();
    int do_adc();
    int do_myadd();
    int do_mysub();
    int do_mymul();
    int do_mydiv();
    int do_movsd();
    int do_stosd();
    int do_scasb();
    int do_scasw();
    int do_scasd();
    int do_cmpsb();
    int do_cmpsw();
    int do_cmpsd();
    int do_faddp();
    virtual int do_int() = 0;
};

#endif // !defined(AFX_VM_H__AB27D9BE_8A7B_400C_9E88_DE4308C30EE9__INCLUDED_)
