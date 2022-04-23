//ParamINI.h And ParamINI.c
//读写"ParamTemplet"文件夹下的参数INI文件，获取参数的个数、名称和属性
 
#pragma once

#include <windows.h>
#include "VikiInc.h"
#include "DataBase.h"

#define	cKeyName_ParamNum		"ParamNum"
#define	cKeyName_ParamName		"Name_Param"
#define	cKeyName_ParamType		"Type_Param"
#define	cKeyName_MaxNum			0x100000


ULONG GetParamNumAndType(PCHAR _pszINIFilePath, PCHAR _pszAPIName, PSTParam _pstParam);
ULONG GetParamName(PCHAR _pszINIFilePath, PCHAR _pszAPIName, PVOID _pBuffer);