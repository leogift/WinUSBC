#include "stdafx.h"
#include "LeoURLProtocol.h"
#include <Strsafe.h>

CLeoURLProtocol::CLeoURLProtocol(void)
{
	m_wszProtocolName =	"LeoWebProtocol";
	m_wszAppPath		=	"Notepad.exe";
}

CLeoURLProtocol::CLeoURLProtocol(std::string pwProtocolName, std::string pwAppPath)
{
	m_wszProtocolName =	pwProtocolName;
	m_wszAppPath		=	pwAppPath;
}

CLeoURLProtocol::~CLeoURLProtocol(void)
{
}

void CLeoURLProtocol::FormatErrorMsg()
{
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					FORMAT_MESSAGE_FROM_SYSTEM |
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					m_dwErrorCode,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(LPTSTR) &lpMsgBuf,
					0, NULL );

	// Display the error message and exit the process
	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
	(lstrlen((LPCTSTR)lpMsgBuf)+ 40) * sizeof(TCHAR)); 

	StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("错误代码 %d: %s "), 
        m_dwErrorCode, lpMsgBuf); 

	m_wszErrorMsg.append((LPCTSTR)lpDisplayBuf);
	OutputDebugString((LPCTSTR)lpDisplayBuf);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}

int CLeoURLProtocol::CreateCustomProtocol()
{
	CHAR szValue[MAX_PATH] = {0};
	HKEY hKey				= NULL;
	HKEY hKeyDefaultIcon	= NULL;
	HKEY hKeyCommand		= NULL;
	bool IsRegAlreadyPresent=	false;

	do
	{
		if((m_dwErrorCode = RegOpenKeyEx(HKEY_CLASSES_ROOT, m_wszProtocolName.c_str(), 0L,  KEY_READ, &hKey)) != ERROR_SUCCESS)
		{
			if((m_dwErrorCode = RegCreateKeyEx( HKEY_CLASSES_ROOT,
						m_wszProtocolName.c_str(),
						0,
						NULL,
						REG_OPTION_NON_VOLATILE,
						KEY_ALL_ACCESS,
						NULL,
						&hKey,
						NULL)) == ERROR_SUCCESS)
			{
				sprintf_s(szValue,MAX_PATH,URL_PROTOCOL_STRING,m_wszProtocolName.c_str());
				if((m_dwErrorCode = RegSetValueEx( hKey,"",0,REG_SZ,(BYTE *)&szValue,strlen(szValue)*2+2)) != ERROR_SUCCESS)
					break;

				if((m_dwErrorCode = RegSetValueEx( hKey,URL_PROTOCOL,0,REG_SZ,(BYTE *)"",sizeof(REG_SZ))) != ERROR_SUCCESS)
					break;

				if( (m_dwErrorCode = RegCreateKey(hKey,URL_PROTOCOL_DEFAULTICON,&hKeyDefaultIcon)) == ERROR_SUCCESS )
				{
					sprintf_s(szValue,MAX_PATH,"%s",m_wszAppPath.c_str());
					if((m_dwErrorCode = RegSetValueEx( hKeyDefaultIcon,"",0,REG_SZ,(BYTE *)&szValue,strlen(szValue)*2 + 2)) != ERROR_SUCCESS)
						break;

					if( (m_dwErrorCode = RegCreateKey(hKey,URL_PROTOCOL_COMMAND,&hKeyCommand)) == ERROR_SUCCESS )
					{
						sprintf_s(szValue,MAX_PATH,"%s %%1",m_wszAppPath.c_str());
						m_dwErrorCode = RegSetValueEx( hKeyCommand,"",0,REG_SZ,(BYTE *)&szValue,strlen(szValue)*2+2);
					}
				}
			}
		}
		else
		{
			m_dwErrorCode	=	-1;
			IsRegAlreadyPresent = true;
			sprintf_s(szValue,MAX_PATH,"%s 协议已经存在！",m_wszProtocolName.c_str());
			m_wszErrorMsg=szValue;
		}
	}
	while(FALSE);
	
	if(m_dwErrorCode != ERROR_SUCCESS)
		FormatErrorMsg();

	if(hKeyCommand)
	{
		::RegCloseKey(hKeyCommand);
		hKeyCommand = NULL;
	}
	if(hKeyDefaultIcon)
	{
		::RegCloseKey(hKeyDefaultIcon);
		hKeyDefaultIcon = NULL;
	}
	if(hKey)
	{
		::RegCloseKey(hKey);
		hKey = NULL;
	}

	return m_dwErrorCode;
}

int CLeoURLProtocol::DeleteCustomProtocol()
{
	HKEY hKey = NULL;

	if((m_dwErrorCode = RegOpenKeyEx(HKEY_CLASSES_ROOT, m_wszProtocolName.c_str(), 0L,  KEY_ALL_ACCESS, &hKey)) == ERROR_SUCCESS)
	{
		if( m_dwErrorCode = ::RegDeleteKey(hKey,URL_PROTOCOL_DEFAULTICON) == ERROR_SUCCESS)	
			if( m_dwErrorCode = ::RegDeleteKey(hKey,URL_PROTOCOL_COMMAND) == ERROR_SUCCESS)	
				if( m_dwErrorCode = ::RegDeleteKey(hKey,URL_PROTOCOL_OPEN) == ERROR_SUCCESS)	
					if( m_dwErrorCode = ::RegDeleteKey(hKey,URL_PROTOCOL_SHELL) == ERROR_SUCCESS)
						if( m_dwErrorCode = ::RegCloseKey(hKey) == ERROR_SUCCESS)
							m_dwErrorCode = ::RegDeleteKey(HKEY_CLASSES_ROOT,m_wszProtocolName.c_str());						
	}
	if(m_dwErrorCode != ERROR_SUCCESS)
		FormatErrorMsg();
	return m_dwErrorCode;
}