//Funtion.h And Funtion.c
//功能函数，调用Call.c中的函数实现操作界面的功能

#pragma once

#include <windows.h>
#include "VikiInc.h"
#include "resource.h"
#include <stdio.h>
#include <TLHELP32.H>
#include "DataBase.h"
#include "LDisEngine.h"
#include "ListView.h"
#include "ParamINI.h"

//#define _WIN32_WINNT				0x0500

#define cSTATUS_Fail				0xFFFFFFFF
#define cLV_ReflushCallTimeID		1						//刷新调用次数定时器的ID
#define cMaxDataBaseSize			0x4000000				//定义存放数据库的最大内存数（10M）
#define cMaxModuleNum				256						//定义进程中所有模块的最大数目
#define cHookCodeLength				5						//定义HookAPI所Hook写入代码的长度
#define cOldAPICodeOffset			15						//定义Hook的API原代码在CodeBuffer中的偏移位置
#define cOldAPICodeEndCodeNum		5						//定义Hook的API原代码在CodeBuffer中后边的代码字节数
#define	cAPIMonitorDllName			"APIMonitor.dll"		//定义APIMonitor的注入Dll名称
#define	cHookAPIMsgFileName			"APICallMsg.txt"
#define	cUnHookAPIFileName			"UnHookAPI.txt"
#define	cINIFolderName				"ParamTemplet\\"

typedef struct _STModule
{
	HANDLE	ulModuleHandle;
	ULONG	ulModuleSize;
	BYTE	pbModuleName[32];
}STModule, *PSTModule, **PPSTModule;



BOOL	FuntionInit();
VOID	EndHookAPI();
ULONG	GetProcessModules(ULONG _ulProcessID, PSTModule _pstModule);
BOOL	HookAPI(ULONG _ulSTModuleIndex, PVOID _pAPIAddr, PBYTE _pszAPIName);
VOID	UnHookAPIByNum(ULONG _ulNum);	
VOID	PrintDataBase();
VOID	Proc_OnTime(UINT _nIDEvent);
VOID	BTNClickProc_AddAPI();
VOID	BTNClickProc_DelAPI();
VOID	BTNClickProc_UnHookAllAPI();
ULONG	BTNClickProc_HookDllAPI();
VOID	BTNClickProc_GetParam();
VOID	BTNClickProc_OrderShowList();
VOID	BTNClickProc_GetModuleMsg();
VOID	WriteListMsgtoFile();
BOOL	CheckCallModule(PBYTE _pszModuleName);
DWORD	GetEditData(DWORD _EditID, PBYTE _pszFormat);
VOID	SetEditData(DWORD _EditID, PBYTE _pszFormat, DWORD _EditData);
ULONG	GetCodeLength(PVOID _pCodeAddr, ULONG _ulNeedLength);
BOOL	VK_WriteCallCode(PVOID _pWriteAddr, PVOID _pCallAddr);
BOOL	VK_WriteJmpCode(PVOID _pWriteAddr, PVOID _pJmpAddr);
BOOL	_stdcall HookCall(ULONG _ulEsp);
VOID	_stdcall VEHCallBackProc(PVOID _ExceptionAddr, ULONG _Param);


HWND	ghDlgMain;									//Dll的窗口句柄
extern	PVOID	hVectoredHandlerHandle;


