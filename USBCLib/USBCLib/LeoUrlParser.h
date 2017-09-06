#ifndef _LEO_URL_PARSER_H
#define _LEO_URL_PARSER_H

#include <string>

enum eLeoUrlParserError
{
	LeoUrlParserError_Ok = 0,
	LeoUrlParserError_Uninitialized = 1,
	LeoUrlParserError_NoUrlCharacter = 2,
	LeoUrlParserError_InvalidSchemeName = 3,
	LeoUrlParserError_NoDoubleSlash = 4,
	LeoUrlParserError_NoAtSign = 5,
	LeoUrlParserError_UnexpectedEndOfLine = 6,
	LeoUrlParserError_NoSlash = 7,
};

typedef struct
{
	std::string QueryName;
	std::string QueryValue;
} sQueryParam;

class CParser
{
public:
	eLeoUrlParserError m_ErrorCode;
	std::string m_Scheme;
	std::string m_Host;
	std::string m_Port;
	std::string m_Path;

	sQueryParam m_QueryParam[16];
	unsigned char m_QueryLength;

	CParser( const std::string& URL );
	void ThrowError(eLeoUrlParserError err){m_ErrorCode = err;};
	/// return 'true' if the parsing was successful
	bool IsValid() const { return m_ErrorCode == LeoUrlParserError_Ok; }

	/// helper to convert the port number to int, return 'true' if the port is valid (within the 0..65535 range)
	bool GetPort( int* OutPort ) const;

private:
	explicit CParser( eLeoUrlParserError ErrorCode )
		: m_ErrorCode( ErrorCode )
	{}
};

#endif
