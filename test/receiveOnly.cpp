//
// Created by JinxBIGBIG on 2022/8/3.
//

#include <iostream>

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment (lib,"ws2_32.lib")
using namespace  std;

int main()
{
    WSADATA  wsData;

    int nret=WSAStartup(MAKEWORD(2, 2), &wsData);
    if(nret!=0)
    {
        return nret;
    }

    sockaddr_in sa,recSa;

    int len = sizeof(sa);

    sa.sin_addr.S_un.S_addr = INADDR_ANY;

    sa.sin_family = AF_INET;
    sa.sin_port = htons(9999);

    SOCKET  sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock==INVALID_SOCKET)
    {
        return WSAGetLastError();
    }

    bind(sock, (sockaddr*)&sa, len);

    while (true)
    {
        char buf[1024];

        memset(buf, 0, 1024);

        int nlen = recvfrom(sock, buf, 1024, 0, (sockaddr*)&recSa, &len);

        if (nlen>0)
        {
            //char sIP[20];
            //inet_ntop(AF_INET, &recSa.sin_addr, sIP, 20);
            inet_ntoa(recSa.sin_addr);
            cout << buf << endl;
        }
    }
}
