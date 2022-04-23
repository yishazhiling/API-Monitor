//ListView.c
//ListView表的处理功能模块

#include "ListView.h"

HWND	hDlgMain, hListView;
ULONG	ulListNum = 0;
STLVCallArray	pulCallAddrArray[cLV_CallAddrArrayNum];

LV_ITEM		gstLV_Item = {LVIF_TEXT, 0, 0, LVIS_ACTIVATING, LVIS_SELECTED, NULL, 0, 0, 0, 0};
LV_COLUMN	gstLV_Column = {LVCF_TEXT + LVCF_WIDTH, LVCFMT_CENTER, cLV_ColumnWidth, "Default", 0, 0, 0, 0};


//名称：LV_InitListView
//功能：初始化ListView
//参数：_hWindow = 窗口句柄
//参数：_hListView = ListView控件句柄
//返回：成功返回TRUE，失败返回FALSE
BOOL	LV_InitListView(HWND _hWindow, HWND _hListView)
{	
	if((_hWindow == 0) || (_hListView == 0))
	{
		return	FALSE;
	}
	

	hDlgMain = _hWindow;
	hListView = _hListView;
	//初始化列
	gstLV_Column.pszText = cLV_Column0_Name;
	SendMessage(hListView, LVM_INSERTCOLUMN, 0, &gstLV_Column);
	gstLV_Column.pszText = cLV_Column1_Name;
	SendMessage(hListView, LVM_INSERTCOLUMN, 1, &gstLV_Column);
	gstLV_Column.pszText = cLV_Column2_Name;
	SendMessage(hListView, LVM_INSERTCOLUMN, 2, &gstLV_Column);
	gstLV_Column.pszText = cLV_Column3_Name;
	SendMessage(hListView, LVM_INSERTCOLUMN, 3, &gstLV_Column);	

	return	TRUE;
}


//名称：LV_AddKeyItem
//功能：加入主键项目
//参数：_ulItem = 行号，从0开始
//参数：_ulParam = 附加参数
//参数：_pbText = 要写入的字符串
//返回：无
VOID	LV_AddKeyItem(ULONG _ulItem, ULONG _ulParam, PBYTE _pbText)
{
	
	gstLV_Item.iSubItem = 0;					//第0列设为项目，即主键
	gstLV_Item.iItem = _ulItem;
	gstLV_Item.lParam = _ulParam;
	gstLV_Item.pszText = _pbText;
	SendMessage(hListView, LVM_INSERTITEM, 0, &gstLV_Item);
}


//名称：LV_AddItem
//功能：获取当前结构的数目
//参数：_SubItem = 列号，从0开始
//参数：_ulItem = 行号，从0开始
//参数：_ulParam = 附加参数
//参数：_pbText = 要写入的字符串
//返回：无
VOID	LV_AddItem(ULONG _SubItem, ULONG _ulItem, ULONG _ulParam, PBYTE _pbText)
{
	
	gstLV_Item.iSubItem = _SubItem;				
	gstLV_Item.iItem = _ulItem;
	gstLV_Item.lParam = _ulParam;
	gstLV_Item.pszText = _pbText;
	SendMessage(hListView, LVM_SETITEM, 0, &gstLV_Item);
}


