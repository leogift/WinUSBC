#pragma once;

#include "GlobalDef.h"

#define LIB_VERSION_H 1
#define LIB_VERSION_L 1

#define HARDWARE_VERSION_H 1
#define HARDWARE_VERSION_L 0

#ifdef DLL_IMPLEMENT
	#define DLL_USBC __declspec(dllexport)
#else
	#define DLL_USBC __declspec(dllimport)
#endif

const int ERROR_NOUSBCDEVICE = -1;			//错误码：未连接设备
const int ERROR_WITHOUTPOST = -2;			//错误码：没有自检
const int ERROR_SELECT_DEVICEID = -4;		//错误码：所选设备ID不存在
const int ERROR_OUTOFRANGE_DEVICEID = -8;	//错误码：设备ID超出范围
const int ERROR_OUTOFRANGE_PORTID = -16;	//错误码：USB端口ID超出范围
const int ERROR_GENERIC = -128;			//错误码：其他

extern "C" DLL_USBC int GetUSBCLibVersion();
extern "C" DLL_USBC int GetHardwareVersion();
extern "C" DLL_USBC int GetUSBCDevList(BYTE* pComNO, BYTE* pDevID, BYTE* pDevSize);
extern "C" DLL_USBC int RunUSBCLibPost();
extern "C" DLL_USBC int RunUSBCLibSelect(BYTE ucDevID, BYTE ucPortID);
