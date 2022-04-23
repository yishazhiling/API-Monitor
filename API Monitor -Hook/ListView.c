//ListView.c
//ListView��Ĵ�����ģ��

#include "ListView.h"

HWND	hDlgMain, hListView;
ULONG	ulListNum = 0;
STLVCallArray	pulCallAddrArray[cLV_CallAddrArrayNum];

LV_ITEM		gstLV_Item = {LVIF_TEXT, 0, 0, LVIS_ACTIVATING, LVIS_SELECTED, NULL, 0, 0, 0, 0};
LV_COLUMN	gstLV_Column = {LVCF_TEXT + LVCF_WIDTH, LVCFMT_CENTER, cLV_ColumnWidth, "Default", 0, 0, 0, 0};


//���ƣ�LV_InitListView
//���ܣ���ʼ��ListView
//������_hWindow = ���ھ��
//������_hListView = ListView�ؼ����
//���أ��ɹ�����TRUE��ʧ�ܷ���FALSE
BOOL	LV_InitListView(HWND _hWindow, HWND _hListView)
{	
	if((_hWindow == 0) || (_hListView == 0))
	{
		return	FALSE;
	}
	

	hDlgMain = _hWindow;
	hListView = _hListView;
	//��ʼ����
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


//���ƣ�LV_AddKeyItem
//���ܣ�����������Ŀ
//������_ulItem = �кţ���0��ʼ
//������_ulParam = ���Ӳ���
//������_pbText = Ҫд����ַ���
//���أ���
VOID	LV_AddKeyItem(ULONG _ulItem, ULONG _ulParam, PBYTE _pbText)
{
	
	gstLV_Item.iSubItem = 0;					//��0����Ϊ��Ŀ��������
	gstLV_Item.iItem = _ulItem;
	gstLV_Item.lParam = _ulParam;
	gstLV_Item.pszText = _pbText;
	SendMessage(hListView, LVM_INSERTITEM, 0, &gstLV_Item);
}


//���ƣ�LV_AddItem
//���ܣ���ȡ��ǰ�ṹ����Ŀ
//������_SubItem = �кţ���0��ʼ
//������_ulItem = �кţ���0��ʼ
//������_ulParam = ���Ӳ���
//������_pbText = Ҫд����ַ���
//���أ���
VOID	LV_AddItem(ULONG _SubItem, ULONG _ulItem, ULONG _ulParam, PBYTE _pbText)
{
	
	gstLV_Item.iSubItem = _SubItem;				
	gstLV_Item.iItem = _ulItem;
	gstLV_Item.lParam = _ulParam;
	gstLV_Item.pszText = _pbText;
	SendMessage(hListView, LVM_SETITEM, 0, &gstLV_Item);
}


//���ƣ�LV_ListViewHandler
//���ܣ�ListView�Ķ��⴦�����ݺ���
//������_pSTParams = LV_ListViewHandler�Ĳ�����ṹָ��
//���أ�������к�
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
			
			//�������
			
			if (_pSTParams->ulParamAddr && _pSTParams->pstParam->bParamNum)
			{
				pulTemp	= pulCallAddrArray[i].pParamMem;
				if (LV_CheckParam(pulTemp, _pSTParams->ulParamAddr) == cLV_Fail)			//��API�ĵ��ò����ʹ�������ڴ��Ĳ�����ȫ����ͬʱ����������ڴ��
				{
					//��������ڴ�����ŵĲ�������������cLV_MaxParamNum
					LV_AddParam(pulTemp[cLV_ParamMemCallTimeIndex] % cLV_MaxParamNum, pulTemp, _pSTParams->ulParamAddr);
					pulTemp[cLV_ParamMemCallTimeIndex]++;
				}
			}
			
			return	i;
		}
		if (pulCallAddrArray[i].ulCallAddr > _pSTParams->ulCallAddr)				//���δ�ҵ���˵�����µ��ã����µ��ò����б��У����ڵ��õ�ַ������ȫ���³�
		{
			for (j = ulListNum; j > i; j--)
			{
				pulCallAddrArray[j] = pulCallAddrArray[j - 1];			//LVM_SETITEM��LVM_INSERTITEM�������ǲ���һ�У���λ���л��Զ��³�һ�У������ǽ����޸ļ����е�����
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
		pulTemp[cLV_ParamMemSTParamIndex] = *(PULONG)_pSTParams->pstParam;			//�����ڴ�ĵ�0������ΪAPI�Ĳ��������Ͳ�������STParam����
		pulTemp[cLV_ParamMemCallTimeIndex] = 1;
		pulTemp[cLV_ParamMemNumIndex] = 1;											//�����ڴ�ĵ�2������Ϊ�����ڴ��д���˶�����Ĳ���
		LV_AddParam(0, pulCallAddrArray[i].pParamMem, _pSTParams->ulParamAddr);
	}
	else
	{
		pulCallAddrArray[i].pParamMem = NULL;
	}
	
	LV_AddList(i, _pSTParams->ulCallAddr, _pSTParams->pszModuleName, _pSTParams->pszAPIName);			//������õ�ַ��������pulCallAddrArray��ĵ�ַ������������һ��
	
	ulListNum++;
	return	ulListNum - 1;
}



