#pragma once
#include "afxcmn.h"

// CUSBCSelectDlg 对话框

class CUSBCSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CUSBCSelectDlg)

public:
	CUSBCSelectDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUSBCSelectDlg();

	unsigned int m_uiPortNO;
	unsigned char m_ucDevID;

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
};
