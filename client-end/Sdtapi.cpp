// Sdtapi.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

//#ifdef SDTAPI_EXPORTS
#include "Sdtapi.h"
#include <time.h>
//#endif
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <objbase.h>
#include <initguid.h>
#include <Setupapi.h>
//#include "wltrs.h"
#include "dewlt.h"
#include "mydes.h"
#include <sys/stat.h>
#include "io.h"
#include "jpg.h"
#include "SavePhoto.h"
#define USB_HANDLE_PIPE2	0x222020
#define USB_HANDLE_PIPE6	0x222024
#define USB_HANDLE_PIPE81	0x222028
#define USB_HANDLE_PIPE85	0x22202C
#define MAX_USB_PORT		0x0f
#define debug				0
#define _AFX_SECURE_NO_DEPRECATE
#define _ATL_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

DEFINE_GUID(USB_DRIVER_GUID, 0xff646f81, 0x8def, 0x11d2, 0x94, 0x49, 0x0, 0x10, 0x5a, 0x07, 0x5f, 0x6b);
char inPipe[32] = "PIPE00";     // pipe name for bulk input pipe on our test board
char outPipe[32] = "PIPE01";    // pipe name for bulk output pipe on our test board
DEFINE_GUID(GUID_CLASS_I82930_BULK, 
0x873fdf, 0x61a8, 0x11d1, 0xaa, 0x5e, 0x0, 0xc0, 0x4f, 0xb1, 0x72, 0x8b); //iDR310

unsigned int g_iBaudRate[] = {115200, 57600, 38400, 19200, 9600};
unsigned int g_iCurBaudRate = 0;
unsigned long g_SN = 0;
//int STDCALL SDT_GetSAMStatus(int iPortID,int iIfOpen);
typedef struct
{
        //char DevName[MAX_PATH];
	HANDLE hRead;
	HANDLE hWrite;
	DWORD  IOType; //type =1是串口，2为安全模块USB，3为iDR310的USB 
}IOPORT;
static IOPORT hPort = { 0, 0, 0 };

IOPORT ghPort[16]={	{ 0, 0, 0 },
					{ 0, 0, 0 },
					{ 0, 0, 0 },
					{ 0, 0, 0 },
					{ 0, 0, 0 },
					{ 0, 0, 0 },
					{ 0, 0, 0 },
					{ 0, 0, 0 },
					{ 0, 0, 0 },
					{ 0, 0, 0 },
					{ 0, 0, 0 },
					{ 0, 0, 0 },
					{ 0, 0, 0 },
					{ 0, 0, 0 },
					{ 0, 0, 0 },
					{ 0, 0, 0 }
					};

typedef struct _ChkList ChkList;
struct _ChkList
{
      char buff[40];
};
typedef struct _ChkItem ChkItem;
struct _ChkItem
{
        char head[15];
        char start[11];
        char end[11];
};
static ChkList *chklist = NULL;
static int chklen = 0;
#define filecheck "check.dat"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
//TCHAR str[256]={0};
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
//            GetModuleFileName(hModule, str, 256);   // hinstDLL就是本DLL的句柄
//			printf("hdll=%s\r\n",str);
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

void enchk(char *data, int len, char *buff, int size, int flag)
{
	unsigned char str[8];
	unsigned char *p = (unsigned char *)data;
	unsigned char *pb = (unsigned char *)buff;

	deskey((unsigned char *)"routon.com", flag);
	while( (len > 0) && (size > 0) )
	{
		memset(str, 0, 8);
		memcpy(str, p, len<8?len:8);
		des(str, pb);
		p += 8;
		pb += 8;
		len -= 8;
		size -= 8;
	}
}

int getCheckFile(char *fName)
{
	int fd;
	struct stat st;
	int i;
	ChkList *plst;

	fd = open(fName, 0);
	if( -1 == fd ) return 0x23;
	if( fstat(fd, &st) ) return 0x23;
	if( st.st_size < 40 ) return 0x23;

	chklen = st.st_size / 40;
	if( chklist ) free(chklist);
	chklist = (ChkList *)malloc(st.st_size);
	if(!chklist) return 0x23;
	read(fd, chklist, st.st_size);

	plst = chklist;
	for( i=0; i<chklen; i++,plst++ )
	{
		enchk(plst->buff, 40, plst->buff, 40, 1);
		plst->buff[14] = '\0';
		plst->buff[25] = '\0';
		plst->buff[36] = '\0';
	}
    close(fd);
	return 0;
}

int checkItem(ChkItem *item, char *samID)
{
	long start, end, current;

	if( !strncmp(item->head, samID, 14) )
	{
		start = atol(item->start);
		end = atol(item->end);
		sscanf(samID, "%02*d.%02*d-%08*d-%010d-%010*d\n", &current);
		if( (start <= current) && (current <= end) ) return 1;
	}
	return 0;
}

int checkSAMID(int iPort)
{
	int ret, i;
	char pcSAMID[64] = {0};

	ret = SDT_GetSAMIDToStr(iPort,pcSAMID,0);
	if( 0x90 == ret )
	{
		ret = getCheckFile(filecheck);
		if( !ret )
		{
			ChkList *plst = chklist;
			for( i=0; i<chklen; i++,plst++ )
			{
				ChkItem *item = (ChkItem *)plst;
				if( checkItem(item, pcSAMID) ) return 0x90;
			}
		}
	}

	return ret;
}

void ParseSerial(char *OutData, char *InData)
{
	unsigned short s1[2];
	unsigned int s2[3];
	int offset = 0;
	s1[0] =  (*(unsigned short*) &InData[offset]);			offset += sizeof (unsigned short);
	s1[1] =  (*(unsigned short*) &InData[offset]);			offset += sizeof (unsigned short);
	s2[0] =  (*(unsigned int*)   &InData[offset]);			offset += sizeof (unsigned int);
	s2[1] =  (*(unsigned int*)   &InData[offset]);			offset += sizeof (unsigned int);
	s2[2] =  (*(unsigned int*)   &InData[offset]);			offset += sizeof (unsigned int);
	sprintf (OutData, "%.2d.%.2d-%.8u-%.10u-%.10u", s1[0], s1[1], s2[0], s2[1], s2[2]);
}

unsigned short reverse_short (unsigned short iData)
{
	unsigned short iRet=0;
	unsigned char temp1[2];
	unsigned char temp2[2];
	memcpy (temp1, &iData, 2);
	temp2[0] = temp1[1];
	temp2[1] = temp1[0];
	memcpy (&iRet, temp2, 2);

	return iRet;
}

