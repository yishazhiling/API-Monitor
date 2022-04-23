//Funtion.c
//�������ܺ���

#include "Funtion.h"

BOOL		IsShowSystem = FALSE;
HANDLE		hHookWindowsThread;									//���̵����߳̾��
HANDLE		hWriteUnHookAPIFile = NULL;
PVOID		pDataBaseMem = NULL;
ULONG		ulModuleNum = 0;									//����ģ�������
BOOL		IsMonitor = TRUE;
ULONG		hTimer_ReflushCallTime = 0;
BOOL		IsHookCallAgain = FALSE;							//��HookCall()�м���Ƿ�ѭ������
BOOL		IsExceptionMonitor = FALSE;							//���"�쳣���"�Ƿ��ڽ�����
BYTE		pszAPIMonitorPath[MAX_PATH];
BYTE		pszAPIMonitorDirectory[MAX_PATH];
PBYTE		pbLastCallAPIName = 0x55555550;
STModule	stModuleArray[cMaxModuleNum];						//����ģ��ṹ����

//���ƣ�FuntionInit() 
//���ܣ���ʼ����������һЩ��ʼ������
//��������
//���أ��ɹ�����TRUE��ʧ�ܷ���FALSE
BOOL	FuntionInit()
{
	HANDLE		hAPIMonitor;
	ULONG		i, APIMonitorNum;
	TCHAR		pFilePath[MAX_PATH];

	STParam		stParam;
	
	//�����ڴ棬����������ݿ�
	pDataBaseMem = VirtualAlloc(NULL, cMaxDataBaseSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (pDataBaseMem == NULL)
	{
		VK_MsgBox("�����ڴ�ʧ��");
		return	FALSE;
	}
	DB_InitDataArray(FALSE, pDataBaseMem, cMaxDataBaseSize);					//����Ĭ�ϵķ�ʽ��ʼ��API���ݿ�
	InitLDisEngine();															//��ʼ�����������
	ulModuleNum = GetProcessModules(GetCurrentProcessId(), stModuleArray);		//��ȡ��ǰ�������е�Module

	SendDlgItemMessage(ghDlgMain, IDC_COMBO_Mudles, CB_SETCURSEL, 0, NULL);		//��ʾModule
	SetWindowText(ghDlgMain, stModuleArray[0].pbModuleName);					//��������������Ϊע����̵�����
	LV_InitListView(ghDlgMain, GetDlgItem(ghDlgMain, IDC_LISTV_Show));			//��ʼ��ListView

	//��ȡ����·�������������ļ�
	VK_GetFileDirectory(pszAPIMonitorPath, pFilePath);
	sprintf(pszAPIMonitorDirectory, "%s%s", pFilePath, cINIFolderName);

	sprintf(pFilePath, "%s%s", pFilePath, cUnHookAPIFileName);
	hWriteUnHookAPIFile = CreateFile(pFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);
	if (!hWriteUnHookAPIFile)
	{
		VK_PrintfMsg("����%s�ļ����ʧ��", cUnHookAPIFileName);
	}
	
	hTimer_ReflushCallTime = SetTimer(ghDlgMain, cLV_ReflushCallTimeID, cLV_ReflushCallTime, NULL);	
	return	TRUE;
}

VOID	EndHookAPI()
{
	ULONG	i, ulHookAPINum = 0;
	
	ulHookAPINum = DB_GetStructNum();
	for (i = 0; i<ulHookAPINum; i++)
	{
		UnHookAPIByNum(i);
	}

	if (hTimer_ReflushCallTime != 0)
	{
		KillTimer(ghDlgMain, cLV_ReflushCallTimeID);
	}

	WriteListMsgtoFile();
	

	DB_EndDataArray();
	LV_ClearListView();						//�ͷ�ListView�еĲ����ڴ��
	
	if(pDataBaseMem != NULL)
	{
		VirtualFree(pDataBaseMem, cMaxDataBaseSize, MEM_DECOMMIT);
	}
	if (hWriteUnHookAPIFile != NULL)
	{
		CloseHandle(hWriteUnHookAPIFile);
	}
}


//���ƣ�PrintDataBase
//���ܣ������ݿ����ŵ�API������ʾ��"API�б��"��
//��������
//���أ���
VOID	PrintDataBase()
{
	ULONG		i, StructNum;
	PSTDataBase	pstTempDataBase;
	
	SendDlgItemMessage(ghDlgMain, IDC_LIST_APIArray, LB_RESETCONTENT, 0, NULL);
	StructNum = DB_GetStructNum();
	for (i = 0; i < StructNum; i++ )
	{
		pstTempDataBase = DB_GetStruct(i);
		SendDlgItemMessage(ghDlgMain, IDC_LIST_APIArray, LB_ADDSTRING, 0, pstTempDataBase->bDataBuffer);		
	}
}


//���ƣ�Proc_OnTime 
//���ܣ���ʱ���¼�������
//������_nIDEvent = ��ʱ�����
//���أ���
VOID Proc_OnTime(UINT _nIDEvent)
{
	switch (_nIDEvent)
	{
	case	cLV_ReflushCallTimeID:
		{
			LV_ReflushCallTime();
			break;
		}
		
	case	2:
		{
			
			break;	
		}
	}
}


//���ƣ�BTNClickProc_OrderShowList
//���ܣ�"����"��ť�Ĵ�����
//��������
//���أ���
VOID	BTNClickProc_OrderShowList()
{
	HWND	hShowList;
	ULONG	ulGWLStyle;
	BYTE	pbMsg[256];
	
	hShowList = GetDlgItem(ghDlgMain, IDC_LIST_Show);
	ulGWLStyle = GetWindowLong(hShowList, GWL_STYLE);
	SetWindowLong(hShowList, GWL_STYLE, ulGWLStyle | LBS_SORT);	
	//ulGWLStyle = GetWindowLong(hShowList, GWL_STYLE);
	//VK_PrintfMsg("�ؼ����Ϊ0x%X", ulGWLStyle);	
}


//���ƣ�BTNClickProc_AddAPI
//���ܣ�"���API"��ť�Ĵ�����
//��������
//���أ���
VOID	BTNClickProc_AddAPI()
{
	HANDLE	hDllMudle;
	PVOID	pAPIAddr;
	ULONG	ulSeclectModulesCBOIdex;
	BYTE	EditBuffer[MAX_PATH];
	
	GetDlgItemText(ghDlgMain, IDC_EDIT_APIName, EditBuffer, MAX_PATH);
	if (DB_FindData((ULONG)EditBuffer, cDB_FindMethod_ByDataBuffer) == cDB_Fail)		//�ж��Ƿ���ͬ����API�Ѿ��������ݿ���
	{
		ulSeclectModulesCBOIdex = SendDlgItemMessage(ghDlgMain, IDC_COMBO_Mudles, CB_GETCURSEL, 0, NULL);	//��ȡ��ǰѡ�е�Dll
		hDllMudle = stModuleArray[ulSeclectModulesCBOIdex].ulModuleHandle;
		pAPIAddr = GetProcAddress(hDllMudle, EditBuffer);
		if (pAPIAddr != NULL)
		{
			if(HookAPI(ulSeclectModulesCBOIdex, pAPIAddr, EditBuffer))
			{
				PrintDataBase();
			}
			else
			{
				VK_MsgBox("Hook ʧ��");
			}
		}
		else
		{
			VK_MsgBox("���APIʧ��,����ѡ��ģ���API�����Ƿ���ȷ");
		}
	}
	else
	{
		VK_MsgBox("��API�Ѿ���Hook");
	}
}

//���ƣ�BTNClickProc_DelAPI
//���ܣ�"ɾ��API"��ť�Ĵ�����
//��������
//���أ���
VOID	BTNClickProc_DelAPI()
{
	ULONG	SelectAPIListIndex = LB_ERR;

	SelectAPIListIndex = SendDlgItemMessage(ghDlgMain, IDC_LIST_APIArray, LB_GETCURSEL, 0, 0);
	if (SelectAPIListIndex == LB_ERR)
	{
		VK_MsgBox("��ѡ��Ҫɾ����API");
	} 
	else
	{
		UnHookAPIByNum(SelectAPIListIndex);
		DB_DeleteData(SelectAPIListIndex);
		PrintDataBase();
	}
}


//���ƣ�BTNClickProc_UnHookAllAPI
//���ܣ�"ɾ������API"��ť�Ĵ�����
//��������
//���أ���
VOID	BTNClickProc_UnHookAllAPI()
{
	ULONG	ulHookAPINum, i;

	ulHookAPINum = DB_GetStructNum();
	for (i = 0; i < ulHookAPINum; i++)
	{
		UnHookAPIByNum(i);
	}
	DB_ClearDataArray();
	PrintDataBase();
}

//���ƣ�BTNClickProc_HookDllAPI
//���ܣ�����DLL�������Hookָ��DLL�����е������Ƶ�API
//��������
//���أ�����Hook API����
ULONG	BTNClickProc_HookDllAPI()
{
	HANDLE	hModule;
	PVOID	pAPIAddr;
	PBYTE	pAPIName, pbModuleName;
	PWORD	pFOT;
	PULONG	pFAT, pFNT;
	BYTE	pbMsg[64];
	ULONG	ulHookAPINum = 0;
	ULONG	i, ulSeclectModulesCBOIdex, ulDataBaseNum, ulWritedBytes;
	PSTDataBase					pstTempDataBase;
	PIMAGE_DOS_HEADER			pImg_DosHeader;			//DOSͷ��
	PIMAGE_NT_HEADERS			pImg_NtHeader;			//PE�ļ�ͷ
	PIMAGE_EXPORT_DIRECTORY		pImg_ExportDirectory;	//������


	ulSeclectModulesCBOIdex = SendDlgItemMessage(ghDlgMain, IDC_COMBO_Mudles, CB_GETCURSEL, 0, NULL);	//��ȡ��ǰѡ�е�DLL
	if (ulSeclectModulesCBOIdex == 0)
	{
		VK_MsgBox("����ѡ��һ��DLL");
		return -1;
	}
	pstTempDataBase = pDataBaseMem;												//ɾ����UnhookҪHook��Dll�Ѵ������ݿ��е�API
	pbModuleName = stModuleArray[ulSeclectModulesCBOIdex].pbModuleName;
	sprintf(pbMsg, "%s��Hookʧ�ܵ�API��\r\n\r\n", pbModuleName);
	WriteFile(hWriteUnHookAPIFile, pbMsg, VK_Lengthsz(pbMsg), &ulWritedBytes, NULL);

	ulDataBaseNum = DB_GetStructNum();
	for (i = 0; i < ulDataBaseNum; i++)
	{
		if (pstTempDataBase[i].ulIndex == ulSeclectModulesCBOIdex)
		{
			UnHookAPIByNum(i);
			DB_DeleteData(i);
		}
	}

	//��ȡDLL�������Hook FNT�������е�API
	(HANDLE)pImg_DosHeader = stModuleArray[ulSeclectModulesCBOIdex].ulModuleHandle;
	(ULONG)pImg_NtHeader = (ULONG)pImg_DosHeader + pImg_DosHeader->e_lfanew;
	if (pImg_NtHeader->Signature != 0x4550)										//�ж��Ƿ�Ϊ��׼PE�ļ�					
	{
		VK_MsgBox("���ļ����Ǳ�׼PE�ļ�\n");
		return	0;
	}

	(ULONG)pImg_ExportDirectory = (ULONG)pImg_DosHeader + pImg_NtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;		//��λ������
	(ULONG)pFOT = (ULONG)pImg_DosHeader + pImg_ExportDirectory->AddressOfNameOrdinals;			//����������ű�
	(ULONG)pFAT = (ULONG)pImg_DosHeader + pImg_ExportDirectory->AddressOfFunctions;				//����������ַ��
	(ULONG)pFNT = (ULONG)pImg_DosHeader + pImg_ExportDirectory->AddressOfNames;					//�����������Ʊ�
	
	for (i =0; i< pImg_ExportDirectory->NumberOfNames; i++)
	{
		(ULONG)pAPIAddr = (ULONG)pImg_DosHeader + pFAT[pFOT[i]];
		(ULONG)pAPIName = (ULONG)pImg_DosHeader + pFNT[i];

		if(HookAPI(ulSeclectModulesCBOIdex, pAPIAddr, pAPIName))
		{
			ulHookAPINum++;
		}
		else
		{
			sprintf(pbMsg, "%s\r\n", pAPIName);									//��Hook���ɹ���APIд��UnHookAPI.txt�ļ���
			WriteFile(hWriteUnHookAPIFile, pbMsg, VK_Lengthsz(pbMsg), &ulWritedBytes, NULL);
		}
	}
	
	PrintDataBase();
	sprintf(pbMsg, "%s���� %d ��API��Hook�ɹ� %d��", pbModuleName, pImg_ExportDirectory->NumberOfNames, ulHookAPINum);
	VK_MsgBox(pbMsg);

	return	ulHookAPINum;
}



//���ƣ�BTNClickProc_GetModuleMsg
//���ܣ�"��ȡģ����Ϣ"��ť�Ĵ�����
//��������
//���أ���
VOID	BTNClickProc_GetModuleMsg()
{
	ULONG	ulModulesCBOIdex;
	BYTE	pbModuleMsg[MAX_PATH];
	

	ulModulesCBOIdex = SendDlgItemMessage(ghDlgMain, IDC_COMBO_Mudles, CB_GETCURSEL, 0, NULL);
	sprintf(pbModuleMsg, "ģ�����ƣ�%s\nģ���ַ��0x%X\nģ���С��0x%X",
			stModuleArray[ulModulesCBOIdex].pbModuleName, 
			stModuleArray[ulModulesCBOIdex].ulModuleHandle,
			stModuleArray[ulModulesCBOIdex].ulModuleSize);
	VK_MsgBox(pbModuleMsg);
}



//���ƣ�BTNClickProc_GetParam
//���ܣ�"������Ϣ"��ť�Ĵ�����
//��������
//���أ���
VOID	BTNClickProc_GetParam()
{
	ULONG	ulSelectItem, ulAPIIndex;
	BYTE	pbAPIName[MAX_PATH];
	BYTE	pbModuleName[MAX_PATH];
	BYTE	pbAPIParamINIPath[MAX_PATH];
	BYTE	pbParamNameBuffer[MAX_PATH];
	BYTE	pbMsgBuffer[0x1000];
	PSTLVCallArray	pstLVCallArray;
	

	ulSelectItem = LV_GetSelectItem();
	if (ulSelectItem == cLV_Fail)
	{
		VK_MsgBox("û�и�ѡ����");
	}
	else
	{
		LV_GetItemText(2, ulSelectItem, pbAPIName);
		pstLVCallArray = LV_GetSTLVCallArray(ulSelectItem);
		
		sprintf(pbAPIParamINIPath, "%s%s%s", pszAPIMonitorDirectory, 
			stModuleArray[pstLVCallArray->wAPIModuleIndex].pbModuleName, ".ini");
		GetParamName(pbAPIParamINIPath, pbAPIName, pbParamNameBuffer);
		
		if(LV_OutPutParamMsg(ulSelectItem, pbMsgBuffer, pbParamNameBuffer))
		{
			VK_MsgBox(pbMsgBuffer);
		}
		else
		{
			VK_MsgBox("��APIû�в��������δ����");
		}	
	}
}


//���ƣ�GetCodeLength
//���ܣ������һ��ָ���ȡ��Ҫ���ȵ���С�����С
//������_pCodeAddr = �����ָ�����ʼ��ַ
//������_ulNeedLength = ����ĳ���
//���أ���
ULONG	GetCodeLength(PVOID _pCodeAddr, ULONG _ulNeedLength)
{
	ULONG			CodeLength = 0, RetLength = 0;
	PVOID			pTemp;
	STINSTRUCTION	stInstruction;
	
	pTemp = _pCodeAddr;
	while (RetLength < _ulNeedLength)
	{
		CodeLength = LDisEngine(pTemp, &stInstruction);
		(ULONG)pTemp += CodeLength;
		RetLength += CodeLength; 
	}
	
	return	RetLength;
}


//���ƣ�HookAPI
//���ܣ�Hook API,���������ݿ���
//������_ulSTModuleIndex = API����ģ����STModuleArray�����е����
//������_pulAPIAddr = API�ĵ�ַ
//������_pbAPIName = API����ָ��
//���أ��ɹ�����TRUE��ʧ�ܷ���FALSE
BOOL	HookAPI(ULONG _ulSTModuleIndex, PVOID _pAPIAddr, PBYTE _pszAPIName)
{
	ULONG	i, ulCodeLength, ulOldProtect = 0;
	BYTE	pbCodeBuffer[32];
	BYTE	pbAPIParamINIPath[MAX_PATH];
	PBYTE	pNopCode;
	PULONG	pulCallAddr, pulCode;
	STParam	stParam;
	PSTDataBase	pstCurrentDataBase;
	
	
	//VK_PrintfMsg("��ʼHook %s ����", _pszAPIName);
	if (strcmp(_pszAPIName, "Sleep") == 0)
	{
		return	FALSE;
	}
	
	ulCodeLength = GetCodeLength(_pAPIAddr, cHookCodeLength);
	//����VirtualProtect,�޸��ڴ�
	//����PAGE_EXECUTE_READWRITE��0x40�����ڴ�ҳΪ�ɶ���д��ִ��
	//����ulOldProtect�������ڴ�ԭʼ�������͵�ַ
	VirtualProtect(_pAPIAddr, ulCodeLength, PAGE_EXECUTE_READWRITE, &ulOldProtect);

	pulCode = (PULONG)_pAPIAddr;
	if ((*pulCode != 0x8B55FF8B) && ((*pulCode & 0xFFFFFF) != 0xEC8B55))		//�ڴ��ͻ
	{
		//VK_PrintfMsg("Hook %s ����ʧ�� ", _pszAPIName );
		return	FALSE;
	}

	//д��API Hook���ִ�д���
	pbCodeBuffer[0] = 0x60;														//pushad
	pbCodeBuffer[1] = 0x9C;														//pushfd
	pbCodeBuffer[2] = 0x54;														//push esp
	pbCodeBuffer[3] = 0xB8;														//mov eax, &HookCall
	pulCallAddr = (PULONG) &pbCodeBuffer[4];
	*pulCallAddr = (ULONG)&HookCall;														
	pbCodeBuffer[8] = 0xFF;														//call eax
	pbCodeBuffer[9] = 0xD0;														
	pbCodeBuffer[10] = 0x9D;													//popfd
	pbCodeBuffer[11] = 0x61;													//popad
	pbCodeBuffer[12] = 0x83;													//д��add esp, 4 ƽ���ջ
	pbCodeBuffer[13] = 0xC4;
	pbCodeBuffer[14] = 0x4;

	//���汻Hook��APIԭ����
	VK_CopyMem(_pAPIAddr, &pbCodeBuffer[cOldAPICodeOffset], ulCodeLength);		
	
	//��ȡ��ӦAPI������Ϣ
	sprintf(pbAPIParamINIPath, "%s%s%s", pszAPIMonitorDirectory, stModuleArray[_ulSTModuleIndex].pbModuleName, ".ini");
	GetParamNumAndType(pbAPIParamINIPath, _pszAPIName, &stParam);

	DB_AddData(	_ulSTModuleIndex, (ULONG)_pAPIAddr, VK_Lengthsz(_pszAPIName) + 1, _pszAPIName, 
				ulCodeLength + cOldAPICodeOffset + cOldAPICodeEndCodeNum, pbCodeBuffer, &stParam);
	pstCurrentDataBase = DB_GetStruct(DB_GetStructNum() - 1);					//��ȡд���STDataBase�ṹ��ַ

	//д������ԭAPIִ�еĴ���
	if(WriteJmpCode(&pstCurrentDataBase->bCodeBuffer[cOldAPICodeOffset + ulCodeLength], (PVOID)((ULONG)_pAPIAddr + ulCodeLength))==FALSE)
		VK_PrintfMsg("д�� JMP %s ����ʧ�� ", _pszAPIName );

	_try
	{
		WriteCallCode(_pAPIAddr, pstCurrentDataBase->bCodeBuffer);
		//������Ĵ���дΪnop
		/*(ULONG)pNopCode = (ULONG)_pAPIAddr + cHookCodeLength;
		for (i = 0; i < (ulCodeLength - cHookCodeLength); i++)
		{
			pbCodeBuffer[i] = 0x90;
		}*/
		
	}
	_except(1)
	{
		DB_DeleteData(DB_GetStructNum() - 1);									//���Hookָ��APIʧ��������ݿ���ɾ��
		VK_PrintfMsg("%s����Hookʧ��", _pszAPIName);
		return	FALSE;
	}
	
	return	TRUE;
}


//���ƣ�UnHookAPIByNum
//���ܣ�ж��API Hook
//������_ulNum = Ҫж�ص����
//���أ��ɹ�����TRUE��ʧ�ܷ���FALSE
VOID	UnHookAPIByNum(ULONG _ulNum)
{
	ULONG	ulOldAPICodeSize;
	PVOID	pOldAPICode;
	PSTDataBase	pstTempDataBase;

	pstTempDataBase	= DB_GetStruct(_ulNum);
	if (pstTempDataBase != NULL)
	{
		ulOldAPICodeSize = pstTempDataBase->ulCodeSize - cOldAPICodeOffset - cOldAPICodeEndCodeNum;
		(ULONG)pOldAPICode = (ULONG)pstTempDataBase->bCodeBuffer + cOldAPICodeOffset;
		VK_CopyMem(pOldAPICode, pstTempDataBase->ulData, ulOldAPICodeSize);
	}
}


//���ƣ�HookCall
//���ܣ�API Hook��Ĵ�����������Ϊ_stdcall
//������_ulEsp = esp��ֵ
//���أ���
BOOL	_stdcall HookCall(ULONG _ulEsp)
{
	ULONG		ulNum, ulFindStringIndex, ulSetitemData;
	ULONG		i, ulHookCallAddr, ulAPICallAddr, ulParamAddr;
	PULONG		pulCheckModuleName = NULL;
	PBYTE		pCallModuleName = NULL;
	BYTE		pUnFindModuleName[] = {"δ֪ģ��"};
	BYTE		pListMsg[MAX_PATH];
	PSTDataBase	pstTempDataBase;
	STParam_ListViewHandler	stParam_ListViewHandler;
	if (IsHookCallAgain)					//����Ƿ�����HookCall()���е��ú�����Hook�������ѭ������
	{	
		return	FALSE;						//�����⵽ѭ�����ã�����ֱ�ӷ���
	}
	else
	{
		IsHookCallAgain = TRUE;
	}

	if (!IsMonitor)							//����Ƿ������أ���������ֱ�ӷ���
	{
		IsHookCallAgain = FALSE;			//�ָ������
		
		return	FALSE;
	}

	//����pushad��pushfd�����ϵ�9��ΪHook��callָ��ķ��ص�ַ����10��Ϊ�����ߵķ��ص�ַ
	_asm
	{
		mov		eax, _ulEsp
		add		eax, 36
		mov		eax, [eax]
		sub		eax, cHookCodeLength
		mov		ulHookCallAddr, eax

		mov		eax, _ulEsp
		add		eax, 40
		mov		eax, [eax]
		mov		ulAPICallAddr, eax

		mov		eax, _ulEsp
		add		eax, 44
		mov		ulParamAddr, eax
	}

	ulNum = DB_FindData(ulHookCallAddr, cDB_FindMethod_ByData);
	pstTempDataBase = DB_GetStruct(ulNum);

	pbLastCallAPIName = pstTempDataBase->bDataBuffer;
	//VK_PrintfMsg(" %sģ�����API%s", stModuleArray[i].pbModuleName,pstTempDataBase->bDataBuffer);
	//��ʾAPI���ã���д���ļ�
	for (i = 0; i < ulModuleNum; i++)
	{
		if ((ulAPICallAddr > (ULONG)stModuleArray[i].ulModuleHandle) && ulAPICallAddr < ((ULONG)stModuleArray[i].ulModuleHandle + stModuleArray[i].ulModuleSize))
		{
			pCallModuleName = stModuleArray[i].pbModuleName;
			break;
		}
	}
	if (pCallModuleName == NULL)
	{
		pCallModuleName = pUnFindModuleName;
	}
	
	pulCheckModuleName = pCallModuleName;

	//�жϵ������Ƿ���"uxtheme.dll"��"APIMonitor.dll",����������Դ���
	if ((pulCheckModuleName[0] != 0x68747875 || pulCheckModuleName[1] != 0x2E656D65) &&  pulCheckModuleName[0] != 0x2E6C6C44)
	{
		if (IsShowSystem)															//����Ƿ�������ʾϵͳ����
		{
			stParam_ListViewHandler.ulCallAddr = ulAPICallAddr;
			stParam_ListViewHandler.pszModuleName = pCallModuleName;
			stParam_ListViewHandler.pszAPIName = pstTempDataBase->bDataBuffer;
			stParam_ListViewHandler.ulParamAddr = ulParamAddr;
			stParam_ListViewHandler.pstParam = &(pstTempDataBase->stParam);
			stParam_ListViewHandler.wAPIModeleIndex = pstTempDataBase->ulIndex;
			LV_ListViewHandler(&stParam_ListViewHandler);
		}
		else
		{
			if (CheckCallModule(pCallModuleName))									//������������жϵ������ǲ���ϵͳDLL
			{
				stParam_ListViewHandler.ulCallAddr = ulAPICallAddr;
				stParam_ListViewHandler.pszModuleName = pCallModuleName;
				stParam_ListViewHandler.pszAPIName = pstTempDataBase->bDataBuffer;
				stParam_ListViewHandler.ulParamAddr = ulParamAddr;
				stParam_ListViewHandler.pstParam = &(pstTempDataBase->stParam);
				stParam_ListViewHandler.wAPIModeleIndex = pstTempDataBase->ulIndex;
				LV_ListViewHandler(&stParam_ListViewHandler);
			}
		}
	}
	
	IsHookCallAgain = FALSE;				//�ָ������

	return	TRUE;
}

//���ƣ�CheckCallModule
//���ܣ�������API��ģ���Ƿ�������ʾ����
//������_pszModuleName = ģ������
//���أ�������TRUE��������FALSE
BOOL CheckCallModule(PBYTE _pszModuleName)
{

	if(lstrcmpi(_pszModuleName, "kernel32.dll") == 0)
	{
		return	FALSE;
	}

	if (lstrcmpi(_pszModuleName, "kernelbase.dll") == 0)
	{
		return	FALSE;
	}

	if(lstrcmpi(_pszModuleName, "user32.dll") == 0)
	{
		return	FALSE;
	}

	if(lstrcmpi(_pszModuleName, "MSCTF.dll") == 0)
	{
		return	FALSE;
	}

	if(lstrcmpi(_pszModuleName, "COMCTL32.dll") == 0)
	{
		return	FALSE;
	}

	if(lstrcmpi(_pszModuleName, "GDI32.dll") == 0)
	{
		return	FALSE;
	}

	if(lstrcmpi(_pszModuleName, "GDI32.dll") == 0)
	{
		return	FALSE;
	}

	if(lstrcmpi(_pszModuleName, "msctfime.ime") == 0)
	{
		return	FALSE;
	}
	
	if(lstrcmpi(_pszModuleName, "IMM32.dll") == 0)
	{
		return	FALSE;
	}

	return	TRUE;
}



//���ƣ�GetProcessModules
//���ܣ���ȡָ������ID������Dllģ��
//������_ulProcessID = ����ID
//������_pstModule = PSTModule�ṹ�����ŵĵ�ַ
//���أ���ǰģ������
ULONG	GetProcessModules(ULONG _ulProcessID, PSTModule _pstModule)
{
	ULONG	ulModuleNum = 0;
	HANDLE	hSnapShot;
	MODULEENTRY32 stModuleEntry32;
	
	stModuleEntry32.dwSize = sizeof(MODULEENTRY32);
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, _ulProcessID);
	if(Module32First(hSnapShot, &stModuleEntry32))
	{
		_pstModule[0].ulModuleHandle = (HANDLE)stModuleEntry32.hModule;
		_pstModule[0].ulModuleSize = stModuleEntry32.modBaseSize;
		VK_Copysz(stModuleEntry32.szModule, _pstModule[0].pbModuleName);
		SendDlgItemMessage(ghDlgMain, IDC_COMBO_Mudles, CB_ADDSTRING, 0, stModuleEntry32.szModule);
		ulModuleNum++;

		while (Module32Next(hSnapShot, &stModuleEntry32))
		{
			_pstModule[ulModuleNum].ulModuleHandle = (HANDLE)stModuleEntry32.hModule;
			_pstModule[ulModuleNum].ulModuleSize = stModuleEntry32.modBaseSize;
			VK_Copysz(stModuleEntry32.szModule, _pstModule[ulModuleNum].pbModuleName);
			SendDlgItemMessage(ghDlgMain, IDC_COMBO_Mudles, CB_ADDSTRING, 0, stModuleEntry32.szModule);

			if(strcmp(cAPIMonitorDllName, stModuleEntry32.szModule) == 0)				//��ȡAPIMonitor����ĵ�ַ
			{
				VK_Copysz(stModuleEntry32.szExePath, pszAPIMonitorPath);
			}
			ulModuleNum++;
		}
	}
	
	return ulModuleNum;
}


