
// USBCCtrlPanelDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "USBCCtrlPanel.h"
#include "USBCCtrlPanelDlg.h"
#include "afxdialogex.h"

#include "USBCPostDlg.h"
#include "USBCSelectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CUSBCCtrlPanelDlg 对话框



CUSBCCtrlPanelDlg::CUSBCCtrlPanelDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUSBCCtrlPanelDlg::IDD, pParent)
	, m_uiPort(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUSBCCtrlPanelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PORT, m_uiPort);
}

BEGIN_MESSAGE_MAP(CUSBCCtrlPanelDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_BTN_UNREG, &CUSBCCtrlPanelDlg::OnBnClickedBtnUnreg)
	ON_BN_CLICKED(ID_BTN_REG, &CUSBCCtrlPanelDlg::OnBnClickedBtnReg)
	ON_BN_CLICKED(IDC_BTN_POST, &CUSBCCtrlPanelDlg::OnBnClickedBtnPost)
	ON_BN_CLICKED(IDC_BTN_SELECT, &CUSBCCtrlPanelDlg::OnBnClickedBtnSelect)
END_MESSAGE_MAP()


// CUSBCCtrlPanelDlg 消息处理程序

BOOL CUSBCCtrlPanelDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CUSBCCtrlPanelDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUSBCCtrlPanelDlg::OnPaint()
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
HCURSOR CUSBCCtrlPanelDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CUSBCCtrlPanelDlg::OnBnClickedBtnReg()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD errorCode = 0;

	m_URLProtocol.setProtocolName("LeoWebCmd");
	m_URLProtocol.setAppPath(gFileExe);

	if(m_URLProtocol.CreateCustomProtocol() != ERROR_SUCCESS)
	{
		MessageBox(m_URLProtocol.getErrorMsg().c_str());
	}
	else
	{
		MessageBox("协议注册成功！");
	}
}

void CUSBCCtrlPanelDlg::OnBnClickedBtnUnreg()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD errorCode = 0;

	m_URLProtocol.setProtocolName("LeoWebCmd");
	m_URLProtocol.setAppPath(gFileExe);

	if(m_URLProtocol.DeleteCustomProtocol() != ERROR_SUCCESS)
	{
		MessageBox(m_URLProtocol.getErrorMsg().c_str());
	}
	else
	{
		MessageBox("协议删除成功！");
	}
}

void CUSBCCtrlPanelDlg::OnBnClickedBtnPost()
{
	// TODO: 在此添加控件通知处理程序代码
	CUSBCPostDlg dlg;
	dlg.DoModal();
	
	char strTmp[256]={};
	if(dlg.m_DevSize>0)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_COMLIST))->Clear();
		((CComboBox*)GetDlgItem(IDC_COMBO_COMLIST))->ResetContent();
		for(int idx=0;idx<dlg.m_DevSize;idx++)
		{
			_itoa_s(dlg.m_DevIDList[idx], strTmp, 10);
			((CComboBox*)GetDlgItem(IDC_COMBO_COMLIST))->AddString(strTmp);
			m_uiPort=0;
		}
		((CComboBox*)GetDlgItem(IDC_COMBO_COMLIST))->SetWindowText(strTmp);
		UpdateData(false);
	}
}

void CUSBCCtrlPanelDlg::OnBnClickedBtnSelect()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	CString strTmp="";
	((CComboBox*)GetDlgItem(IDC_COMBO_COMLIST))->GetWindowText(strTmp);
	int ucDevID = atoi(strTmp);
	if(ucDevID>=0 && ucDevID<256 && m_uiPort>=0 && m_uiPort<256)
	{
		CUSBCSelectDlg dlg;
		dlg.m_ucDevID=ucDevID;
		dlg.m_uiPortNO=m_uiPort;
		dlg.DoModal();
	}
	else
	{
		MessageBox("设备序号或USB序号错误！","警告",MB_OK);
	}
}
