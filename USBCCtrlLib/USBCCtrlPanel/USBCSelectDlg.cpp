// USBCSelectDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "USBCCtrlPanel.h"
#include "USBCSelectDlg.h"
#include "afxdialogex.h"

#include "USBCLib.h"

#define TIMER_USER			0x1000
#define TIMER_EXIT			(TIMER_USER+1)
#define TIMER_PROGRESS		(TIMER_USER+2)

const int TotalTime=5000;
const int ProgressRange = 100;
const int ProgressStepTime = TotalTime/ProgressRange;

static volatile int ProgressPos=0;

// CUSBCSelectDlg 对话框

IMPLEMENT_DYNAMIC(CUSBCSelectDlg, CDialog)

CUSBCSelectDlg::CUSBCSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUSBCSelectDlg::IDD, pParent)
{
	m_uiPortNO=0;
	m_ucDevID=0;
	
	InitUSBCLib();
}

CUSBCSelectDlg::~CUSBCSelectDlg()
{
	UninitUSBCLib();
}

void CUSBCSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_CTRL, m_ProgressCtrl);
}


BEGIN_MESSAGE_MAP(CUSBCSelectDlg, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CUSBCSelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowText("接入指定USB端口");
	ModifyStyle(WS_CAPTION, NULL, SWP_DRAWFRAME );
	SetDlgItemText(IDC_STATIC_TITLE,"接入指定USB端口" );
	SetDlgItemText(IDC_STATIC_LOG,"信息： 准备接入指定USB端口..." );
	
	m_ProgressCtrl.SetRange(0, 100);  
    m_ProgressCtrl.SetPos(ProgressPos);
  
	SetTimer(TIMER_EXIT, TotalTime, NULL);
	SetTimer(TIMER_PROGRESS, ProgressStepTime, NULL);
	
	char res=dllRunUSBCLibSelect(m_ucDevID,m_uiPortNO);
	if(res>=0)
	{
		ProgressPos=100;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL CUSBCSelectDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message == WM_KEYDOWN)
	{
		switch(pMsg->wParam) 
		{
			case VK_RETURN: //回车
				return TRUE;
			case VK_ESCAPE: //ESC
				return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CUSBCSelectDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if(nIDEvent>TIMER_USER)
	{
		switch(nIDEvent) 
		{
			case TIMER_EXIT:
				KillTimer(TIMER_EXIT);
				KillTimer(TIMER_PROGRESS);
				SetDlgItemText(IDC_STATIC_LOG,"错误： 操作超时，即将退出" );
				MessageBox("操作超时，即将退出","错误",MB_ICONEXCLAMATION );
				OnCancel();
				break;

			case TIMER_PROGRESS:
				KillTimer(TIMER_PROGRESS);
				if(ProgressPos++<ProgressRange*0.9)
				{
					m_ProgressCtrl.SetPos(ProgressPos);
					SetTimer(TIMER_PROGRESS,ProgressStepTime,NULL);
				}
				else if(ProgressPos>=100)
				{
					KillTimer(TIMER_EXIT);
					SetDlgItemText(IDC_STATIC_LOG,"操作成功" );
					Sleep(1000);
					OnOK();
				}
				break;
		}
	}

	CDialog::OnTimer(nIDEvent);
}
