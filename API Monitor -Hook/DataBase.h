//DataBase.h And DataBase.c
//功能函数，构建一个由数据+代码+名称组成的一个数据库管理函数,由纯C组成，任意调用

#include <windows.h>

#pragma once 

#define		cDB_MaxNumofSTCodeBuffer	64							//定义STDataBase结构中的CodeBuffer的长度
#define		cDB_MaxNumofSTDataBuffer	64							//定义STDataBase结构中的DataBuffer的长度
#define		cDB_MaxNumofSTDataBase		256							//定义默认数据库的数据结构数目
#define		cDB_Fail					0xFFFFFFFF					//定义查找或获取失败时返回此值
#define		cDB_FindMethod_ByIndex		0							//定义用STDataBase结构中的ulIndex匹配查找数据库的方法
#define		cDB_FindMethod_ByData		1							//定义用STDataBase结构中的ulData匹配查找数据库的方法
#define		cDB_FindMethod_ByDataBuffer	2							//定义用STDataBase结构中的bDataBuffer匹配查找数据库的方法


typedef struct _STParam
{
	BYTE	bParamNum;
	BYTE	bStringNum;
	WORD	wType;
}STParam, *PSTParam, **PPSTParam;


typedef struct _STDataBase											//数据库结构
{
	ULONG	ulIndex;												//序号, 本程序中用于存放模块在模块ListBox中的序号
	ULONG	ulData;													//数据, 本程序中用于存放HookAPI的地址
	STParam	stParam;												//API参数结构
	ULONG	ulCodeSize;												//Code长度
	ULONG	ulDataSize;												//数据缓冲区要存入的数据长度
	BYTE	bCodeBuffer[cDB_MaxNumofSTCodeBuffer];					//Code存放的缓冲区, 本程序中用于存放HookAPI的原代码和跳转代码
	BYTE	bDataBuffer[cDB_MaxNumofSTDataBuffer];					//数据缓冲区, 本程序中用于存放HookAPI的名称

}STDataBase, *PSTDataBase, **PPSTDataBase;


ULONG		DB_GetStructNum();
VOID		DB_ClearDataArray();
VOID		DB_EndDataArray();
PSTDataBase	DB_GetStruct(ULONG _ulNum);	
BOOL		DB_DeleteData(ULONG _ulNum);
ULONG		DB_FindData(ULONG _ulDatatoFind, ULONG _ulFindMethod);	
PVOID		DB_InitDataArray(BOOL _IsDefault, PVOID	_pMemAddr, ULONG _ulMemSize);
BOOL		DB_AddData(ULONG _Index, ULONG _Data, ULONG _DataBufferSize, PBYTE _pDataBuffer, ULONG _CodeLength, PBYTE _pCode, PSTParam	_pstParam);


