//APIMonitor.h And APIMonitor.c
//APIMonitor入口函数及窗口响应函数

#pragma once

#include <windows.h>
#include "VikiInc.h"
#include "resource.h"
#include "Funtion.h"

#ifdef TESTDLL_EXPORTS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllexport)
#endif

DWORD	WINAPI	PorcDllAttach(PVOID pArg);
VOID	PorcDllDetach();
BOOL	CALLBACK DialogProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
VOID	InitDialog(HWND _hWnd);
VOID	CloseDialog(HWND _hWnd);

extern	BOOL	IsShowSystem;						//是否显示系统Dll的调用信息
EXPORT	DWORD	ExportData = 0x5555;
EXPORT	VOID	DllMsg(PVOID _pszMsgText);

