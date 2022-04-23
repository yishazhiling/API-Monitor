
// APIMonitorDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "APIMonitor.h"
#include "APIMonitorDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAPIMonitorDlg 对话框


CAPIMonitorDlg::CAPIMonitorDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_APIMONITOR_DIALOG, pParent)
{
}

void CAPIMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_PREOCESSNAME, e_process);
	DDX_Control(pDX, IDC_DLLNAME, e_dllname);
	DDX_Control(pDX, IDC_DLLPATH, e_dllpath);
}

BEGIN_MESSAGE_MAP(CAPIMonitorDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CAPIMonitorDlg::OnDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON1, &CAPIMonitorDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CAPIMonitorDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &CAPIMonitorDlg::OnBnClickedButton2)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CAPIMonitorDlg 消息处理程序

BOOL CAPIMonitorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_list.ModifyStyle(LVS_ICON | LVS_SMALLICON | LVS_LIST, LVS_REPORT);
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES| LVS_EX_ONECLICKACTIVATE);
	m_list.InsertColumn(0, "PROCESS", LVCFMT_LEFT,260);

	EnumProcess();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAPIMonitorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAPIMonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAPIMonitorDlg::EnumProcess()
{
	// 删除所有节点
	m_list.DeleteAllItems();

	PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
	// 创建快照
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	// 得到第一个进程的信息
	BOOL bSuccess = Process32First(hSnap, &pe);
	// 循环遍历所有进程的信息
	if (bSuccess == TRUE)
	{
		int nidx=0;
		do
		{
			if (!Is64BitPorcess(pe.th32ProcessID)) 
			{
				m_list.InsertItem(nidx, pe.szExeFile);
				nidx++;
			}
				
		} while (Process32Next(hSnap, &pe));
	}
	CloseHandle(hSnap);
}

// 判断是不是64位操作系统
BOOL CAPIMonitorDlg::Is64BitOS()
{
	typedef VOID(WINAPI* LPFN_GetNativeSystemInfo)(__out LPSYSTEM_INFO lpSystemInfo);
	LPFN_GetNativeSystemInfo fnGetNativeSystemInfo = (LPFN_GetNativeSystemInfo)GetProcAddress(GetModuleHandleW(L"kernel32"), "GetNativeSystemInfo");
	if (fnGetNativeSystemInfo)
	{
		SYSTEM_INFO stInfo = { 0 };
		fnGetNativeSystemInfo(&stInfo);
		if (stInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64
			|| stInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
		{
			return TRUE;
		}
	}
	return FALSE;
}

// 判断是不是64位程序
BOOL CAPIMonitorDlg::Is64BitPorcess(DWORD dwProcessID)
{
	// 32位系统
	if (!Is64BitOS())
	{
		return FALSE;
	}
	else
	{
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessID);
		if (hProcess)
		{
			typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
			LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandleW(L"kernel32"), "IsWow64Process");
			if (NULL != fnIsWow64Process)
			{
				BOOL bIsWow64 = FALSE;
				fnIsWow64Process(hProcess, &bIsWow64);
				CloseHandle(hProcess);
				if (bIsWow64)
				{
					return FALSE;
				}
				else
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

DWORD CAPIMonitorDlg::GetOneProcessPid(CString ProcessName)
{
	HANDLE hSnapShot;
	PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
	// 1、获得当前进程的快照

	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnapShot == INVALID_HANDLE_VALUE) {
		return -1;
	}

	bool bMore;
	// 2、遍历进程的名称是否为指定名称,获取指定进程名称的PID
	bMore = Process32First(hSnapShot, &pe);
	while (bMore) {
		// 3、获取指定进程名称的PID
		if (!strcmp(pe.szExeFile, ProcessName)) {
			CloseHandle(hSnapShot);
			return pe.th32ProcessID;
		}
		bMore = Process32Next(hSnapShot, &pe);  		//遍历
	}

	// 4、释放资源
	CloseHandle(hSnapShot);
	return 0;
}

BOOL CAPIMonitorDlg::LoadDll(DWORD ProcessPid, CString DllPath)
{
	HANDLE hProcess;
	DWORD DllPathLen;
	PDWORD addr;
	HMODULE hModule;
	PDWORD FuncAddr;

	// 1、获取要注入进程的句柄
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessPid);
	if (hProcess == NULL) 
		return FALSE;

	// 2、获取DLL的信息
	DllPathLen = strlen(DllPath) + 1; //结尾'\0'

	// 3、申请指定进程中的内存
	addr = (PDWORD)VirtualAllocEx(
		hProcess, //申请指定进程的句柄
		NULL,  // 安全描述符
		DllPathLen,  // 申请内存的字节大小
		MEM_COMMIT,  // 内存分配的类型
		PAGE_READWRITE // 内存的属性
	);
	
		
	// 4、将DLL的信息写入到要注入的进程内存中
	if (addr == NULL)
		return FALSE; 
	WriteProcessMemory(hProcess, addr, DllPath, DllPathLen, NULL);
		

	// 5、要获取其进程的LoadLibraryA/W的函数地址，该函数保存在系统中的Kernel32.dll中，那么需要先获得Kernel32.dll的句柄
	hModule = GetModuleHandle("Kernel32.dll");

	// 6、从中获得LoadLibraryA/W的函数地址
	FuncAddr = (PDWORD)GetProcAddress(hModule, "LoadLibraryA"); //返回值为函数地址

	// 7、注入到指定进程中进行加载内存中申请的DLL信息，把LoadLibraryA的地址作为函数来加载addr，也就是DLL的路径
	CreateRemoteThread(hProcess, NULL, 0, LPTHREAD_START_ROUTINE(FuncAddr), addr, 0, NULL);

	return TRUE;
}



//进程列表双击选择进程
void CAPIMonitorDlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	NMLISTVIEW* pp = (NMLISTVIEW*)pNMHDR;
	int nidx = pp->iItem;	//获取行号
	CString str = m_list.GetItemText(nidx, 0);
	e_process.SetWindowText(str);
}

