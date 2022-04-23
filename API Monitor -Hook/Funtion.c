//Funtion.c
//辅助功能函数

#include "Funtion.h"

BOOL		IsShowSystem = FALSE;
HANDLE		hHookWindowsThread;									//进程的主线程句柄
HANDLE		hWriteUnHookAPIFile = NULL;
PVOID		pDataBaseMem = NULL;
ULONG		ulModuleNum = 0;									//进程模块的总数
BOOL		IsMonitor = TRUE;
ULONG		hTimer_ReflushCallTime = 0;
BOOL		IsHookCallAgain = FALSE;							//在HookCall()中检测是否被循环调用
BOOL		IsExceptionMonitor = FALSE;							//检测"异常检测"是否在进行中
BYTE		pszAPIMonitorPath[MAX_PATH];
BYTE		pszAPIMonitorDirectory[MAX_PATH];
PBYTE		pbLastCallAPIName = 0x55555550;
STModule	stModuleArray[cMaxModuleNum];						//进程模块结构数组

//名称：FuntionInit() 
//功能：初始化函数，做一些初始化工作
//参数：无
//返回：成功返回TRUE，失败返回FALSE
BOOL	FuntionInit()
{
	HANDLE		hAPIMonitor;
	ULONG		i, APIMonitorNum;
	TCHAR		pFilePath[MAX_PATH];

	STParam		stParam;
	
	//申请内存，用来存放数据库
	pDataBaseMem = VirtualAlloc(NULL, cMaxDataBaseSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (pDataBaseMem == NULL)
	{
		VK_MsgBox("申请内存失败");
		return	FALSE;
	}
	DB_InitDataArray(FALSE, pDataBaseMem, cMaxDataBaseSize);					//按非默认的方式初始化API数据库
	InitLDisEngine();															//初始化反汇编引擎
	ulModuleNum = GetProcessModules(GetCurrentProcessId(), stModuleArray);		//获取当前进程所有的Module

	SendDlgItemMessage(ghDlgMain, IDC_COMBO_Mudles, CB_SETCURSEL, 0, NULL);		//显示Module
	SetWindowText(ghDlgMain, stModuleArray[0].pbModuleName);					//将窗口名称设置为注入进程的名称
	LV_InitListView(ghDlgMain, GetDlgItem(ghDlgMain, IDC_LISTV_Show));			//初始化ListView

	//获取程序路径，建立所需文件
	VK_GetFileDirectory(pszAPIMonitorPath, pFilePath);
	sprintf(pszAPIMonitorDirectory, "%s%s", pFilePath, cINIFolderName);

	sprintf(pFilePath, "%s%s", pFilePath, cUnHookAPIFileName);
	hWriteUnHookAPIFile = CreateFile(pFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);
	if (!hWriteUnHookAPIFile)
	{
		VK_PrintfMsg("创建%s文件句柄失败", cUnHookAPIFileName);
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
	LV_ClearListView();						//释放ListView中的参数内存块
	
	if(pDataBaseMem != NULL)
	{
		VirtualFree(pDataBaseMem, cMaxDataBaseSize, MEM_DECOMMIT);
	}
	if (hWriteUnHookAPIFile != NULL)
	{
		CloseHandle(hWriteUnHookAPIFile);
	}
}


//名称：PrintDataBase
//功能：将数据库类存放的API名称显示在"API列表框"中
//参数：无
//返回：无
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


//名称：Proc_OnTime 
//功能：定时器事件处理函数
//参数：_nIDEvent = 定时器序号
//返回：无
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


//名称：BTNClickProc_OrderShowList
//功能："排序"按钮的处理函数
//参数：无
//返回：无
VOID	BTNClickProc_OrderShowList()
{
	HWND	hShowList;
	ULONG	ulGWLStyle;
	BYTE	pbMsg[256];
	
	hShowList = GetDlgItem(ghDlgMain, IDC_LIST_Show);
	ulGWLStyle = GetWindowLong(hShowList, GWL_STYLE);
	SetWindowLong(hShowList, GWL_STYLE, ulGWLStyle | LBS_SORT);	
	//ulGWLStyle = GetWindowLong(hShowList, GWL_STYLE);
	//VK_PrintfMsg("控件风格为0x%X", ulGWLStyle);	
}


//名称：BTNClickProc_AddAPI
//功能："添加API"按钮的处理函数
//参数：无
//返回：无
VOID	BTNClickProc_AddAPI()
{
	HANDLE	hDllMudle;
	PVOID	pAPIAddr;
	ULONG	ulSeclectModulesCBOIdex;
	BYTE	EditBuffer[MAX_PATH];
	
	GetDlgItemText(ghDlgMain, IDC_EDIT_APIName, EditBuffer, MAX_PATH);
	if (DB_FindData((ULONG)EditBuffer, cDB_FindMethod_ByDataBuffer) == cDB_Fail)		//判断是否有同名的API已经存在数据库中
	{
		ulSeclectModulesCBOIdex = SendDlgItemMessage(ghDlgMain, IDC_COMBO_Mudles, CB_GETCURSEL, 0, NULL);	//获取当前选中的Dll
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
				VK_MsgBox("Hook 失败");
			}
		}
		else
		{
			VK_MsgBox("添加API失败,请检查选择模块或API名称是否正确");
		}
	}
	else
	{
		VK_MsgBox("该API已经被Hook");
	}
}

