#include <algorithm>
#include <string>
#include <stdlib.h>
#include "LeoUrlParser.h"

// check if the scheme name is valid
static bool IsSchemeValid( const std::string& SchemeName )
{
	for ( auto c : SchemeName  )
	{
		if ( !isalpha( c ) && c != '+' && c != '-' && c != '.' ) return false;
	}

	return true;
}

bool CParser::GetPort( int* OutPort ) const
{
	if ( !IsValid() ) { return false; }

	int Port = atoi( m_Port.c_str() );

	if ( Port <= 0 || Port > 65535 ) { return false; }

	if ( OutPort ) { *OutPort = Port; }

	return true;
}

// based on RFC 1738 and RFC 3986
CParser::CParser( const std::string& URL )
{
	m_ErrorCode=LeoUrlParserError_Ok;
	m_Scheme="";
	m_Host="";
	m_Port="";
	m_Path="";

	memset( m_QueryParam, 0x00, sizeof(sQueryParam)*16);
	m_QueryLength = 0;

	const char* CurrentString = URL.c_str();

	/*
	 *	<scheme>:<scheme-specific-part>
	 *	<scheme> := [a-z\+\-\.]+
	 *	For resiliency, programs interpreting URLs should treat upper case letters as equivalent to lower case in scheme names
	 */

	// try to read scheme
	{
		const char* LocalString = strchr( CurrentString, ':' );

		if ( !LocalString )
		{
			ThrowError( LeoUrlParserError_NoUrlCharacter );
			return;
		}

		// save the scheme name
		m_Scheme = std::string( CurrentString, LocalString - CurrentString );

		if ( !IsSchemeValid( m_Scheme ) )
		{
			ThrowError( LeoUrlParserError_InvalidSchemeName );
			return;
		}

		// scheme should be lowercase
		std::transform( m_Scheme.begin(), m_Scheme.end(), m_Scheme.begin(), ::tolower );

		// skip ':'
		CurrentString = LocalString+1;
	}

	/*
	 *	//<user>:<password>@<host>:<port>/<url-path>
	 *	any ":", "@" and "/" must be normalized
	 */

	// skip "//"
	if ( *CurrentString++ != '/' ) 
	{
		ThrowError( LeoUrlParserError_NoDoubleSlash );
		return;
	}
	if ( *CurrentString++ != '/' ) 
	{
		ThrowError( LeoUrlParserError_NoDoubleSlash );
		return;
	}

	const char* LocalString = CurrentString;

	while ( *LocalString )
	{
		if ( *LocalString == '/' )
		{
			break;
		}

		LocalString++;
	}

	LocalString = CurrentString;

	// go ahead, read the host name
	LocalString = CurrentString;

	while ( *LocalString )
	{
		if ( *LocalString == ':' || *LocalString == '/' ) 
		{
			// port number is specified
			break;
		}

		LocalString++;
	}

	m_Host = std::string( CurrentString, LocalString - CurrentString );

	CurrentString = LocalString;

	// is port number specified?
	if ( *CurrentString == ':' )
	{
		CurrentString++;

		// read port number
		LocalString = CurrentString;

		while ( *LocalString && *LocalString != '/' ) LocalString++;

		m_Port = std::string( CurrentString, LocalString - CurrentString );

		CurrentString = LocalString;
	}

	// end of string
	if ( !*CurrentString )
	{
		ThrowError( LeoUrlParserError_UnexpectedEndOfLine );
		return;
	}

	// skip '/'
	if ( *CurrentString != '/' )
	{
		ThrowError( LeoUrlParserError_NoSlash );
		return;
	}

	CurrentString++;

	// parse the path
	LocalString = CurrentString;

	while ( *LocalString && *LocalString != '#' && *LocalString != '?' ) LocalString++;

	m_Path = std::string( CurrentString, LocalString - CurrentString );

	CurrentString = LocalString;

	// check for query
	if ( *CurrentString == '?' )
	{
		// skip '?'
		CurrentString++;

		// read query
		LocalString = CurrentString;

		/*
		do something to parse query string
		*/
		unsigned char nameLength = 0;
		unsigned char valueLength = 0;

		std::string name = "";
		std::string value = "";
		while ( *LocalString )
		{
			LocalString++;
			if( *LocalString == '=' )
			{
				name = std::string( CurrentString, LocalString - CurrentString );
				CurrentString=LocalString;
				CurrentString++;
				nameLength++;
	
				while( *LocalString != '&' && *LocalString ) LocalString++;
			
				value = std::string( CurrentString, LocalString - CurrentString );
				CurrentString=LocalString;
				CurrentString++;
				valueLength++;
						
				if(nameLength==valueLength)
				{
					m_QueryLength = nameLength;
					m_QueryParam[m_QueryLength-1].QueryName=name;
					m_QueryParam[m_QueryLength-1].QueryValue=value;
				}
			}
		}
	}

	m_ErrorCode = LeoUrlParserError_Ok;
}
