#include "APIMonitor.h"

HANDLE	hDllInstance;
HWND	hDlgMain;
DWORD	DlgThreadID; 	//���ص��̵߳�ID���

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

	CreateDialogParam(hDllInstance, DIALOGMAIN, NULL, DialogProc, 0); 		//DialogProc ָ��Ի�����̵�ָ��

	 //��Ϣѭ��
	while (GetMessage(&stMessage, NULL, 0, 0))
	{
		TranslateMessage(&stMessage);
		DispatchMessage(&stMessage);
	}
	
	return 1;
}

VOID PorcDllDetach()
{
	//DllMsg("APIMonitor.dll��ж��");
}


//���ƣ�DialogProc 
//���ܣ���Ϣ������
//���ڹ��̴����ص�����
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
	case   WM_TIMER :                           //��Ӷ�ʱ����Ϣ������
		{
			Proc_OnTime(_wParam);
			return TRUE ;
		}
		
	case   WM_COMMAND :
		{
			switch (LOWORD (_wParam))
				
			{
			case   IDC_BUTTON_AddAPI :                          //���"���API"������
				{
					BTNClickProc_AddAPI();
					return TRUE ;	
				}
			case   IDC_BUTTON_DelAPI :                          //���"ɾ��API"������
				{
					BTNClickProc_DelAPI();
					return TRUE ;	
				}
			case   IDC_BUTTON_HookDllAPI :                       //���"��ʼ"������
				{
					BTNClickProc_HookDllAPI();
					return TRUE ;	
				}
			case   IDC_BUTTON_UnHookAllAPI :					//���"ɾ������Hook"������
				{
					BTNClickProc_UnHookAllAPI();
					return TRUE ;	
				}	
			case   IDC_BUTTON_GetModuleMsg :					//���"ģ����Ϣ"������
				{
					BTNClickProc_GetModuleMsg();
					return TRUE ;	
				}
			case   IDC_BUTTON_GetParam : 						//���"������Ϣ"������
				{
					BTNClickProc_GetParam();
					return TRUE ;	
				}
			case   IDC_BUTTON_ClearShowList : 					//���"����"������
				{
					LV_ClearListView();
					return TRUE ;	
				}
			case   IDC_CHECK_IsShowSystem : 					//���"�Ƿ���ʾϵͳ����"������
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

//���ƣ�InitDialog 
//���ܣ��Ի��򴰿ڳ�ʼ��(��ȡ���ؼ����)
VOID InitDialog(HWND _hWnd)
{
	hDlgMain = _hWnd;
	ghDlgMain = _hWnd;
	
	//��ʼ��
	FuntionInit();
	
}

//���ƣ�CloseDialog 
//���ܣ��Ի��򴰿��˳�ǰ�账��Ĵ���
VOID CloseDialog(HWND _hWnd)
{
	//�˴����һЩ�˳�ǰ��Ҫ����ĵĴ���
	EndHookAPI();
	
	DestroyWindow(_hWnd);
	FreeLibraryAndExitThread(hDllInstance, 0);
}


EXPORT VOID DllMsg(PVOID _pszMsgText)
{
	VK_MsgBox(_pszMsgText);
}