//���ƣ�LV_CheckParam
//���ܣ����������ڴ�飬�Ƿ��ָ��������ͬ
//������_pParamMem = ���������ڴ���ַ
//������_ulParamAddr = API�����һ��ѹջ������ջ��ַ
//���أ��������ͬ�Ĳ������ã��򷵻ز����ڲ����ڴ���е���ţ�ʧ�ܷ���cLV_Fail(0xFFFFFFFF)
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
	ulMemParamNum = pulTemp[cLV_ParamMemNumIndex];				//��ȡ�����ڴ���еĲ���������Ŀ

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
				_try									//���ڴ��ݵ��ַ�������ΪNULL����ʱ���ַ������в����������ڴ��дʧ��
				{
					if (strcmp(pbTemp, *pParamAddr) == 0)
					{
						IsSameParams = TRUE;
					}
					else
					{
						IsSameParams = FALSE;			//����ַ�����ƥ�䣬�������������Ƚ�
						break;
					}
				}
				_except(1)
				{
					if (pbTemp[0] == 0)					//�����ȡ�ַ���ʧ�ܣ����жϴ���ڲ����ڴ���ַ����Ƿ�Ϊ��
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
					IsSameParams = FALSE;			//���������ƥ�䣬�������������Ƚ�
					break;
				}
			}
		}

		if (IsSameParams)							//������еĲ�������ȣ�˵������ͬ���ã����������ڲ����ڴ�����
		{
			return	i;
		}
	}

	return	cLV_Fail;
}




//���ƣ�LV_AddParam
//���ܣ���"���ô���"�б�������ݸ���pulCallAddrArray.ulNum���и��£��˺��������ɶ�ʱ����������
//������_ulAddIndex = Ҫ��ӵĲ��������
//������_pParamMem = ���������ڴ���ַ
//������_ulParamAddr = API�����һ��ѹջ������ջ��ַ
//���أ��ɹ�����TRUE��ʧ�ܷ���FALSE
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
		
		if ((pstParam->wType & 1 << i))								//�жϲ��������Ƿ�Ϊ�ַ���
		{
			_try													//���ڴ��ݵ��ַ�������ΪNULL����ʱ���ַ������в����������ڴ��дʧ��
			{
				ulStringLen = strlen(*pParamAddr);					//������ַ������򿽱��ַ�������
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
			_except(1)												//��������ַ�ʧ�ܣ������ַ�������Ϊ0	
			{
				pbTemp[0] = 0;
			}
						
			pulTemp[cLV_ParamMemMsgIndex + i + _ulAddIndex * pstParam->bParamNum] = (ULONG)pbTemp;
			ulStringParamIndex++;
		}
		else
		{
			pulTemp[cLV_ParamMemMsgIndex + i + _ulAddIndex * pstParam->bParamNum] = *pParamAddr;		//��������ַ��������������
		}
	}

	return	TRUE;
}



//���ƣ�LV_AddList
//���ܣ�����һ�У��˺����ɵ��������ж���
//������_ulNum = �кţ���0��ʼ
//������_ulCallAddr = ���õ�ַ
//������_pszModuleName = ��������
//������_pszAPIName = API����
//���أ�������к�
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