//���ƣ�VEHCallBackProc
//���ܣ�VEH�����쳣��Ļص�����
//������_ExceptionAddr = �����쳣�ĵ�ַ
//������_Param = δ�������
//���أ���
VOID	_stdcall VEHCallBackProc(ULONG _ExceptionAddr, ULONG _Param)
{
	ULONG		ulNum, i;
	PBYTE		pCallModuleName = NULL;
	BYTE		pExceptionName[] = {"�쳣"};
	BYTE		pUnFindModuleName[] = {"δ֪ģ��"};
	STParam_ListViewHandler	stParam_ListViewHandler;
	
	//��ʾAPI���ã���д���ļ�
	for (i = 0; i < ulModuleNum; i++)
	{
		if ((_ExceptionAddr > (ULONG)stModuleArray[i].ulModuleHandle) && 
			 _ExceptionAddr < ((ULONG)stModuleArray[i].ulModuleHandle + stModuleArray[i].ulModuleSize))
		{
			pCallModuleName = stModuleArray[i].pbModuleName;
			break;
		}
	}
	if (pCallModuleName == NULL)
	{
		pCallModuleName = pUnFindModuleName;
	}
	
	stParam_ListViewHandler.ulCallAddr = _ExceptionAddr;
	stParam_ListViewHandler.pszModuleName = pCallModuleName;
	stParam_ListViewHandler.pszAPIName = pExceptionName;
	stParam_ListViewHandler.ulParamAddr = 0;
	stParam_ListViewHandler.pstParam = NULL;
	stParam_ListViewHandler.wAPIModeleIndex = 0;
	LV_ListViewHandler(&stParam_ListViewHandler);
}


