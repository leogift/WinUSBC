#pragma once
#include "afxcmn.h"

// CUSBCPostDlg 对话框

class CUSBCPostDlg : public CDialog
{
	DECLARE_DYNAMIC(CUSBCPostDlg)

public:
	CUSBCPostDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUSBCPostDlg();

// 对话框数据
	enum { IDD = IDD_USBCPROCDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CProgressCtrl m_ProgressCtrl;
	BYTE m_DevIDList[255];
	BYTE m_ComNOList[255];
	BYTE m_DevSize;
};