HANDLE InitCom (unsigned int iPortID, unsigned int iBaudRate)
{
	HANDLE hCom;
	
	char sPort [16];
	sprintf (sPort, "%s%d", "COM", iPortID);
	//打开串口
	hCom = CreateFile(sPort,GENERIC_READ|GENERIC_WRITE,0,NULL,
		        OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		        
	if(hCom == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	
	//设置超时
    COMMTIMEOUTS CommTimeOuts;
    CommTimeOuts.ReadIntervalTimeout = 1000;
    CommTimeOuts.ReadTotalTimeoutMultiplier = 2;
    CommTimeOuts.ReadTotalTimeoutConstant = 1000;
    CommTimeOuts.WriteTotalTimeoutMultiplier = 2;
    CommTimeOuts.WriteTotalTimeoutConstant = 1000;
    if (!SetCommTimeouts(hCom, &CommTimeOuts))
    {
    	CloseHandle(hCom);
        return 0;
    }
    
    //填充设备控制块
    DCB dcb;
    dcb.DCBlength = sizeof( DCB );

    if (!GetCommState(hCom, &dcb ))
    {
    	CloseHandle(hCom);
        return 0;
    }
    
    dcb.BaudRate = iBaudRate;
#if 0
	char mssg[256]={0};
	sprintf(mssg,"dcb.BaudRate=%d",dcb.BaudRate);
	
		 MessageBox(NULL,mssg,"debug1",MB_OK);
#endif
    dcb.ByteSize = 8;
    dcb.Parity=NOPARITY;
    dcb.StopBits=ONESTOPBIT;
    if (!SetCommState(hCom, &dcb ))
    {
        CloseHandle(hCom);
        return 0;
    }

	return hCom;
}

//还要改1001-10016
HANDLE InitUsb ( GUID *pGuid, unsigned int iPort)
{
	HANDLE hOut = INVALID_HANDLE_VALUE;
	char sDevNameBuf[MAX_PATH] = "";
	HDEVINFO hDevInfo = SetupDiGetClassDevs(pGuid, NULL, NULL, DIGCF_PRESENT|DIGCF_INTERFACEDEVICE);

//	ULONG nGuessCount = MAX_USB_PORT;
	for(unsigned int iDevIndex=0; iDevIndex<=iPort; iDevIndex++)
	{
		SP_INTERFACE_DEVICE_DATA deviceInfoData;
		deviceInfoData.cbSize = sizeof (SP_INTERFACE_DEVICE_DATA);
		if( (SetupDiEnumDeviceInterfaces(hDevInfo, 0, pGuid, iDevIndex, &deviceInfoData) ) 
				&& (iDevIndex==iPort))
		{
			ULONG  iReqLen = 0;
			SetupDiGetInterfaceDeviceDetail(hDevInfo, &deviceInfoData, NULL, 0, &iReqLen, NULL);

			ULONG iDevDataLen = iReqLen; //sizeof(SP_FNCLASS_DEVICE_DATA) + 512;
			PSP_INTERFACE_DEVICE_DETAIL_DATA pDevData = (PSP_INTERFACE_DEVICE_DETAIL_DATA)malloc(iDevDataLen);

			pDevData->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
			if(SetupDiGetInterfaceDeviceDetail(hDevInfo, &deviceInfoData, pDevData, iDevDataLen, &iReqLen, NULL))
			{
				strcpy(sDevNameBuf, pDevData->DevicePath);
				hOut = CreateFile(pDevData->DevicePath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
				
			}
			free(pDevData);
		}
		else if(GetLastError() == ERROR_NO_MORE_ITEMS) //No more items
		{
			break;
		}
	}
	SetupDiDestroyDeviceInfoList(hDevInfo);

	return hOut;
}

int IOWriteFrm ( IOPORT hIO, unsigned char * data, WORD len )
{
	DWORD retLen = 0;
	HANDLE hWrite = hIO.hWrite;
	DWORD type = hIO.IOType;
	unsigned char sndbuf[16]; // 
    //填充包头
    sndbuf[0] = 0xAA; sndbuf[1] = 0xAA; sndbuf[2] = 0xAA;
    sndbuf[3] = 0x96; sndbuf[4] = 0x69;
    //填充数据长度
    sndbuf[5] = ((unsigned char *)&len)[1];
    sndbuf[6] = ((unsigned char *)&len)[0];
    //填充数据
    memcpy ( &sndbuf[7], data, len );
    //send
	switch ( type )
	{
		case 1: //串口或iDR310的USB
		case 3:
		{
			if (!WriteFile( hWrite, sndbuf, len+7, &retLen, NULL))
			{
				return 0;
			}
			retLen -= 7;
			break;
		}
		case 2: //安全模块USB
		{
			DWORD nRet = 0;
			BOOL bSuccess = DeviceIoControl( hWrite, 0x222020, 
								sndbuf, 7, NULL, 0, &nRet, NULL);
			if ( ( !bSuccess ) )
			{
				return 0;
			}
			bSuccess = DeviceIoControl( hWrite, 0x222024, 
								&sndbuf[7], len, NULL, 0, &retLen, NULL);
			if ( !bSuccess )
			{
				return 0;
			}
			retLen = len;
			break;

		}
		default:
			break;
	}
	return retLen;		
}

int IOReadFrm ( IOPORT hIO, unsigned char * data, DWORD *len )
{
	WORD DataLen = 0;
	DWORD retLen;
	BOOL bSuccess;
	HANDLE hRead = hIO.hRead;
	DWORD type = hIO.IOType;
	unsigned char head[7]; //
	int i,j=1;

	switch ( type )
	{
		case 1: //串口或iDR310的USB
		{
			DWORD start = GetTickCount();
			DWORD end = start,end2;
			DWORD error;
			COMSTAT Stat;
			while ( 1 )
			{

				SetupComm(hRead,2048,2048); 
#if 0
	char mssg1[256]={0};
	sprintf(mssg1,"j=%d",j);
	
		 MessageBox(NULL,mssg1,"debug2",MB_OK);
		 j++;
#endif

				bSuccess = ClearCommError ( hRead, &error, &Stat );
				if ( error )
                {
                         if ( Stat.cbInQue )
                          {
                                  PurgeComm ( hRead, PURGE_RXCLEAR  );
                          }
                          return -1; 
                }
				if ( Stat.cbInQue >= 7 && !DataLen )
				{
					bSuccess = ReadFile( hRead, head, 7, &retLen, NULL);
					if ( (!bSuccess) || ( 7 != retLen ) )
					{
						return -1;
					}

					//头是否错
					if ((head[0]!=0xAA) || (head[1]!=0xAA)
							|| (head[2]!=0xAA) || (head[3]!=0x96)
							|| (head[4]!=0x69) )
					{
						return -3;
					}

					((unsigned char*)&DataLen)[0] = head[6];
					((unsigned char*)&DataLen)[1] = head[5];
				}
#if 0
	char mssg2[256]={0};
	sprintf(mssg2,"retlen=%d,DataLen=%d,Stat.cbInQue=%d",retLen,DataLen,Stat.cbInQue);
	
		 MessageBox(NULL,mssg2,"debug!!!",MB_OK);
#endif
//				Sleep(200);

				if ( DataLen && Stat.cbInQue >= DataLen )
				{
					bSuccess = ReadFile( hRead, data, DataLen, &retLen, NULL);
					if ( (!bSuccess) || ( DataLen != retLen ) )
					{
						return -4;
					}

#if 1
	char mssg[256]={0};
	sprintf(mssg,"read com ok!retLen=%d,DataLen=%d,Stat.cbInQue=%d",retLen,DataLen,Stat.cbInQue);
	
		 MessageBox(NULL,mssg,"debug333",MB_OK);
#endif

					break;
				}
				if ( DataLen >= 1288 )
				{

					retLen=0;
					int recvLen=0;

					while(recvLen<DataLen )
					{
						end2 = GetTickCount();
						if ((end2 - start) > 5000)
							return -8;

//						Sleep(100);
						bSuccess=ReadFile( hRead, data+recvLen, DataLen-recvLen, &retLen, NULL);
//						bSuccess=ReadFile( hRead, data+recvLen, 256, &retLen, NULL);
						recvLen+=retLen;
int dwError = GetLastError(); 

#if 1
	char mssg2[256]={0};
	sprintf(mssg2,"bSuccess=%d,retLen=%d,recvLen=%d,DataLen=%d,Stat.cbInQue=%d,dwError=%d",bSuccess,retLen,recvLen,DataLen,Stat.cbInQue,dwError);
	
		 MessageBox(NULL,mssg2,"debug3",MB_OK);
#endif

					}

//					bSuccess = ReadFile( hRead, data, DataLen, &retLen, NULL);
#if 0
	char mssg2[256]={0};
	sprintf(mssg2,"bSuccess=%d,retLen=%d,DataLen=%d,Stat.cbInQue=%d",bSuccess,retLen,DataLen,Stat.cbInQue);
	
		 MessageBox(NULL,mssg2,"debug3",MB_OK);
#endif


//					bSuccess = ReadFile( hRead, data+retLen, DataLen-retLen, &retLen, NULL);
#if 0
	char mssg[256]={0};
	sprintf(mssg,"bSuccess=%d,retLen=%d,DataLen=%d,Stat.cbInQue=%d",bSuccess,retLen,DataLen,Stat.cbInQue);
	
		 MessageBox(NULL,mssg,"debug4",MB_OK);
#endif				
				
					if ( (!bSuccess) || ( DataLen != recvLen ) )
					{
						return -4;
//						continue;
					}

					break;
				}
				//
				Sleep(100);
                end = GetTickCount();
                if ( ( (end - start) > 2000 )  && ( !DataLen ) )
                {
                        return -5; //超时
                }
				else 
				if ( (end - start) > 5000 )
				{
#if 0
	char mssg[256]={0};
	sprintf(mssg,"DataLen=%d,Stat.cbInQue=%d",DataLen,Stat.cbInQue);
	
		 MessageBox(NULL,mssg,"debug 5001",MB_OK);
#endif
					return -6; //
				}
			}

			break;
		}
		case 2: //安全模块USB
		{
			bSuccess = DeviceIoControl( hRead, 0x222028, NULL, 0,
								      head, 7, &retLen, NULL);
 			if ( (!bSuccess) || ( 7 != retLen ) )
			{
				return 3;
			}
			//头是否错
			if ((head[0]!=0xAA) || (head[1]!=0xAA)
					|| (head[2]!=0xAA) || (head[3]!=0x96)
					|| (head[4]!=0x69) )
			{
				return 3;
			}
			((unsigned char*)&DataLen)[0] = head[6];
            ((unsigned char*)&DataLen)[1] = head[5];
			bSuccess = DeviceIoControl( hRead, 0x22202C, NULL, 0,
								data, DataLen,  &retLen, NULL);
			if ( (!bSuccess) || ( DataLen != retLen ) )
			{
				return 3;
			}
			break;
		}
		case 3:
		{
/*
			unsigned char buf[1024*2]; 
			bSuccess = ReadFile( hRead, buf, 1024*2, &retLen, NULL);
			if ( (!bSuccess) || (  retLen <= 7 ) )
			{
			return 3;
			}
			*/
			unsigned char buf[2048*2]; 
#if 1
			retLen=0;
			bSuccess=0;
			int  waittime = 10;
			int readed = 0;
			
			while(readed < 7)
			{
				bSuccess = ReadFile( hRead, &buf[readed], 2048*2, &retLen, NULL);
				readed += retLen;
				Sleep(100);
				if(--waittime<=0)break;
			}
#else
			bSuccess = ReadFile( hRead, buf, 2048*2, &retLen, NULL);
			if ( bSuccess==0  )
			{
				return 2;
			}
#endif
			//头是否错
			if ((buf[0]!=0xAA) || (buf[1]!=0xAA)
					|| (buf[2]!=0xAA) || (buf[3]!=0x96)
					|| (buf[4]!=0x69) )
			{
				return 3;
			}

			((unsigned char*)&DataLen)[0] = buf[6];
            ((unsigned char*)&DataLen)[1] = buf[5];
			memcpy ( head, buf, 7 );
			memcpy ( data, &buf[7], DataLen );
			break;
		}
		default:
			break;
	}
	//校验和是否错
	unsigned char chChkSum = 0;
	for ( i=5; i<7; i++)
	{
		chChkSum  ^= head[i];
	}
	for ( i=0; i<DataLen; i++)
	{
		chChkSum  ^= data[i];
	}
	if (chChkSum)
	{
		return -9;
	}
	*len = DataLen; 
	return 0;	
}

int STDCALL SDT_GetCOMBaud(int iComID,unsigned int *puiBaud)
{
	//填充设备控制块
	DCB dcb;
	dcb.DCBlength = sizeof( DCB );
	int bOk = 0;
	if ( (iComID>16) || (iComID<1))
		return 1;
	if ( !hPort.hRead )
	{
		HANDLE hCom;
		char sPort [16];
		sprintf (sPort, "%s%d", "COM", iComID);
		//打开串口
		hCom = CreateFile(sPort,GENERIC_READ|GENERIC_WRITE,0,NULL,
					OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
					
		if(hCom == INVALID_HANDLE_VALUE)
		{
			return 1;
		}
		
		//设置超时
		COMMTIMEOUTS CommTimeOuts;
		CommTimeOuts.ReadIntervalTimeout = 1000;
		CommTimeOuts.ReadTotalTimeoutMultiplier = 2;
		CommTimeOuts.ReadTotalTimeoutConstant = 1000;
		CommTimeOuts.WriteTotalTimeoutMultiplier = 2;
		CommTimeOuts.WriteTotalTimeoutConstant = 1000;
		if (!SetCommTimeouts(hCom, &CommTimeOuts))
		{
    		CloseHandle(hCom);
			return 5;
		}

		if (!GetCommState(hCom, &dcb ))
		{
    		CloseHandle(hCom);
			return 5;
		}
		for (int i=0; i<5; i++)
		{
			dcb.BaudRate = g_iBaudRate[i];
			dcb.ByteSize = 8;
			dcb.Parity=NOPARITY;
			dcb.StopBits=ONESTOPBIT;
			if (!SetCommState(hCom, &dcb ))
			{
				CloseHandle(hCom);
				return 5;
			}
			hPort.hRead = hPort.hWrite = hCom;
			hPort.IOType = 1;
			if (SDT_GetSAMStatus (iComID, 0) == 0x90)
			{
				*puiBaud = g_iBaudRate[i];
				bOk = 1;
				break;
			}
		}
		SDT_ClosePort(iComID);
	}
	else
	{
		if (!GetCommState(hPort.hRead, &dcb ))
		{
    		SDT_ClosePort(iComID);
			return 5;
		}
		*puiBaud = dcb.BaudRate;
	}
	if (bOk)
		return 0x90;
	else
		return 0x5;
}

int STDCALL SDT_SetCOMBaud(int iComID,unsigned int  uiCurrBaud,
									   unsigned int  uiSetBaud)
{
	int i;
	if ( (iComID>16) || (iComID<1))
		return 1;
	if ( hPort.hRead )
	{
		SDT_ClosePort(iComID);
	}
	HANDLE hComm = InitCom (iComID, uiCurrBaud);
	if (!hComm)
	{
		return 1;
	}
	else
	{
		unsigned char uchInTestHeader[7];
		unsigned char uchInTestData[4];
		unsigned char uchTestSAM1[7] = {0xAA, 0xAA, 0xAA, 0x96, 0x69, 0x00, 0x03};
		unsigned char uchTestSAM2[3];
		//= {0x11, 0xFF, 0xED};
		DWORD retLen = 0;
		char uchInMsg[10];
		memcpy (uchInMsg, uchTestSAM1, 7);
		for ( i=0; i<5; i++ )
		{
			if (uiSetBaud == g_iBaudRate[i])
				break;
		}
		if (5==i)
		{
			return 0x21;
		}
		uchTestSAM2[0] = 0x60;
		uchTestSAM2[1] = i;
		uchTestSAM2[2] = 0x03^uchTestSAM2[0]^uchTestSAM2[1];

		memcpy (&uchInMsg[7], uchTestSAM2, 3);
		if (!WriteFile(hComm, uchInMsg, 10, &retLen, NULL))
		{
			CloseHandle (hComm);
			return 0x21;
		}
    
		//写串口失败
		if (retLen != 10)
		{
			CloseHandle (hComm);
			return 0x21;
		}
		
		unsigned char uchInAll[11];
		if (!ReadFile(hComm, uchInAll, 11, &retLen, NULL))
		{
    		CloseHandle (hComm);
			return 3;
		}
		if (retLen != 11)
		{
			CloseHandle (hComm);
			return 3;
		}

		memcpy (uchInTestHeader, uchInAll, 7);
		memcpy (uchInTestData, &uchInAll[7], 4);
		
		//头是否错
		if ((uchInTestHeader[0]!=0xAA) || (uchInTestHeader[1]!=0xAA)
				|| (uchInTestHeader[2]!=0xAA) || (uchInTestHeader[3]!=0x96)
				|| (uchInTestHeader[4]!=0x69) || (uchInTestHeader[6]!=0x04))
		{
			CloseHandle (hComm);
			return 3;
		}

		//校验和是否错
		unsigned char chChkSum = 0;
		for (i=5; i<7; i++)
		{
			chChkSum  ^= uchInTestHeader[i];
		}
		for (i=0; i<4; i++)
		{
			chChkSum  ^= uchInTestData[i];
		}
		if (chChkSum)
		{
			CloseHandle (hComm);
			return 3;
		}
		CloseHandle (hComm);
		g_iCurBaudRate = uiSetBaud;
		return uchInTestData[2];
	}

}

int Init310Usb ( HANDLE * phRead, HANDLE * phWrite )
{
	HANDLE hRead = 0;
	HANDLE hWrite = 0;
	bool bUsb = false;
    char sDevNameBuf[MAX_PATH] = {0};
	HDEVINFO hDevInfo = SetupDiGetClassDevs( (LPGUID)&GUID_CLASS_I82930_BULK,
                                              NULL,
                                              NULL,
                                              DIGCF_PRESENT|DIGCF_INTERFACEDEVICE);
            //
    for( unsigned int iDevIndex=0; iDevIndex < 1; iDevIndex++ )
    {
            SP_INTERFACE_DEVICE_DATA deviceInfoData;
            deviceInfoData.cbSize = sizeof (SP_INTERFACE_DEVICE_DATA);
            if( (SetupDiEnumDeviceInterfaces(hDevInfo,
                                            0,
                                            (struct _GUID*)&GUID_CLASS_I82930_BULK,
                                            iDevIndex,
                                            &deviceInfoData) ) )
            {
                    ULONG  iReqLen = 0;
                    SetupDiGetInterfaceDeviceDetail(hDevInfo,
                                                    &deviceInfoData,
                                                    NULL,
                                                    0,
                                                    &iReqLen,
                                                    NULL);

                    ULONG iDevDataLen = iReqLen; //sizeof(SP_FNCLASS_DEVICE_DATA) + 512;
                    PSP_INTERFACE_DEVICE_DETAIL_DATA pDevData =
                                            (PSP_INTERFACE_DEVICE_DETAIL_DATA)malloc(iDevDataLen);

                    pDevData->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
                    if( SetupDiGetInterfaceDeviceDetail ( hDevInfo,
                                                            &deviceInfoData,
                                                            pDevData,
                                                            iDevDataLen,
                                                            &iReqLen,
                                                            NULL))
                    {
                            //
                            strcpy ( sDevNameBuf, pDevData->DevicePath );
                            bUsb = true;
                    }
                    free(pDevData);
            }
            else //if(GetLastError() == ERROR_NO_MORE_ITEMS) //No more items
            {
                    int result = GetLastError();
                    if ( result == ERROR_NO_MORE_ITEMS )
                    break;
            }
    }
    SetupDiDestroyDeviceInfoList(hDevInfo);
    //
    if ( !bUsb )
            return 1;
    //
    char completeDeviceName[MAX_PATH];
    strcpy ( completeDeviceName, sDevNameBuf );
    //open read pipe
    strcat ( completeDeviceName,
                        "\\"
                        );

    strcat (completeDeviceName,
                        inPipe
                        );

    hRead = CreateFile(completeDeviceName,
            GENERIC_WRITE | GENERIC_READ,
            0,/*FILE_SHARE_WRITE | FILE_SHARE_READ,*/
            NULL,
            OPEN_EXISTING,
            0,
            NULL);


    memset ( completeDeviceName, 0, MAX_PATH );
    strcpy ( completeDeviceName, sDevNameBuf );
    //open write pipe
    strcat (completeDeviceName,
            "\\"
            );

    strcat (completeDeviceName,
                                  outPipe
                                  );

    hWrite = CreateFile(completeDeviceName,
                          GENERIC_WRITE | GENERIC_READ,
                          0, /* FILE_SHARE_WRITE | FILE_SHARE_READ,*/
                          NULL,
                          OPEN_EXISTING,
                          0,
                          NULL);

    if ( ( !hRead ) || ( !hWrite ) )
            return  2;

    *phRead = hRead;
    *phWrite = hWrite;
	return 0;
}

int STDCALL SDT_OpenPort ( int iPortID )
{
	SDT_ClosePort ( iPortID );
	if ( (iPortID<=16) && (iPortID>=1) )
	{
		if (0 == g_iCurBaudRate)
		{
			if (0x90 != SDT_GetCOMBaud(iPortID, &g_iCurBaudRate))
				return 1;
		}

#if 1
	char mssg[256]={0};
	sprintf(mssg,"SDT_GetCOMBaud ok!g_iCurBaudRate=%d",g_iCurBaudRate);
	
		 MessageBox(NULL,mssg,"g_iCurBaudRate",MB_OK);
#endif
		
		HANDLE hComm = InitCom (iPortID, g_iCurBaudRate);
		if (hComm)
		{
			hPort.hRead = hPort.hWrite= hComm;
			hPort.IOType = 1;
			return 0x90;
		}
		else
		{
			return 1;
		}
	}
	else if ((iPortID<=1016) && (iPortID>=1001))
	{
		HANDLE hUsb = InitUsb ( (GUID*)&USB_DRIVER_GUID, iPortID-1001);
		if ( hUsb != INVALID_HANDLE_VALUE )
		{
			hPort.hRead = hPort.hWrite= hUsb;
			hPort.IOType = 2;
			return 0x90;
		}
		//iDR310
		HANDLE hRead, hWrite;
		if ( !Init310Usb ( &hRead, &hWrite ) )
		{
			hPort.hRead = hRead;
			hPort.hWrite= hWrite;
			hPort.IOType = 3;
			return 0x90;	
		}
		return 1;
	}
	else
	{
		return 1;
	}
}

int STDCALL SDT_ClosePort (int iPortID)
{
	switch ( hPort.IOType )
	{
		case 1: //串口或iDR310的USB
		case 2:
		{
			CloseHandle (hPort.hRead);
			break;
		}
		case 3: //安全模块USB
		{
			CloseHandle (hPort.hRead);
			CloseHandle (hPort.hWrite);
			break;
		}
		default:
			break;
	}
	hPort.hRead = NULL;
	hPort.hWrite = NULL;
	hPort.IOType = 0;
	return 0x90;
}

int STDCALL SDT_GetSAMStatus(int iPortID,int iIfOpen)
{
	if (iIfOpen)
	{
		if (SDT_OpenPort (iPortID) != 0x90)
		{
			return 1;
		}
	}
	unsigned char uchRead[4];
	unsigned char uchSnd[3] = {0x11, 0xFF, 0xED};
	if ( ( (iPortID<=1016) && (iPortID>=1001) )
			|| ( (iPortID<=16) && (iPortID>=1) ) )
	{
		DWORD retLen = IOWriteFrm( hPort, uchSnd, 3 );
		if (retLen != 3 )
		{
			if (iIfOpen)
    			SDT_ClosePort(iPortID);
			return 3;
		}
		if ( IOReadFrm( hPort, uchRead, &retLen) )
		{
    		if (iIfOpen) SDT_ClosePort(iPortID);
			return 3;
		}		
	}
	else
	{
		if (iIfOpen) SDT_ClosePort(iPortID);
		return 1;
	}
	if (iIfOpen)
	{
		SDT_ClosePort(iPortID);
	}
	if (uchRead[1]==0&&uchRead[0]==0)
		return uchRead[2];
	else
		return 0;
}

int STDCALL SDT_ResetSAM(int iPortID,int iIfOpen)
{
	if (iIfOpen)
	{
		if (SDT_OpenPort (iPortID) != 0x90)
		{
			return 1;
		}
	}
	unsigned char uchRead[4];
	unsigned char uchSnd[3] = {0x10, 0xFF, 0xEC};
	if ( ( (iPortID<=1016) && (iPortID>=1001) )
			|| ( (iPortID<=16) && (iPortID>=1) ) )
	{
		DWORD retLen = IOWriteFrm( hPort, uchSnd, 3 );
		if (retLen != 3 )
		{
			if (iIfOpen)
    			SDT_ClosePort(iPortID);
			return 3;
		}
		if ( IOReadFrm( hPort, uchRead, &retLen) )
		{
    		if (iIfOpen) SDT_ClosePort(iPortID);
			return 3;
		}		
	}
	else
	{
		if (iIfOpen) SDT_ClosePort(iPortID);
		return 1;
	}
	if (iIfOpen)
		SDT_ClosePort(iPortID);
	return uchRead[2];
}

int STDCALL SDT_GetSAMID(int iPortID,unsigned char *pucSAMID,int iIfOpen)
{
	if (iIfOpen)
	{
		if (SDT_OpenPort (iPortID) != 0x90)
		{
			return 1;
		}
	}
	unsigned char uchRead[20];
	unsigned char uchSnd[3] = {0x12, 0xFF, 0xEE};
	if ( ( (iPortID<=1016) && (iPortID>=1001) )
			|| ( (iPortID<=16) && (iPortID>=1) ) )
	{
		DWORD retLen = IOWriteFrm( hPort, uchSnd, 3 );
		if (retLen != 3 )
		{
			if (iIfOpen)
    			SDT_ClosePort(iPortID);
			return 3;
		}
		if ( IOReadFrm( hPort, uchRead, &retLen) )
		{
    		if (iIfOpen) SDT_ClosePort(iPortID);
			return 3;
		}
		if (retLen != 20)
		{
			if (iIfOpen) SDT_ClosePort(iPortID);
			return 3;
		}
	}
	else
	{
		if (iIfOpen) SDT_ClosePort(iPortID);
		return 1;
	}
	memcpy (pucSAMID, &uchRead[3], 16);
	if (iIfOpen)
		SDT_ClosePort(iPortID);
	return uchRead[2];
}

int STDCALL SDT_GetSAMIDToStr(int iPortID,char *pcSAMID,int iIfOpen)
{
	char pucSAMID[16];
	if (iIfOpen)
	{
		if (SDT_OpenPort (iPortID) != 0x90)
		{
			return 1;
		}
	}
	unsigned char uchRead[20];
	unsigned char uchSnd[3] = {0x12, 0xFF, 0xEE};
	if ( ( (iPortID<=1016) && (iPortID>=1001) )
			|| ( (iPortID<=16) && (iPortID>=1) ) )
	{
		DWORD retLen = IOWriteFrm( hPort, uchSnd, 3 );
		if (retLen != 3 )
		{
			if (iIfOpen)
    			SDT_ClosePort(iPortID);
			return 3;
		}
		if ( IOReadFrm( hPort, uchRead, &retLen) )
		{
    		if (iIfOpen) SDT_ClosePort(iPortID);
			return 3;
		}
		if (retLen != 20)
		{
			if (iIfOpen) SDT_ClosePort(iPortID);
			return 3;
		}
	}
	memcpy ( pucSAMID, &uchRead[3], 16 );
	ParseSerial ( pcSAMID, pucSAMID );
	if (iIfOpen)
		SDT_ClosePort(iPortID);
	return uchRead[2];
}

int STDCALL	SDT_StartFindIDCard(int iPortID,unsigned char *pucIIN,int iIfOpen)
{
	if (iIfOpen)
	{
		if (SDT_OpenPort (iPortID) != 0x90)
		{
			return 1;
		}
	}
	unsigned char uchRead[8];
	unsigned char uchSnd[3] = {0x20, 0x01, 0x22};
	if ( ( (iPortID<=1016) && (iPortID>=1001) )
			|| ( (iPortID<=16) && (iPortID>=1) ) )
	{
		DWORD retLen = IOWriteFrm( hPort, uchSnd, 3 );
		if (retLen != 3 )
		{
			if (iIfOpen)
    			SDT_ClosePort(iPortID);
			return 3;
		}
		if ( IOReadFrm( hPort, uchRead, &retLen) )
		{
    		if (iIfOpen) SDT_ClosePort(iPortID);
			return 3;
		}
		if ( (8 == retLen) && ( pucIIN ) )
		{
			memcpy ( pucIIN, &uchRead[3], 4);
		}
	}
	else
	{
		if (iIfOpen) SDT_ClosePort(iPortID);
		return 1;
	}	
	if (iIfOpen)
		SDT_ClosePort(iPortID);
	return uchRead[2];
}

int STDCALL	SDT_SelectIDCard(int iPortID,unsigned char *pucSN,int iIfOpen)
{
	if (iIfOpen)
	{
		if (SDT_OpenPort (iPortID) != 0x90)
		{
			return 1;
		}
	}
	unsigned char uchRead[12];
	unsigned char uchSnd[3] = {0x20, 0x02, 0x21};
	if ( ( (iPortID<=1016) && (iPortID>=1001) )
			|| ( (iPortID<=16) && (iPortID>=1) ) )
	{
		DWORD retLen = IOWriteFrm( hPort, uchSnd, 3 );
		if (retLen != 3 )
		{
			if (iIfOpen)
    			SDT_ClosePort(iPortID);
			return 3;
		}
		if ( IOReadFrm( hPort, uchRead, &retLen) )
		{
    		if (iIfOpen) SDT_ClosePort(iPortID);
			return 3;
		}
		if ( (12 == retLen) && ( pucSN ) )
		{
			memcpy (pucSN, &uchRead[3], 8);
		}
	}
	else
	{
		if (iIfOpen) SDT_ClosePort(iPortID);
		return 1;
	}
	if (iIfOpen)
		SDT_ClosePort(iPortID);
	return uchRead[2];
}


int STDCALL SDT_ReadBaseMsg(int iPortID,unsigned char * pucCHMsg,unsigned int *	puiCHMsgLen,unsigned char * pucPHMsg,unsigned int  *puiPHMsgLen,int iIfOpen)
{
	int ret=0;
	if (iIfOpen)
	{
		if (SDT_OpenPort (iPortID) != 0x90)
		{
			return 1;
		}
	}
//	unsigned char uchRead[1288];
	unsigned char uchRead[2048]={0};
	unsigned char uchSnd[3] = {0x30, 0x01, 0x32};
	if ( ( (iPortID<=1016) && (iPortID>=1001) )
			|| ( (iPortID<=16) && (iPortID>=1) ) )
	{
		DWORD retLen = IOWriteFrm( hPort, uchSnd, 3 );
		if (retLen != 3 )
		{
			if (iIfOpen)
    			SDT_ClosePort(iPortID);
			return 2;
		}
		if (ret= IOReadFrm( hPort, uchRead, &retLen) )
		{
#if 1
	char mssg[256]={0};
	sprintf(mssg,"IOReadFrm=%d",ret);
	
		 MessageBox(NULL,mssg,"debug1",MB_OK);
#endif

    		if (iIfOpen) SDT_ClosePort(iPortID);
			return 3;
		}
		if ( retLen < 15 )
		{
			if (iIfOpen) SDT_ClosePort(iPortID);
			return 4;
		}
		memcpy ( puiCHMsgLen, &uchRead[3], sizeof (short) );
		memcpy ( puiPHMsgLen, &uchRead[3 + sizeof (short)], sizeof (short));
		*puiCHMsgLen = reverse_short (*puiCHMsgLen);
		*puiPHMsgLen = reverse_short (*puiPHMsgLen);
		if ( *puiCHMsgLen > 256 )
			*puiCHMsgLen = 256;
		if ( *puiPHMsgLen >1024 )
			*puiPHMsgLen = 1024;
		memcpy (pucCHMsg, &uchRead[3 + 2 * sizeof (short)],  *puiCHMsgLen);
		memcpy (pucPHMsg, &uchRead[3 + 2 * sizeof (short) + *puiCHMsgLen],  *puiPHMsgLen);
	}
	else
	{
		if (iIfOpen) SDT_ClosePort(iPortID);
		return 5;
	}
	if (iIfOpen)
		SDT_ClosePort(iPortID);
	return uchRead[2];
}

int STDCALL SDT_ReadBaseMsgToFile(int iPortID,char * pcCHMsgFileName,unsigned int *puiCHMsgFileLen,char * pcPHMsgFileName,unsigned int  *puiPHMsgFileLen,int iIfOpen)
{
	int ret;
	unsigned int uiCHMsgLen, uiPHMsgLen;
	unsigned char unCHMsg[256];
	unsigned char ucPHMsg[1024];
	if ( (ret = SDT_ReadBaseMsg(iPortID, unCHMsg, &uiCHMsgLen, ucPHMsg, &uiPHMsgLen, iIfOpen)) != 0x90)
	{
		return ret;
	}
	FILE *file = fopen (pcCHMsgFileName, "wb+");
	if (!file)
		return 0;
	if (fwrite (unCHMsg, 1, uiCHMsgLen, file) != uiCHMsgLen)
	{
		return 0;
	}
	fclose (file);
	*puiCHMsgFileLen = uiCHMsgLen;

	file = fopen (pcPHMsgFileName, "wb+");
	if (!file)
		return 0;
	if (fwrite (ucPHMsg, 1, uiPHMsgLen, file) != uiPHMsgLen)
	{
		return 0;
	}
	*pcPHMsgFileName = uiPHMsgLen;
	fclose (file);
	
	return 0x90;
}

int STDCALL SDT_ReadNewAppMsg(int iPortID,unsigned char * pucAppMsg,unsigned int *puiAppMsgLen,int iIfOpen)
{
	if (iIfOpen)
	{
		if (SDT_OpenPort (iPortID) != 0x90)
		{
			return 1;
		}
	}
	unsigned char uchRead[284];
	unsigned char uchSnd[3] = {0x30, 0x03, 0x30};
	if ( ( (iPortID<=1016) && (iPortID>=1001) )
			|| ( (iPortID<=16) && (iPortID>=1) ) )
	{
		DWORD retLen = IOWriteFrm( hPort, uchSnd, 3 );
		if (retLen != 3 )
		{
			if (iIfOpen)
    			SDT_ClosePort(iPortID);
			return 3;
		}
		if ( IOReadFrm( hPort, uchRead, &retLen) )
		{
    		if (iIfOpen) SDT_ClosePort(iPortID);
			return 3;
		}
		*puiAppMsgLen = retLen - 4;
		if ( *puiAppMsgLen > 280 )
			*puiAppMsgLen = 280;
		if ( retLen <= 4 )
		{
			pucAppMsg = NULL;
		}
		else
		{
			memcpy (pucAppMsg, &uchRead[3], *puiAppMsgLen );
		}
	}
	else
	{
		if (iIfOpen) SDT_ClosePort(iPortID);
		return 1;
	}
	if (iIfOpen)
		SDT_ClosePort(iPortID);
	return uchRead[2];
}

int STDCALL SDT_ReadMngInfo(int iPortID, unsigned char * pucManageMsg,int iIfOpen)
{
	if (iIfOpen)
	{
		if (SDT_OpenPort (iPortID) != 0x90)
		{
			return 1;
		}
	}
	unsigned char uchRead[32] = {0};
	unsigned char uchSnd[3] = {0x30, 0x05, 0x36};
	if ( ( (iPortID<=1016) && (iPortID>=1001) )
			|| ( (iPortID<=16) && (iPortID>=1) ) )
	{
		DWORD retLen = IOWriteFrm( hPort, uchSnd, 3 );
		if (retLen != 3 )
		{
			if (iIfOpen)
    			SDT_ClosePort(iPortID);
			return 3;
		}
		if ( IOReadFrm( hPort, uchRead, &retLen) )
		{
    		if (iIfOpen) SDT_ClosePort(iPortID);
			return 3;
		}
		if ( retLen != 32)
		{
			pucManageMsg = NULL;
		}
		else
		{
			memcpy (pucManageMsg, &uchRead[3], 28 );
		}
	}
	else
	{
		if (iIfOpen) SDT_ClosePort(iPortID);
		return 1;
	}
	if (iIfOpen)
		SDT_ClosePort(iPortID);
	return uchRead[2];
}


int STDCALL SDT_SetMaxRFByte(int iPortID,unsigned char ucByte,int iIfOpen)
{
	if (iIfOpen)
	{
		if (SDT_OpenPort (iPortID) != 0x90)
		{
			return 1;
		}
	}
	unsigned char uchRead[20];
	unsigned char uchSnd[4];
	uchSnd[0] = 0x61;
	uchSnd[1] = 0xFF;
	uchSnd[2] = ucByte;
	uchSnd[3] = 0x04^0x61^0xFF^ucByte;
	
	if ( ( (iPortID<=1016) && (iPortID>=1001) )
			|| ( (iPortID<=16) && (iPortID>=1) ) )
	{
		DWORD retLen = IOWriteFrm( hPort, uchSnd, 4 );
		if (retLen != 4 )
		{
			if (iIfOpen)
    			SDT_ClosePort(iPortID);
			return 3;
		}
		if ( IOReadFrm( hPort, uchRead, &retLen) )
		{
    		if (iIfOpen) SDT_ClosePort(iPortID);
			return 3;
		}
	}
	else
	{
		if (iIfOpen) SDT_ClosePort(iPortID);
		return 1;
	}
	if (iIfOpen)
		SDT_ClosePort(iPortID);
	return uchRead[2];
}

#define filewlt   "photo.wlt"
#define filebmp   "photo.bmp"
#define filejpg   "photo.jpg"
#define	ucByte    0x58  
int gPort = 0;
char * gfolk[] = {
"汉",
"蒙古",
"回",
"藏",
"维吾尔",
"苗",
"彝",
"壮",
"布依",
"朝鲜",
"满",
"侗",
"瑶",
"白",
"土家",
"哈尼",
"哈萨克",
"傣",
"黎",
"傈僳",
"佤",
"畲",
"高山",
"拉祜",
"水",
"东乡",
"纳西",
"景颇",
"柯尔克孜",
"土",
"达斡尔",
"仫佬",
"羌",
"布朗",
"撒拉",
"毛南",
"仡佬",
"锡伯",
"阿昌",
"普米",
"塔吉克",
"怒",
"乌孜别克",
"俄罗斯",
"鄂温克",
"德昂",
"保安",
"裕固",
"京",
"塔塔尔",
"独龙",
"鄂伦春",
"赫哲",
"门巴",
"珞巴",
"基诺",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"其他",
"芒人",
"摩梭人",
"革家人",
"穿青人",
"入籍",
"其他族"
};


int STDCALL InitComm ( int iPort )
{
#if 0
	int ret;

	if ( iPort > 0 && iPort < 17 ) //COM
	{
		if ( !g_iCurBaudRate ) //
		{
			ret = SDT_GetCOMBaud( iPort, &g_iCurBaudRate );
			if ( 0x90 == ret ) // set com baud ok
			{
				if( 0x90 == SDT_OpenPort( iPort ) )
				{
					gPort = iPort;
					return 1;
				}
				else
				{
					return 0; //基本不可能
				}
			}
			else //可能是PC的COM所支持的Baud比SAM缺省的低
			{
				
			}
		}
		else //已经在加载dll中使用com成功
		{
		}
	}
	else if ( iPort > 1000 && iPort < 1017 ) //USB
	{
	}
	return 0;
#else
       //状态检查
       if ( ( iPort > 0 && iPort < 17 ) || ( iPort > 1000 && iPort < 1017) )
       {
            int ret =   SDT_GetSAMStatus( iPort, 1  );
#if debug
			char mssg[256]={0};
			sprintf(mssg,"%s%d","SDT_GetSAMStatus=",ret);
			MessageBox(NULL,mssg,"debug",MB_OK);
#endif 
            if ( 0x90 == ret  )
			{
				if( 0x90 == SDT_OpenPort( iPort ) )
				{
#if debug
					sprintf(mssg,"%s","SDT_OpenPort OK!");
					MessageBox(NULL,mssg,"debug",MB_OK);
#endif 
//						ret=checkSAMID( iPort );
#if debug
						sprintf(mssg,"%s%d","checkSAMID=",ret);
						MessageBox(NULL,mssg,"debug",MB_OK);
#endif 

						if( 0x90 != ret )
                        {
#if debug
							sprintf(mssg,"%s","checkSAMID err!");
							MessageBox(NULL,mssg,"debug",MB_OK);
#endif 
                             SDT_ClosePort( iPort );
							 if( chklist ) free(chklist);
                             chklist = NULL;
							 return 0;
                        }
                        else
                        {
#if debug
							sprintf(mssg,"%s","checkSAMID ok!");
							MessageBox(NULL,mssg,"debug",MB_OK);
#endif 

							if( chklist ) free(chklist);
                             chklist = NULL;
                             int ret = SDT_SetMaxRFByte( iPort, ucByte, 0 );
                             if ( 0x90 != ret )
                             {
                                 SDT_ClosePort( iPort );
								 return 0;
                             }
                        }
					    gPort = iPort;
						return 1;
				}
            }
       }

       return 0;
#endif

}

int STDCALL CloseComm(void)
{
        if ( ( gPort > 0 && gPort < 17 ) || ( gPort > 1000 && gPort < 1017) )
        {
                SDT_ClosePort( gPort );
                gPort = 0;
                return 1;
        }

        return 0;
}

int  STDCALL Authenticate(void)
{
//		int ret =   SDT_GetSAMStatus( gPort, 1  );
#if debug
//		char mssg[256]={0};
//		sprintf(mssg,"%s%d","SDT_GetSAMStatus=",ret);
//		MessageBox(NULL,mssg,"debug",MB_OK);
#endif
        if ( ( gPort > 0 && gPort < 17 ) || ( gPort > 1000 && gPort < 1017) )
        {
               unsigned char pucIIN[4];
               unsigned char pucSN[8];

                int ret =  SDT_StartFindIDCard( gPort, pucIIN, 0  );
#if debug
				char mssg[256]={0};
				sprintf(mssg,"%s%d","SDT_StartFindIDCard=",ret);
				MessageBox(NULL,mssg,"debug",MB_OK);
#endif 
                if ( 0x80 ==  ret )
					ret =  SDT_StartFindIDCard( gPort, pucIIN, 0  );
#if debug
				sprintf(mssg,"%s%d","SDT_StartFindIDCard=",ret);
				MessageBox(NULL,mssg,"debug",MB_OK);
#endif 

                if ( 0x9f ==  ret )
                {
                        ret = SDT_SelectIDCard( gPort, pucSN, 0  );
#if debug
						char mssg[256]={0};
						sprintf(mssg,"%s%d","SDT_SelectIDCard=",ret);
						MessageBox(NULL,mssg,"debug",MB_OK);
#endif 
                        if ( 0x90 == ret )
							return 1;    
                }
        }

        return 0;       

}

int  STDCALL ReadBaseMsg( unsigned char * pMsg, int * len )
{
	int ret = 0;
	if ( !pMsg )
		return 0;

      if ( ( gPort > 0 && gPort < 17 ) || ( gPort > 1000 && gPort < 1017) )
       {
			   unsigned char uchRespond[256];
               unsigned char uchPHMsg[1024];
               unsigned int len1, len2;
			   memset( pMsg, 0, 192 );
               ret = SDT_ReadBaseMsg( gPort, uchRespond, &len1, uchPHMsg, &len2, 0  );
               if ( 0x90 == ret )
               {
					int type = 1;
					int i;
					
					TCHAR dewltPath[256];
					TCHAR tempstr[256]="\\";

					GetModuleFileName(::GetModuleHandle("sdtapi.dll"),dewltPath,sizeof(dewltPath) / sizeof(dewltPath[0]));
					strcat(tempstr,filewlt);
					strcpy(strrchr(dewltPath, '\\'), tempstr);

#if 0
					char mssg[256]={0};
					char mssg2[256]={0};
					sprintf(mssg,"filewlt=%s",filewlt);
					sprintf(mssg2,"dewltPath=%s",dewltPath);
					
					MessageBox(NULL,mssg,"filewlt",MB_OK);
					MessageBox(NULL,mssg2,"dewltPath",MB_OK);
#endif


					FILE * fp = fopen ( dewltPath, "wb+");
//					FILE * fp = fopen ( filewlt, "wb+");
				    if ( !fp )return 0;
					if (len2!=fwrite( (void *)uchPHMsg, 1, len2, fp ) ){
						 fclose(fp);
                         return 0;
					} 
					fclose(fp);
                    /*if ( gPort > 1000 )type = 2;
                    SDT_ClosePort( gPort );
                    if (GetBmp(filewlt,type ) != 1)
					{
						SDT_OpenPort ( gPort );
						return 0;
					}
					SDT_OpenPort ( gPort );*/
//					if (dewlt(filewlt) != 1)
					if (dewlt(dewltPath) != 1)
					{
						return 0;
					}
					//phrase text
					unsigned char * pChar= uchRespond;
					int offset = 0;
					char name[30];
					memset (name, 0, 30);	
					WideCharToMultiByte ( CP_ACP, 0, (LPCWSTR)pChar, 
										15, name, 30, NULL, NULL);
					for ( i=29; i>0; i--)
					{
						if ( !name[i] )
							continue;
						if (name[i] == 0x20 )
							name[i] = 0x0;
						else
							break;
					}
					memcpy ( (void*)&pMsg[offset], (void*)name, 30 );
					pChar += 30;
					offset += 31;
					if (*pChar == 0x31)
						memcpy ( (void*)&pMsg[offset], (void*)"男", 2 );
					else
						memcpy ( (void*)&pMsg[offset], (void*)"女", 2 );
					pChar += 2;
					offset += 3;
					char folk[5];
					memset (folk, 0, 5);
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 2, folk, 4, NULL, NULL);
					char * pfolk = gfolk[atoi( folk) - 1];
					memcpy ( (void*)&pMsg[offset], (void*)pfolk, strlen(pfolk) );
					pChar += 4;
					offset += 10;

					char birth[16];
					memset (birth, 0, 16);
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 8, birth, 16, NULL, NULL);
					memcpy ( (void*)&pMsg[offset], (void*)birth, 8 );
					pChar += 16;
					offset += 9;

					char address[70];
					memset (address, 0, 70);
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 35, address, 70, NULL, NULL);
					for (i=0; i<69; i++)
					{
						if (address[i] == 0x20)
							address[i] = 0x0;
					}
					memcpy ( (void*)&pMsg[offset], (void*)address, 70 );
					pChar += 70;
					offset += 71;

					char code [36];
					memset (code, 0, 36);
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 18, code, 36, NULL, NULL);
					memcpy ( (void*)&pMsg[offset], (void*)code, 18 );
					pChar += 36;
					offset += 19;

					char agency [30];
					memset (agency, 0, 30);
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 15, agency, 30, NULL, NULL);
					memcpy ( (void*)&pMsg[offset], (void*)agency, 30 );
					pChar += 30;
					offset += 31;

					//2005-7-18 针对有效期截至日期问题（有长期表示方式）
					char expire [16];
					memset (expire, 0, 16);
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 8, expire, 16, NULL, NULL);
					memcpy ( (void*)&pMsg[offset], (void*)expire, 8 );
					pChar += 16;
					offset += 9;

					//有效期截至日期
					memset (expire, 0, 16);
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 8, expire, 16, NULL, NULL);
					memcpy ( (void*)&pMsg[offset], (void*)expire, 8 );

					if ( len )
						*(unsigned int*)len = 192;		
               }
        }
	if (ret != 0x90)
		return 0;
	else
	{
		return 1;
	}
}

