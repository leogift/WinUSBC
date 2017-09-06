#ifndef _LeoSerialPort_H
#define _LeoSerialPort_H

#include <Windows.h>
#include <string.h>

#define  uint8_t unsigned char
#define  uint16_t unsigned short

    class CLeoSerialPort
    {
    public:
        CLeoSerialPort();
        ~CLeoSerialPort();

        BOOL Open ( int nPort = 8, int nBaud = 9600 );//打开串口，默认Com2&9600
        BOOL Close ( void );//关闭串口

        int ReadData ( void *, int );//读回数据
		int WriteData ( const char *, int );//读回数据

        int ReadDataWaiting ( void );	// 查询缓冲区内是否有未读取的数据，查询标志位

        BOOL IsOpened ( void )//测试Com口
		{
            return ( m_bOpened );
        }

    protected:
        HANDLE m_hIDComDev;
        OVERLAPPED m_OverlappedRead, m_OverlappedWrite;
        BOOL m_bOpened;
    };
#endif
