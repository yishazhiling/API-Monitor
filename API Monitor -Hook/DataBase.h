//DataBase.h And DataBase.c
//���ܺ���������һ��������+����+������ɵ�һ�����ݿ������,�ɴ�C��ɣ��������

#include <windows.h>

#pragma once 

#define		cDB_MaxNumofSTCodeBuffer	64							//����STDataBase�ṹ�е�CodeBuffer�ĳ���
#define		cDB_MaxNumofSTDataBuffer	64							//����STDataBase�ṹ�е�DataBuffer�ĳ���
#define		cDB_MaxNumofSTDataBase		256							//����Ĭ�����ݿ�����ݽṹ��Ŀ
#define		cDB_Fail					0xFFFFFFFF					//������һ��ȡʧ��ʱ���ش�ֵ
#define		cDB_FindMethod_ByIndex		0							//������STDataBase�ṹ�е�ulIndexƥ��������ݿ�ķ���
#define		cDB_FindMethod_ByData		1							//������STDataBase�ṹ�е�ulDataƥ��������ݿ�ķ���
#define		cDB_FindMethod_ByDataBuffer	2							//������STDataBase�ṹ�е�bDataBufferƥ��������ݿ�ķ���


typedef struct _STParam
{
	BYTE	bParamNum;
	BYTE	bStringNum;
	WORD	wType;
}STParam, *PSTParam, **PPSTParam;


typedef struct _STDataBase											//���ݿ�ṹ
{
	ULONG	ulIndex;												//���, �����������ڴ��ģ����ģ��ListBox�е����
	ULONG	ulData;													//����, �����������ڴ��HookAPI�ĵ�ַ
	STParam	stParam;												//API�����ṹ
	ULONG	ulCodeSize;												//Code����
	ULONG	ulDataSize;												//���ݻ�����Ҫ��������ݳ���
	BYTE	bCodeBuffer[cDB_MaxNumofSTCodeBuffer];					//Code��ŵĻ�����, �����������ڴ��HookAPI��ԭ�������ת����
	BYTE	bDataBuffer[cDB_MaxNumofSTDataBuffer];					//���ݻ�����, �����������ڴ��HookAPI������

}STDataBase, *PSTDataBase, **PPSTDataBase;


ULONG		DB_GetStructNum();
VOID		DB_ClearDataArray();
VOID		DB_EndDataArray();
PSTDataBase	DB_GetStruct(ULONG _ulNum);	
BOOL		DB_DeleteData(ULONG _ulNum);
ULONG		DB_FindData(ULONG _ulDatatoFind, ULONG _ulFindMethod);	
PVOID		DB_InitDataArray(BOOL _IsDefault, PVOID	_pMemAddr, ULONG _ulMemSize);
BOOL		DB_AddData(ULONG _Index, ULONG _Data, ULONG _DataBufferSize, PBYTE _pDataBuffer, ULONG _CodeLength, PBYTE _pCode, PSTParam	_pstParam);


