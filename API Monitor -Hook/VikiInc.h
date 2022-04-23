//VikiInc.h
#pragma once 
#include <windows.h>

#pragma comment(lib, "VikiProcLib.lib")

#ifdef __cplusplus
extern "C" 
{
#endif

//���ƣ�VK_AllocMem 
//���ܣ�����ָ����С�������ڴ�, ���ڴ�����ΪPAGE_EXECUTE_READWRITE
//������_dwSize = �����ڴ�Ĵ�С
//���أ��ɹ������ڴ��ָ�룬ʧ�ܷ���0

PVOID _stdcall VK_AllocMem (ULONG _dwSize);


//���ƣ�VK_FreeMem
//���ܣ��ͷ�ָ����С�������ڴ�
//������_pMemAddr = �ڴ��ַ
//������_dwSize = �ͷ��ڴ�Ĵ�С
//���أ�eax = 1��ʾ�ɹ���eax=0��ʾʧ��

BOOL _stdcall VK_FreeMem (PVOID _pMemAddr, ULONG _dwSize);


//���ƣ�VK_DwordToHex 
//���ܣ���һ��32λ����ת��Ϊ��0������16�����ַ���
//������dwNum=Ҫת����32λ����
//������pHexBuffer=16�����ַ�����ŵĵ�ַ,��ŵ�ַ�Ĵ�С>=9�ֽ�
//���أ���

VOID _stdcall VK_DwordToHex (DWORD dwNum, PVOID pHexBuffer);


//���ƣ�VK_DwordToDec 
//���ܣ���һ��32λ����ת��Ϊ��0������10�����ַ���
//������dwNum=Ҫת����32λ����
//������pHexBuffer=16�����ַ�����ŵĵ�ַ,��ŵ�ַ�ĳ���>=12�ֽ�
//���أ���
    
VOID _stdcall VK_DwordToDec (DWORD dwNum, PVOID pHexBuffer);
    
    
//���ƣ�VK_Lengthsz 
//���ܣ�������0��β���ַ����ĳ���
//������lpsz=��0��β���ַ����׵�ַ
//���أ�eax=�ַ������ȣ����ֽڼƣ����ƽ�β��0��

DWORD _stdcall VK_Lengthsz (PVOID lpsz);


//���ƣ�VK_CmpMem
//���ܣ��Ƚ�2���ڴ��Ƿ����
//������_pSourceAddr = Ҫ�Ƚϵ��ڴ��ַ, ������esi�Ƚ�
//������_pTargetAddr = �Ƚ϶�����ڴ��ַ��������edi�Ƚ�
//������_dwSize = Ҫ�ȽϵĴ�С�ڴ�Ĵ�С
//���أ���ȷ���1������ȷ���0

BOOL _stdcall VK_CmpMem (LPVOID _pSourceAddr, LPVOID _pTargetAddr, DWORD _dwSize);


//���ƣ�VK_CopyMem 
//���ܣ������ڴ棬��ָ��λ��ָ����С���ڴ渴�Ƶ�ָ��λ��
//������lpSource=Ҫ�����ڴ���׵�ַ
//������lpTarget=�����ڴ��Ŀ�ĵ�ַ
//������dwSize=�踴���ڴ�Ĵ�С
//���أ���

VOID _stdcall VK_CopyMem (PVOID lpSourceAddr, PVOID lpTargetAddr, DWORD dwSize);

//���ƣ�VK_Copysz 
//���ܣ������ַ�������һ����0�������ַ������Ƶ���һ����ַ�У�����0��β
//������lpszSource=Ҫ���Ƶ���0��β���ַ����׵�ַ
//������lpszTarget=����Ŀ���ַ����׵�ַ
//���أ����Ƶ��ַ����ȣ���������β��0

VOID _stdcall VK_Copysz (PVOID lpszSource, PVOID lpszTarget);


//���ƣ�VK_Connectsz 
//���ܣ��ϲ��ַ�������������0�������ַ����ϲ�Ϊһ����0��β���ַ���
//������lpsz1=��һ����0��β���ַ����׵�ַ
//������lpsz2=�ڶ�����0��β���ַ����׵�ַ
//������lpszBuffer=�ϲ�����ַ�����ŵĵ�ַ
//���أ���

VOID _stdcall VK_Connectsz (PVOID lpsz1, PVOID lpsz2, PVOID lpszBuffer);
    

//���ƣ�VK_PrintfMsg
//���ܣ�����һ����Ϣ����ʾ_pszFormatָ����ʽ�ı�����ע��ֻ����ʾһ������������Ϊ�ַ�����
//������_pszFormat=Ҫ��ʾ���ݸ�ʽ
//������_Data=Ҫ��ʾ����
//���أ���


//���ƣ� VK_FindData
//���ܣ�����ƥ�������
//����1��_pStartAddr=���ҵ���ʼ��ַ
//����2��_ulDataSize=�������ݵĴ�С
//����3��_pFindData=�������ݵ��׵�ַ����ָ��
//����4��_ulDataLength=�������ݵ��ֽ���
//���أ����ҵ��򷵻��������ڵ�ַ��δ�ҵ�����NULL
PVOID _stdcall VK_FindData (PVOID _pStartAddr, DWORD _ulDataSize, PVOID _pFindData, DWORD _ulDataLength);


VOID _stdcall VK_PrintfMsg (PVOID _pszFormat, DWORD _Data);

//���ƣ�VK_MsgBox 
//���ܣ�����һ����Ϣ����ʾ0�������ַ���
//������lpszMsg=Ҫ��ʾ����0��β���ַ����׵�ַ
//���أ���

VOID _stdcall VK_MsgBox (PVOID lpszMsg);
 
//���ƣ�VK_DwordMsgBox 
//���ܣ�����һ����Ϣ����ʾ0������16�����ַ���
//������Num=Ҫ��ʾ˫������
//���أ���

VOID _stdcall VK_DwordMsgBox (DWORD _dwNum);


//���ƣ�VK_NumMsgBox
//���ܣ�����һ����Ϣ����ʾ0�������ַ���
//������_dwNum=Ҫ��ʾ˫������
//���أ���

VOID _stdcall VK_NumMsgBox (DWORD _dwNum);

    
//���ƣ�VK_ReplaceMemory 
//���ܣ���ָ�����Ⱥ͵�ַ���ڴ�ֵ�滻Ϊָ����ֵ
//������lpMemory=Ҫ�滻�ڴ���׵�ַ
//������MemorySiz=�ڴ��С�����ֽڼ�
//������bDATA=Ҫ�滻����ֵ����СΪ�ֽ�
//���أ���

VOID _stdcall VK_ReplaceMemory (PVOID lpMemory, DWORD MemorySize, BYTE bDATA);


//���ƣ�VK_ReadFile 
//���ܣ���ȡ�ļ��������ļ����
//������szReadFilePath=�ļ��ľ���·��
//������pReadBuffer=�ļ���ȡ���ŵĻ����ַ
//������pSizeRead=����ʵ�ʶ�ȡ�ļ���С�ĵ�ַ
//���أ�eax��eax=�ļ��ľ���������ȡʧ����eax=FALSE

HANDLE _stdcall VK_ReadFile (PVOID szReadFilePath, PVOID pReadBuffer, PDWORD pSizeRead);


//���ƣ�VK_WriteFile 
//���ܣ�д�ļ��������ļ������
//������szWriteFilePath=�ļ��ľ���·��
//������pWriteBuffer=Ҫд����ļ��Ļ����ַ
//������SizeWrite=д���ļ��Ĵ�С
//���أ�eax��eax=�ļ��ľ���������ȡʧ����eax=FALSE

HANDLE _stdcall VK_WriteFile (PVOID  szWriteFilePath, PVOID pWriteBuffer, DWORD SizeWrite);


//���ƣ�VK_CloseFile 
//���ܣ��ر�VK_ReadFile�򿪵��ļ���������������ڴ��ָ��pMemory���������hMemory�ͷ�
//������hFile=�ļ��ľ��
//������pMemory=�����ڴ��ָ��
//���أ�eax����رճɹ�eax=TRUE��ʧ��eax=FALSE

BOOL _stdcall VK_CloseFile (HANDLE hCloseFile, PVOID pMemory);


//���ƣ�VK_MapViewOfFile
//���ܣ����ļ������ڴ�ӳ�񣬷����ļ������ӳ����
//������_pszFilePath = �ļ��ľ���·��
//������_pFileHandle = д���ļ�����ĵ�ַ
//������_pMapViewHandle = д���ļ�ӳ��������ĵ�ַ
//���أ�eax = ӳ���ļ�ָ��,�����ȡʧ����eax = 0

PVOID _stdcall VK_MapViewOfFile 	(PVOID _szFilePath, PHANDLE _pFileHandle, PHANDLE _pMapViewHandle);

//���ƣ�VK_CloseMapFile 
//���ܣ��ر�VK_MapViewOfFile�򿪵��ļ������ӳ����
//������_hMapFile=ӳ����
//������_hFile=�ļ����
//���أ�eax����رճɹ�eax=TRUE��ʧ��eax=FALSE

BOOL _stdcall VK_CloseMapFile 	(HANDLE _hMapFile, HANDLE _hFile);


//���ƣ�VK_FindWndThread 
//���ܣ�����ָ���������ƻ򴰿�������������߳�
//������_szWndName=���Ҵ��ڵ�����
//������_szWndClassName=���Ҵ��ڵ�����,��ΪNULL�������࿪ʱ����������
//������_pdwProcessID=���Ҵ��ڵĽ���ID��д���ַ
//���أ�eax��eax=0��ʾ����ʧ�ܣ����򷵻��߳�ID

DWORD _stdcall VK_FindWndThread (PVOID _szWndName, PVOID _szWndClassName, PDWORD _pdwProcessID);


//���ƣ�VK_FindProcess 
//���ܣ��������̣����ҽ��̵ľ��
//������_szFindProcessName=Ҫ���ҵĽ�������,����������̺�׺(.exe)
//���أ�eax=���̾����edx=����ID���������δ�鵽��eax,edx����0

HANDLE _stdcall VK_FindProcess (PVOID _szFindProcessName);


//���ƣ�DebugActiveProcessStop 
//���ܣ��ñ����Խ����˳�����״̬
//������_DebugProcessID=���Խ���ID
//���أ�eax=0��ʾ�˳�����״̬ʧ��

BOOL _stdcall DebugActiveProcessStop (DWORD _DebugProcessID);


//���ƣ�VK_FindDllProc 
//���ܣ�����ָ����̬���ӿ���ָ�����Ƶĺ���
//������_szDllName=��̬���ӿ����ƣ���0��β
//������_szDllProcName=���Һ������ƣ���0��β
//���أ�eax=��̬���ӿ⺯����ַ���綯̬���ӿ����ʧ�ܻ���δ�ҵ���eax=0

PVOID _stdcall VK_FindDllProc	(PVOID _szDllName, PVOID _szDllProcName);

//���ƣ�VK_OpenFile
//���ܣ�ʹ�á����ļ����ؼ�
//������_pszFileName="dll Files (*.dll)"����0��β���ؼ���ʾ������
//������_pszFileType="*.dll"����0��β�������޶��ؼ����ļ�������
//������_pszOpenFilePath = �ļ�·���ַ�����ָ�룬����д��
//���أ�����û�ָ����һ���ļ��������OK��ť������ֵΪ����,���ȡ���򷵻�0
BOOL _stdcall VK_OpenFile (PBYTE _pszFileName, PBYTE _pszFileType, PBYTE _pszOpenFilePath);


//���ƣ� VK_GetFileDirectory
//���ܣ���ȡ�����ļ�·����Ŀ¼
//������_pszFilePath = �ļ�������·��
//������_pszFileDirectory = ��������ļ�Ŀ¼�ĵ�ַ
//���أ��ɹ�eax = 1��ʧ��eax = 0

BOOL _stdcall VK_GetFileDirectory (PBYTE _pszFilePath, PBYTE _pszFileDirectory);

//���ƣ� VK_GetFileName
//���ܣ���ȡ�����ļ�·�����ļ�����,��������׺��
//������_pszFilePath = �ļ�������·��
//������_pszFileName= ��������ļ����Ƶĵ�ַ
//���أ��ɹ�eax = �ļ����Ƴ��ȣ�ʧ��eax = 0

DWORD _stdcall VK_GetFileName (PBYTE _pszFilePath, PBYTE _pszFileName);

#ifdef __cplusplus
}
#endif
