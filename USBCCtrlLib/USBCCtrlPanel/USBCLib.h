#ifndef __USBC_LIB_H
#define __USBC_LIB_H

#include<Windows.h>

const char USBC_SUCCEEDED = 0;					//操作成功
const char USBC_ERROR_NOUSBCDEVICE = -1;		//错误码：未连接设备
const char USBC_ERROR_WITHOUTPOST = -2;			//错误码：没有自检
const char USBC_ERROR_SELECT_DEVICEID = -4;		//错误码：所选设备ID不存在
const char USBC_ERROR_OUTOFRANGE_DEVICEID = -8;	//错误码：设备ID超出范围
const char USBC_ERROR_OUTOFRANGE_PORTID = -16;	//错误码：USB端口ID超出范围
const char USBC_ERROR_GENERIC = -128;			//错误码：其他

typedef int (*GetUSBCLibVersion)();
typedef int (*GetHardwareVersion)();
typedef int (*GetUSBCDevList)(BYTE* pComNO, BYTE* pDevID, BYTE* pDevSize);
typedef int (*RunUSBCLibPost)();
typedef int (*RunUSBCLibSelect)(BYTE ucDevID, BYTE ucPortID);

static HINSTANCE  hMod;

static GetUSBCLibVersion	dllGetUSBCLibVersion;
static GetHardwareVersion	dllGetHardwareVersion;
static GetUSBCDevList		dllGetUSBCDevList;
static RunUSBCLibPost		dllRunUSBCLibPost;
static RunUSBCLibSelect		dllRunUSBCLibSelect;

static void ParseError(char error)
{
	switch(error)
	{
		case USBC_SUCCEEDED:
			printf("成功!\r\n");
			break;
		case USBC_ERROR_NOUSBCDEVICE:
			printf("错误码：未连接设备!\r\n");
			break;
		case USBC_ERROR_WITHOUTPOST:
			printf("错误码：没有自检!\r\n");
			break;
		case USBC_ERROR_SELECT_DEVICEID:
			printf("错误码：所选设备ID不存在!\r\n");
			break;
		case USBC_ERROR_OUTOFRANGE_DEVICEID:
			printf("错误码：设备ID超出范围!\r\n");
			break;
		case USBC_ERROR_OUTOFRANGE_PORTID:
			printf("错误码：USB端口ID超出范围!\r\n");
			break;
		case USBC_ERROR_GENERIC:
			printf("错误码：USB端口ID超出范围!\r\n");
			break;
		default:
			printf("未知\r\n");
			break;
	}
};

static BYTE InitUSBCLib()
{
	hMod = LoadLibrary("USBCLib.DLL");

	BYTE bFlag=0x00;

	dllGetUSBCLibVersion  =  (GetUSBCLibVersion)GetProcAddress(hMod, "GetUSBCLibVersion");
	if( dllGetUSBCLibVersion )
	{
		bFlag|=0x01;
	}
				
	dllGetHardwareVersion  =  (GetHardwareVersion)GetProcAddress(hMod, "GetHardwareVersion");
	if( dllGetHardwareVersion )
	{
		bFlag|=0x02;
	}

	dllGetUSBCDevList = (GetUSBCDevList)GetProcAddress(hMod, "GetUSBCDevList");

	dllRunUSBCLibPost = (RunUSBCLibPost)GetProcAddress(hMod, "RunUSBCLibPost");
	if( dllRunUSBCLibPost )
	{
		bFlag|=0x04;
	}

	dllRunUSBCLibSelect = (RunUSBCLibSelect)GetProcAddress(hMod, "RunUSBCLibSelect");
	if( dllRunUSBCLibPost )
	{
		bFlag|=0x08;
	}

	return bFlag;
};

static void UninitUSBCLib()
{
	if(hMod)
		FreeLibrary(hMod);
};
#endif
