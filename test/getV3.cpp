//
// Created by JinxBIGBIG on 2022/7/28.
//

#include "OdrManager.hh"
#include "positionInfo.h"
#include "ShareMessage.h"
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include<WinSock2.h>
#include<WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

using namespace std;
using namespace OpenDrive;


int main(int argc, char** argv) {
    OpenDrive::OdrManager manager;
    ShareMessage shareMessage;
    string xodrPath = "..\\data\\Germany_2018.xodr";
    bool xodrLoad = manager.loadFile(xodrPath);
    cout << "xodrLoad: " << xodrLoad << endl;
//    string xodrPath = "..\\data\\Germany_2018.xodr";
//    Point startPoint(5915.00, -2937.76, 0); //roadId:40,laneId:-1
//    Point endPoint(6031.85, -3539.57, 0); //roadId:22,laneId:-2
//    Point startPoint(2996.44, -3023.28, 0);
//    Point endPoint(3111.44, -3003.28, 0);

    //单独使用manager获取相关信息测

    Point startPoint(5915.00, -2937.76, 0);
    PositionInfo posInfo;
    GetPositionInfo posInfoManager(manager,startPoint, posInfo);
    posInfoManager.GetInertialPosInfo(posInfoManager.myManager, posInfo);
    posInfo.road->printData();

    //加载套接字库
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(1, 1);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0)
    {
        return -1;
    }

    if (LOBYTE(wsaData.wVersion) != 1 ||     //低字节为主版本
        HIBYTE(wsaData.wVersion) != 1)      //高字节为副版本
    {
        WSACleanup();
        return -1;
    }

    printf("Client is operating!\n\n");
    //1.创建用于监听的套接字
    SOCKET sockSrv = socket(AF_INET, SOCK_DGRAM, 0);

    //2.确定服务端通讯地址信息
    sockaddr_in  addrSrv;
    inet_addr("127.0.0.1");
    //addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//输入你想通信的她（此处是本机内部）
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(6000);


    int len = sizeof(SOCKADDR);

    char recvBuf[100];    //收
    char sendBuf[100];    //发
    char tempBuf[100];    //存储中间信息数据

    while (1)
    {

        printf("Please input data: ");
        gets_s(sendBuf);
        //3.发送数据
        sendto(sockSrv, sendBuf, strlen(sendBuf) + 1, 0, (SOCKADDR*)&addrSrv, len);
        //4.等待数据
        recvfrom(sockSrv, recvBuf, 100, 0, (SOCKADDR*)&addrSrv, &len);

        if ('q' == recvBuf[0])
        {
            sendto(sockSrv, "q", strlen("q") + 1, 0, (SOCKADDR*)&addrSrv, len);
            printf("Chat end!\n");
            break;
        }
        sprintf_s(tempBuf, "%s say : %s", "server", recvBuf);
        printf("%s\n", tempBuf);

    }
    closesocket(sockSrv);
    WSACleanup();




    return 0;
}