//���ƣ�LV_ReflushCallTime
//���ܣ���"���ô���"�б�������ݸ���pulCallAddrArray.ulNum���и��£��˺��������ɶ�ʱ����������
//��������
//���أ���
VOID	LV_ReflushCallTime()
{
	ULONG	i;
	BYTE	pszTemp[32];
	
	for (i = 0; i < ulListNum; i++)
	{
		if (pulCallAddrArray[i].ulNum > 1)			//�Ե��ô�������1���б���ĵ��ô����н��и���
		{
			sprintf(pszTemp, "%d", pulCallAddrArray[i].ulNum);
			LV_AddItem(3, i, i, pszTemp);
		}
	}
}


//���ƣ�LV_OutPut
//���ܣ������Ϣ���˺����ɵ��������ж���
//������_ulNum = �к�
//������_pszOutText = �����Ϣ�Ĵ�ŵ�ַ
//���أ�����ulNum��ŵ���Ŀ�򷵻�FALSE���ɹ�����TRUE
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

	
	sprintf(_pszOutText, "%s ��%s ���ã����õ�ַ = 0x%08X�����ô��� = %d \r\n", 
			pAPIName, pCallModuleName, pulCallAddrArray[_ulNum].ulCallAddr, pulCallAddrArray[_ulNum].ulNum);
	
	return	TRUE;
 }


//���ƣ�LV_OutPutParamMsg
//���ܣ����������Ϣ���˺����ɵ��������ж���
//������_ulNum = �к�
//������_pszOutText = �����Ϣ�Ĵ�ŵ�ַ
//������_pParamNameBufer = �������ƴ�Ż�������ַ����Ź���Ϊ"ParamName1", 0, "ParamName2", 0, "ParamName3", 0...
//���أ�����ulNum��ŵ���Ŀ�򷵻�FALSE���ɹ�����TRUE
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


//���ƣ�LV_GetSTLVCallArray
//���ܣ���ȡָ����ŵ�STLVCallArray�ṹ
//������_ulIndex = pulCallAddrArray �����
//���أ���ǰListView��������
PSTLVCallArray	LV_GetSTLVCallArray(ULONG _ulIndex)
{
	if (_ulIndex < ulListNum)
	{
		return &pulCallAddrArray[_ulIndex];
	}
	
	return	NULL;
}


//���ƣ�LV_GetNum
//���ܣ���ȡ��ǰListView��������
//��������
//���أ���ǰListView��������
ULONG	LV_GetNum()
{
	return ulListNum;
}


//���ƣ�LV_GetSelectItem
//���ܣ���ȡ��ǰѡ�е���
//��������
//���أ���ǰѡ�е�����û��ѡ�����򷵻�cLV_Fail(0xFFFFFFFF)
ULONG	LV_GetSelectItem()
{
	ULONG	ulSelectItem, ulState, i;

	return SendMessage(hListView, LVM_GETNEXTITEM, -1, LVIS_SELECTED);
}


//���ƣ�LV_GetItemText
//���ܣ�ָ���к��еĵ�Ԫ���ı�
//������_ulColumn = �к�
//������_ulRow = �к�
//������_pbBufer = �����ı���д���ַ
//���أ���ǰѡ�е�����кţ����û��ѡ�����򷵻�cLV_Fail(0xFFFFFFFF)
ULONG	LV_GetItemText(ULONG _ulColumn, ULONG _ulRow, PBYTE _pbBufer)
{
	LV_ITEM	stTempLV_Item = {LVIF_TEXT, 0, 0, LVIS_ACTIVATING, LVIS_SELECTED, NULL, MAX_PATH, 0, 0, 0};

	stTempLV_Item.iItem = _ulRow;
	stTempLV_Item.iSubItem = _ulColumn;
	stTempLV_Item.pszText = _pbBufer;

	return	SendMessage(hListView, LVM_GETITEMTEXT, _ulRow, &stTempLV_Item);
}



//���ƣ�LV_ClearListView
//���ܣ�����
//��������
//���أ���
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