//���ƣ�GetEditData
//���ܣ���ȡ�༭������
//������ _EditID = �༭���ID
//������_pszFormat = ��ȡ�ĸ�ʽ
//���أ��༭���������,ʧ���򷵻�0xFFFFFFFF(cSTATUS_Fail)
DWORD	GetEditData(DWORD _EditID, PBYTE _pszFormat)
{
	DWORD	EditData = cSTATUS_Fail;
	BYTE	EditBuffer[MAX_PATH];
	
	if(GetDlgItemText(ghDlgMain, _EditID, EditBuffer, 12))
	{
		if(!sscanf(EditBuffer, _pszFormat, &EditData))
		{
			EditData = cSTATUS_Fail;
		}	
	}
	
	return	EditData;
}

//���ƣ�SetEditData
//���ܣ���ȡ�༭������
//������ _EditID = �༭���ID
//������_pszFormat = д��ĸ�ʽ
//������_EditData = д�������
//���أ���
VOID	SetEditData(DWORD _EditID, PBYTE _pszFormat, DWORD _EditData)
{
	BYTE	EditBuffer[MAX_PATH];
	
	sprintf(EditBuffer, _pszFormat, _EditData);
	SetDlgItemText(ghDlgMain, _EditID, EditBuffer);
}


//���ƣ�WriteListMsgtoFile
//���ܣ�����ListView�ϵ����ݵ��ļ�
//��������
//���أ���
VOID	WriteListMsgtoFile()
{
	ULONG	i, ulListNum, ulWritedBytes;
	HANDLE	hWriteFile;
	BYTE	pszFilePath[MAX_PATH];
	BYTE	pszListMsg[MAX_PATH];
	SYSTEMTIME	stSystemtime;

	GetLocalTime(&stSystemtime);
	VK_GetFileDirectory(pszAPIMonitorPath, pszFilePath);
	sprintf(pszFilePath, "%sHookRecord\\%s(%d-%d-%d-%d%d).txt", pszFilePath, stModuleArray[0].pbModuleName, 
			LOWORD(stSystemtime.wYear), LOWORD(stSystemtime.wMonth),  LOWORD(stSystemtime.wDay), 
			LOWORD(stSystemtime.wHour), LOWORD(stSystemtime.wMinute));

	hWriteFile = CreateFile(pszFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);
	if (hWriteFile)
	{
		ulListNum = LV_GetNum();
		for (i = 0; i < ulListNum; i++ )
		{
			LV_OutPut(i, pszListMsg);
			WriteFile(hWriteFile, pszListMsg, VK_Lengthsz(pszListMsg), &ulWritedBytes, NULL);
		}
		CloseHandle(hWriteFile);
	}
	else
	{
		VK_MsgBox("����%s�ļ����ʧ��");
	}	
}

