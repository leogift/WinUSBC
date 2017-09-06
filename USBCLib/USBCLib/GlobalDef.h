#ifndef _GLOBAL_DEFINE
#define _GLOBAL_DEFINE

#include <Windows.h>
#include <stdio.h>

#define BYTE unsigned char

#ifdef _DEBUG
	#include <CONIO.H>
	#define _DEBUG_PRINTF _cprintf
#else
	#define _DEBUG_PRINTF (void)
#endif

using namespace std;

#endif
