#include <string>
#include <vector>
using namespace std;

// Struct used when enumerating the available serial ports
// Holds information about an individual serial port.
struct SSerInfo {
    SSerInfo() : bUsbDevice(FALSE) {}
    string strDevPath;          // Device path for use with CreateFile()
    BYTE i8PortName;         // Simple name (i.e. COM1)
    string strFriendlyName;     // Full name to be displayed to a user
    BOOL bUsbDevice;             // Provided through a USB connection?
    string strPortDesc;         // friendly name without the COMx
};

// Routine for enumerating the available serial ports. Throws a CString on
// failure, describing the error that occurred. If bIgnoreBusyPorts is TRUE,
// ports that can't be opened for read/write access are not included.
void EnumSerialPorts(vector<SSerInfo> &asi, BOOL bIgnoreBusyPorts=TRUE);