//名称：BTNClickProc_DelAPI
//功能："删除API"按钮的处理函数
//参数：无
//返回：无
VOID	BTNClickProc_DelAPI()
{
	ULONG	SelectAPIListIndex = LB_ERR;

	SelectAPIListIndex = SendDlgItemMessage(ghDlgMain, IDC_LIST_APIArray, LB_GETCURSEL, 0, 0);
	if (SelectAPIListIndex == LB_ERR)
	{
		VK_MsgBox("请选择要删除的API");
	} 
	else
	{
		UnHookAPIByNum(SelectAPIListIndex);
		DB_DeleteData(SelectAPIListIndex);
		PrintDataBase();
	}
}


//名称：BTNClickProc_UnHookAllAPI
//功能："删除所有API"按钮的处理函数
//参数：无
//返回：无
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

//名称：BTNClickProc_HookDllAPI
//功能：根据DLL的输出表，Hook指定DLL中所有的有名称的API
//参数：无
//返回：返回Hook API数量
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
	PIMAGE_DOS_HEADER			pImg_DosHeader;			//DOS头部
	PIMAGE_NT_HEADERS			pImg_NtHeader;			//PE文件头
	PIMAGE_EXPORT_DIRECTORY		pImg_ExportDirectory;	//导出表


	ulSeclectModulesCBOIdex = SendDlgItemMessage(ghDlgMain, IDC_COMBO_Mudles, CB_GETCURSEL, 0, NULL);	//获取当前选中的DLL
	if (ulSeclectModulesCBOIdex == 0)
	{
		VK_MsgBox("请先选择一个DLL");
		return -1;
	}
	pstTempDataBase = pDataBaseMem;												//删除并Unhook要Hook的Dll已存入数据库中的API
	pbModuleName = stModuleArray[ulSeclectModulesCBOIdex].pbModuleName;
	sprintf(pbMsg, "%s中Hook失败的API：\r\n\r\n", pbModuleName);
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

	//读取DLL的输出表，Hook FNT表中所有的API
	(HANDLE)pImg_DosHeader = stModuleArray[ulSeclectModulesCBOIdex].ulModuleHandle;
	(ULONG)pImg_NtHeader = (ULONG)pImg_DosHeader + pImg_DosHeader->e_lfanew;
	if (pImg_NtHeader->Signature != 0x4550)										//判断是否为标准PE文件					
	{
		VK_MsgBox("该文件不是标准PE文件\n");
		return	0;
	}

	(ULONG)pImg_ExportDirectory = (ULONG)pImg_DosHeader + pImg_NtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;		//定位导出表
	(ULONG)pFOT = (ULONG)pImg_DosHeader + pImg_ExportDirectory->AddressOfNameOrdinals;			//导出函数序号表
	(ULONG)pFAT = (ULONG)pImg_DosHeader + pImg_ExportDirectory->AddressOfFunctions;				//导出函数地址表
	(ULONG)pFNT = (ULONG)pImg_DosHeader + pImg_ExportDirectory->AddressOfNames;					//导出函数名称表
	
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
			sprintf(pbMsg, "%s\r\n", pAPIName);									//将Hook不成功的API写入UnHookAPI.txt文件中
			WriteFile(hWriteUnHookAPIFile, pbMsg, VK_Lengthsz(pbMsg), &ulWritedBytes, NULL);
		}
	}
	
	PrintDataBase();
	sprintf(pbMsg, "%s共有 %d 个API，Hook成功 %d个", pbModuleName, pImg_ExportDirectory->NumberOfNames, ulHookAPINum);
	VK_MsgBox(pbMsg);

	return	ulHookAPINum;
}