int  STDCALL ReadBaseMsgPhoto( unsigned char * pMsg, int * len, char * directory )
{
	int ret = 0;
	if ( !pMsg )
		return 0;

      if ( ( gPort > 0 && gPort < 17 ) || ( gPort > 1000 && gPort < 1017) )
       {
			   unsigned char uchRespond[256];
               unsigned char uchPHMsg[1024];
               unsigned int len1, len2;
			   memset( pMsg, 0, 192 );
               ret = SDT_ReadBaseMsg( gPort, uchRespond, &len1, uchPHMsg, &len2, 0  );
               if ( 0x90 == ret )
               {
					int type = 1;
					int i;
					char FileName[1024];
					memset(FileName,0,1024);
					char bmpFileName[1024];
					memset(bmpFileName,0,1024);
					char jpgFileName[1024];
					memset(jpgFileName,0,1024);

					if ( !directory || strlen(directory)==0 )
					{
						sprintf(FileName,"%s",filewlt);
						sprintf(bmpFileName,"%s",filebmp);
						sprintf(jpgFileName,"%s",filejpg);
					}
					else if ( directory[strlen(directory)]=='\\' )
					{
						sprintf(FileName,"%s%s",directory,filewlt);
						sprintf(bmpFileName,"%s%s",directory,filebmp);
						sprintf(jpgFileName,"%s%s",directory,filejpg);
					}
					else
					{
						sprintf(FileName,"%s%s%s",directory,"\\",filewlt);
						sprintf(bmpFileName,"%s%s%s",directory,"\\",filebmp);
						sprintf(jpgFileName,"%s%s%s",directory,"\\",filejpg);
					}
					FILE * fp = fopen ( FileName, "wb+");
				    if ( !fp )return 4;
					if (len2!=fwrite( (void *)uchPHMsg, 1, len2, fp ) ){
						 fclose(fp);
                         return 0;
					} 
					fclose(fp);
                    /*if ( gPort > 1000 )type = 2;
                    SDT_ClosePort( gPort );
                    if (GetBmp(FileName,type ) != 1)
					{
						SDT_OpenPort ( gPort );
						return 0;
					}
					SDT_OpenPort ( gPort );*/
					if (dewlt(FileName) != 1)
					{
						return 0;
					}
					//phrase text
					unsigned char * pChar= uchRespond;
					int offset = 0;
					char name[30];
					memset (name, 0, 30);	
					WideCharToMultiByte ( CP_ACP, 0, (LPCWSTR)pChar, 
										15, name, 30, NULL, NULL);
					for ( i=29; i>0; i--)
					{
						if ( !name[i] )
							continue;
						if (name[i] == 0x20 )
							name[i] = 0x0;
						else
							break;
					}
					memcpy ( (void*)&pMsg[offset], (void*)name, 30 );
					pChar += 30;
					offset += 31;
					if (*pChar == 0x31)
						memcpy ( (void*)&pMsg[offset], (void*)"男", 2 );
					else
						memcpy ( (void*)&pMsg[offset], (void*)"女", 2 );
					pChar += 2;
					offset += 3;
					char folk[5];
					memset (folk, 0, 5);
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 2, folk, 4, NULL, NULL);
					char * pfolk = gfolk[atoi( folk) - 1];
					memcpy ( (void*)&pMsg[offset], (void*)pfolk, strlen(pfolk) );
					pChar += 4;
					offset += 10;

					char birth[16];
					memset (birth, 0, 16);
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 8, birth, 16, NULL, NULL);
					memcpy ( (void*)&pMsg[offset], (void*)birth, 8 );
					pChar += 16;
					offset += 9;

					char address[70];
					memset (address, 0, 70);
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 35, address, 70, NULL, NULL);
					for (i=0; i<69; i++)
					{
						if (address[i] == 0x20)
							address[i] = 0x0;
					}
					memcpy ( (void*)&pMsg[offset], (void*)address, 70 );
					pChar += 70;
					offset += 71;

					char code [36];
					memset (code, 0, 36);
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 18, code, 36, NULL, NULL);
					memcpy ( (void*)&pMsg[offset], (void*)code, 18 );
					pChar += 36;
					offset += 19;

					char agency [30];
					memset (agency, 0, 30);
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 15, agency, 30, NULL, NULL);
					memcpy ( (void*)&pMsg[offset], (void*)agency, 30 );
					pChar += 30;
					offset += 31;

					//2005-7-18 针对有效期截至日期问题（有长期表示方式）
					char expire [16];
					memset (expire, 0, 16);
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 8, expire, 16, NULL, NULL);
					memcpy ( (void*)&pMsg[offset], (void*)expire, 8 );
					pChar += 16;
					offset += 9;

					//有效期截至日期
					memset (expire, 0, 16);
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 8, expire, 16, NULL, NULL);
					memcpy ( (void*)&pMsg[offset], (void*)expire, 8 );

					if ( len )
						*(unsigned int*)len = 192;		
               }
        }
	if (ret != 0x90)
		return 0;
	else
		return 1;
}

