#ifndef _DATA_FORMAT
#define _DATA_FORMAT

#include <string.h>
#include "CRC16.h"

typedef struct
{
	BYTE Head[2];
	BYTE Cmd;
	BYTE Resv;
	BYTE PayloadLength[2];
} sDataHead;

typedef enum
{
	eDataType_Post = 0,
	eDataType_Select = 1,
	eDataType_Update = 2
} eDataType;

class CDataFormat
{
public: 
	sDataHead DataHead;
	BYTE HeadLength;
	short PayloadLength;
	BYTE DataPayload[4096];
	BYTE CRC16[2];
	
	CDataFormat()
	{
		DataReset();
	};

	void DataReset()
	{
		memset((BYTE*) &DataHead, 0, sizeof(sDataHead));
		memset((BYTE*) DataPayload, 0, 256);
		PayloadLength=0;
		CRC16[0] = CRC16[1] = 0;
	};

	unsigned int DataPackage(BYTE* pBuffer)
	{
		memcpy(pBuffer, (BYTE*) &DataHead, HeadLength);
		memcpy(pBuffer+HeadLength, (BYTE*) DataPayload, (size_t)PayloadLength);

		short tmp=funcCalcCrc16(pBuffer+2, HeadLength+PayloadLength-2);
		CRC16[0] = tmp&0xFF;
		CRC16[1] = tmp>>8;
		*(pBuffer+HeadLength+PayloadLength)=CRC16[0];
		*(pBuffer+HeadLength+PayloadLength+1)=CRC16[1];

		DataTransform(pBuffer, HeadLength+PayloadLength+2);

		return HeadLength+PayloadLength+2;
	};
	
	BYTE DataParse(BYTE* pBuffer, short length)
	{
		DataTransform(pBuffer, length);

		if(*pBuffer!=0x55 || *(pBuffer+1)!=0xAA)
			return -1;

		sDataHead head;
		memcpy((BYTE*) &head, pBuffer, sizeof(sDataHead));

		PayloadLength=(short)(head.PayloadLength[1]*0x0100+head.PayloadLength[0]);

		if(head.Cmd<0x80 || head.Cmd>0x8F || PayloadLength<0 || PayloadLength>16)
			return -1;

		memcpy((BYTE*) DataPayload, pBuffer+HeadLength, (size_t)PayloadLength);
		short crc=*(pBuffer+HeadLength+PayloadLength+1)*0x0100+*(pBuffer+HeadLength+PayloadLength);
		short crcSelf=funcCalcCrc16(pBuffer+2, HeadLength+PayloadLength-2);

		if(crc!=crcSelf)
			return -1;

		return *(pBuffer+HeadLength);
	};

	void DataHeaderInit(eDataType cmd, short payloadLength)
	{
		DataHead.Head[0]=0x55;
		DataHead.Head[1]=0xAA;
		DataHead.Cmd=(BYTE)cmd;
		DataHead.PayloadLength[0]=payloadLength%0x0100;
		DataHead.PayloadLength[1]=payloadLength/0x0100;
		
		HeadLength = sizeof(sDataHead);
		PayloadLength = payloadLength;
	};

	int DataTotalLength()
	{
		return HeadLength+PayloadLength+2;
	}

	void DataTransform(BYTE* pBuffer, short length)
	{
		for(int i=0;i<length;i++)
		{
			*(pBuffer+i)=(*(pBuffer+i))^0x6E;
		}
	}
};

#endif