//名称：BTNClickProc_GetModuleMsg
//功能："获取模块信息"按钮的处理函数
//参数：无
//返回：无
VOID	BTNClickProc_GetModuleMsg()
{
	ULONG	ulModulesCBOIdex;
	BYTE	pbModuleMsg[MAX_PATH];
	

	ulModulesCBOIdex = SendDlgItemMessage(ghDlgMain, IDC_COMBO_Mudles, CB_GETCURSEL, 0, NULL);
	sprintf(pbModuleMsg, "模块名称：%s\n模块基址：0x%X\n模块大小：0x%X",
			stModuleArray[ulModulesCBOIdex].pbModuleName, 
			stModuleArray[ulModulesCBOIdex].ulModuleHandle,
			stModuleArray[ulModulesCBOIdex].ulModuleSize);
	VK_MsgBox(pbModuleMsg);
}



//名称：BTNClickProc_GetParam
//功能："参数信息"按钮的处理函数
//参数：无
//返回：无
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
		VK_MsgBox("没有该选择项");
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
			VK_MsgBox("该API没有参数或参数未定义");
		}	
	}
}


//名称：GetCodeLength
//功能：反汇编一段指令，获取需要长度的最小代码大小
//参数：_pCodeAddr = 反汇编指令的起始地址
//参数：_ulNeedLength = 所需的长度
//返回：无
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


