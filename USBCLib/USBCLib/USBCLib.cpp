#define DLL_IMPLEMENT

#include "USBCLib.h"

#include "LeoUrlParser.h"	//Url解析器
#include "EnumSerial.h"		//枚举所有串口
#include "DataFormat.h"		//串口交互数据格式
#include "LeoSerialPort.h"	//串口操作
#include "cJSON.h"			//JSON

#include <shlwapi.h>		//为了使用PathFileExists
#pragma comment(lib, "Shlwapi") 

#define BAUDRATE 9600

#define RECVBUFFER 256
#define SENDBUFFER 4096

#define MAX_DEVICE_NUM 7
#define MAX_PORT_NUM 192

#define TIME_DELAY 1000

BYTE SendBuffer[SENDBUFFER]={0};
BYTE RecvBuffer[RECVBUFFER]={0};

char gFileExe[MAX_PATH]={0};	//	EXE路径
char gFilePath[MAX_PATH]={0};	//	文件夹路径
char gFileCfgJson[MAX_PATH]={0};//	JSON文件路径

////////	----JSON----START----	 ////////

////////  生成JSON文件
cJSON* Create_Json(const char* path, FILE* &fp)
{
	fopen_s(&fp, path, "wb");
    return cJSON_CreateArray();
}
void Insert_Json(cJSON* json, char* com, char* devid)
{
	cJSON *item;

    cJSON_AddItemToArray(json,item=cJSON_CreateObject());
    cJSON_AddStringToObject(item, "COMNO", com);
    cJSON_AddStringToObject(item, "DEVID", devid);
}

void Close_Json(cJSON* json, FILE* &fp)
{
    char* out=cJSON_Print(json);
    fprintf(fp, out);
    fclose(fp);

    cJSON_Delete(json);
    free(out);
	json=NULL;
}
////////  从文件获取JSON结构
cJSON* GetJsonObject(char* fileName)
{
    long len;
    char* pContent;
    int tmp;
    FILE* fp = NULL;
	fopen_s(&fp, fileName, "rb");
    if(!fp)
    {
        return NULL;
    }
    fseek(fp,0,SEEK_END);
    len=ftell(fp);
    if(0 == len)
    {
        return NULL;
    }
   
    fseek(fp,0,SEEK_SET);
    pContent = (char*) malloc (sizeof(char)*len);
    tmp = fread(pContent,1,len,fp);

    fclose(fp);
	cJSON* json=cJSON_Parse(pContent);
	//
    if (!json)
    {
        return NULL;
    }
    free(pContent);
    return json;
}
////////  从JSON结构获取信息
BOOL GetValueString(cJSON* json,int id, char* name, char* param)
{
    cJSON* node;
    node = cJSON_GetArrayItem(json,id);
    if(!node)
    {
        return FALSE;
    }
    sprintf(param, "%s", cJSON_GetObjectItem(node, name)->valuestring);
    return TRUE;
}
////////	----JSON----END----	 ////////

////////  获取DLL路径和本地文件夹路径
void GetDllPath()
{
	HMODULE module = GetModuleHandle(0);  
	GetModuleFileName(module, gFileExe, MAX_PATH); 
	string csFullPath=gFileExe; 
	int nPos = csFullPath.find_last_of("\\"); 
	strcpy_s(gFilePath,csFullPath.substr(0, nPos).c_str());
	strcpy_s(gFileCfgJson,gFilePath);
	strcat(gFileCfgJson,"\\config.json");
}

////////  获取DLL版本
extern "C" DLL_USBC int GetUSBCLibVersion()
{
	printf("VER: %d.%d\r\n",LIB_VERSION_H, LIB_VERSION_L);
	return LIB_VERSION_H*100+LIB_VERSION_L;
}

extern "C" DLL_USBC int GetHardwareVersion()
{
	printf("HARDWARE: %d.%d\r\n",HARDWARE_VERSION_H, HARDWARE_VERSION_L);
	return HARDWARE_VERSION_H*100+HARDWARE_VERSION_L;
}

