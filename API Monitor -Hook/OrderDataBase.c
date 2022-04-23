//DataBase.c
//构建一个由数据+名称组成的一个数据库管理函数
 
#include "DataBase.h"

PSTOrderDataBase	pstOrderDataBase = NULL;
STOrderDataBase	stOrderDataBaseArray[cODB_MaxNumofSTOrderDataBase];			//默认数据库
ULONG		ulStuctMaxNum = 0;									//数据库结构的数量的最大数量
ULONG		ulStuctNum = 0;										//数据库结构的数量		

//名称：ODB_InitDataArray 
//功能：初始化数据库，如果_IsDefault指定为TRUE，则数据存放在自定义的数组中，如果为FALSE则存放在_MemAddr指定的内存中
//参数：_IsDefault = 是否使用默认的数组
//参数：_MemAddr = 如果不使用默认数组，则_MemAddr为指定的存放内存的地址，否则无意义
//参数：_MemSize = 如果不使用默认数组，则_MemSize为指定的存放内存的大小，否则无意义
//返回：返回存放数据库的首地址，失败返回NULL
PVOID	ODB_InitDataArray(BOOL _IsDefault, PVOID	_pMemAddr, ULONG _ulMemSize)
{
	if (_IsDefault)
	{
		pstOrderDataBase = stOrderDataBaseArray;
		ulStuctMaxNum = cODB_MaxNumofSTOrderDataBase;
	} 
	else
	{
		if (_pMemAddr != NULL && _ulMemSize >= sizeof(STOrderDataBase))
		{
			pstOrderDataBase = _pMemAddr;
			ulStuctMaxNum =  _ulMemSize / sizeof(STOrderDataBase);
		}
	}

	return	pstOrderDataBase;
}


//名称：ODB_AddData
//功能：添加数据
//参数：STOrderDataBase类型的数据
//返回：成功返回TRUE，失败返回FALSE
BOOL	ODB_AddData(ULONG _Index, ULONG _Data, ULONG _DataBufferSize, PBYTE _pDataBuffer, ULONG _CodeLength, PBYTE _pCode)
{


	return	FALSE;
}


//名称：ODB_DeleteData
//功能：删除数据
//参数：_ulNum = 要删除数据的序号
//返回：成功返回TRUE，失败返回FALSE
BOOL	ODB_DeleteData(ULONG _ulNum)
{
	ULONG	i;

	if (_ulNum < ulStuctNum)
	{
		for (i = _ulNum; i< ulStuctNum; i++)
		{
			ODB_MoveStruct(&pstOrderDataBase[i+1], &pstOrderDataBase[i]);
		}
		ulStuctNum--;
		return	TRUE;		
	}
	
	return	FALSE;
}

//名称：ODB_GetStruct
//功能：获取指定序号的的数据
//参数：_ulNum = 数据的序号，指PSTOrderDataBase结构数据在数据库中存放的序号，从0开始
//返回：成功指定结构的指针
PSTOrderDataBase	ODB_GetStruct(ULONG _ulNum)
{
	if (_ulNum < ulStuctNum)
	{
		return &pstOrderDataBase[_ulNum];
	}
	
	return	NULL;
}


//名称：ODB_FindData
//功能：查找数据是否在数据库中
//参数：_DatatoFind = 要检测的数据，可为ULONG型数据或转化后的数据名称指针
//参数：_ulFindMethod = 检测的方式，cODB_FindMethod_ByIndex(0)、cODB_FindMethod_ByData(1)、cODB_FindMethod_ByDataBuffer(2)
//返回：成功则返回指定序号的数据，失败返回cODB_Fail(0xFFFFFFFF)
ULONG	ODB_FindData(ULONG _ulDatatoFind, ULONG _ulFindMethod)	
{
	
	
	return	cODB_Fail;
}

//名称：ODB_MoveStruct
//功能：将一条STOrderDataBase结构数据复制到另一个STOrderDataBase结构中去,注意此函数不进行安全检查，仅限内部使用
//参数：_pstSource = 要移动的STOrderDataBase结构指针
//参数：_pstTarget = 目地STOrderDataBase结构指针
//返回：无
VOID	ODB_MoveStruct(PSTOrderDataBase _pstSource, PSTOrderDataBase _pstTarget)
{
	ULONG	i;
	PBYTE	pbSource, pbTarget;
	
	pbSource = (PBYTE)_pstTarget;
	pbTarget = (PBYTE)_pstSource;
	for (i = 0; i < sizeof(STOrderDataBase); i++)
	{
		pbSource[i] = pbTarget[i];
	}
}

//名称：ODB_GetStructNum
//功能：获取当前结构的数目
//参数：无
//返回：当前结构的数目
ULONG	ODB_GetStructNum()
{
	return	ulStuctNum;
}


//名称：ODB_ClearDataArray
//功能：将DataArray清0
//参数：无
//返回：无
VOID	ODB_ClearDataArray()
{
	ulStuctNum = 0;
}


//名称：ODB_EndDataArray
//功能：获取当前结构的数目
//参数：无
//返回：当前结构的数目
VOID	ODB_EndDataArray()
{
	pstOrderDataBase = NULL;
	ulStuctNum = 0;
	ulStuctMaxNum = 0;
}
