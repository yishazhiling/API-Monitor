
// APIMonitorDlg.h: 头文件
//

#pragma once

#include <TlHelp32.h>
#include <Psapi.h>

// CAPIMonitorDlg 对话框
class CAPIMonitorDlg : public CDialogEx
{
// 构造
public:
	CAPIMonitorDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_APIMONITOR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

private:
	void EnumProcess();
	BOOL Is64BitOS();
	BOOL Is64BitPorcess(DWORD dwProcessID);
	DWORD GetOneProcessPid(CString ProcessName);
	BOOL LoadDll(DWORD ProcessPid, CString DllPath);
public:
	
	CListCtrl m_list;
	CStatic e_process;
	CStatic e_dllname;
	CStatic e_dllpath;
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton2();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
