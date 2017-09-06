
// USBCCtrlPanel.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "USBCCtrlPanel.h"

#include "USBCCtrlPanelDlg.h"
#include "USBCPostDlg.h"
#include "USBCSelectDlg.h"

#include "LeoUrlParser.h"

BYTE SendBuffer[4096]={0};
BYTE RecvBuffer[256]={0};

char gFileExe[MAX_PATH]={0};
char gFilePath[MAX_PATH]={0};


// CUSBCCtrlPanelApp

BEGIN_MESSAGE_MAP(CUSBCCtrlPanelApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CUSBCCtrlPanelApp 构造

CUSBCCtrlPanelApp::CUSBCCtrlPanelApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CUSBCCtrlPanelApp 对象

CUSBCCtrlPanelApp theApp;


// CUSBCCtrlPanelApp 初始化
BOOL CUSBCCtrlPanelApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	
#ifdef _DEBUG  
    //AllocConsole();  
#endif

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	HMODULE module = GetModuleHandle(0);  
	GetModuleFileName(module, gFileExe, MAX_PATH); 
	CString csFullPath(gFileExe); 
	int nPos = csFullPath.ReverseFind( _T('\\') ); 
	strcpy_s(gFilePath,csFullPath.Left( nPos ).GetBuffer());

	CString strParam = AfxGetApp()->m_lpCmdLine;
	strParam=strParam.MakeLower();
	_DEBUG_PRINTF("%s\r\n", strParam);

	INT_PTR nResponse = NULL;
	CParser* pUrlParser = new CParser(strParam.GetBuffer());

	_DEBUG_PRINTF("HELLO HELL!");

	if(strcmp(pUrlParser->m_Path.c_str(),"post")==0)
	{
		CUSBCPostDlg dlg;

		m_pMainWnd = &dlg;
		nResponse= dlg.DoModal();
	}
	else if(strcmp(pUrlParser->m_Path.c_str(),"select")==0)
	{
		CUSBCSelectDlg dlg;

		unsigned char ucFoundDev=-1;
		unsigned char ucFoundPort=-1;

		for(int idx=0; idx<pUrlParser->m_QueryLength; idx++)
		{
			if(strcmp(pUrlParser->m_QueryParam[idx].QueryName.c_str(),"dev")==0)
			{
				dlg.m_ucDevID=atoi(pUrlParser->m_QueryParam[idx].QueryValue.c_str());
				ucFoundDev=0;
				if(dlg.m_ucDevID>=0 && dlg.m_ucDevID<=7)
				{
					ucFoundDev=1;
				}
			}
			else if(strcmp(pUrlParser->m_QueryParam[idx].QueryName.c_str(),"port")==0)
			{
				dlg.m_uiPortNO=atoi(pUrlParser->m_QueryParam[idx].QueryValue.c_str());
				ucFoundPort=0;
				if(dlg.m_uiPortNO>=0 && dlg.m_uiPortNO<=192)
				{
					ucFoundPort=1;
				}
			}
		}
		if(ucFoundDev==1 && ucFoundPort==1)
		{
			m_pMainWnd = &dlg;
			nResponse= dlg.DoModal();
		}
		else
		{
			if(ucFoundDev<0 || ucFoundPort<0)
			{
				MessageBox(NULL,"请按 <leowebcmd://usbc/select?dev=x&port=y> 格式执行协议！","错误",MB_OK);
			}
			else if(ucFoundDev==0)
			{
				MessageBox(NULL,"所选择的设备ID序号超出范围！","错误",MB_OK);
			}
			else if(ucFoundPort==0)
			{
				MessageBox(NULL,"所选择的USB序号超出范围！","错误",MB_OK);
			}
			nResponse= 0;
		}
	}
#if _DEBUG==0
	else if(strcmp(pUrlParser->m_Path.c_str(),"ctrl")==0)
#endif
	{
		CUSBCCtrlPanelDlg dlg;
		m_pMainWnd = &dlg;
		nResponse= dlg.DoModal();
	}
#if _DEBUG==0
	else
	{
		CLeoURLProtocol URLProtocol;
		URLProtocol.setProtocolName("LeoWebCmd");
		URLProtocol.setAppPath(gFileExe);
		URLProtocol.DeleteCustomProtocol();
		URLProtocol.CreateCustomProtocol();
		nResponse = 0;
	}
#endif
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	delete pUrlParser;

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

BOOL CUSBCCtrlPanelApp::ExitInstance()
{
#ifdef _DEBUG  
    FreeConsole();  
#endif 

	return CWinApp::ExitInstance();
}