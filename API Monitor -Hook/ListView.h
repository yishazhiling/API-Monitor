//ListView.h 
//ListView��Ĵ�����ģ��

#pragma once
#include <windows.h>
#include <COMMCTRL.h>
#include "DataBase.h"

#define cLV_Fail						0xFFFFFFFF
#define cLV_ReflushCallTime				3000					//������ʾ���ô�����ˢ��ʱ��
#define cLV_CallAddrArrayNum			0x2000					//���屣����õ�ַ�������Ŀ
#define cLV_ParamMemSize				0x800					//��������Ų��������ڴ�Ĵ�С
#define cLV_MaxParamNum					10						//�����Ų�������������Ŀ	
#define cLV_ParamStringSize				32						//���屣���ַ�������������ֽ���
#define cLV_ParamMemSTParamIndex		0						//�����Ų��������ڴ�Ĳ���STParam�ṹ�����
#define cLV_ParamMemCallTimeIndex		1						//�����Ų��������ڴ��API��ͬ�������ܵ��ô���
#define cLV_ParamMemNumIndex			2						//�����Ų������ݵ��ڴ��д���˶�������������
#define cLV_ParamMemMsgIndex			4						//�����Ų��������ڴ�Ĳ�����Ϣ��ʼ�����
#define cLV_ColumnWidth					99
#define cLV_Column0_Name				"API����"
#define cLV_Column1_Name				"����ģ��"
#define cLV_Column2_Name				"���õ�ַ"
#define cLV_Column3_Name				"���ô���"

typedef struct _STLVCallArray									//API�����ߵ�ַ��Žṹ
{
	ULONG	ulCallAddr;											//���õ�ַ
	ULONG	ulNum;												//���ô���
	WORD	wAPIModuleIndex;									//������stModuleArray�����е����
	PVOID	pParamMem;											//������Ϣ��ŵ�ַ	
}STLVCallArray, *PSTLVCallArray, **PPSTLVCallArray;

typedef struct _STParam_ListViewHandler							//LV_ListViewHandler�Ĳ����ṹ
{
	ULONG		ulCallAddr;										//���õ�ַ
	PBYTE		pszModuleName;									//����ģ������
	PBYTE		pszAPIName;										//API����
	ULONG		ulParamAddr;									//API�����һ��ѹջ������ջ��ַ
	WORD		wAPIModeleIndex;								//������stModuleArray�����е����
	PSTParam	pstParam;										//�����ṹָ��
}STParam_ListViewHandler, *PSTParam_ListViewHandler, **PPSTParam_ListViewHandler;


//��������
VOID	LV_ClearListView();
VOID	LV_AddKeyItem(ULONG _ulItem, ULONG _ulParam, PBYTE _pbText);
VOID	LV_AddItem(ULONG _SubItem, ULONG _ulItem, ULONG _ulParam, PBYTE _pbText);

//��չ����
ULONG	LV_GetNum();
ULONG	LV_GetSelectItem();
VOID	LV_ReflushCallTime();
ULONG	LV_GetItemText(ULONG _ulColumn, ULONG _ulRow, PBYTE _pbBufer);
BOOL	LV_OutPut(ULONG ulNum, PBYTE _pszOutText);
BOOL	LV_OutPutParamMsg(ULONG _ulNum, PBYTE _pszOutText, PVOID _pParamNameBufer);
BOOL	LV_InitListView(HWND _hWindow, HWND _hListView);
VOID	LV_AddList(ULONG _ulNum, ULONG _ulCallAddr, PBYTE _pszModuleName, PBYTE _pszAPIName);
ULONG	LV_CheckParam(PVOID _pParamMem, ULONG _ulParamAddr);
BOOL	LV_AddParam(ULONG _ulAddIndex, PVOID _pParamMem, ULONG _ulParamAddr);
ULONG	LV_ListViewHandler(PSTParam_ListViewHandler _pSTParam_ListViewHandler);
PSTLVCallArray	LV_GetSTLVCallArray(ULONG _ulIndex);