int  STDCALL ReadNewAppMsg( unsigned char * pMsg, int * num )
{
	if ( !pMsg || !num )
		return 0;

	if ( ( gPort > 0 && gPort < 17 ) || ( gPort > 1000 && gPort < 1017) )
	{
		unsigned char uchAdd[280];
		unsigned int len2;
		memset( pMsg, 0, 284 );
		*num = 0;
		int ret = SDT_ReadNewAppMsg( gPort, uchAdd, &len2, 0 );
		if ( 0x90 == ret )
		{
				char buf[72];
				int i = 0;
				int j;
                while ( len2 >= 70 )
                {
					memset( buf, 0, 72 );
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)&uchAdd[i], 35, buf, 70, NULL, NULL);
					for (j=0; j<70; j++)
					{
						if (buf[j] == 0x20)
							buf[j] = 0x0;
					}
					if ( strlen( buf ) ){
						memcpy ( &pMsg[(*num)*71], buf, 71 );
						(*num)++;
					}
					i += 70;
                    len2 += -70;
                }
				return 1;
		}
	}
	return 0;
}

#if 1
int  STDCALL ReadBaseMsgW( unsigned char * pMsg, int * len )
{
	int ret = 0;
	if ( (!pMsg) || (!len) )
		return 0;

      if ( ( gPort > 0 && gPort < 17 ) || ( gPort > 1000 && gPort < 1017) )
       {
			   //unsigned char uchRespond[256];
               unsigned char uchPHMsg[1024];
               unsigned int len1, len2;
			   //memset( pMsg, 0, 256 );
               ret = SDT_ReadBaseMsg( gPort, pMsg, &len1, uchPHMsg, &len2, 0  );
               if ( 0x90 == ret )
               {
					int type = 1;

					TCHAR dewltPath[256];
					TCHAR tempstr[256]="\\";
					
					GetModuleFileName(::GetModuleHandle("sdtapi.dll"),dewltPath,sizeof(dewltPath) / sizeof(dewltPath[0]));
					strcat(tempstr,filewlt);
					strcpy(strrchr(dewltPath, '\\'), tempstr);
					

					FILE * fp = fopen ( dewltPath, "wb+");
//					FILE * fp = fopen ( filewlt, "wb+");
				    if ( !fp )return 0;
					if (len2!=fwrite( (void *)uchPHMsg, 1, len2, fp ) ){
						 fclose(fp);
                         return 0;
					} 
					fclose(fp);
                    /*if ( gPort > 1000 )type = 2;
                    SDT_ClosePort( gPort );
                    if (GetBmp(filewlt,type ) != 1)
					{
						SDT_OpenPort ( gPort );
						return 0;
					}
					SDT_OpenPort ( gPort );*/
					if (dewlt(dewltPath) != 1)

//					if (dewlt(filewlt) != 1)
					{
						return 0;
					}
					//
					* len = len1;	
               }
        }
	if (ret != 0x90)
		return 0;
	else
		return 1;
}

