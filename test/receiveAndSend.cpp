//
// Created by JinxBIGBIG on 2022/8/3.
//
#include <sstream>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "positionInfo.h"
#include "OdrManager.hh"
#include "ShareMessage.h"
//链接静态库
#pragma comment (lib,"ws2_32.lib")


using namespace std;
using namespace OpenDrive;

int sendTo(char *sendBuf, short sendHostShort, const char *sendIP)
{

    WSADATA wdata;

    WORD wVersion;

    wVersion = MAKEWORD(2, 2);

    WSAStartup(wVersion, &wdata);

    if (HIBYTE(wdata.wVersion) != 2 || LOBYTE(wdata.wVersion) != 2)
    {
        return -1;
    }

    sockaddr_in sClient;

    sClient.sin_family = AF_INET;
    sClient.sin_port = htons(sendHostShort);

    //inet_pton(AF_INET, "127.0.0.1", &sClient.sin_addr);
    sClient.sin_addr.S_un.S_addr = inet_addr(sendIP);

    SOCKET psock = socket(AF_INET, SOCK_DGRAM, 0);

    int len = sizeof(sClient);

    //char sendBuf[128];
    while (1)
    {

        //memset(sendBuf, 0, sizeof(sendBuf));

        //cout << "pelase input word:";

        //cin.getline(sendBuf, 64);
        sendto(psock, sendBuf, sizeof(sendBuf), 0, (sockaddr*)&sClient, len);
        //cout << "Send over;" << endl;

    }
    return 0;

}

int main()
{

    OpenDrive::OdrManager manager;
    Point point;
    PointLaneInfo pointLaneInfo;
    string xodrPath = "..\\map2.xodr";
    //string xodrPath = "..\\data\\map.xodr";
    bool xodrLoad = manager.loadFile(xodrPath);
    OpenDrive::Position* pos = manager.createPosition();
    manager.activatePosition(pos);
    char const *receiveIP = "127.0.0.1";
    char const *sendIP = "127.0.0.1";
    short receiveHostShort;
    short sendHostShort;
    cout << "Please input receiveHostPort(like:9999) and sendHostPort(like:9999)" << endl;
    cin >> receiveHostShort >> sendHostShort;
    //cout << receiveHostShort << ";" << sendHostShort << "over." << endl;

    WSADATA  wsData;
    int nret = WSAStartup(MAKEWORD(2, 2), &wsData);
    if(nret!=0)
        return nret;
    sockaddr_in sa,recSa;
    int len = sizeof(sa);
    sa.sin_addr.S_un.S_addr = inet_addr(receiveIP);
    sa.sin_family = AF_INET;
    sa.sin_port = htons(receiveHostShort);
    SOCKET  sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock==INVALID_SOCKET)
        return WSAGetLastError();

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
            cout << "The information :" << buf << endl;
            istringstream str(buf);
            double out;
            int i = 0;
            double p[3];
            while (str >> out) {
                //cout << out << endl;
                p[i] = out;
                i++;
            }
            point = {p[0], p[1], p[2]};
            //cout << "(" << point.x << ", "<< point.y << ", "<< point.z << ")" << endl;
            manager.setInertialPos(point.x, point.y, point.z);
            bool result = manager.inertial2lane();
            cout << "Position initialing result :" << result << endl;
            int roadID = manager.getRoadHeader()->mId;
            string str1 = "";
            str1 += to_string(roadID);
            char* sendBuf = const_cast<char *>(str1.data());
            cout << "sendBuf: "<< *sendBuf << endl;
            sendTo(sendBuf, sendHostShort, sendIP);
        }
    }
    /*int count = 1;
    string str = "";
    str += to_string(count);
    char* roadID = const_cast<char *>(str.data());
    sendTo(roadID, sendHostShort, sendIP);*/
}