//添加DLL按钮处理程序
void CAPIMonitorDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Dynamic Link Library (*.dll)|*.dll||"), NULL);
	if (dlgFile.DoModal() == IDOK)
	{
		e_dllpath.SetWindowText(dlgFile.GetPathName());
		e_dllname.SetWindowText(dlgFile.GetFileName());
	}
}

//开始注入按钮处理程序
void CAPIMonitorDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString Processname,DLLpath;
	e_process.GetWindowText(Processname);
	if (Processname.IsEmpty()) 
	{
		MessageBox("未选取进程,请先选择进程");
		return;
	}
	e_dllpath.GetWindowText(DLLpath);
	if (DLLpath.IsEmpty())
	{
		MessageBox("未选取DLL，请先选择DLL");
		return;
	}
	DWORD Pid = GetOneProcessPid(Processname);
	switch (Pid)
	{
	case -1:
		MessageBox("获取句柄失败");
		break;
	case 0:
		MessageBox("当前进程不存在");
		break;
	default:
		LoadDll(Pid, DLLpath);
		break;
	}
}

//刷新进程按钮处理程序
void CAPIMonitorDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	EnumProcess();
}




HBRUSH CAPIMonitorDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO:  在此更改 DC 的任何特性
	  
	if (pWnd->GetDlgCtrlID() == IDC_PREOCESSNAME)
	{
		hbr = CreateSolidBrush(RGB(255, 255, 255));
		pDC->SetBkColor(RGB(255, 255, 255));
		pDC->SetBkMode(TRANSPARENT);
		
	}
	if (pWnd->GetDlgCtrlID() == IDC_DLLNAME)
	{
		hbr = CreateSolidBrush(RGB(255, 255, 255));
		pDC->SetBkColor(RGB(255, 255, 255));
		pDC->SetBkMode(TRANSPARENT);
	}
	if (pWnd->GetDlgCtrlID() == IDC_DLLPATH)
	{
		hbr = CreateSolidBrush(RGB(255, 255, 255));
		pDC->SetBkColor(RGB(255, 255, 255));
		pDC->SetBkMode(TRANSPARENT);
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