int  STDCALL ReadBaseMsgWPhoto( unsigned char * pMsg, int * len, char * directory )
{
	int ret = 0;
	if ( (!pMsg) || (!len))
		return 0;

      if ( ( gPort > 0 && gPort < 17 ) || ( gPort > 1000 && gPort < 1017) )
       {
			   //unsigned char uchRespond[256];
               unsigned char uchPHMsg[1024];
               unsigned int len1, len2;
			   //memset( pMsg, 0, 256 );
               ret = SDT_ReadBaseMsg( gPort, pMsg, &len1, uchPHMsg, &len2, 0  );
               if ( 0x90 == ret )
               {
					int type = 1;
					char FileName[1024];
					memset(FileName,0,1024);
					if ( !directory || strlen(directory)==0 )
						sprintf(FileName,"%s",filewlt);
					else if ( directory[strlen(directory)]=='\\' )
						sprintf(FileName,"%s%s",directory,filewlt);
					else
						sprintf(FileName,"%s%s%s",directory,"\\",filewlt);
					FILE * fp = fopen ( FileName, "wb+");
				    if ( !fp )return 4;
					if (len2!=fwrite( (void *)uchPHMsg, 1, len2, fp ) ){
						 fclose(fp);
                         return 0;
					} 
					fclose(fp);
                    /*if ( gPort > 1000 )type = 2;
                    SDT_ClosePort( gPort );
                    if (GetBmp(FileName,type ) != 1)
					{
						SDT_OpenPort ( gPort );
						return 0;
					}
					SDT_OpenPort ( gPort );*/
					if (dewlt(FileName) != 1)
					{
						return 0;
					}
					//
					* len = len1;	
               }
        }
	if (ret != 0x90)
		return 0;
	else
		return 1;
}

int  STDCALL ReadBaseInfos( char * Name, char * Gender, char * Folk,
						  char *BirthDay, char * Code, char * Address,
						  char *Agency, char * ExpireStart,char* ExpireEnd )
{
	int ret = 0;
	unsigned char uchRespond[256];
	unsigned char uchPHMsg[1024];
    unsigned int len1, len2;
//	printf("enter ReadBaseInfos!\r\n");

      if ( ( gPort > 0 && gPort < 17 ) || ( gPort > 1000 && gPort < 1017) )
       {

               ret = SDT_ReadBaseMsg( gPort, uchRespond, &len1, uchPHMsg, &len2, 0  );

               if ( 0x90 == ret )
               {
					int type = 1;
					int i;
					FILE * fp = fopen ( filewlt, "wb+");
				    if ( !fp )return 0;
					if (len2!=fwrite( (void *)uchPHMsg, 1, len2, fp ) ){
						 fclose(fp);
                         return 0;
					} 
					fclose(fp);
                    /*if ( gPort > 1000 )type = 2;
                    SDT_ClosePort( gPort );
                    if (GetBmp(filewlt,type ) != 1)
					{
						SDT_OpenPort ( gPort );
						return 0;
					}
					SDT_OpenPort ( gPort );*/
					if (dewlt(filewlt) != 1)
					{
						return 0;
					}
					//phrase text
					unsigned char * pChar= uchRespond;
					int offset = 0;
					//name
					memset (Name, 0, 31);
					WideCharToMultiByte ( CP_ACP, 0, (LPCWSTR)pChar, 
										15, Name, 30, NULL, NULL);
					for ( i=29; i>0; i--)
					{
						if (Name[i] == 0x20)
							Name[i] = 0x0;
						else
							break;
					}
					pChar += 30;
					//sex
					memset(Gender, 0, 4 );
					if (*pChar == 0x31)
						memcpy ( (void*)Gender, (void*)"男", 2 );
					else
						memcpy ( (void*)Gender, (void*)"女", 2 );
					pChar += 2;
					//folk
					char folk[4];
					memset (folk, 0, 4);
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 2, folk, 2, NULL, NULL);
					char * pfolk = gfolk[atoi( folk) - 1];
					memset ( Folk, 0, 10 );
					memcpy ( (void*)Folk, (void*)pfolk, strlen(pfolk) );
					pChar += 4;
					
					//birth
					memset ( BirthDay, 0, 9 );
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 8, BirthDay, 8, NULL, NULL);
					pChar += 16;

					//addr
					memset (Address, 0, 71);
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 35, Address, 70, NULL, NULL);
					for (i=0; i<69; i++)
					{
						if (Address[i] == 0x20)
							Address[i] = 0x0;
					}
					pChar += 70;

					memset (Code, 0, 19 );
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 18, Code, 18, NULL, NULL);
					pChar += 36;

					memset (Agency, 0, 31);
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 15, Agency, 30, NULL, NULL);
					pChar += 30;

					//2005-7-18 针对有效期截至日期问题（有长期表示方式）
					memset (ExpireStart, 0, 9);
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 8, ExpireStart, 8, NULL, NULL);
					pChar += 16;

					//有效期截至日期
					memset (ExpireEnd, 0, 9);
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 8, ExpireEnd, 8, NULL, NULL);
               }
			   else
				   printf("SDT_ReadBaseMsg=%d\r\n",ret);

        }
	if (ret != 0x90)
		return 0;
	else
	{
		HMODULE hMyDll;
		TCHAR tcPath[256];
		
		int  (WINAPI *pSaveCardJPG)(unsigned char * pucCHMsg,unsigned int puiCHMsgLen,unsigned char * pucPHMsg,unsigned int  puiPHMsgLen,char * front,char *back);
		

		GetModuleFileName(::GetModuleHandle("sdtapi.dll"),tcPath,sizeof(tcPath) / sizeof(tcPath[0]));
//		GetModuleFileName(::GetModuleHandle(_T("你的DLL名字")),strPath,MAX_PATH);   
//		GetModuleFileName(NULL,tcPath,sizeof(tcPath)/sizeof
		
		strcpy(strrchr(tcPath, '\\'), "\\SavePhoto.dll");
		printf("dllpath=%s\r\n",tcPath);
		
		hMyDll=LoadLibrary(tcPath);

//		hMyDll=LoadLibrary(".\\SavePhoto.dll");
	
		if(hMyDll == NULL)
		{
			return -3;
		}
		
		//获取接口地址
		pSaveCardJPG    = (int (WINAPI *)(unsigned char * pucCHMsg,unsigned int 	puiCHMsgLen,unsigned char * pucPHMsg,unsigned int  puiPHMsgLen,char * front,char * back))GetProcAddress(hMyDll,"SaveCard2JPG");
		
		
		
		if(pSaveCardJPG == NULL )
		{
			//卸载动态库
			FreeLibrary(hMyDll);
			return 4;
		}
		if (pSaveCardJPG(uchRespond,len1,uchPHMsg,len2,"1.jpg","2.jpg")==0)
		{
					FreeLibrary(hMyDll);
					return 0;
		}
		
		FreeLibrary(hMyDll);
		
		return 1;
	}
}