//名称：HookAPI
//功能：Hook API,并加入数据库中
//参数：_ulSTModuleIndex = API所在模块在STModuleArray数组中的序号
//参数：_pulAPIAddr = API的地址
//参数：_pbAPIName = API名称指针
//返回：成功返回TRUE，失败返回FALSE
BOOL	HookAPI(ULONG _ulSTModuleIndex, PVOID _pAPIAddr, PBYTE _pszAPIName)
{
	ULONG	i, ulCodeLength, ulOldProtect = 0;
	BYTE	pbCodeBuffer[32];
	BYTE	pbAPIParamINIPath[MAX_PATH];
	PBYTE	pNopCode;
	PULONG	pulCallAddr, pulCode;
	STParam	stParam;
	PSTDataBase	pstCurrentDataBase;
	
	
	//VK_PrintfMsg("开始Hook %s 函数", _pszAPIName);
	if (strcmp(_pszAPIName, "Sleep") == 0)
	{
		return	FALSE;
	}
	
	ulCodeLength = GetCodeLength(_pAPIAddr, cHookCodeLength);
	//函数VirtualProtect,修改内存
	//参数PAGE_EXECUTE_READWRITE（0x40），内存页为可读可写可执行
	//参数ulOldProtect，保存内存原始属性类型地址
	VirtualProtect(_pAPIAddr, ulCodeLength, PAGE_EXECUTE_READWRITE, &ulOldProtect);

	pulCode = (PULONG)_pAPIAddr;
	if ((*pulCode != 0x8B55FF8B) && ((*pulCode & 0xFFFFFF) != 0xEC8B55))		//内存冲突
	{
		//VK_PrintfMsg("Hook %s 函数失败 ", _pszAPIName );
		return	FALSE;
	}

	//写入API Hook后的执行代码
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
	pbCodeBuffer[12] = 0x83;													//写入add esp, 4 平衡堆栈
	pbCodeBuffer[13] = 0xC4;
	pbCodeBuffer[14] = 0x4;

	//保存被Hook的API原代码
	VK_CopyMem(_pAPIAddr, &pbCodeBuffer[cOldAPICodeOffset], ulCodeLength);		
	
	//读取对应API参数信息
	sprintf(pbAPIParamINIPath, "%s%s%s", pszAPIMonitorDirectory, stModuleArray[_ulSTModuleIndex].pbModuleName, ".ini");
	GetParamNumAndType(pbAPIParamINIPath, _pszAPIName, &stParam);

	DB_AddData(	_ulSTModuleIndex, (ULONG)_pAPIAddr, VK_Lengthsz(_pszAPIName) + 1, _pszAPIName, 
				ulCodeLength + cOldAPICodeOffset + cOldAPICodeEndCodeNum, pbCodeBuffer, &stParam);
	pstCurrentDataBase = DB_GetStruct(DB_GetStructNum() - 1);					//获取写入的STDataBase结构地址

	//写入跳回原API执行的代码
	if(WriteJmpCode(&pstCurrentDataBase->bCodeBuffer[cOldAPICodeOffset + ulCodeLength], (PVOID)((ULONG)_pAPIAddr + ulCodeLength))==FALSE)
		VK_PrintfMsg("写入 JMP %s 函数失败 ", _pszAPIName );

	_try
	{
		WriteCallCode(_pAPIAddr, pstCurrentDataBase->bCodeBuffer);
		//将多出的代码写为nop
		/*(ULONG)pNopCode = (ULONG)_pAPIAddr + cHookCodeLength;
		for (i = 0; i < (ulCodeLength - cHookCodeLength); i++)
		{
			pbCodeBuffer[i] = 0x90;
		}*/
		
	}
	_except(1)
	{
		DB_DeleteData(DB_GetStructNum() - 1);									//如果Hook指定API失败则从数据库中删除
		VK_PrintfMsg("%s函数Hook失败", _pszAPIName);
		return	FALSE;
	}
	
	return	TRUE;
}


//名称：UnHookAPIByNum
//功能：卸载API Hook
//参数：_ulNum = 要卸载的序号
//返回：成功返回TRUE，失败返回FALSE
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


//名称：HookCall
//功能：API Hook后的处理函数，必须为_stdcall
//参数：_ulEsp = esp的值
//返回：无
BOOL	_stdcall HookCall(ULONG _ulEsp)
{
	ULONG		ulNum, ulFindStringIndex, ulSetitemData;
	ULONG		i, ulHookCallAddr, ulAPICallAddr, ulParamAddr;
	PULONG		pulCheckModuleName = NULL;
	PBYTE		pCallModuleName = NULL;
	BYTE		pUnFindModuleName[] = {"未知模块"};
	BYTE		pListMsg[MAX_PATH];
	PSTDataBase	pstTempDataBase;
	STParam_ListViewHandler	stParam_ListViewHandler;
	if (IsHookCallAgain)					//检测是否由于HookCall()内有调用函数被Hook而引起的循环调用
	{	
		return	FALSE;						//如果检测到循环调用，则函数直接返回
	}
	else
	{
		IsHookCallAgain = TRUE;
	}

	if (!IsMonitor)							//检测是否允许监控，不允许则直接返回
	{
		IsHookCallAgain = FALSE;			//恢复检测标记
		
		return	FALSE;
	}

	//经过pushad和pushfd后，往上第9个为Hook的call指令的返回地址，第10个为调用者的返回地址
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
	//VK_PrintfMsg(" %s模块调用API%s", stModuleArray[i].pbModuleName,pstTempDataBase->bDataBuffer);
	//显示API调用，并写入文件
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

	//判断调用者是否是"uxtheme.dll"和"APIMonitor.dll",如果是则不予以处理
	if ((pulCheckModuleName[0] != 0x68747875 || pulCheckModuleName[1] != 0x2E656D65) &&  pulCheckModuleName[0] != 0x2E6C6C44)
	{
		if (IsShowSystem)															//检测是否允许显示系统调用
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
			if (CheckCallModule(pCallModuleName))									//如果不允许则判断调用者是不是系统DLL
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
	
	IsHookCallAgain = FALSE;				//恢复检测标记

	return	TRUE;
}

//名称：CheckCallModule
//功能：检查调用API的模块是否允许显示调用
//参数：_pszModuleName = 模块名称
//返回：允许返回TRUE，否则凡是FALSE
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



//名称：GetProcessModules
//功能：获取指定进程ID的所有Dll模块
//参数：_ulProcessID = 进程ID
//参数：_pstModule = PSTModule结构数组存放的地址
//返回：当前模块数量
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

			if(strcmp(cAPIMonitorDllName, stModuleEntry32.szModule) == 0)				//获取APIMonitor程序的地址
			{
				VK_Copysz(stModuleEntry32.szExePath, pszAPIMonitorPath);
			}
			ulModuleNum++;
		}
	}
	
	return ulModuleNum;
}


