//DataBase.c
//����һ��������+������ɵ�һ�����ݿ������
 
#include "DataBase.h"

PSTOrderDataBase	pstOrderDataBase = NULL;
STOrderDataBase	stOrderDataBaseArray[cODB_MaxNumofSTOrderDataBase];			//Ĭ�����ݿ�
ULONG		ulStuctMaxNum = 0;									//���ݿ�ṹ���������������
ULONG		ulStuctNum = 0;										//���ݿ�ṹ������		

//���ƣ�ODB_InitDataArray 
//���ܣ���ʼ�����ݿ⣬���_IsDefaultָ��ΪTRUE�������ݴ�����Զ���������У����ΪFALSE������_MemAddrָ�����ڴ���
//������_IsDefault = �Ƿ�ʹ��Ĭ�ϵ�����
//������_MemAddr = �����ʹ��Ĭ�����飬��_MemAddrΪָ���Ĵ���ڴ�ĵ�ַ������������
//������_MemSize = �����ʹ��Ĭ�����飬��_MemSizeΪָ���Ĵ���ڴ�Ĵ�С������������
//���أ����ش�����ݿ���׵�ַ��ʧ�ܷ���NULL
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


//���ƣ�ODB_AddData
//���ܣ��������
//������STOrderDataBase���͵�����
//���أ��ɹ�����TRUE��ʧ�ܷ���FALSE
BOOL	ODB_AddData(ULONG _Index, ULONG _Data, ULONG _DataBufferSize, PBYTE _pDataBuffer, ULONG _CodeLength, PBYTE _pCode)
{


	return	FALSE;
}


//���ƣ�ODB_DeleteData
//���ܣ�ɾ������
//������_ulNum = Ҫɾ�����ݵ����
//���أ��ɹ�����TRUE��ʧ�ܷ���FALSE
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

//���ƣ�ODB_GetStruct
//���ܣ���ȡָ����ŵĵ�����
//������_ulNum = ���ݵ���ţ�ָPSTOrderDataBase�ṹ���������ݿ��д�ŵ���ţ���0��ʼ
//���أ��ɹ�ָ���ṹ��ָ��
PSTOrderDataBase	ODB_GetStruct(ULONG _ulNum)
{
	if (_ulNum < ulStuctNum)
	{
		return &pstOrderDataBase[_ulNum];
	}
	
	return	NULL;
}


//���ƣ�ODB_FindData
//���ܣ����������Ƿ������ݿ���
//������_DatatoFind = Ҫ�������ݣ���ΪULONG�����ݻ�ת�������������ָ��
//������_ulFindMethod = ���ķ�ʽ��cODB_FindMethod_ByIndex(0)��cODB_FindMethod_ByData(1)��cODB_FindMethod_ByDataBuffer(2)
//���أ��ɹ��򷵻�ָ����ŵ����ݣ�ʧ�ܷ���cODB_Fail(0xFFFFFFFF)
ULONG	ODB_FindData(ULONG _ulDatatoFind, ULONG _ulFindMethod)	
{
	
	
	return	cODB_Fail;
}

//���ƣ�ODB_MoveStruct
//���ܣ���һ��STOrderDataBase�ṹ���ݸ��Ƶ���һ��STOrderDataBase�ṹ��ȥ,ע��˺��������а�ȫ��飬�����ڲ�ʹ��
//������_pstSource = Ҫ�ƶ���STOrderDataBase�ṹָ��
//������_pstTarget = Ŀ��STOrderDataBase�ṹָ��
//���أ���
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

//���ƣ�ODB_GetStructNum
//���ܣ���ȡ��ǰ�ṹ����Ŀ
//��������
//���أ���ǰ�ṹ����Ŀ
ULONG	ODB_GetStructNum()
{
	return	ulStuctNum;
}


//���ƣ�ODB_ClearDataArray
//���ܣ���DataArray��0
//��������
//���أ���
VOID	ODB_ClearDataArray()
{
	ulStuctNum = 0;
}


//���ƣ�ODB_EndDataArray
//���ܣ���ȡ��ǰ�ṹ����Ŀ
//��������
//���أ���ǰ�ṹ����Ŀ
VOID	ODB_EndDataArray()
{
	pstOrderDataBase = NULL;
	ulStuctNum = 0;
	ulStuctMaxNum = 0;
}
