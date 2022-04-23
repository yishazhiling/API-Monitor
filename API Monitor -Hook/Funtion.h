//Funtion.h And Funtion.c
//���ܺ���������Call.c�еĺ���ʵ�ֲ�������Ĺ���

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
#define cLV_ReflushCallTimeID		1						//ˢ�µ��ô�����ʱ����ID
#define cMaxDataBaseSize			0x4000000				//���������ݿ������ڴ�����10M��
#define cMaxModuleNum				256						//�������������ģ��������Ŀ
#define cHookCodeLength				5						//����HookAPI��Hookд�����ĳ���
#define cOldAPICodeOffset			15						//����Hook��APIԭ������CodeBuffer�е�ƫ��λ��
#define cOldAPICodeEndCodeNum		5						//����Hook��APIԭ������CodeBuffer�к�ߵĴ����ֽ���
#define	cAPIMonitorDllName			"APIMonitor.dll"		//����APIMonitor��ע��Dll����
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


HWND	ghDlgMain;									//Dll�Ĵ��ھ��
extern	PVOID	hVectoredHandlerHandle;