extern "C" DLL_USBC int GetUSBCDevList(BYTE* pComNO, BYTE* pDevID, BYTE* pDevSize)
{
	GetDllPath();

	if(!PathFileExists(gFileCfgJson))
	{
		printf("ERROR: 未找到已联机USB集群设备或未自检，请连接设备后重新自检！\r\n");
		return ERROR_WITHOUTPOST;
	}

	cJSON *json , *jsonArray, *jsonCOMNO, *jsonDEVID;  

    // 解析数据包  
	json=GetJsonObject(gFileCfgJson);
    if (NULL!=json)  
    {  
		for(int cnt=0;cnt<cJSON_GetArraySize(json);cnt++)
		{
			// 解析开关值  
			jsonArray = cJSON_GetArrayItem( json , cnt);  
			if( jsonArray != NULL )  
			{ 
				jsonDEVID=cJSON_GetObjectItem(jsonArray,"DEVID");
				jsonCOMNO=cJSON_GetObjectItem(jsonArray,"COMNO");
				if(jsonDEVID != NULL && jsonCOMNO != NULL)
				{
					*pDevID=atoi(jsonDEVID->valuestring);
					*pComNO=atoi(jsonCOMNO->valuestring);
					pDevID++;
					pComNO++;
					(*pDevSize)=(*pDevSize)+1;
				}
			}
		}
        
        // 释放内存空间  
        cJSON_Delete(json);  
    }
	return 0;
}

extern "C" DLL_USBC int RunUSBCLibPost()
{
	vector<SSerInfo> sSIList;				//空列表用于保存所有串口信息
	unsigned char ucSICnt=0,ucSITotal=0;	//串口列表序号
	BOOL bFoundFlag=FALSE;//是否找到设备的标志位

	GetDllPath();

	EnumSerialPorts(sSIList,FALSE/*include all*/);

	//判断config.json是否存在，若已存在删之更新
	if(PathFileExists(gFileCfgJson))
		DeleteFile(gFileCfgJson);

	ucSITotal=sSIList.size();
	if(ucSITotal>0)
	{
		CDataFormat* pData = new CDataFormat();
		CLeoSerialPort* pSerialPort = new CLeoSerialPort();

		FILE* fp = NULL;
		cJSON *json = Create_Json(gFileCfgJson, fp);

		for(ucSICnt=0;ucSICnt<ucSITotal;ucSICnt++)
		{
			_DEBUG_PRINTF("INFO: Post COM%d\r\n", sSIList[ucSICnt].i8PortName);
			printf("INFO: Post COM%d\r\n", sSIList[ucSICnt].i8PortName);

			pData->DataReset();
			pData->DataHeaderInit(eDataType_Post, 0x0000);
			pData->DataPackage(SendBuffer);

			BOOL res = pSerialPort->Open(sSIList[ucSICnt].i8PortName, BAUDRATE);
			if(!res)
			{
				continue;
			}
			else
			{
				pSerialPort->WriteData((char*)SendBuffer,pData->DataTotalLength());
				Sleep(TIME_DELAY);
				short recvLen = pSerialPort->ReadData((char*)RecvBuffer,RECVBUFFER);
				if(recvLen>=8)
				{
					BYTE res=pData->DataParse(RecvBuffer, recvLen);
					if(res==0x00 && *(pData->DataPayload)==0)
					{
						_DEBUG_PRINTF("INFO: USBC Found, COM%d\r\n",sSIList[ucSICnt].i8PortName);
						
						BYTE ucComNO=sSIList[ucSICnt].i8PortName;

						char strCOM[16]={0};
						_itoa_s( sSIList[ucSICnt].i8PortName, strCOM, 10);

						char strDevID[16]={0};
						BYTE ucDevID=*(pData->DataPayload+1);
						_itoa_s( ucDevID, strDevID, 10);
						
						bFoundFlag=TRUE;

						Insert_Json(json,strCOM,strDevID);

					}
				}
				pSerialPort->Close();
			}
		}
		    
		Close_Json(json, fp);

		delete pSerialPort;
		delete pData;
	}
	else
	{
		printf("ERROR: 未找到已联机USB集群设备，请连接设备后重新自检！\r\n");
		return ERROR_NOUSBCDEVICE;
	}

	if(bFoundFlag)
		return 0;
	else
	{
		printf("ERROR: 未找到已联机USB集群设备，请连接设备后重新自检！\r\n");
		return ERROR_NOUSBCDEVICE;
	}
}