//名称：VEHCallBackProc
//功能：VEH捕获异常后的回调函数
//参数：_ExceptionAddr = 发生异常的地址
//参数：_Param = 未定义参数
//返回：无
VOID	_stdcall VEHCallBackProc(ULONG _ExceptionAddr, ULONG _Param)
{
	ULONG		ulNum, i;
	PBYTE		pCallModuleName = NULL;
	BYTE		pExceptionName[] = {"异常"};
	BYTE		pUnFindModuleName[] = {"未知模块"};
	STParam_ListViewHandler	stParam_ListViewHandler;
	
	//显示API调用，并写入文件
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


//名称：GetEditData
//功能：获取编辑框数据
//参数： _EditID = 编辑框的ID
//参数：_pszFormat = 获取的格式
//返回：编辑框类的数据,失败则返回0xFFFFFFFF(cSTATUS_Fail)
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

//名称：SetEditData
//功能：获取编辑框数据
//参数： _EditID = 编辑框的ID
//参数：_pszFormat = 写入的格式
//参数：_EditData = 写入的数据
//返回：无
VOID	SetEditData(DWORD _EditID, PBYTE _pszFormat, DWORD _EditData)
{
	BYTE	EditBuffer[MAX_PATH];
	
	sprintf(EditBuffer, _pszFormat, _EditData);
	SetDlgItemText(ghDlgMain, _EditID, EditBuffer);
}


//名称：WriteListMsgtoFile
//功能：保存ListView上的数据到文件
//参数：无
//返回：无
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
		VK_MsgBox("创建%s文件句柄失败");
	}	
}

//名称： WriteCallCode
//功能：在指定的地址写入Call代码
//参数1：_pWriteAddr=要写入的地址
//参数2：_pCallAddr=要写入的Call代码的跳转地址
//返回：成功则返回TURE，否则返回FALSE
BOOL	WriteCallCode(PVOID _pWriteAddr, PVOID _pCallAddr)
{
	ULONG ulCallCode;
	
	if (_pWriteAddr != NULL && _pCallAddr != NULL)
	{
		ulCallCode= (ULONG)_pCallAddr - (ULONG)_pWriteAddr -5;							//得到要写入的跳转地址码
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


//名称： WriteJmpCode
//功能：在指定的地址写入Jmp代码
//参数1：_pWriteAddr=要写入的地址
//参数2：_pJmpAddr=要写入的Jmp代码的跳转地址
//返回：成功则返回TURE，否则返回FALSE
BOOL	WriteJmpCode(PVOID _pWriteAddr, PVOID _pJmpAddr)
{
	ULONG ulJmpCode;
	
	if (_pWriteAddr != NULL && _pJmpAddr != NULL)
	{
		//得到要写入的跳转地址码
		//计算公式：jmp跳转的地址=自己实现的函数地址-（jmp所在的地址+5) 
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