//���ƣ� WriteCallCode
//���ܣ���ָ���ĵ�ַд��Call����
//����1��_pWriteAddr=Ҫд��ĵ�ַ
//����2��_pCallAddr=Ҫд���Call�������ת��ַ
//���أ��ɹ��򷵻�TURE�����򷵻�FALSE
BOOL	WriteCallCode(PVOID _pWriteAddr, PVOID _pCallAddr)
{
	ULONG ulCallCode;
	
	if (_pWriteAddr != NULL && _pCallAddr != NULL)
	{
		ulCallCode= (ULONG)_pCallAddr - (ULONG)_pWriteAddr -5;							//�õ�Ҫд�����ת��ַ��
		_asm
		{
			pushad
			
			mov	eax, ulCallCode
			mov	ebx, _pWriteAddr
			mov	byte ptr ds:[ebx], 0xE8		//Call
			inc	ebx
			mov	[ebx], eax

			popad			
		}
		return TRUE;
	}

	return	FALSE;
}


//���ƣ� WriteJmpCode
//���ܣ���ָ���ĵ�ַд��Jmp����
//����1��_pWriteAddr=Ҫд��ĵ�ַ
//����2��_pJmpAddr=Ҫд���Jmp�������ת��ַ
//���أ��ɹ��򷵻�TURE�����򷵻�FALSE
BOOL	WriteJmpCode(PVOID _pWriteAddr, PVOID _pJmpAddr)
{
	ULONG ulJmpCode;
	
	if (_pWriteAddr != NULL && _pJmpAddr != NULL)
	{
		//�õ�Ҫд�����ת��ַ��
		//���㹫ʽ��jmp��ת�ĵ�ַ=�Լ�ʵ�ֵĺ�����ַ-��jmp���ڵĵ�ַ+5) 
		//*((ULONG*)(__HookCode + 1)) = (ULONG)FakeFuncAddress - ((ULONG)OriginalFuncAddress + 5);
		ulJmpCode= (ULONG)_pJmpAddr - (ULONG)_pWriteAddr -5;							
		_asm
		{
			pushad
				
				mov	eax, ulJmpCode
				mov	ebx, _pWriteAddr
				mov	byte ptr ds:[ebx], 0xE9		//Jmp
				inc	ebx
				mov	[ebx], eax
				
			popad			
		}
		return TRUE;
	}
	
	return	FALSE;
}