//名称：LV_ListViewHandler
//功能：ListView的对外处理数据函数
//参数：_pSTParams = LV_ListViewHandler的参数组结构指针
//返回：加入的行号
ULONG	LV_ListViewHandler(PSTParam_ListViewHandler _pSTParams)
{
	ULONG	i, j, n, ulKeyItem, ulMemParamNum;
	ULONG	ulStringLen = 0;
	PULONG	pParamAddr = NULL;
	PULONG	pulTemp = NULL;
	BOOL	IsSameParam = TRUE;
	PBYTE	pbTemp = NULL;
	
	if (ulListNum > cLV_CallAddrArrayNum)
	{
		return	cLV_Fail;
	}
	
	for (i = 0; i < ulListNum; i++)
	{
		if (pulCallAddrArray[i].ulCallAddr == _pSTParams->ulCallAddr)
		{
			pulCallAddrArray[i].ulNum ++;
			
			//处理参数
			
			if (_pSTParams->ulParamAddr && _pSTParams->pstParam->bParamNum)
			{
				pulTemp	= pulCallAddrArray[i].pParamMem;
				if (LV_CheckParam(pulTemp, _pSTParams->ulParamAddr) == cLV_Fail)			//当API的调用参数和存入参数内存块的参数组全部不同时，加入参数内存块
				{
					//如果参数内存块类存放的参数组总数超过cLV_MaxParamNum
					LV_AddParam(pulTemp[cLV_ParamMemCallTimeIndex] % cLV_MaxParamNum, pulTemp, _pSTParams->ulParamAddr);
					pulTemp[cLV_ParamMemCallTimeIndex]++;
				}
			}
			
			return	i;
		}
		if (pulCallAddrArray[i].ulCallAddr > _pSTParams->ulCallAddr)				//如果未找到，说明是新调用，将新调用插入列表中，高于调用地址的数据全部下沉
		{
			for (j = ulListNum; j > i; j--)
			{
				pulCallAddrArray[j] = pulCallAddrArray[j - 1];			//LVM_SETITEM和LVM_INSERTITEM的作用是插入一行，高位的行会自动下沉一行，而不是仅仅修改加入行的数据
			}
			
			break;
		}
	}
	
	pulCallAddrArray[i].ulCallAddr = _pSTParams->ulCallAddr;
	pulCallAddrArray[i].ulNum = 1;
	pulCallAddrArray[i].wAPIModuleIndex = _pSTParams->wAPIModeleIndex;
	
	if (_pSTParams->ulParamAddr  && _pSTParams->pstParam->bParamNum)
	{
		pulCallAddrArray[i].pParamMem = VirtualAlloc(NULL, cLV_ParamMemSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		
		pulTemp	= pulCallAddrArray[i].pParamMem;									
		pulTemp[cLV_ParamMemSTParamIndex] = *(PULONG)_pSTParams->pstParam;			//参数内存的第0个数据为API的参数数量和参数类型STParam数据
		pulTemp[cLV_ParamMemCallTimeIndex] = 1;
		pulTemp[cLV_ParamMemNumIndex] = 1;											//参数内存的第2个数据为参数内存中存放了多少组的参数
		LV_AddParam(0, pulCallAddrArray[i].pParamMem, _pSTParams->ulParamAddr);
	}
	else
	{
		pulCallAddrArray[i].pParamMem = NULL;
	}
	
	LV_AddList(i, _pSTParams->ulCallAddr, _pSTParams->pszModuleName, _pSTParams->pszAPIName);			//如果调用地址大于所有pulCallAddrArray类的地址，则将其放入最后一个
	
	ulListNum++;
	return	ulListNum - 1;
}



//名称：LV_CheckParam
//功能：搜索参数内存块，是否和指定参数相同
//参数：_pParamMem = 参数数据内存块地址
//参数：_ulParamAddr = API的最后一个压栈参数的栈地址
//返回：如果是相同的参数调用，则返回参数在参数内存块中的序号，失败返回cLV_Fail(0xFFFFFFFF)
ULONG	LV_CheckParam(PVOID _pParamMem, ULONG _ulParamAddr)
{
	ULONG		i, j, ulMemParamNum;
	ULONG		ulStringParamIndex;
	PULONG		pParamAddr = NULL;
	PULONG		pulTemp = NULL;
	PBYTE		pbTemp = NULL;
	BOOL		IsSameParams = FALSE;
	PSTParam	pstParam;
	
	pulTemp = _pParamMem;
	pstParam = &pulTemp[cLV_ParamMemSTParamIndex];
	ulMemParamNum = pulTemp[cLV_ParamMemNumIndex];				//获取参数内存块中的参数组总数目

	for (i = 0; i < ulMemParamNum; i++)
	{
		ulStringParamIndex = 0;

		for (j = 0; j < pstParam->bParamNum; j++)
		{
			(ULONG)pParamAddr = _ulParamAddr + j*4;
			(ULONG)pbTemp = (ULONG)pulTemp + cLV_ParamMemSize - (cLV_ParamMemSize>>2) + 
						(i * pstParam->bStringNum + ulStringParamIndex)* cLV_ParamStringSize;

			if (pstParam->wType & 1 << j)
			{
				_try									//由于传递的字符串可能为NULL，此时对字符串进行操作会引起内存读写失败
				{
					if (strcmp(pbTemp, *pParamAddr) == 0)
					{
						IsSameParams = TRUE;
					}
					else
					{
						IsSameParams = FALSE;			//如果字符串不匹配，跳出此组参数组比较
						break;
					}
				}
				_except(1)
				{
					if (pbTemp[0] == 0)					//如果读取字符串失败，则判断存放在参数内存的字符串是否为空
					{
						IsSameParams = TRUE;
					}
					else
					{
						IsSameParams = FALSE;				
					}
				}
				
				ulStringParamIndex++;
			}
			else
			{
				if (pulTemp[cLV_ParamMemMsgIndex + j + i * pstParam->bParamNum] == *pParamAddr)
				{
					IsSameParams = TRUE;
				}
				else
				{
					IsSameParams = FALSE;			//如果参数不匹配，跳出此组参数组比较
					break;
				}
			}
		}

		if (IsSameParams)							//如果所有的参数都相等，说明是相同调用，返回其所在参数内存块序号
		{
			return	i;
		}
	}

	return	cLV_Fail;
}




//名称：LV_AddParam
//功能：对"调用次数"列表项的数据根据pulCallAddrArray.ulNum进行更新，此函数必须由定时器函数调用
//参数：_ulAddIndex = 要添加的参数组序号
//参数：_pParamMem = 参数数据内存块地址
//参数：_ulParamAddr = API的最后一个压栈参数的栈地址
//返回：成功返回TRUE，失败返回FALSE
BOOL	LV_AddParam(ULONG _ulAddIndex, PVOID _pParamMem, ULONG _ulParamAddr)
{
	ULONG		i, ulMemParamNum;
	ULONG		ulStringParamNum = 0;
	ULONG		ulStringParamIndex = 0;
	ULONG		ulStringLen = 0;
	PULONG		pParamAddr = NULL;
	PULONG		pulTemp = NULL;
	PBYTE		pbTemp = NULL;
	PSTParam	pstParam;
	
	pulTemp = _pParamMem;
	pstParam = &pulTemp[cLV_ParamMemSTParamIndex];

	if (_ulAddIndex >= cLV_MaxParamNum)
	{
		return	FALSE;
	}
	

	ulStringParamNum = pstParam->bStringNum;

	for (i = 0; i < pstParam->bParamNum && i < 16; i++)
	{
		(ULONG)pParamAddr = _ulParamAddr + i*4;
		(ULONG)pbTemp = (ULONG)pulTemp + cLV_ParamMemSize - (cLV_ParamMemSize>>2) + 
						(_ulAddIndex * ulStringParamNum + ulStringParamIndex)* cLV_ParamStringSize;
		
		if ((pstParam->wType & 1 << i))								//判断参数类型是否为字符串
		{
			_try													//由于传递的字符串可能为NULL，此时对字符串进行操作会引起内存读写失败
			{
				ulStringLen = strlen(*pParamAddr);					//如果是字符串，则拷贝字符串保存
				if (ulStringLen > 30)
				{
					RtlMoveMemory(pbTemp, *pParamAddr, 30);
					pbTemp[30] = 0;
				}
				else
				{
					strcpy(pbTemp, *pParamAddr);
				}
			}
			_except(1)												//如果拷贝字符失败，则将其字符串设置为0	
			{
				pbTemp[0] = 0;
			}
						
			pulTemp[cLV_ParamMemMsgIndex + i + _ulAddIndex * pstParam->bParamNum] = (ULONG)pbTemp;
			ulStringParamIndex++;
		}
		else
		{
			pulTemp[cLV_ParamMemMsgIndex + i + _ulAddIndex * pstParam->bParamNum] = *pParamAddr;		//如果不是字符串保存参数即可
		}
	}

	return	TRUE;
}



//名称：LV_AddList
//功能：加入一行，此函数由调用者自行定义
//参数：_ulNum = 行号，从0开始
//参数：_ulCallAddr = 调用地址
//参数：_pszModuleName = 调用名称
//参数：_pszAPIName = API名称
//返回：加入的行号
VOID	LV_AddList(ULONG _ulNum, ULONG _ulCallAddr, PBYTE _pszModuleName, PBYTE _pszAPIName)
{
	BYTE	pszTemp[MAX_PATH];

	sprintf(pszTemp, "0x%08X", _ulCallAddr);
	LV_AddKeyItem(_ulNum, _ulCallAddr,_pszAPIName);
	LV_AddItem(1, _ulNum, _ulCallAddr, _pszModuleName);
	LV_AddItem(2, _ulNum, _ulCallAddr,  pszTemp);
	sprintf(pszTemp, "%d", 1);
	LV_AddItem(3, _ulNum, _ulCallAddr, pszTemp);
}


//名称：LV_ReflushCallTime
//功能：对"调用次数"列表项的数据根据pulCallAddrArray.ulNum进行更新，此函数必须由定时器函数调用
//参数：无
//返回：无
VOID	LV_ReflushCallTime()
{
	ULONG	i;
	BYTE	pszTemp[32];
	
	for (i = 0; i < ulListNum; i++)
	{
		if (pulCallAddrArray[i].ulNum > 1)			//对调用次数超过1的列表项的调用次数列进行更新
		{
			sprintf(pszTemp, "%d", pulCallAddrArray[i].ulNum);
			LV_AddItem(3, i, i, pszTemp);
		}
	}
}


//名称：LV_OutPut
//功能：输出信息，此函数由调用者自行定义
//参数：_ulNum = 行号
//参数：_pszOutText = 输出信息的存放地址
//返回：如无ulNum序号的项目则返回FALSE，成功返回TRUE
BOOL	LV_OutPut(ULONG _ulNum, PBYTE _pszOutText)
{
	BYTE	pAPIName[MAX_PATH], pCallModuleName[MAX_PATH];
	LV_ITEM	stTempLV_Item = {LVIF_TEXT, 0, 0, LVIS_ACTIVATING, LVIS_SELECTED, NULL, MAX_PATH, 0, 0, 0};
	
	if (_ulNum > ulListNum)
	{
		return	FALSE;
	}

	stTempLV_Item.iItem = _ulNum;
	stTempLV_Item.iSubItem = 1;
	stTempLV_Item.pszText = pCallModuleName;
	SendMessage(hListView, LVM_GETITEM, 0, &stTempLV_Item); 
	
	stTempLV_Item.iItem = _ulNum;
	stTempLV_Item.iSubItem = 2;
	stTempLV_Item.pszText = pAPIName;
	SendMessage(hListView, LVM_GETITEM, 0, &stTempLV_Item);  

	
	sprintf(_pszOutText, "%s 被%s 调用，调用地址 = 0x%08X，调用次数 = %d \r\n", 
			pAPIName, pCallModuleName, pulCallAddrArray[_ulNum].ulCallAddr, pulCallAddrArray[_ulNum].ulNum);
	
	return	TRUE;
 }


//名称：LV_OutPutParamMsg
//功能：输出参数信息，此函数由调用者自行定义
//参数：_ulNum = 行号
//参数：_pszOutText = 输出信息的存放地址
//参数：_pParamNameBufer = 参数名称存放缓冲区地址，存放规则为"ParamName1", 0, "ParamName2", 0, "ParamName3", 0...
//返回：如无ulNum序号的项目则返回FALSE，成功返回TRUE
BOOL	LV_OutPutParamMsg(ULONG _ulNum, PBYTE _pszOutText, PVOID _pParamNameBufer)
{
	ULONG		i, j, ulParamsNum;
	PBYTE		ulParamNameAddrArray[cLV_MaxParamNum];
	PULONG		pulTemp = NULL;
	PSTParam	pstParam;
	PBYTE		pbTemp;
	
	if (_ulNum > ulListNum || pulCallAddrArray[_ulNum].pParamMem == NULL)
	{
		return	FALSE;
	}

	pulTemp = pulCallAddrArray[_ulNum].pParamMem;
	pstParam = (PSTParam) &(pulTemp[cLV_ParamMemSTParamIndex]);
	ulParamsNum = pulTemp[cLV_ParamMemCallTimeIndex];
	if (ulParamsNum > cLV_MaxParamNum)
	{
		ulParamsNum = cLV_MaxParamNum;
	}
	*_pszOutText = 0;
	
	pbTemp = _pParamNameBufer;
	for (i = 0; i < pstParam->bParamNum && i < cLV_MaxParamNum; i++)
	{
		ulParamNameAddrArray[i] = pbTemp;
		pbTemp += (strlen(pbTemp) + 1);
	}

	for (i = 0; i < ulParamsNum; i++)
	{
		for (j = 0; j < pstParam->bParamNum; j++)
		{
			if (pstParam->wType & 1 << j)
			{
				sprintf(_pszOutText, "%s%s = %-30s," , _pszOutText, ulParamNameAddrArray[j], 
						pulTemp[cLV_ParamMemMsgIndex + j + i * pstParam->bParamNum]);
			}
			else
			{
				sprintf(_pszOutText, "%s%s = 0x%X, ", _pszOutText, ulParamNameAddrArray[j], 
						pulTemp[cLV_ParamMemMsgIndex + j + i * pstParam->bParamNum]);
			}
		}
		sprintf(_pszOutText, "%s\n", _pszOutText);
	}

	return	TRUE;
}


//名称：LV_GetSTLVCallArray
//功能：获取指定序号的STLVCallArray结构
//参数：_ulIndex = pulCallAddrArray 的序号
//返回：当前ListView的总行数
PSTLVCallArray	LV_GetSTLVCallArray(ULONG _ulIndex)
{
	if (_ulIndex < ulListNum)
	{
		return &pulCallAddrArray[_ulIndex];
	}
	
	return	NULL;
}


//名称：LV_GetNum
//功能：获取当前ListView的总行数
//参数：无
//返回：当前ListView的总行数
ULONG	LV_GetNum()
{
	return ulListNum;
}


//名称：LV_GetSelectItem
//功能：获取当前选中的项
//参数：无
//返回：当前选中的项，如果没有选择项则返回cLV_Fail(0xFFFFFFFF)
ULONG	LV_GetSelectItem()
{
	ULONG	ulSelectItem, ulState, i;

	return SendMessage(hListView, LVM_GETNEXTITEM, -1, LVIS_SELECTED);
}


//名称：LV_GetItemText
//功能：指定列和行的单元格文本
//参数：_ulColumn = 列号
//参数：_ulRow = 行号
//参数：_pbBufer = 返回文本的写入地址
//返回：当前选中的项的行号，如果没有选择项则返回cLV_Fail(0xFFFFFFFF)
ULONG	LV_GetItemText(ULONG _ulColumn, ULONG _ulRow, PBYTE _pbBufer)
{
	LV_ITEM	stTempLV_Item = {LVIF_TEXT, 0, 0, LVIS_ACTIVATING, LVIS_SELECTED, NULL, MAX_PATH, 0, 0, 0};

	stTempLV_Item.iItem = _ulRow;
	stTempLV_Item.iSubItem = _ulColumn;
	stTempLV_Item.pszText = _pbBufer;

	return	SendMessage(hListView, LVM_GETITEMTEXT, _ulRow, &stTempLV_Item);
}



//名称：LV_ClearListView
//功能：清屏
//参数：无
//返回：无
VOID	LV_ClearListView()
{
	ULONG	i;
	
	for (i = 0; i< ulListNum; i++)
	{
		if (pulCallAddrArray[i].pParamMem)
		{
			VirtualFree(pulCallAddrArray[i].pParamMem, cLV_ParamMemSize, MEM_DECOMMIT);
		}
	}
	ulListNum = 0;
	SendMessage(hListView, LVM_DELETEALLITEMS, 0, 0);
}