extern "C" DLL_USBC int RunUSBCLibSelect(BYTE ucDevID, BYTE ucPortID)
{
	GetDllPath();

	if(ucDevID>MAX_DEVICE_NUM)
	{
		printf("ERROR: USB集群设备ID超出范围，请重新输入设备ID！\r\n");
		return ERROR_OUTOFRANGE_DEVICEID;
	}

	if(ucPortID>MAX_PORT_NUM || ucPortID<=0 )
	{
		printf("ERROR: USB端口号超出范围，请重新输入USB端口号！\r\n");
		return ERROR_OUTOFRANGE_PORTID;
	}

	if(!PathFileExists(gFileCfgJson))
	{
		printf("ERROR: 未找到已联机USB集群设备或未自检，请连接设备后重新自检！\r\n");
		return ERROR_WITHOUTPOST;
	}

	cJSON *json , *jsonArray, *jsonCOMNO, *jsonDEVID;  
	BOOL bFoundFlag=FALSE;
	BYTE ucComNO=0;
    // 解析数据包  
	json=GetJsonObject(gFileCfgJson);
    if (NULL!=json)  
    {  
		for(int cnt=0;cnt<cJSON_GetArraySize(json);cnt++)
		{
			// 解析开关值  
			jsonArray = cJSON_GetArrayItem( json , cnt);  
			if( jsonArray != NULL )  
			{ 
				jsonDEVID=cJSON_GetObjectItem(jsonArray,"DEVID");
				if(jsonDEVID != NULL)
				{
					if(ucDevID!=atoi(jsonDEVID->valuestring))
						continue;
					else
						bFoundFlag=TRUE;
				}

				jsonCOMNO=cJSON_GetObjectItem(jsonArray,"COMNO");
				if(jsonCOMNO != NULL)
				{
					_DEBUG_PRINTF("INFO: Found DEV%s in COM%s\r\n",jsonDEVID->valuestring,jsonCOMNO->valuestring);
					ucComNO=atoi(jsonCOMNO->valuestring);
				}
			}
		}
        
        // 释放内存空间  
        cJSON_Delete(json);  
    }  

	if(bFoundFlag==FALSE)
	{
		printf("ERROR: 未找到指定ID的USB集群设备，请设备自检后重新测试！\r\n");
		return ERROR_SELECT_DEVICEID;
	}
	
	CDataFormat* pData = new CDataFormat();
	CLeoSerialPort* pSerialPort = new CLeoSerialPort();

	BOOL res=pSerialPort->Open(ucComNO,BAUDRATE);
	if(!res)
	{
		printf("ERROR: 指定的COM%d设备无法打开\r\n", ucComNO);

		delete pSerialPort;
		delete pData;

		return ERROR_SELECT_DEVICEID;
	}

	pData->DataReset();
	pData->DataHeaderInit(eDataType_Select, 0x0001);
	pData->DataPayload[0]=ucPortID;
	pData->DataPackage(SendBuffer);

	pSerialPort->WriteData((char*)SendBuffer,pData->DataTotalLength());
	Sleep(TIME_DELAY);
	short recvLen=pSerialPort->ReadData((char*)RecvBuffer,RECVBUFFER);

	printf("Recv %d Bytes:", recvLen);
	for(int idx=0;idx<recvLen;idx++)
	{
		printf("%x ", RecvBuffer[idx]);
	}
	printf("\r\n");

	if(recvLen<8)
	{
		_DEBUG_PRINTF("ERROR: Select ReadData\r\n");
		bFoundFlag=FALSE;
	}
	else
	{
		res=pData->DataParse(RecvBuffer, recvLen);
		if(res==0x00 && *(pData->DataPayload)==0)
		{
			bFoundFlag=TRUE;
		}
	}
	pSerialPort->Close();

	delete pSerialPort;
	delete pData;

	if(bFoundFlag)
		return 0;
	else
	{
		return ERROR_GENERIC;
	}
}