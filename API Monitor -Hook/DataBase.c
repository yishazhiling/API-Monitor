//DataBase.c
//构建一个由数据+名称组成的一个数据库管理函数

#include "DataBase.h" 

ULONG		ulStuctMaxNum = 0;									//数据库结构的数量的最大数量
ULONG		ulStuctNum = 0;										//数据库结构的数量
PSTDataBase	pstDataBase = NULL;
STDataBase	stDataBaseArray[cDB_MaxNumofSTDataBase];			//默认数据库		

//名称：DB_InitDataArray 
//功能：初始化数据库，如果_IsDefault指定为TRUE，则数据存放在自定义的数组中，如果为FALSE则存放在_MemAddr指定的内存中
//参数：_IsDefault = 是否使用默认的数组
//参数：_pMemAddr = 如果不使用默认数组，则_MemAddr为指定的存放内存的地址，否则无意义
//参数：_ulMemSize = 如果不使用默认数组，则_MemSize为指定的存放内存的大小，否则无意义
//返回：返回存放数据库的首地址，失败返回NULL
PVOID	DB_InitDataArray(BOOL _IsDefault, PVOID	_pMemAddr, ULONG _ulMemSize)
{
	if (_IsDefault)
	{
		pstDataBase = stDataBaseArray;
		ulStuctMaxNum = cDB_MaxNumofSTDataBase;
	} 
	else
	{
		if (_pMemAddr != NULL && _ulMemSize >= sizeof(STDataBase))
		{
			pstDataBase = _pMemAddr;
			ulStuctMaxNum =  _ulMemSize / sizeof(STDataBase);
		}
	}

	return	pstDataBase;
}


//名称：DB_AddData
//功能：添加一个STDataBase数据
//参数：_ulIndex = 存入数据库STDataBase结构中的ulIndex数据
//参数：_ulData = 存入数据库STDataBase结构中的ulData数据
//参数：_ulDataSize = 存入数据库STDataBase结构中的pDataBuffer的数据大小
//参数：_pDataBuffer = 存入数据库STDataBase结构中的pDataBuffer数据
//参数：_ululCodeSize = 存入数据库STDataBase结构中的pbCodeBuffer数据
//参数：_ulpbCodeBuffer = 存入数据库STDataBase结构中的pbCodeBuffer数据
//参数：_pstParam = 存入数据库STDataBase结构中的stParam数据，即参数结构
//返回：成功返回TRUE，失败返回FALSE
BOOL	DB_AddData(ULONG _ulIndex, ULONG _ulData, ULONG _ulDataSize, 
				   PBYTE _pDataBuffer, ULONG _ulCodeSize, PBYTE _pbCodeBuffer,
				   PSTParam	_pstParam)
{
	ULONG	i;

	if (_pbCodeBuffer != NULL && _pDataBuffer != NULL && ulStuctNum < ulStuctMaxNum)
	{
		pstDataBase[ulStuctNum].ulIndex = _ulIndex;
		pstDataBase[ulStuctNum].ulData = _ulData;
		pstDataBase[ulStuctNum].ulDataSize = _ulDataSize;
		pstDataBase[ulStuctNum].ulCodeSize = _ulCodeSize;
		pstDataBase[ulStuctNum].stParam = *_pstParam;
		for (i = 0; i < _ulDataSize && i < cDB_MaxNumofSTDataBase * 4; i++)
		{
			pstDataBase[ulStuctNum].bDataBuffer[i] = _pDataBuffer[i];
		}
		for (i = 0; i < _ulCodeSize && i < cDB_MaxNumofSTCodeBuffer; i++)
		{
			pstDataBase[ulStuctNum].bCodeBuffer[i] = _pbCodeBuffer[i];
		}
		ulStuctNum++;

		return	TRUE;
	}

	return	FALSE;
}


//名称：DB_DeleteData
//功能：删除一个指定序号的STDataBase数据
//参数：_ulNum = 要删除数据的序号，从0开始
//返回：成功返回TRUE，失败返回FALSE
BOOL	DB_DeleteData(ULONG _ulNum)
{
	ULONG	i, ulCopySize;
	PBYTE	pbCopySource, pbCopyTarget;

	if (_ulNum < ulStuctNum)
	{
		pbCopySource = &pstDataBase[_ulNum + 1];				//将高于要删除序号的数据前提一行
		pbCopyTarget = &pstDataBase[_ulNum];
		ulCopySize	 = (ulStuctNum - _ulNum -1) * sizeof(PSTDataBase);
		for (i = 0; i < ulCopySize; i++)					
		{
			pbCopyTarget[i] = pbCopySource[i];
		}
		ulStuctNum--;

		return	TRUE;		
	}
	
	return	FALSE;
}

//名称：DB_GetStruct
//功能：获取指定序号的的数据
//参数：_ulNum = 数据的序号，指PSTDataBase结构数据在数据库中存放的序号，从0开始
//返回：成功指定结构的指针
PSTDataBase	DB_GetStruct(ULONG _ulNum)
{
	if (_ulNum < ulStuctNum)
	{
		return &pstDataBase[_ulNum];
	}
	
	return	NULL;
}


//名称：DB_FindData
//功能：查找数据是否在数据库中
//参数：_ulDatatoFind = 要检测的数据，如果查找方式为cDB_FindMethod_ByDataBuffer，_ulDatatoFind则表示数据缓冲区的指针
//参数：_ulFindMethod = 检测的方式，cDB_FindMethod_ByIndex(0)、cDB_FindMethod_ByData(1)、cDB_FindMethod_ByDataBuffer(2)
//返回：成功则返回指定序号的数据，失败返回cDB_Fail(0xFFFFFFFF)
ULONG	DB_FindData(ULONG _ulDatatoFind, ULONG _ulFindMethod)	
{
	ULONG	i, j;
	BOOL	IsFind = FALSE;
	PBYTE	pbTemp = NULL;

	for (i = 0; i < ulStuctNum; i++)
	{
		switch (_ulFindMethod)
		{
		case cDB_FindMethod_ByIndex:
			{
				if (pstDataBase[i].ulIndex == _ulDatatoFind)
				{
					return	i;
				}
				break;
			}
		case cDB_FindMethod_ByData:
			{
				if (pstDataBase[i].ulData == _ulDatatoFind)
				{
					return	i;
				}
				break;
			}
		case cDB_FindMethod_ByDataBuffer:
			{
				(ULONG)pbTemp = _ulDatatoFind;
				for (j = 0; j < pstDataBase[i].ulDataSize; j++)
				{
					if (pstDataBase[i].bDataBuffer[j] != pbTemp[j])
					{
						break;
					}
				}
				if (j >= pstDataBase[i].ulDataSize)
				{
					return	i;
				}
				break;
			}
		}
	}
		
	return	cDB_Fail;
}


//名称：DB_GetStructNum
//功能：获取当前结构的数目
//参数：无
//返回：当前结构的数目
ULONG	DB_GetStructNum()
{
	return	ulStuctNum;
}


//名称：DB_ClearDataArray
//功能：将DataArray清0
//参数：无
//返回：无
VOID	DB_ClearDataArray()
{
	ulStuctNum = 0;
}


//名称：DB_EndDataArray
//功能：获取当前结构的数目
//参数：无
//返回：当前结构的数目
VOID	DB_EndDataArray()
{
	pstDataBase = NULL;
	ulStuctNum = 0;
	ulStuctMaxNum = 0;
}
