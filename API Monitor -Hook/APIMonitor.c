#include "APIMonitor.h"

HANDLE	hDllInstance;
HWND	hDlgMain;
DWORD	DlgThreadID; 	//返回的线程的ID编号

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			{
				hDllInstance = hModule;
				CloseHandle(CreateThread(NULL, 0, PorcDllAttach, NULL, 0, &DlgThreadID));
				break;
			}

		case DLL_PROCESS_DETACH:
			{
				PorcDllDetach();
				break;
			}
		case DLL_THREAD_ATTACH:

		case DLL_THREAD_DETACH:

		break;
	}
    return TRUE;
}

DWORD WINAPI PorcDllAttach(PVOID pArg)
{
	MSG		stMessage;

	CreateDialogParam(hDllInstance, DIALOGMAIN, NULL, DialogProc, 0); 		//DialogProc 指向对话框过程的指针

	 //消息循环
	while (GetMessage(&stMessage, NULL, 0, 0))
	{
		TranslateMessage(&stMessage);
		DispatchMessage(&stMessage);
	}
	
	return 1;
}

VOID PorcDllDetach()
{
	//DllMsg("APIMonitor.dll被卸载");
}


//名称：DialogProc 
//功能：消息处理函数
//窗口过程处理，回调函数
BOOL CALLBACK DialogProc (HWND _hWnd, UINT _Msg, WPARAM _wParam, LPARAM _lParam)
{
	ULONG	ulNum;

	switch (_Msg)	
	{
	case   WM_INITDIALOG :
		{
			InitDialog(_hWnd);
			return TRUE ;
		}
		
	case   WM_CLOSE :
		{
			CloseDialog(_hWnd);
			return TRUE ;
		}
	case   WM_TIMER :                           //添加定时器消息处理函数
		{
			Proc_OnTime(_wParam);
			return TRUE ;
		}
		
	case   WM_COMMAND :
		{
			switch (LOWORD (_wParam))
				
			{
			case   IDC_BUTTON_AddAPI :                          //添加"添加API"处理函数
				{
					BTNClickProc_AddAPI();
					return TRUE ;	
				}
			case   IDC_BUTTON_DelAPI :                          //添加"删除API"处理函数
				{
					BTNClickProc_DelAPI();
					return TRUE ;	
				}
			case   IDC_BUTTON_HookDllAPI :                       //添加"开始"处理函数
				{
					BTNClickProc_HookDllAPI();
					return TRUE ;	
				}
			case   IDC_BUTTON_UnHookAllAPI :					//添加"删除所有Hook"处理函数
				{
					BTNClickProc_UnHookAllAPI();
					return TRUE ;	
				}	
			case   IDC_BUTTON_GetModuleMsg :					//添加"模块信息"处理函数
				{
					BTNClickProc_GetModuleMsg();
					return TRUE ;	
				}
			case   IDC_BUTTON_GetParam : 						//添加"参数信息"处理函数
				{
					BTNClickProc_GetParam();
					return TRUE ;	
				}
			case   IDC_BUTTON_ClearShowList : 					//添加"清屏"处理函数
				{
					LV_ClearListView();
					return TRUE ;	
				}
			case   IDC_CHECK_IsShowSystem : 					//添加"是否显示系统调用"处理函数
				{
					if (IsDlgButtonChecked(hDlgMain, IDC_CHECK_IsShowSystem) == BST_CHECKED)
					{
						IsShowSystem = TRUE;
					}
					else
					{
						IsShowSystem = FALSE;
					}
					return TRUE ;	
				}
			}
			break ;	
		}	
	}
	return FALSE;			
}

//名称：InitDialog 
//功能：对话框窗口初始化(获取各控件句柄)
VOID InitDialog(HWND _hWnd)
{
	hDlgMain = _hWnd;
	ghDlgMain = _hWnd;
	
	//初始化
	FuntionInit();
	
}

//名称：CloseDialog 
//功能：对话框窗口退出前需处理的代码
VOID CloseDialog(HWND _hWnd)
{
	//此处添加一些退出前需要处理的的代码
	EndHookAPI();
	
	DestroyWindow(_hWnd);
	FreeLibraryAndExitThread(hDllInstance, 0);
}


EXPORT VOID DllMsg(PVOID _pszMsgText)
{
	VK_MsgBox(_pszMsgText);
}
