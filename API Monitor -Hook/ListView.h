//ListView.h 
//ListView表的处理功能模块

#pragma once
#include <windows.h>
#include <COMMCTRL.h>
#include "DataBase.h"

#define cLV_Fail						0xFFFFFFFF
#define cLV_ReflushCallTime				3000					//定义显示调用次数的刷新时间
#define cLV_CallAddrArrayNum			0x2000					//定义保存调用地址的最大数目
#define cLV_ParamMemSize				0x800					//定义分配存放参数数据内存的大小
#define cLV_MaxParamNum					10						//定义存放参数数据最大的数目	
#define cLV_ParamStringSize				32						//定义保存字符串参数的最大字节数
#define cLV_ParamMemSTParamIndex		0						//定义存放参数数据内存的参数STParam结构的序号
#define cLV_ParamMemCallTimeIndex		1						//定义存放参数数据内存的API不同参数的总调用次数
#define cLV_ParamMemNumIndex			2						//定义存放参数数据的内存中存放了多少组参数的序号
#define cLV_ParamMemMsgIndex			4						//定义存放参数数据内存的参数信息开始的序号
#define cLV_ColumnWidth					99
#define cLV_Column0_Name				"API名称"
#define cLV_Column1_Name				"调用模块"
#define cLV_Column2_Name				"调用地址"
#define cLV_Column3_Name				"调用次数"

typedef struct _STLVCallArray									//API调用者地址存放结构
{
	ULONG	ulCallAddr;											//调用地址
	ULONG	ulNum;												//调用次数
	WORD	wAPIModuleIndex;									//参数在stModuleArray数组中的序号
	PVOID	pParamMem;											//参数信息存放地址	
}STLVCallArray, *PSTLVCallArray, **PPSTLVCallArray;

typedef struct _STParam_ListViewHandler							//LV_ListViewHandler的参数结构
{
	ULONG		ulCallAddr;										//调用地址
	PBYTE		pszModuleName;									//调用模块名称
	PBYTE		pszAPIName;										//API名称
	ULONG		ulParamAddr;									//API的最后一个压栈参数的栈地址
	WORD		wAPIModeleIndex;								//参数在stModuleArray数组中的序号
	PSTParam	pstParam;										//参数结构指针
}STParam_ListViewHandler, *PSTParam_ListViewHandler, **PPSTParam_ListViewHandler;


//基础函数
VOID	LV_ClearListView();
VOID	LV_AddKeyItem(ULONG _ulItem, ULONG _ulParam, PBYTE _pbText);
VOID	LV_AddItem(ULONG _SubItem, ULONG _ulItem, ULONG _ulParam, PBYTE _pbText);

//扩展函数
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

