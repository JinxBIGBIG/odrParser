//
// Created by JinxBIGBIG on 2022/7/12.
//

//
// Created by JinxBIGBIG on 2022/4/12.
//
#include "OdrManager.hh"
#include "positionInfo.h"
#include "ShareMessage.h"
#include <iostream>

using namespace std;
using namespace OpenDrive;


int main(int argc, char** argv) {
    OpenDrive::OdrManager manager;
    ShareMessage shareMessage;

//    string xodrPath = "..\\data\\Crossing8Course.xodr";
    string xodrPath = "..\\data\\Germany_2018.xodr";
    Point startPoint(5915.00, -2937.76, 0); //roadId:40,laneId:-1
    Point endPoint(6031.85, -3539.57, 0); //roadId:22,laneId:-2
//    Point startPoint(2996.44, -3023.28, 0);
//    Point endPoint(3111.44, -3003.28, 0);

//     string xodrPath = "..\\data\\Crossing8Course.xodr";
//     Point startPoint(21.86, -26.1, 0);
//     Point endPoint(68.03, 77.12, 0);

//    string xodrPath = "..\\data\\CAexample.xodr";
//    Point startPoint(634.46, 594.85, 0);
////    Point startPoint(1120.4, 591.41, 0);
//    Point endPoint(1238.77, 584.29, 0);

    double range = 2000;

    PositionInfo posInfo;

    GetPositionInfo posInfoManager(manager, xodrPath, startPoint, range, posInfo);

    shareMessage.GetSignals(posInfoManager);
    IEvalSignal signalInfo;
    cout << shareMessage;

    return 0;
}







