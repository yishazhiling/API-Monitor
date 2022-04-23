//LDisEngine.h 
//反汇编引擎，引用LDisEngine.lib库

#pragma once

#pragma comment(lib,"LDisEngine.lib")


typedef struct _STINSTRUCTION
{
	BYTE	bPrefix1;
	BYTE	bPrefix2;
	BYTE	bPrefix3;
	BYTE	bPrefix4;
	ULONG	dwPrefixLength;
	BYTE	bOpcode1;
	BYTE	bOpcode2;
	BYTE	bOpcode3;
	BYTE	bOpcode4;
	ULONG	dwOpcodeLength;
	BYTE	bModRM;
	BYTE	bSIB;
	ULONG	dwDisplacement;
	WORD	wSegment;
	ULONG	dwImmediate;
	ULONG	dwImmediateLength;
	ULONG	dwLength;
}STINSTRUCTION, *PSTINSTRUCTION, **PPSTINSTRUCTION;

//名称：InitLDisEngine
//功能：反汇编引擎初始化
//参数：无
//返回：无
VOID	_stdcall InitLDisEngine();

//名称：LDisEngine
//功能：反汇编
//参数：_pPointer = 要反汇编的代码指针
//参数：_pstInstruction = 反汇编后的数据写入的INSTRUCTION结构地址
//返回：反汇编指令的长度
ULONG	_stdcall LDisEngine(PVOID _pPointer, PSTINSTRUCTION _pstInstruction);