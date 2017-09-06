#pragma once

#include <iostream>
#include <string>

#define URL_PROTOCOL_STRING			"URL:%s Protocol"
#define URL_PROTOCOL				"URL Protocol"
#define URL_PROTOCOL_DEFAULTICON	"DefaultIcon"
#define URL_PROTOCOL_COMMAND		"Shell\\Open\\command"
#define URL_PROTOCOL_OPEN			"Shell\\Open"
#define URL_PROTOCOL_SHELL			"Shell"

class CLeoURLProtocol
{
public:
	CLeoURLProtocol(void);
	CLeoURLProtocol(std::string pwProtocolName, std::string pwAppPath);

	~CLeoURLProtocol(void);

	std::string getProtocolName() {return m_wszProtocolName;}
	std::string getAppPath() {return m_wszAppPath;}
	std::string getErrorMsg(){return m_wszErrorMsg;};

	void setProtocolName(std::string pwProtocolName){m_wszProtocolName = pwProtocolName;}
	void setAppPath(std::string pwAppPath){m_wszAppPath = pwAppPath;}

	int CreateCustomProtocol();
	int DeleteCustomProtocol();

private:
	std::string		m_wszProtocolName;
	std::string		m_wszAppPath;

	DWORD				m_dwErrorCode;
	std::string		m_wszErrorMsg;

	void				FormatErrorMsg();

};

