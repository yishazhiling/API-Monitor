//VikiInc.h
#pragma once 
#include <windows.h>

#pragma comment(lib, "VikiProcLib.lib")

#ifdef __cplusplus
extern "C" 
{
#endif

//名称：VK_AllocMem 
//功能：申请指定大小的虚拟内存, 该内存属性为PAGE_EXECUTE_READWRITE
//参数：_dwSize = 申请内存的大小
//返回：成功返回内存的指针，失败返回0

PVOID _stdcall VK_AllocMem (ULONG _dwSize);


//名称：VK_FreeMem
//功能：释放指定大小的虚拟内存
//参数：_pMemAddr = 内存地址
//参数：_dwSize = 释放内存的大小
//返回：eax = 1表示成功，eax=0表示失败

BOOL _stdcall VK_FreeMem (PVOID _pMemAddr, ULONG _dwSize);


//名称：VK_DwordToHex 
//功能：将一个32位数据转化为以0结束的16进制字符串
//参数：dwNum=要转化的32位数字
//参数：pHexBuffer=16进制字符串存放的地址,存放地址的大小>=9字节
//返回：无

VOID _stdcall VK_DwordToHex (DWORD dwNum, PVOID pHexBuffer);


//名称：VK_DwordToDec 
//功能：将一个32位数据转化为以0结束的10进制字符串
//参数：dwNum=要转化的32位数字
//参数：pHexBuffer=16进制字符串存放的地址,存放地址的长度>=12字节
//返回：无
    
VOID _stdcall VK_DwordToDec (DWORD dwNum, PVOID pHexBuffer);
    
    
//名称：VK_Lengthsz 
//功能：计算以0结尾的字符串的长度
//参数：lpsz=以0结尾的字符串首地址
//返回：eax=字符串长度（以字节计，不计结尾的0）

DWORD _stdcall VK_Lengthsz (PVOID lpsz);


//名称：VK_CmpMem
//功能：比较2段内存是否相等
//参数：_pSourceAddr = 要比较的内存地址, 即放入esi比较
//参数：_pTargetAddr = 比较对象的内存地址，即放入edi比较
//参数：_dwSize = 要比较的大小内存的大小
//返回：相等返回1，不相等返回0

BOOL _stdcall VK_CmpMem (LPVOID _pSourceAddr, LPVOID _pTargetAddr, DWORD _dwSize);


//名称：VK_CopyMem 
//功能：复制内存，将指定位置指定大小的内存复制到指定位置
//参数：lpSource=要复制内存的首地址
//参数：lpTarget=复制内存的目的地址
//参数：dwSize=需复制内存的大小
//返回：无

VOID _stdcall VK_CopyMem (PVOID lpSourceAddr, PVOID lpTargetAddr, DWORD dwSize);

//名称：VK_Copysz 
//功能：复制字符串，将一个以0结束的字符串复制到另一个地址中，并以0结尾
//参数：lpszSource=要复制的以0结尾的字符串首地址
//参数：lpszTarget=复制目的字符串首地址
//返回：复制的字符长度，不包括结尾的0

VOID _stdcall VK_Copysz (PVOID lpszSource, PVOID lpszTarget);


//名称：VK_Connectsz 
//功能：合并字符串，将二个以0结束的字符串合并为一个以0结尾的字符串
//参数：lpsz1=第一个以0结尾的字符串首地址
//参数：lpsz2=第二个以0结尾的字符串首地址
//参数：lpszBuffer=合并后的字符串存放的地址
//返回：无

VOID _stdcall VK_Connectsz (PVOID lpsz1, PVOID lpsz2, PVOID lpszBuffer);
    

//名称：VK_PrintfMsg
//功能：弹出一个消息框，显示_pszFormat指定格式的变量，注意只能显示一个变量（可以为字符串）
//参数：_pszFormat=要显示数据格式
//参数：_Data=要显示变量
//返回：无


//名称： VK_FindData
//功能：查找匹配的数据
//参数1：_pStartAddr=查找的起始地址
//参数2：_ulDataSize=查找数据的大小
//参数3：_pFindData=查找数据的首地址，即指针
//参数4：_ulDataLength=查找数据的字节数
//返回：如找到则返回数据所在地址，未找到返回NULL
PVOID _stdcall VK_FindData (PVOID _pStartAddr, DWORD _ulDataSize, PVOID _pFindData, DWORD _ulDataLength);


VOID _stdcall VK_PrintfMsg (PVOID _pszFormat, DWORD _Data);

//名称：VK_MsgBox 
//功能：弹出一个消息框，显示0结束的字符串
//参数：lpszMsg=要显示的以0结尾的字符串首地址
//返回：无

VOID _stdcall VK_MsgBox (PVOID lpszMsg);
 
//名称：VK_DwordMsgBox 
//功能：弹出一个消息框，显示0结束的16进制字符串
//参数：Num=要显示双字数字
//返回：无

VOID _stdcall VK_DwordMsgBox (DWORD _dwNum);


//名称：VK_NumMsgBox
//功能：弹出一个消息框，显示0结束的字符串
//参数：_dwNum=要显示双字数字
//返回：无

VOID _stdcall VK_NumMsgBox (DWORD _dwNum);

    
//名称：VK_ReplaceMemory 
//功能：将指定长度和地址的内存值替换为指定的值
//参数：lpMemory=要替换内存的首地址
//参数：MemorySiz=内存大小，以字节计
//参数：bDATA=要替换的数值，大小为字节
//返回：无

VOID _stdcall VK_ReplaceMemory (PVOID lpMemory, DWORD MemorySize, BYTE bDATA);


//名称：VK_ReadFile 
//功能：读取文件，返回文件句柄
//参数：szReadFilePath=文件的绝对路径
//参数：pReadBuffer=文件读取后存放的缓存地址
//参数：pSizeRead=返回实际读取文件大小的地址
//返回：eax，eax=文件的句柄，如果读取失败则eax=FALSE

HANDLE _stdcall VK_ReadFile (PVOID szReadFilePath, PVOID pReadBuffer, PDWORD pSizeRead);


//名称：VK_WriteFile 
//功能：写文件，返回文件句柄，
//参数：szWriteFilePath=文件的绝对路径
//参数：pWriteBuffer=要写入的文件的缓冲地址
//参数：SizeWrite=写入文件的大小
//返回：eax，eax=文件的句柄，如果读取失败则eax=FALSE

HANDLE _stdcall VK_WriteFile (PVOID  szWriteFilePath, PVOID pWriteBuffer, DWORD SizeWrite);


//名称：VK_CloseFile 
//功能：关闭VK_ReadFile打开的文件句柄，并将分配内存的指针pMemory解锁，句柄hMemory释放
//参数：hFile=文件的句柄
//参数：pMemory=分配内存的指针
//返回：eax，如关闭成功eax=TRUE，失败eax=FALSE

BOOL _stdcall VK_CloseFile (HANDLE hCloseFile, PVOID pMemory);


//名称：VK_MapViewOfFile
//功能：将文件读入内存映像，返回文件句柄和映像句柄
//参数：_pszFilePath = 文件的绝对路径
//参数：_pFileHandle = 写回文件句柄的地址
//参数：_pMapViewHandle = 写回文件映像对象句柄的地址
//返回：eax = 映像文件指针,如果读取失败则eax = 0

PVOID _stdcall VK_MapViewOfFile 	(PVOID _szFilePath, PHANDLE _pFileHandle, PHANDLE _pMapViewHandle);

//名称：VK_CloseMapFile 
//功能：关闭VK_MapViewOfFile打开的文件句柄和映像句柄
//参数：_hMapFile=映像句柄
//参数：_hFile=文件句柄
//返回：eax，如关闭成功eax=TRUE，失败eax=FALSE

BOOL _stdcall VK_CloseMapFile 	(HANDLE _hMapFile, HANDLE _hFile);


//名称：VK_FindWndThread 
//功能：查找指定窗口名称或窗口类名程序的主线程
//参数：_szWndName=查找窗口的名称
//参数：_szWndClassName=查找窗口的类名,可为NULL，但当多开时以类名区分
//参数：_pdwProcessID=查找窗口的进程ID得写入地址
//返回：eax，eax=0表示查找失败，否则返回线程ID

DWORD _stdcall VK_FindWndThread (PVOID _szWndName, PVOID _szWndClassName, PDWORD _pdwProcessID);


//名称：VK_FindProcess 
//功能：遍历进程，查找进程的句柄
//参数：_szFindProcessName=要查找的进程名称,必须包括进程后缀(.exe)
//返回：eax=进程句柄，edx=进程ID，如果进程未查到则eax,edx返回0

HANDLE _stdcall VK_FindProcess (PVOID _szFindProcessName);


//名称：DebugActiveProcessStop 
//功能：让被调试进程退出调试状态
//参数：_DebugProcessID=调试进程ID
//返回：eax=0表示退出调试状态失败

BOOL _stdcall DebugActiveProcessStop (DWORD _DebugProcessID);


//名称：VK_FindDllProc 
//功能：查找指定动态链接库中指定名称的函数
//参数：_szDllName=动态链接库名称，以0结尾
//参数：_szDllProcName=查找函数名称，以0结尾
//返回：eax=动态链接库函数地址，如动态链接库加载失败或函数未找到，eax=0

PVOID _stdcall VK_FindDllProc	(PVOID _szDllName, PVOID _szDllProcName);

//名称：VK_OpenFile
//功能：使用“打开文件”控件
//参数：_pszFileName="dll Files (*.dll)"，以0结尾，控件显示的名称
//参数：_pszFileType="*.dll"，以0结尾，用来限定控件打开文件的类型
//参数：_pszOpenFilePath = 文件路径字符串的指针，用来写回
//返回：如果用户指定了一个文件名，点击OK按钮，返回值为非零,点击取消则返回0
BOOL _stdcall VK_OpenFile (PBYTE _pszFileName, PBYTE _pszFileType, PBYTE _pszOpenFilePath);


//名称： VK_GetFileDirectory
//功能：获取完整文件路径的目录
//参数：_pszFilePath = 文件的完整路径
//参数：_pszFileDirectory = 用来存放文件目录的地址
//返回：成功eax = 1，失败eax = 0

BOOL _stdcall VK_GetFileDirectory (PBYTE _pszFilePath, PBYTE _pszFileDirectory);

//名称： VK_GetFileName
//功能：获取完整文件路径的文件名称,不包含后缀名
//参数：_pszFilePath = 文件的完整路径
//参数：_pszFileName= 用来存放文件名称的地址
//返回：成功eax = 文件名称长度，失败eax = 0

DWORD _stdcall VK_GetFileName (PBYTE _pszFilePath, PBYTE _pszFileName);

#ifdef __cplusplus
}
#endif
