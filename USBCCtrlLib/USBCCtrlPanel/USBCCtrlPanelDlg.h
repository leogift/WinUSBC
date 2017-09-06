
// USBCCtrlPanelDlg.h : 头文件
//

#pragma once
#include "LeoURLProtocol.h"

// CUSBCCtrlPanelDlg 对话框
class CUSBCCtrlPanelDlg : public CDialogEx
{
// 构造
public:
	CUSBCCtrlPanelDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_USBCCTRLPANEL_DIALOG };

	void InitParameter();

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	CLeoURLProtocol m_URLProtocol;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnUnreg();
	afx_msg void OnBnClickedBtnReg();
	afx_msg void OnBnClickedBtnPost2();
	afx_msg void OnBnClickedBtnPost();
	afx_msg void OnBnClickedBtnSelect();
	UINT m_uiPort;
};