int  STDCALL ReadBaseInfosPhoto( char * Name, char * Gender, char * Folk,
						  char *BirthDay, char * Code, char * Address,
						  char *Agency, char * ExpireStart,char* ExpireEnd, char * directory )
{
	int ret = 0;

      if ( ( gPort > 0 && gPort < 17 ) || ( gPort > 1000 && gPort < 1017) )
       {
			   unsigned char uchRespond[256];
               unsigned char uchPHMsg[1024];
               unsigned int len1, len2;
               ret = SDT_ReadBaseMsg( gPort, uchRespond, &len1, uchPHMsg, &len2, 0  );
               if ( 0x90 == ret )
               {
					int type = 1;
					int i;
					char FileName[1024];
					memset(FileName,0,1024);
					char bmpFileName[1024];
					memset(bmpFileName,0,1024);
					char jpgFileName[1024];
					memset(jpgFileName,0,1024);
					
					if ( !directory || strlen(directory)==0 )
					{
						sprintf(FileName,"%s",filewlt);
						sprintf(bmpFileName,"%s",filebmp);
						sprintf(jpgFileName,"%s",filejpg);
					}
					else if ( directory[strlen(directory)]=='\\' )
					{
						sprintf(FileName,"%s%s",directory,filewlt);
						sprintf(bmpFileName,"%s%s",directory,filebmp);
						sprintf(jpgFileName,"%s%s",directory,filejpg);
					}
					else
					{
						sprintf(FileName,"%s\%s",directory,filewlt);
						sprintf(bmpFileName,"%s\%s",directory,filebmp);
						sprintf(jpgFileName,"%s\%s",directory,filejpg);
					}

					FILE * fp = fopen ( FileName, "wb+");
				    if ( !fp )return 4;
					if (len2!=fwrite( (void *)uchPHMsg, 1, len2, fp ) ){
						 fclose(fp);
                         return 0;
					} 
					fclose(fp);
                    /*if ( gPort > 1000 )type = 2;
                    SDT_ClosePort( gPort );
                    if (GetBmp(FileName,type ) != 1)
					{
						SDT_OpenPort ( gPort );
						return 0;
					}
					SDT_OpenPort ( gPort );*/
					if (dewlt(FileName) != 1)
					{
						return 0;
					}
/*
					char mssg[256]={0};
					sprintf(mssg,"bmpFileName=%s,jpgFileName=%s",bmpFileName,jpgFileName);
					MessageBox(NULL,mssg,"debug",MB_OK);
*/
					BmpToJpg(bmpFileName,jpgFileName);

/*
					HMODULE hMyDll;
					TCHAR tcPath[256];
					
					int  (WINAPI *pb2j)(char *bmppath,char *jpgpath);
					
					
					GetModuleFileName(::GetModuleHandle("sdtapi.dll"),tcPath,sizeof(tcPath) / sizeof(tcPath[0]));
					strcpy(strrchr(tcPath, '\\'), "\\jpgdll.dll");
//					printf("dllpath=%s\r\n",tcPath);
					
					hMyDll=LoadLibrary(tcPath);
					
					if(hMyDll == NULL)
					{
						return -3;
					}
					
					//获取接口地址
					pb2j    = (int (WINAPI *)(char *bmppath,char *jpgpath))GetProcAddress(hMyDll,"_BmpToJpg");
					
					
					
					if(pb2j == NULL )
					{
						//卸载动态库
						FreeLibrary(hMyDll);
						return 4;
					}
					pb2j(bmpFileName,jpgFileName);
//					pb2j("c:\photo.bmp","c:\photo.jpg");
					
					FreeLibrary(hMyDll);
*/

					//phrase text
					unsigned char * pChar= uchRespond;
					int offset = 0;
					//name
					memset (Name, 0, 31);
					WideCharToMultiByte ( CP_ACP, 0, (LPCWSTR)pChar, 
										15, Name, 30, NULL, NULL);
					for ( i=29; i>0; i--)
					{
						if (Name[i] == 0x20)
							Name[i] = 0x0;
						else
							break;
					}
					pChar += 30;
					//sex
					memset(Gender, 0, 4 );
					if (*pChar == 0x31)
						memcpy ( (void*)Gender, (void*)"男", 2 );
					else
						memcpy ( (void*)Gender, (void*)"女", 2 );
					pChar += 2;
					//folk
					char folk[4];
					memset (folk, 0, 4);
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 2, folk, 2, NULL, NULL);
					char * pfolk = gfolk[atoi( folk) - 1];
					memset ( Folk, 0, 10 );
					memcpy ( (void*)Folk, (void*)pfolk, strlen(pfolk) );
					pChar += 4;
					
					//birth
					memset ( BirthDay, 0, 9 );
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 8, BirthDay, 8, NULL, NULL);
					pChar += 16;

					//addr
					memset (Address, 0, 71);
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 35, Address, 70, NULL, NULL);
					for (i=0; i<69; i++)
					{
						if (Address[i] == 0x20)
							Address[i] = 0x0;
					}
					pChar += 70;

					memset (Code, 0, 19 );
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 18, Code, 18, NULL, NULL);
					pChar += 36;

					memset (Agency, 0, 31);
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 15, Agency, 30, NULL, NULL);
					pChar += 30;

					//2005-7-18 针对有效期截至日期问题（有长期表示方式）
					memset (ExpireStart, 0, 9);
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 8, ExpireStart, 8, NULL, NULL);
					pChar += 16;

					//有效期截至日期
					memset (ExpireEnd, 0, 9);
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)pChar, 8, ExpireEnd, 8, NULL, NULL);
               }
        }
	if (ret != 0x90)
		return 0;
	else
		return 1;
}

int  STDCALL ReadNewAppMsgW( unsigned char * pMsg, int * num )
{
	if ( !pMsg || !num )
		return 0;

	if ( ( gPort > 0 && gPort < 17 ) || ( gPort > 1000 && gPort < 1017) )
	{
		unsigned int len2;
		//memset( pMsg, 0, 280 );
		*num = 0;
		int ret = SDT_ReadNewAppMsg( gPort, pMsg, &len2, 0 );
		if ( 0x90 == ret )
		{
				* num = len2/70;
				return 1;
		}
	}
	return 0;
}

int  STDCALL ReadNewAppInfos( unsigned char * addr1, unsigned char * addr2,
							 unsigned char * addr3, unsigned char * addr4,
							 int * num )
{
	if ( !addr1 || !addr2 || !addr3 || !addr4 || !num )
		return 0;

	if ( ( gPort > 0 && gPort < 17 ) || ( gPort > 1000 && gPort < 1017) )
	{
		unsigned char uchAdd[280];
		unsigned int len2;
		memset( addr1, 0, 71 );
		memset( addr2, 0, 71 );
		memset( addr3, 0, 71 );
		memset( addr4, 0, 71 );
		*num = 0;
		int ret = SDT_ReadNewAppMsg( gPort, uchAdd, &len2, 0 );
		if ( 0x90 == ret )
		{
				char buf[72];
				unsigned char *pMsg[4];
				pMsg[0] = addr1; 
				pMsg[1] = addr2; 
				pMsg[2] = addr3; 
				pMsg[3] = addr4; 
				int i = 0;
				int j;
                while ( len2 >= 70 )
                {
					memset( buf, 0, 72 );
					WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)&uchAdd[i], 35, 
								buf, 70, NULL, NULL);
					for (j=0; j<70; j++)
					{
						if (buf[j] == 0x20)
							buf[j] = 0x0;
					}
					if ( strlen( buf ) ){
						memcpy ( pMsg[*num], buf, 71 );
						(*num)++;
					}
					i += 70;
                    len2 += -70;
                }
				return 1;
		}
	}
	return 0;
}

int  STDCALL ReadIINSNDN( char * pMsg )
{
	if ( !pMsg )
		return 0;

	if ( ( gPort > 0 && gPort < 17 ) || ( gPort > 1000 && gPort < 1017) )
	{
		unsigned char msg[30];
		memset( pMsg, 0, 16 );
		int ret = SDT_ReadMngInfo( gPort, msg,  0 );
		if ( 0x90 == ret )
		{
			WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)msg, 
							14, pMsg, 14, NULL, NULL);
			return 1;
		}
	}
	return 0;
}

int STDCALL GetSAMIDToStr(char *pcSAMID )
{
	if ( !pcSAMID )
		return 0;

	if ( ( gPort > 0 && gPort < 17 ) || ( gPort > 1000 && gPort < 1017) )
	{
		memset ( pcSAMID, 0, 37 );
		int ret = SDT_GetSAMIDToStr( gPort, pcSAMID,  0 );
		if ( 0x90 == ret )
		{
			return 1;
		}
	}
	return 0;
}

#endif

int Routon_IOWriteFrm ( IOPORT hIO,unsigned char * data,WORD len )
{
	DWORD retLen = 0;
	HANDLE hWrite = hIO.hWrite;
	DWORD type = hIO.IOType;
	unsigned char sndbuf[1024] = {0}; 
	
	//填充包头
	sndbuf[0] = 0x55; 
	sndbuf[1] = 0xAA; 
	sndbuf[2] = len + 1;
	//填充数据
    memcpy ( &sndbuf[3], data, len );
	//计算校验位
	len += 3;
	unsigned char chChkSum = 0;

	for ( int i=0; i < len; i++ )
	{
		chChkSum  ^= sndbuf[i];
	}
	sndbuf[len] = chChkSum;
	len++;
	switch ( type )
	{
		case 1: //串口
		case 3:
			{
#if 0

				FILE *file = fopen ("send", "wb+");

				if (!file)
					return 0;
				if (fwrite (sndbuf, len, 1, file) != 1)
				{
					return 0;
				}
				fclose (file);

clock_t start1, end1,end2,end3,end4;
start1=clock();
#endif				
				if ( !WriteFile( hWrite, sndbuf, len, &retLen, NULL) )
				{
					return 0;
				}
#if 0
end1=clock();
char mssg[256]={0};
sprintf(mssg,"%2.1f%s",(double)(end1-start1)/CLOCKS_PER_SEC,"秒");
if (data[0]==0x18&&data[1]==0xff)
//MessageBox(NULL,mssg,"debug1",MB_OK);
#endif

				//retLen -= 7;
				break;
			}
		default:
			break;
	}
	return retLen;		
}

