#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <system_error>
#include <string>
#include <iostream>
#include "opencv2/opencv.hpp"

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)
#pragma once

//--------length
#define bufLen 1460
#define datalen 1436
#define lastlen 1004
#define totalLen 6748

//packet offset 1st=0
#define pfirst 0
#define psecond 1436 
#define pthird 2872 
#define pfour 4308 
#define pfive 5744
#define plast 7180
#define oBeam 128  //beam's distance offset
//packet block data offset

#define configOff 96
#define dataOff 124 

//number of beam
#define NOBD 1651

//Angular Range
#define rAngular 0.1665657177468201

//Start Angle
#define sAngle -47.5 //-47.5 
#define eAngle 227.5
#define tAngle 275


#define OPI 3.141592 
#define DEGREE_TO_RADIAN(degree) ((OPI/180)*(degree)) // 60분법 -> 호도법 
#define RADIAN_TO_DEGREE(radian) ((180/OPI)*(radian)) // 호도법 -> 60분법
#define PORT 6060
#define BUFFER_SIZE 1460

#pragma comment(lib,"ws2_32.lib")

typedef struct _ST_HEADER
{
	unsigned char datagram_marker[4];
	unsigned char protocol[2];
	//wchar_t protocol;
	unsigned char Version_maj;
	unsigned char Version_min;
	unsigned int Total_Length;
	unsigned int Identification;
	unsigned int Fragment_offset;
	unsigned char Reserve[4];


} ST_HEADER;

//Data_output_Header
typedef struct _ST_DATA
{
	
	// Version info
	unsigned char Version;
	unsigned char Major_Version;
	unsigned char Minor_Version;
	unsigned char Release;
	//device serial num
	unsigned long Serial_Num;
	//device serial num2
	unsigned long Serial_Num2; //serial num for system plug 
	//channel number
	unsigned char channel_num;
	unsigned char Reserve1[3];
	//Sequence number
	unsigned long Sequence_num;
	//Scan number
	unsigned long Scan_num;
	//Time data
	wchar_t tDate;      //time data date
	unsigned char tReserve[2]; //titme data reserve
	unsigned long tTime; //time data time

	//Block device status
	wchar_t Status_offset;
	wchar_t Status_size;
	//Block configuration of data output
	wchar_t Config_offset;
	wchar_t Config_size;
	//Block measurement data
	wchar_t mData_offset;
	wchar_t mData_size;
	//Block field interruption
	wchar_t fInter_offset;
	wchar_t fInter_size;
	//Blcok application data
	wchar_t AppData_offset;
	wchar_t AppData_size;
	//Block Local_I/O
	wchar_t LIO_offset;
	wchar_t LIO_size;




}ST_HDATA;

typedef struct _ST_BLOCK_STATUS
{
	//Data from assembly 113 device status
	unsigned char dStatus[16];

}ST_STATUS;

typedef struct _ST_BLOCK_CONFIG
{
	//Configuration of the data output
	wchar_t Factor;
	wchar_t cNumberOfBeams; //config NOB
	wchar_t Scan_Cycle_Time; //ms
	unsigned char Reserved_config[2];
	long Start_Angle; //devided by 4194304
	long Angular_Resolution; //devided by 4194304
	unsigned long Beam_Interval; //us
	unsigned char Reserved_confing2[4];

}ST_CONFIG;
/*
typedef struct _ST_BLOCK_DATA
{
	unsigned long dNumberOfBeams;
	wchar_t Distance; //mm
	unsigned char RSSI;

}ST_MDATA;*/

typedef struct _ST_1
{
	wchar_t Distance; //mm
	unsigned char RSSI;
	unsigned char Status;

}ST_1;

typedef struct _ST_BLOCK_DATA
{
	unsigned long dNumberOfBeams;
	ST_1 *pData;

}ST_MDATA;

class WSASession
{
public:
	WSASession()
	{
		int ret = WSAStartup(MAKEWORD(2, 2), &data);
		if (ret != 0)
			throw std::system_error(WSAGetLastError(), std::system_category(), "WSAStartup Failed");
	}
	~WSASession()
	{
		WSACleanup();
	}
private:
	WSAData data;
};

class UDPSocket
{
public:
	UDPSocket()
	{
		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (sock == INVALID_SOCKET)
			throw std::system_error(WSAGetLastError(), std::system_category(), "Error opening socket");
	}
	~UDPSocket()
	{
		closesocket(sock);
	}

	void SendTo(const std::string& address, unsigned short port, const char* buffer, int len, int flags = 0)
	{
		sockaddr_in add;
		add.sin_family = AF_INET;
		add.sin_addr.s_addr = inet_addr(address.c_str());
		add.sin_port = htons(port);
		int ret = sendto(sock, buffer, len, flags, reinterpret_cast<SOCKADDR *>(&add), sizeof(add));
		if (ret < 0)
			throw std::system_error(WSAGetLastError(), std::system_category(), "sendto failed");
	}
	void SendTo(sockaddr_in& address, const char* buffer, int len, int flags = 0)
	{
		int ret = sendto(sock, buffer, len, flags, reinterpret_cast<SOCKADDR *>(&address), sizeof(address));
		if (ret < 0)
			throw std::system_error(WSAGetLastError(), std::system_category(), "sendto failed");
		std::cout << "\n";
	}
	sockaddr_in RecvFrom(char* buffer, int len, int flags = 0)
	{
		sockaddr_in from;
		int size = sizeof(from);
		int ret = recvfrom(sock, buffer, len, flags, reinterpret_cast<SOCKADDR *>(&from), &size);
		if (ret < 0)
			throw std::system_error(WSAGetLastError(), std::system_category(), "recvfrom failed");

		// make the buffer zero terminated
		buffer[ret] = 0;
		return from;
	}
	void Bind(unsigned short port)
	{
		sockaddr_in add;
		add.sin_family = AF_INET;
		add.sin_addr.s_addr = htonl(INADDR_ANY);
		add.sin_port = htons(port);

		int ret = bind(sock, reinterpret_cast<SOCKADDR *>(&add), sizeof(add));
		if (ret < 0)
			throw std::system_error(WSAGetLastError(), std::system_category(), "Bind failed");
	}

	
	
	

private:
	SOCKET sock;
};