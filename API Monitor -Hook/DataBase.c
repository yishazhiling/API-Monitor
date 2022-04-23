//DataBase.c
//����һ��������+������ɵ�һ�����ݿ������

#include "DataBase.h" 

ULONG		ulStuctMaxNum = 0;									//���ݿ�ṹ���������������
ULONG		ulStuctNum = 0;										//���ݿ�ṹ������
PSTDataBase	pstDataBase = NULL;
STDataBase	stDataBaseArray[cDB_MaxNumofSTDataBase];			//Ĭ�����ݿ�		

//���ƣ�DB_InitDataArray 
//���ܣ���ʼ�����ݿ⣬���_IsDefaultָ��ΪTRUE�������ݴ�����Զ���������У����ΪFALSE������_MemAddrָ�����ڴ���
//������_IsDefault = �Ƿ�ʹ��Ĭ�ϵ�����
//������_pMemAddr = �����ʹ��Ĭ�����飬��_MemAddrΪָ���Ĵ���ڴ�ĵ�ַ������������
//������_ulMemSize = �����ʹ��Ĭ�����飬��_MemSizeΪָ���Ĵ���ڴ�Ĵ�С������������
//���أ����ش�����ݿ���׵�ַ��ʧ�ܷ���NULL
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


//���ƣ�DB_AddData
//���ܣ����һ��STDataBase����
//������_ulIndex = �������ݿ�STDataBase�ṹ�е�ulIndex����
//������_ulData = �������ݿ�STDataBase�ṹ�е�ulData����
//������_ulDataSize = �������ݿ�STDataBase�ṹ�е�pDataBuffer�����ݴ�С
//������_pDataBuffer = �������ݿ�STDataBase�ṹ�е�pDataBuffer����
//������_ululCodeSize = �������ݿ�STDataBase�ṹ�е�pbCodeBuffer����
//������_ulpbCodeBuffer = �������ݿ�STDataBase�ṹ�е�pbCodeBuffer����
//������_pstParam = �������ݿ�STDataBase�ṹ�е�stParam���ݣ��������ṹ
//���أ��ɹ�����TRUE��ʧ�ܷ���FALSE
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


//���ƣ�DB_DeleteData
//���ܣ�ɾ��һ��ָ����ŵ�STDataBase����
//������_ulNum = Ҫɾ�����ݵ���ţ���0��ʼ
//���أ��ɹ�����TRUE��ʧ�ܷ���FALSE
BOOL	DB_DeleteData(ULONG _ulNum)
{
	ULONG	i, ulCopySize;
	PBYTE	pbCopySource, pbCopyTarget;

	if (_ulNum < ulStuctNum)
	{
		pbCopySource = &pstDataBase[_ulNum + 1];				//������Ҫɾ����ŵ�����ǰ��һ��
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

//���ƣ�DB_GetStruct
//���ܣ���ȡָ����ŵĵ�����
//������_ulNum = ���ݵ���ţ�ָPSTDataBase�ṹ���������ݿ��д�ŵ���ţ���0��ʼ
//���أ��ɹ�ָ���ṹ��ָ��
PSTDataBase	DB_GetStruct(ULONG _ulNum)
{
	if (_ulNum < ulStuctNum)
	{
		return &pstDataBase[_ulNum];
	}
	
	return	NULL;
}


//���ƣ�DB_FindData
//���ܣ����������Ƿ������ݿ���
//������_ulDatatoFind = Ҫ�������ݣ�������ҷ�ʽΪcDB_FindMethod_ByDataBuffer��_ulDatatoFind���ʾ���ݻ�������ָ��
//������_ulFindMethod = ���ķ�ʽ��cDB_FindMethod_ByIndex(0)��cDB_FindMethod_ByData(1)��cDB_FindMethod_ByDataBuffer(2)
//���أ��ɹ��򷵻�ָ����ŵ����ݣ�ʧ�ܷ���cDB_Fail(0xFFFFFFFF)
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


//���ƣ�DB_GetStructNum
//���ܣ���ȡ��ǰ�ṹ����Ŀ
//��������
//���أ���ǰ�ṹ����Ŀ
ULONG	DB_GetStructNum()
{
	return	ulStuctNum;
}


//���ƣ�DB_ClearDataArray
//���ܣ���DataArray��0
//��������
//���أ���
VOID	DB_ClearDataArray()
{
	ulStuctNum = 0;
}


//���ƣ�DB_EndDataArray
//���ܣ���ȡ��ǰ�ṹ����Ŀ
//��������
//���أ���ǰ�ṹ����Ŀ
VOID	DB_EndDataArray()
{
	pstDataBase = NULL;
	ulStuctNum = 0;
	ulStuctMaxNum = 0;
}