int Routon_IOReadFrm ( IOPORT hIO, unsigned char * data, DWORD *len )
{
	WORD DataLen = 0;
	DWORD retLen;
	BOOL bSuccess;
	HANDLE hRead = hIO.hRead;
	DWORD type = hIO.IOType;
	unsigned char head[7]; //

	switch ( type )
	{
	case 1: //串口或iDR310的USB
		{
			DWORD start = GetTickCount();
			DWORD end = start;
			DWORD error;
			COMSTAT Stat;
			while ( 1 )
			{
				bSuccess = ClearCommError ( hRead, &error, &Stat );
				if ( error )
				{
					if ( Stat.cbInQue )
					{
						PurgeComm ( hRead, PURGE_RXCLEAR  );
					}
					return 3; 
				}
				if ( Stat.cbInQue >= 3 && !DataLen )
				{
					bSuccess = ReadFile( hRead, head, 3, &retLen, NULL);
					if ( (!bSuccess) || ( 3 != retLen ) )
					{
						return 3;
					}

					//头是否错
					if ( (head[0]!=0x55) || (head[1]!=0xAA) )
					{
						return 3;
					}

					DataLen = head[2];
				}
				if ( DataLen && Stat.cbInQue >= DataLen )
				{
					bSuccess = ReadFile( hRead, data, DataLen, &retLen, NULL);
					if ( (!bSuccess) || ( DataLen != retLen ) )
					{
						return 3;
					}
					break;
				}
				//

//影响读效率，去掉sleep后未发现不良反应 zhangli @2009-11-16
//				Sleep(100);
				end = GetTickCount();
				if ( ( (end - start) > 2000 )  && ( !DataLen ) )
				{
					return 3; //超时
				}
				else 
					if ( (end - start) > 5000 )
					{
						return 3; //
					}
			}

			break;
		}
	case 3:
		{
			unsigned char buf[1024*2]; 
			bSuccess = ReadFile( hRead, buf, 1024*2, &retLen, NULL);
			if ( (!bSuccess) || (  retLen <= 7 ) )
			{
				return 3;
			}
			//头是否错
			if ( (buf[0]!=0x55) || (buf[1]!=0xAA) )
			{
				return 3;
			}
			
			DataLen = buf[2];
			memcpy ( head, buf, 3 );
			memcpy ( data, &buf[3], DataLen );
			break;
		}
	default:
		break;
	}
	//校验和是否错
	unsigned char chChkSum = 0;
	int i;
	for ( i=0; i<3; i++)
	{
		chChkSum  ^= head[i];
	}
	for ( i=0; i<DataLen; i++)
	{
		chChkSum  ^= data[i];
	}
	if (chChkSum)
	{
		return 3;
	}
	*len = DataLen; 
#if 0
				FILE *file = fopen ("rcv", "wb+");
				
				if (!file)
					return 0;
				if (fwrite (data, *len, 1, file) != 1)
				{
					return 0;
				}
				fclose (file);
#endif
	return 0;	
}


/*新增Type A找卡、读卡接口*/
int STDCALL	Routon_StartFindIDCard(unsigned char *err)
{
	unsigned char uchRead[8];
	unsigned char uchSnd[3] = {0x01,0xFF,0xFF};
	
	DWORD retLen = Routon_IOWriteFrm( hPort, uchSnd, 3 );
	if (retLen != 7 )
	{
		return 3;
	}
	if ( Routon_IOReadFrm( hPort, uchRead, &retLen) )
	{
		return 4;
	}
	if ( (7 == retLen) && ( err ) )
	{
		memcpy ( err, &uchRead[2], 1);
	}
	
	return uchRead[0];
}
int STDCALL	Routon_IC_FindCard()
{
	unsigned char uchRespond[8] = {0};
	int ret = Routon_StartFindIDCard( uchRespond );
	
	if ( 0x56 !=  ret )
		return 0;
	else
		return 1;
	
}
int STDCALL	Routon_IC_HL_ReadCardSN(char * SN)
{
	if ( !SN )
		return 0;

	unsigned char uchRespond[1024] = {0};
	unsigned int len1;
	int ret = Routon_ReadBaseMsg( uchRespond ,&len1);
	char  hexchar[256] = {0};
	memset(SN, 0, 1024);
	if ( ret == 0x02 ) //读卡成功
	{
		unsigned int tmp = 0;
		while ( tmp<len1 )
		{
			sprintf(hexchar,"%02X",uchRespond[tmp]);
			strcat(SN, hexchar);
			tmp++;
        }

		return 1;
	}
	else
		return 0;
	
}
int STDCALL	Routon_IC_HL_ReadCard (int SID,int BID,int KeyType,unsigned char * Key,unsigned char * data)
{
	if ( SID<0||SID>15||BID<0||BID>3||!Key||!data||(KeyType!=0x60&&KeyType!=0x61) )
		return 0;
	unsigned char uchRead[1024]={0};
	unsigned char uchSnd[12] = {0x03,0xFF};
	uchSnd[2]=SID;
	uchSnd[3]=BID;
	uchSnd[4]=KeyType;
	uchSnd[5]=Key[0];
	uchSnd[6]=Key[1];
	uchSnd[7]=Key[2];
	uchSnd[8]=Key[3];
	uchSnd[9]=Key[4];
	uchSnd[10]=Key[5];
	
#if 0
				FILE *file = fopen ("readcardsend", "wb+");
				if (!file)
					return 0;
				if (fwrite (uchSnd, 11, 1, file) != 1)
				{
					return 0;
				}
				fclose (file);
#endif
				DWORD retLen = Routon_IOWriteFrm( hPort, uchSnd, 11 );
				if ( retLen != 15 )
				{
					return retLen;
				}
				if ( Routon_IOReadFrm( hPort, uchRead, &retLen) )
				{
					return 0;
				}
#if 0
				char mssg[256]={0};
				sprintf(mssg,"%d",retLen);
				
				MessageBox(NULL,mssg,"debug",MB_OK);
#endif				
				if ( retLen < 15 )
				{
					return 0;
				}
#if 0
				FILE *file1 = fopen ("readcardrcv", "wb+");
				if (!file1)
					return 0;
				if (fwrite (uchRead, retLen, 1, file1) != 1)
				{
					return 0;
				}
				fclose (file1);
#endif
				
				if (!(uchRead[0]==0x03&&uchRead[1]==0xFF))
				{
					return 0;
					
				}
				memcpy (data, &uchRead[2], 16);
				
				return 1;
				
				
}
int STDCALL	Routon_IC_HL_WriteCard (int SID,int BID,int KeyType,unsigned char * Key,unsigned char * data)
{
	if ( SID<0||SID>15||BID<0||BID>3||!Key||!data||(KeyType!=0x60&&KeyType!=0x61) )
		return 0;

	
	
	unsigned char uchRead[1024]={0};
	unsigned char uchSnd[1024] = {0x04,0xFF};
	uchSnd[2]=SID;
	uchSnd[3]=BID;
	uchSnd[4]=KeyType;
	uchSnd[5]=Key[0];
	uchSnd[6]=Key[1];
	uchSnd[7]=Key[2];
	uchSnd[8]=Key[3];
	uchSnd[9]=Key[4];
	uchSnd[10]=Key[5];

	uchSnd[11]=data[0];
	uchSnd[12]=data[1];
	uchSnd[13]=data[2];
	uchSnd[14]=data[3];
	uchSnd[15]=data[4];
	uchSnd[16]=data[5];
	uchSnd[17]=data[6];
	uchSnd[18]=data[7];
	uchSnd[19]=data[8];
	uchSnd[20]=data[9];
	uchSnd[21]=data[10];
	uchSnd[22]=data[11];
	uchSnd[23]=data[12];
	uchSnd[24]=data[13];
	uchSnd[25]=data[14];
	uchSnd[26]=data[15];
	
	
#if 0
				FILE *file = fopen ("writecardsend", "wb+");
				if (!file)
					return 0;
				if (fwrite (uchSnd, 27, 1, file) != 1)
				{
					return 0;
				}
				fclose (file);
#endif
				DWORD retLen = Routon_IOWriteFrm( hPort, uchSnd, 27 );
				if ( retLen != 31 )
				{
					return retLen;
				}
				if ( Routon_IOReadFrm( hPort, uchRead, &retLen) )
				{
					return 0;
				}
#if 0
				char mssg[256]={0};
				sprintf(mssg,"%d",retLen);
				
				MessageBox(NULL,mssg,"debug",MB_OK);
#endif				
				if ( retLen < 15 )
				{
//					return 0;
				}
#if 0
				FILE *file1 = fopen ("writecardrcv", "wb+");
				if (!file1)
					return 0;
				if (fwrite (uchRead, retLen, 1, file1) != 1)
				{
					return 0;
				}
				fclose (file1);
#endif
				
				if (!(uchRead[0]==0x04&&uchRead[1]==0xFF))
				{
					return 0;
					
				}
//				memcpy (data, &uchRead[2], 16);
				
				return 1;
				
				
}

int SDTAPI_API STDCALL Routon_SetNewVersion(bool flag)
{
	unsigned char uchRead[8];
	unsigned char uchnewSnd[3] = {0xC1,0xFF,0x02};
	unsigned char ucholdSnd[3] = {0xC1,0xFF,0x01};
	DWORD retLen;

	if (flag)
		retLen = Routon_IOWriteFrm( hPort, uchnewSnd, 3 );
	else
		retLen = Routon_IOWriteFrm( hPort, ucholdSnd, 3 );

	if (retLen != 7 )
	{
		return 3;
	}
	if ( Routon_IOReadFrm( hPort, uchRead, &retLen) )
	{
		return 4;
	}

	
	return uchRead[0];

}

