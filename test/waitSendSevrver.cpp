//
// Created by JinxBIGBIG on 2022/8/3.
//
#include<stdio.h>
#include <iostream>
#include <WinSock2.h>
#include<WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
int main()
{
    WORD wVersionRequested;//unsigned short
    WSADATA wsaData;//这个结构被用来存储被WSAStartup函数调用后返回的Windows Sockets数据
    int err;

    wVersionRequested = MAKEWORD(1,1);

    //WSAStartup就是为了向操作系统说明，我们要用哪个库文件，让该库文件与当前的应用程序绑定，从而就可以调用该版本的socket的各种函数了
    err = WSAStartup(wVersionRequested, &wsaData);

    if (err != 0)
    {
        return -1;
    }

    if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)//低字节为主版本，高字节为副版本
    {
        WSACleanup();
        return -1;
    }
//.......以上为准备环境....以下为具体服务端udp通讯流程..........
    std::cout << "server is operating!\n\n";
    //1.创建套接字
    SOCKET socketSrv = socket(AF_INET, SOCK_DGRAM, 0);

    //2.服务端地址
    SOCKADDR_IN addrSrv;
    addrSrv.sin_family = AF_INET;
    //inet_pton(AF_INET, "127.0.0.1", (void *)&addrSrv.sin_addr.S_un.S_addr);
    addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    addrSrv.sin_port = htons(6000);

    //3.套接字绑定
    bind(socketSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

    SOCKADDR_IN addrClient;//用来接收客户端地址信息
    char ClientIP[20];
    int len = sizeof(SOCKADDR);
    char recvBuf[100];
    char sendBuf[100];
    char tempBuf[100];

    while (1)
    {
        //4.等待并接收数据
        recvfrom(socketSrv, recvBuf, 100, 0, (SOCKADDR*)&addrClient, &len);
        if ('q' == recvBuf[0])
        {
            sendto(socketSrv, "q", strlen("q") + 1, 0, (SOCKADDR*)&addrClient, len);
            std::cout << "chat end!\n\n";
            break;
        }
        //inet_ntop(AF_INET, (void *)&addrClient.sin_addr, ClientIP, 16);
        inet_ntoa(addrClient.sin_addr);
        sprintf_s(tempBuf, "%s say : %s", ClientIP, recvBuf);
        printf("%s\n",tempBuf);




        //5.发送数据
        std::cout << "Please input data:";
        gets_s(sendBuf, 64);
        sendto(socketSrv, sendBuf, strlen(sendBuf) + 1, 0, (SOCKADDR*)&addrClient, len);


    }
    //6.关闭资源
    closesocket(socketSrv);
    WSACleanup();
    return 0;

}

