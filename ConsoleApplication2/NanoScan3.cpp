// ConsoleApplication2.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#define _WINSOCK_DEPRECATED_NO_WARNINGS




#include <iostream>
#include "Socekt.h"
#include <cmath>
#include <cstring>
#include <ctime>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;


int main(void)
{
	
	
	
	BOOL bReceive[5] = { FALSE, }; //분할된 패킷 수신 확인용
	memset(bReceive, FALSE, sizeof(bReceive));

	
	ST_HEADER st1; //header data 받아와서 분류
	ST_HDATA st_oHeader; // data output header 분류
	ST_CONFIG st_config; //config output
	ST_MDATA st_mData; //measurement data output
	ST_1 st_beam; //ST_MDATA의 동적할당부분

	int Cx = 500; //mat img 가운데점
	int Cy = 500;
	Mat img;

	char mData[totalLen] = { 0, };   //합친 패킷 저장
	unsigned long id1 = 0;			//같은 Packet 확인용
	int nOffset = 0;			    //패킷의 offset
	double dx[NOBD] = { 0, };		//각 빔들 좌표 저장할 배열
	double dy[NOBD] = { 0, };

	while (1)
	{
			   	
		int nOffset = 0;
		try
		{
			WSASession Session;
			UDPSocket Socket;
			char buffer[1460]; 
			Socket.Bind(6060); 
			while (1)
			{
			
				img = Mat::zeros(1000, 1000, CV_8UC3);
				line(img, Point(500, 0), Point(500, 1000), Scalar(0, 0, 255));
				line(img, Point(0, 500), Point(1000, 500), Scalar(0, 0, 255));
				circle(img, Point(500, 500), 10, Scalar(0, 0, 255), 1, 8, 0);
				circle(img, Point(500, 500), 25, Scalar(0, 0, 255), 1, 8, 0);
				circle(img, Point(500, 500), 50, Scalar(0, 0, 255), 1, 8, 0);
				circle(img, Point(500, 500), 100, Scalar(0, 0, 255), 1, 8, 0);
				circle(img, Point(500, 500), 200, Scalar(0, 0, 255), 1, 8, 0);
				circle(img, Point(500, 500), 300, Scalar(0, 0, 255), 1, 8, 0);
				circle(img, Point(500, 500), 400, Scalar(0, 0, 255), 1, 8, 0);
				circle(img, Point(500, 500), 500, Scalar(0, 0, 255), 1, 8, 0);
				imshow("LidarPoint", img);

				sockaddr_in add = Socket.RecvFrom(buffer, sizeof(buffer));
				std::cout <<"Message : " <<buffer << std::endl;

				int nOffset = 0;
				memcpy(&st1, &buffer[nOffset], sizeof(st1));
				if (st1.Fragment_offset == 0)
				{
					id1 = st1.Identification;
				}
				nOffset += sizeof(st1); 

				//thorugh the Fragment Offset , make the total_data_packet
				if (id1 == st1.Identification)
				{
					switch (st1.Fragment_offset)
					{
					case pfirst: //1번째 패킷 offset:0

						memcpy(&mData[pfirst], &buffer[nOffset], datalen);
						bReceive[0] = TRUE;
						break;
					case psecond: //2번째 패킷 offset:1436                    3:2872 4:4308 5:5744 6:7180
						memcpy(&mData[psecond], &buffer[nOffset], datalen);
						bReceive[1] = TRUE;
						break;
					case pthird: //3번째 패킷 offset : 2872
						memcpy(&mData[pthird], &buffer[nOffset], datalen);
						bReceive[2] = TRUE;
						break;
					case pfour: //4번째 패킷 offset : 4308
						memcpy(&mData[pfour], &buffer[nOffset], datalen);
						bReceive[3] = TRUE;
						break;
					case pfive: //5번째 패킷 offset : 5744
						memcpy(&mData[pfive], &buffer[nOffset], lastlen);
						bReceive[4] = TRUE;
						break;
						//case plast: //마지막 6번째 패킷 
							//memcpy(&mData[plast], &Buffer[mOffset], lastlen);
							//bReceive[5] = TRUE;
							//pCount += 1;
					default:
						break;
					}
				}
				//모두 TRUE일때 나눠진 패킷 수신완료
				BOOL bAllReceive = bReceive[0] & bReceive[1] & bReceive[2] & bReceive[3] & bReceive[4];

				//if complete packet, Put in data to each Structure
				if (bAllReceive == TRUE)
				{
					memcpy(&st_oHeader, &mData, sizeof(st_oHeader));
					memcpy(&st_config, &mData[configOff], sizeof(st_config));
					memcpy(&st_mData, &mData[dataOff], sizeof(st_mData));

					double angle = sAngle; //시작각도
					int beamOff = oBeam; //beam의 distance 값이 시작하는 위치

					for (int i = 0; i < NOBD; i++) {

						double dRadian = DEGREE_TO_RADIAN(angle); //호도법
						memcpy(&st_beam, &mData[beamOff], sizeof(st_beam));
						beamOff += sizeof(st_beam);
						dx[i] = cos(dRadian) * st_beam.Distance; 
						dy[i] = sin(dRadian) * st_beam.Distance;
						circle(img, Point(500 + dx[i] / 10, 500 - dy[i] / 10), 1, Scalar(255, 255, 0), 1, 8, 0);
						angle += rAngular; //다음각도

					}
					imshow("LidarPoint", img); //찍은좌표 갱신
					if (waitKey(27) == 0) {
						break;
					}
					//초기화
					memset(bReceive, FALSE, sizeof(bReceive));
					memset(&st1, 0, sizeof(st1));
					memset(&mData, 0, sizeof(mData));
					memset(&st_oHeader, 0, sizeof(st_oHeader));
					memset(&st_config, 0, sizeof(st_config));
					memset(&st_beam, 0, sizeof(st_beam));
					id1 = 0;
				}

			}

		}
		catch (std::system_error& e)
		{
			std::cout << e.what();
		}
	}
	return 0;

	
	
	
	
}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
