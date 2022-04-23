#include "ParamINI.h"

//���ƣ�GetParamNumAndType
//���ܣ���ȡAPI���������������ͣ�������1��word
//������_pszINIFilePath = Ҫ���ҵ�INI�ļ���·��
//������_pszAPIName = API����
//������_pstAPIParam = д��API���������������͵�STParam�ṹ��ַ
//���أ��ɹ����ز����ĸ�����ʧ�ܷ���0
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

		if (ulParamType == 1)								//�жϲ����������Ƿ����ַ�������ParamType = 1��ʾ���ַ���
		{
			wParamType = wParamType | 1 << i;				//wParamType��16λÿһλ��Ϊ1��ʶ��Ӧ�ĵ�n+1������Ϊ�ַ���
			bStringNum++;
		}
	}
	
	_pstAPIParam->bParamNum = (BYTE)ulParamNum;
	_pstAPIParam->bStringNum = bStringNum;
	_pstAPIParam->wType = wParamType;

	return	ulParamNum;
}

//���ƣ�GetParamParamName
//���ܣ���ȡAPI����������
//������_pszINIFilePath = Ҫ���ҵ�INI�ļ���·��
//������_pszAPIName = API����
//������_pBuffer = д������API���������ƵĻ�������ַ����Ź���Ϊ"ParamName1", 0, "ParamName2", 0, "ParamName3", 0...
//���أ�д��_pBuffer�����ֽ�����0��ʾʧ��
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