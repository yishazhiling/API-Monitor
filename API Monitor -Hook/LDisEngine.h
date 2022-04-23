//LDisEngine.h 
//��������棬����LDisEngine.lib��

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

//���ƣ�InitLDisEngine
//���ܣ�����������ʼ��
//��������
//���أ���
VOID	_stdcall InitLDisEngine();

//���ƣ�LDisEngine
//���ܣ������
//������_pPointer = Ҫ�����Ĵ���ָ��
//������_pstInstruction = �����������д���INSTRUCTION�ṹ��ַ
//���أ������ָ��ĳ���
ULONG	_stdcall LDisEngine(PVOID _pPointer, PSTINSTRUCTION _pstInstruction);