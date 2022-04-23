#include "ParamINI.h"

//名称：GetParamNumAndType
//功能：获取API参数的数量和类型，属性用1个word
//参数：_pszINIFilePath = 要查找的INI文件的路径
//参数：_pszAPIName = API名称
//参数：_pstAPIParam = 写回API参数的数量和类型的STParam结构地址
//返回：成功返回参数的个数，失败返回0
ULONG GetParamNumAndType(PCHAR _pszINIFilePath, PCHAR _pszAPIName, PSTParam _pstAPIParam)
{
	ULONG	ulParamType = 0;
	ULONG	ulParamNum = 0, i;
	BYTE	bStringNum = 0;
	WORD	wParamType = 0;
	BYTE	pbParamTypeKeyName[MAX_PATH];
	
	ulParamNum = GetPrivateProfileInt(_pszAPIName, cKeyName_ParamNum, 0, _pszINIFilePath);
	for (i = 0; i < ulParamNum && i < 16; i++)
	{
		sprintf(pbParamTypeKeyName, "%s%d", cKeyName_ParamType, i+1);
		ulParamType = GetPrivateProfileInt(_pszAPIName, pbParamTypeKeyName, 0, _pszINIFilePath);

		if (ulParamType == 1)								//判断参数的类型是否是字符串，即ParamType = 1表示是字符串
		{
			wParamType = wParamType | 1 << i;				//wParamType的16位每一位如为1标识对应的第n+1个参数为字符串
			bStringNum++;
		}
	}
	
	_pstAPIParam->bParamNum = (BYTE)ulParamNum;
	_pstAPIParam->bStringNum = bStringNum;
	_pstAPIParam->wType = wParamType;

	return	ulParamNum;
}

//名称：GetParamParamName
//功能：获取API参数的名称
//参数：_pszINIFilePath = 要查找的INI文件的路径
//参数：_pszAPIName = API名称
//参数：_pBuffer = 写回所有API参数的名称的缓冲区地址，存放规则为"ParamName1", 0, "ParamName2", 0, "ParamName3", 0...
//返回：写入_pBuffer的总字节数，0表示失败
ULONG GetParamName(PCHAR _pszINIFilePath, PCHAR _pszAPIName, PVOID _pBuffer)
{
	ULONG	ulParamNum = 0, i;
	ULONG	ulBufferSize = 0;
	PBYTE	pbTemp;
	BYTE	pbParamNameKeyName[MAX_PATH];
	
	ulParamNum = GetPrivateProfileInt(_pszAPIName, cKeyName_ParamNum, 0, _pszINIFilePath);
	pbTemp = _pBuffer;

	for (i = 0; i < ulParamNum && i < 16; i++)
	{
		sprintf(pbParamNameKeyName, "%s%d", cKeyName_ParamName, i+1);
		GetPrivateProfileString(_pszAPIName, pbParamNameKeyName, NULL, pbTemp, MAX_PATH, _pszINIFilePath);
		(ULONG)pbTemp += (strlen(pbTemp) + 1);
	}
	
	ulBufferSize = (ULONG)pbTemp -  (ULONG)_pBuffer+ strlen(pbTemp) + 1;
	
	return	ulBufferSize;
}