#include "GlobalDef.h"

#include <sstream>

// The next 3 includes are needed for serial port enumeration
#include <objbase.h>
#include <initguid.h>
#include <Setupapi.h>

#include "EnumSerial.h"

#pragma comment(lib, "Setupapi.lib")

// The following define is from ntddser.h in the DDK. It is also
// needed for serial port enumeration.
#ifndef GUID_CLASS_COMPORT
	DEFINE_GUID(GUID_CLASS_COMPORT, 0x86e0d1e0L, 0x8089, 0x11d0, 0x9c, 0xe4, \
			0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73);
#endif

//---------------------------------------------------------------
// Helpers for enumerating the available serial ports.
// These throw a CString on failure, describing the nature of
// the error that occurred.

void EnumPortsWdm(vector<SSerInfo> &asi);

//---------------------------------------------------------------
// Routine for enumerating the available serial ports.
// Throws a CString on failure, describing the error that
// occurred. If bIgnoreBusyPorts is TRUE, ports that can't
// be opened for read/write access are not included.

void EnumSerialPorts(vector<SSerInfo> &asi, BOOL bIgnoreBusyPorts)
{
	// Clear the output array
	asi.clear();

	// Use different techniques to enumerate the available serial
	// ports, depending on the OS we're using
	OSVERSIONINFO vi;
	vi.dwOSVersionInfoSize = sizeof(vi);
	if (!::GetVersionEx(&vi)) 
	{
		_DEBUG_PRINTF("Could not get OS version. (err=%lx)",
			GetLastError());
	}
	{
		// Win2k and later support a standard API for
		// enumerating hardware devices.
		EnumPortsWdm(asi);
	}

	for (size_t ii=0; ii<asi.size(); ii++)
	{
		SSerInfo* pRsi = &asi[ii];    
		if (bIgnoreBusyPorts) {
			// Only display ports that can be opened for read/write
			HANDLE hCom = CreateFile(pRsi->strDevPath.c_str(),
				GENERIC_READ | GENERIC_WRITE,
				0,    /* comm devices must be opened w/exclusive-access */
				NULL, /* no security attrs */
				OPEN_EXISTING, /* comm devices must use OPEN_EXISTING */
				0,    /* not overlapped I/O */
				NULL  /* hTemplate must be NULL for comm devices */
				);
			if (hCom == INVALID_HANDLE_VALUE) {
				// It can't be opened; remove it.
				std::vector<SSerInfo>::iterator it = asi.begin()+ii;
				asi.erase(it);
				ii--;               
				continue;
			}
			else {
				// It can be opened! Close it and add it to the list
				::CloseHandle(hCom);
			}
		}

		// If there is no description, try to make one up from
		// the friendly name.
		if (!pRsi->strFriendlyName.empty())
		{
			// If the port name is of the form "ACME Port (COM3)"
			// then strip off the " (COM3)"

			int startdex = pRsi->strFriendlyName.find("(");
			int enddex = pRsi->strFriendlyName.find(")");
			if (startdex > 0 && enddex == 
				(pRsi->strFriendlyName.length()-1))
			{
				pRsi->strPortDesc = pRsi->strFriendlyName.substr(0,startdex);
				pRsi->i8PortName = atoi(pRsi->strFriendlyName.substr(startdex+1+3,enddex-startdex-1-3).c_str());
			}
		}
	}
}

// Helpers for EnumSerialPorts
void EnumPortsWdm(vector<SSerInfo> &asi)
{
	string strErr;
	// Create a device information set that will be the container for 
	// the device interfaces.
	GUID *guidDev = (GUID*) &GUID_CLASS_COMPORT;

	HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
	SP_DEVICE_INTERFACE_DETAIL_DATA *pDetData = NULL;

	try {
		hDevInfo = SetupDiGetClassDevs( guidDev,
			NULL,
			NULL,
			DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
			);

		if(hDevInfo == INVALID_HANDLE_VALUE) 
		{
			_DEBUG_PRINTF("SetupDiGetClassDevs failed. (err=%lx)",
				GetLastError());
		}

		// Enumerate the serial ports
		BOOL bOk = TRUE;
		SP_DEVICE_INTERFACE_DATA ifcData;
		DWORD dwDetDataSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + 256;
		pDetData = (SP_DEVICE_INTERFACE_DETAIL_DATA*) new char[dwDetDataSize];
		// This is required, according to the documentation. Yes,
		// it's weird.
		ifcData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		pDetData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		for (DWORD ii=0; bOk && ii<256; ii++) {
			bOk = SetupDiEnumDeviceInterfaces(hDevInfo,
				NULL, guidDev, ii, &ifcData);
			if (bOk) {
				// Got a device. Get the details.
				SP_DEVINFO_DATA devdata = {sizeof(SP_DEVINFO_DATA)};
				bOk = SetupDiGetDeviceInterfaceDetail(hDevInfo,
					&ifcData, pDetData, dwDetDataSize, NULL, &devdata);
				if (bOk) {
					string strDevPath(pDetData->DevicePath);
					// Got a path to the device. Try to get some more info.
					TCHAR fname[256];
					BOOL bSuccess = SetupDiGetDeviceRegistryProperty(
						hDevInfo, &devdata, SPDRP_FRIENDLYNAME, NULL,
						(PBYTE)fname, sizeof(fname), NULL);

					BOOL bUsbDevice = FALSE;
					TCHAR locinfo[256];
					if (SetupDiGetDeviceRegistryProperty(
						hDevInfo, &devdata, SPDRP_LOCATION_INFORMATION, NULL,
						(PBYTE)locinfo, sizeof(locinfo), NULL))
					{
						// Just check the first three characters to determine
						// if the port is connected to the USB bus. This isn't
						// an infallible method; it would be better to use the
						// BUS GUID. Currently, Windows doesn't let you query
						// that though (SPDRP_BUSTYPEGUID seems to exist in
						// documentation only).
						bUsbDevice = (strncmp(locinfo, "USB", 3)==0);
					}
					if (bSuccess) {
						// Add an entry to the array
						SSerInfo si;
						si.strDevPath = strDevPath;
						si.strFriendlyName = fname;
						si.bUsbDevice = bUsbDevice;
						asi.push_back(si);
					}

				}
				else {
					_DEBUG_PRINTF("SetupDiGetDeviceInterfaceDetail failed. (err=%lx)",
						GetLastError());
				}
			}
			else {
				DWORD err = GetLastError();
				if (err != ERROR_NO_MORE_ITEMS) {
					_DEBUG_PRINTF("SetupDiEnumDeviceInterfaces failed. (err=%lx)", err);
				}
			}
		}
	}
	catch (string strCatchErr) {
		strErr = strCatchErr;
	}

	if (pDetData != NULL)
		delete [] (char*)pDetData;
	if (hDevInfo != INVALID_HANDLE_VALUE)
		SetupDiDestroyDeviceInfoList(hDevInfo);

	if (!strErr.empty())
		throw strErr;
}