int STDCALL Routon_ReadBaseMsg(unsigned char * pucCHMsg,unsigned int *	puiCHMsgLen )
{
	
	unsigned char uchRead[2048] = {0};
	unsigned char uchSnd[3] = {0x02,0xFF,0xFF};

	DWORD retLen = Routon_IOWriteFrm( hPort, uchSnd, 3 );
	if ( retLen != 7 )
	{
		return 3;
	}
	if ( Routon_IOReadFrm( hPort, uchRead, &retLen) )
	{
		return 3;
	}
	//if ( retLen < 15 )
	//{
	//	return 3;
	//}
	*puiCHMsgLen = retLen - 3;
	memcpy (pucCHMsg, &uchRead[2], *puiCHMsgLen);
	
	return uchRead[0];
}
/*控制200-1的蜂鸣器和指示灯*/
int STDCALL	Routon_BeepLED(bool BeepON,bool LEDON,unsigned int duration)
{
	unsigned char uchRead[8];
	unsigned char uchSnd[3] = {0xC0,0xFF,0x00};

	if (BeepON)
		uchSnd[2]|=0x02;

	if (LEDON)
		uchSnd[2]|=0x01;
#if 0	
	FILE *file = fopen ("debug", "wb+");
	if (!file)
		return 0;
	if (fwrite (uchSnd, sizeof(uchSnd), 1, file) != 1)
	{
		return 0;
	}
	fclose (file);
#endif
	
	DWORD retLen = Routon_IOWriteFrm( hPort, uchSnd, 3 );
	if (retLen != 7 )
	{
		uchSnd[2]=0x00;
		retLen = Routon_IOWriteFrm( hPort, uchSnd, 3 );
		return 3;
	}
	if ( Routon_IOReadFrm( hPort, uchRead, &retLen) )
	{
		uchSnd[2]=0x00;
		retLen = Routon_IOWriteFrm( hPort, uchSnd, 3 );
		return 3;
	}
	if (uchRead[0]!=0x58)
	{
		uchSnd[2]=0x00;
		retLen = Routon_IOWriteFrm( hPort, uchSnd, 3 );
		return 0;
	}
	
	Sleep(duration);
	uchSnd[2]=0x00;

	retLen = Routon_IOWriteFrm( hPort, uchSnd, 3 );
	if (retLen != 7 )
	{
		uchSnd[2]=0x00;
		retLen = Routon_IOWriteFrm( hPort, uchSnd, 3 );
		return 3;
	}
	if ( Routon_IOReadFrm( hPort, uchRead, &retLen) )
	{
		return 3;
	}

	if (uchRead[0]!=0x58)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

 int STDCALL dc_init(int port,long baud)
 {
	 SDT_ClosePort ( port );
	 if ( (port<=16) && (port>=1) )
	 {
		 if (0 == g_iCurBaudRate)
		 {
//			 if (0x90 != SDT_GetCOMBaud(port, &g_iCurBaudRate))
//				 return -1;
			 g_iCurBaudRate=115200;//读Type A卡打开端口时将波特率固定设为115200
		 }

#if 0
		 char mssg[256]={0};
		 sprintf(mssg,"%d",g_iCurBaudRate);
		 
		 MessageBox(NULL,mssg,"debug1",MB_OK);
		 g_iCurBaudRate=(unsigned int)baud;
		 sprintf(mssg,"%d",g_iCurBaudRate);
		 
		 MessageBox(NULL,mssg,"debug2",MB_OK);
#endif
		 HANDLE hComm = InitCom (port, g_iCurBaudRate);
		 if (hComm)
		 {
			 hPort.hRead = hPort.hWrite= hComm;
			 hPort.IOType = 1;
			 g_SN=0;
			 gPort = port;
			 ghPort[port-1]=hPort;
			 return port;
		 }
		 else
		 {
			 return -1;
		 }
	}
	 else
		return -1;

  }
 

 int STDCALL dc_exit(int dev)
 {
        if ( dev > 0 && dev < 17 )
        {
                SDT_ClosePort( dev );
				ghPort[dev-1].hRead=0;
				ghPort[dev-1].hWrite=0;
				ghPort[dev-1].IOType=0;

                return 0;
        }
		else		
			return -1;
 }
int STDCALL dc_request(int icdev,unsigned char _Mode,unsigned int *TagType)
{
	if (!(ghPort[icdev-1].IOType>0&&ghPort[icdev-1].hRead>0&&ghPort[icdev-1].hWrite>0))
		return -32;

	unsigned char uchRead[2048] = {0};
	unsigned char uchSndStd[3] = {0x12,0xFF,0xFF};
	unsigned char uchSndAll[3] = {0x13,0xFF,0xFF};
	//unsigned char uchSndSpc[3] = {0x00,0xFF,0xFF};
	DWORD retLen;

	if (_Mode==0)
	{
		retLen = Routon_IOWriteFrm( ghPort[icdev-1], uchSndStd, 3 );
		if ( retLen != 7 )
		{

			return -1;
		}
		if ( Routon_IOReadFrm( ghPort[icdev-1], uchRead, &retLen) )
		{
			return -2;
		}
		if ((uchRead[0]==0x12&&uchRead[1]==0xFF))
		{
			*TagType=uchRead[2]+(uchRead[3]*10);
			return 0;
			
		}
		else 
			return -3;
	}
	else
		if (_Mode==1)
		{

			retLen = Routon_IOWriteFrm( ghPort[icdev-1], uchSndAll, 3 );
			if ( retLen != 7 )
			{
				
				return -1;
			}
			if ( Routon_IOReadFrm( ghPort[icdev-1], uchRead, &retLen) )
			{
				return -2;
			}
			if ((uchRead[0]==0x13&&uchRead[1]==0xFF))
			{
				*TagType=uchRead[2]+(uchRead[3]*10);
				return 0;
				
			}
			else 
				return -3;

		}
		else
			return -4;

	
}
int STDCALL dc_anticoll(int icdev,unsigned char _Bcnt,unsigned long *_Snr)
{
	if (!(ghPort[icdev-1].IOType>0&&ghPort[icdev-1].hRead>0&&ghPort[icdev-1].hWrite>0))
		return -32;

	unsigned char uchRead[2048] = {0};
	unsigned char uchSnd[3] = {0x14,0xFF,0xFF};
	DWORD retLen;
	
	if (_Bcnt==0)
	{
		retLen = Routon_IOWriteFrm( ghPort[icdev-1], uchSnd, 3 );
		if ( retLen != 7 )
		{
			
			return -1;
		}
		if ( Routon_IOReadFrm( ghPort[icdev-1], uchRead, &retLen) )
		{
			return -2;
		}
		if ((uchRead[0]==0x13&&uchRead[1]==0xFF))
		{
			memcpy (_Snr, &uchRead[2], retLen - 3);
			return 0;

		}
		else 
			return -3;
	}
	else
		return -4;
		

}
int STDCALL dc_select(int icdev,unsigned long _Snr,unsigned char *_Size)
{
	if (!(ghPort[icdev-1].IOType>0&&ghPort[icdev-1].hRead>0&&ghPort[icdev-1].hWrite>0))
		return -32;

	unsigned char uchRead[2048] = {0};
	unsigned char uchSnd[6] = {0x15,0xFF};
	uchSnd[5] = (unsigned char)(_Snr >> 24) & 0xff;
	uchSnd[4] = (unsigned char)(_Snr >> 16) & 0xff;
	uchSnd[3] = (unsigned char)(_Snr >> 8) & 0xff;
	uchSnd[2] = (unsigned char)_Snr  & 0xff;
	DWORD retLen;
	
		retLen = Routon_IOWriteFrm( ghPort[icdev-1], uchSnd, 6 );
/*
		char mssg[256]={0};
		sprintf(mssg,"%d",retLen);
		
		MessageBox(NULL,mssg,"debug",MB_OK);
*/
		if ( retLen != 10 )
		{
			
			return -1;
		}
		if ( Routon_IOReadFrm( ghPort[icdev-1], uchRead, &retLen) )
		{
			return -2;
		}
		if ((uchRead[0]==0x56&&uchRead[1]==0xFF))
		{
			g_SN=_Snr;
			return 0;
			
		}
		else 
			return -3;
	
	
}
int STDCALL dc_authentication_passaddr(int icdev, unsigned char _Mode, unsigned char Addr, unsigned char *passbuff)
{
	if (!(ghPort[icdev-1].IOType>0&&ghPort[icdev-1].hRead>0&&ghPort[icdev-1].hWrite>0))
		return -32;

	if (g_SN<=0)
		return -1;
	if (_Mode!=0x61&&_Mode!=0x60)
		return -2;

	unsigned char uchRead[2048] = {0};
	unsigned char uchSnd[15] = {0x16,0xFF};
	unsigned char SNO,BNO;
	
	SNO=0;
	BNO=0;
	SNO=Addr/4;
	BNO=Addr%4;

	uchSnd[2]=SNO;
	uchSnd[3]=BNO;
	uchSnd[4]=_Mode;

	uchSnd[5]=passbuff[0];
	uchSnd[6]=passbuff[1];
	uchSnd[7]=passbuff[2];
	uchSnd[8]=passbuff[3];
	uchSnd[9]=passbuff[4];
	uchSnd[10]=passbuff[5];

	uchSnd[14] = (unsigned char)(g_SN >> 24) & 0xff;
	uchSnd[13] = (unsigned char)(g_SN >> 16) & 0xff;
	uchSnd[12] = (unsigned char)(g_SN >> 8) & 0xff;
	uchSnd[11] = (unsigned char)g_SN  & 0xff;
	DWORD retLen;
	


		retLen = Routon_IOWriteFrm( ghPort[icdev-1], uchSnd, 15 );
/*
		char mssg[256]={0};
		sprintf(mssg,"%d",retLen);
		
		MessageBox(NULL,mssg,"debug",MB_OK);
*/
		if ( retLen != 19 )
		{
			
			return -3;
		}
		if ( Routon_IOReadFrm( ghPort[icdev-1], uchRead, &retLen) )
		{
			return -4;
		}
		if ((uchRead[0]==0x56&&uchRead[1]==0xFF))
		{
			return 0;
			
		}
		else 
			return -5;
}

int STDCALL dc_read(int icdev,unsigned char _Adr,unsigned char *_Data)
{
	if (!(ghPort[icdev-1].IOType>0&&ghPort[icdev-1].hRead>0&&ghPort[icdev-1].hWrite>0))
		return -32;

	if (!_Data)
		return -1;
	unsigned char uchRead[2048] = {0};
	unsigned char uchSnd[4] = {0x17,0xFF};
	unsigned char SNO,BNO;
	
	SNO=0;
	BNO=0;
	SNO=_Adr/4;
	BNO=_Adr%4;

	uchSnd[2]=SNO;
	uchSnd[3]=BNO;
	DWORD retLen;
	


		retLen = Routon_IOWriteFrm( ghPort[icdev-1], uchSnd, 4 );
/*
		char mssg[256]={0};
		sprintf(mssg,"%d",retLen);
		
		MessageBox(NULL,mssg,"debug",MB_OK);
*/
		if ( retLen != 8 )
		{
			
			return -3;
		}
		if ( Routon_IOReadFrm( ghPort[icdev-1], uchRead, &retLen) )
		{
			return -4;
		}
		if ((uchRead[0]==0x17&&uchRead[1]==0xFF))
		{
			memcpy (_Data, &uchRead[2], 16);
			return 0;
			
		}
		else 
			return -5;
}

int STDCALL dc_write(int icdev,unsigned char _Adr,unsigned char *_Data)
{
	if (!(ghPort[icdev-1].IOType>0&&ghPort[icdev-1].hRead>0&&ghPort[icdev-1].hWrite>0))
		return -32;

	if (!_Data)
		return -1;
	unsigned char uchRead[2048] = {0};
	unsigned char uchSnd[20] = {0x18,0xFF};
	unsigned char SNO,BNO;
	
	SNO=0;
	BNO=0;
	SNO=_Adr/4;
	BNO=_Adr%4;

	uchSnd[2]=SNO;
	uchSnd[3]=BNO;

	uchSnd[4]=_Data[0];
	uchSnd[5]=_Data[1];
	uchSnd[6]=_Data[2];
	uchSnd[7]=_Data[3];
	uchSnd[8]=_Data[4];
	uchSnd[9]=_Data[5];
	uchSnd[10]=_Data[6];
	uchSnd[11]=_Data[7];
	uchSnd[12]=_Data[8];
	uchSnd[13]=_Data[9];
	uchSnd[14]=_Data[10];
	uchSnd[15]=_Data[11];
	uchSnd[16]=_Data[12];
	uchSnd[17]=_Data[13];
	uchSnd[18]=_Data[14];
	uchSnd[19]=_Data[15];

	DWORD retLen;
#if 0	
	clock_t start1, end1,end2,end3,end4;
#endif
		retLen = Routon_IOWriteFrm( ghPort[icdev-1], uchSnd, 20 );

/*
		char mssg[256]={0};
		sprintf(mssg,"%d",retLen);
		
		MessageBox(NULL,mssg,"debug",MB_OK);
*/
		if ( retLen != 24 )
		{
			
			return -3;
		}
#if 0
		start1=clock();
#endif
		if ( Routon_IOReadFrm( ghPort[icdev-1], uchRead, &retLen) )
		{
			return -4;
		}
#if 0
		end1=clock();
		char mssg[256]={0};
		sprintf(mssg,"%2.1f%s",(double)(end1-start1)/CLOCKS_PER_SEC,"秒");
		
		//MessageBox(NULL,mssg,"Routon_IOWriteFrm",MB_OK);
#endif
		if ((uchRead[0]==0x56&&uchRead[1]==0xFF))
		{
			return 0;
			
		}
		else 
			return -5;
}

int STDCALL dc_halt(int icdev)
{
	if (!(ghPort[icdev-1].IOType>0&&ghPort[icdev-1].hRead>0&&ghPort[icdev-1].hWrite>0))
		return -32;

	unsigned char uchRead[2048] = {0};
	unsigned char uchSnd[3] = {0x11,0xFF,0xFF};
	DWORD retLen;
	
	retLen = Routon_IOWriteFrm( ghPort[icdev-1], uchSnd, 3 );
	if ( retLen != 7 )
	{
		
		return -1;
	}
	if ( Routon_IOReadFrm( ghPort[icdev-1], uchRead, &retLen) )
	{
		return -2;
	}
	if ((uchRead[0]==0x56&&uchRead[1]==0xFF))
	{
		return 0;
		
	}
	else 
		return -3;
	
}

int STDCALL dc_BeepLED(int icdev,bool BeepON,bool LEDON,unsigned int duration)
{
	if (!(ghPort[icdev-1].IOType>0&&ghPort[icdev-1].hRead>0&&ghPort[icdev-1].hWrite>0))
		return -32;

	unsigned char uchRead[8];
	unsigned char uchSnd[3] = {0xC0,0xFF,0x00};
	
	if (BeepON)
		uchSnd[2]|=0x02;
	
	if (LEDON)
		uchSnd[2]|=0x01;

	
	DWORD retLen = Routon_IOWriteFrm( ghPort[icdev-1], uchSnd, 3 );
	if (retLen != 7 )
	{
		uchSnd[2]=0x00;
		retLen = Routon_IOWriteFrm( ghPort[icdev-1], uchSnd, 3 );
		return -3;
	}
	if ( Routon_IOReadFrm( ghPort[icdev-1], uchRead, &retLen) )
	{
		uchSnd[2]=0x00;
		retLen = Routon_IOWriteFrm( ghPort[icdev-1], uchSnd, 3 );
		return -3;
	}
	if (uchRead[0]!=0x58)
	{
		uchSnd[2]=0x00;
		retLen = Routon_IOWriteFrm( ghPort[icdev-1], uchSnd, 3 );
		return 0;
	}
	
	Sleep(duration);
	uchSnd[2]=0x00;
	
	retLen = Routon_IOWriteFrm( ghPort[icdev-1], uchSnd, 3 );
	if (retLen != 7 )
	{
		uchSnd[2]=0x00;
		retLen = Routon_IOWriteFrm( ghPort[icdev-1], uchSnd, 3 );
		return 3;
	}
	if ( Routon_IOReadFrm( ghPort[icdev-1], uchRead, &retLen) )
	{
		return 3;
	}
	
	if (uchRead[0]!=0x58)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
#if 0
int STDCALL dc_format(int icdev,unsigned char _Adr,unsigned char _Mode, unsigned char *passbuff,unsigned char *_Data)
{
	if (!(ghPort[icdev-1].IOType>0&&ghPort[icdev-1].hRead>0&&ghPort[icdev-1].hWrite>0))
		return -32;

	if (!_Data)
		return -1;
	unsigned char uchRead[2048] = {0};
	unsigned char uchSnd[20] = {0x18,0xFF};
	unsigned char SNO,BNO;
	
	SNO=0;
	BNO=0;
	SNO=_Adr/4;
	BNO=_Adr%4;

	uchSnd[2]=SNO;
	uchSnd[3]=BNO;

	uchSnd[4]=_Data[0];
	uchSnd[5]=_Data[1];
	uchSnd[6]=_Data[2];
	uchSnd[7]=_Data[3];
	uchSnd[8]=_Data[4];
	uchSnd[9]=_Data[5];
	uchSnd[10]=_Data[6];
	uchSnd[11]=_Data[7];
	uchSnd[12]=_Data[8];
	uchSnd[13]=_Data[9];
	uchSnd[14]=_Data[10];
	uchSnd[15]=_Data[11];
	uchSnd[16]=_Data[12];
	uchSnd[17]=_Data[13];
	uchSnd[18]=_Data[14];
	uchSnd[19]=_Data[15];

	DWORD retLen;
	


		retLen = Routon_IOWriteFrm( ghPort[icdev-1], uchSnd, 20 );
/*
		char mssg[256]={0};
		sprintf(mssg,"%d",retLen);
		
		MessageBox(NULL,mssg,"debug",MB_OK);
*/
		if ( retLen != 24 )
		{
			
			return -3;
		}
		if ( Routon_IOReadFrm( ghPort[icdev-1], uchRead, &retLen) )
		{
			return -4;
		}
		if ((uchRead[0]==0x56&&uchRead[1]==0xFF))
		{
			return 0;
			
		}
		else 
			return -5;
}
#endif