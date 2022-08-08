//
// Created by JinxBIGBIG on 2022/7/13.
//

#include "OdrManager.hh"
#include "positionInfo.h"
#include "ShareMessage.h"
#include <iostream>

using namespace std;
using namespace OpenDrive;

const double EPS = 1e-8;

int main(int argc, char** argv) {
    OpenDrive::OdrManager manager;
    ShareMessage shareMessage;

//    string xodrPath = "..\\data\\Germany_2018.xodr";
//    Point startPoint(5915.00, -2937.76, 0); //roadId:40,laneId:-1
//    Point endPoint(6031.85, -3539.57, 0); //roadId:22,laneId:-2
//    Point startPoint(2996.44, -3023.28, 0);
//    Point endPoint(3111.44, -3003.28, 0);

    /* string xodrPath = "..\\data\\Crossing8Course.xodr";
     Point startPoint(21.86, -26.1, 0);
     Point endPoint(68.03, 77.12, 0);*/

      string xodrPath = "..\\data\\IVISTA.xodr";
      //Point startPoint(6776.5,-85.91, 0);
//    Point endPoint(6803,-60, 0);

//    Point startPoint(6792.5,3021.76, 0);
//    Point endPoint(6880.03,2934.45, 0);

//    Point startPoint(3888,2928.8, 0);
//    Point endPoint(2209.5,2928.7, 0);

    Point startPoint( -650.458, 2932.05, 0);//(1335, -2)
    //Point startPoint( -686.621, 2935.358, 0);
    Point endPoint(-595.191,3019.829, 0);

    //单独使用manager获取相关信息测试
    /*bool xodrLoad = manager.loadFile(xodrPath);
    OpenDrive::Position* pos = manager.createPosition();
    manager.activatePosition(pos);
    cout << "ll: " << startPoint.x << "; " << startPoint.y << "; " << startPoint.z << endl;
    manager.setInertialPos(startPoint.x, startPoint.y, startPoint.z);
    bool result = manager.inertial2lane();
    manager.getRoadHeader()->printData();
    manager.getLane()->printData();
    manager.setLanePos(1335, -1, 0, 0);
    manager.lane2track();
    manager.track2inertial();
    Point point;
    point = {manager.getInertialPos().getX(), manager.getInertialPos().getY(), manager.getInertialPos().getZ()};
    //(startPoint.x - point.x)== EPS||(startPoint.y - point.y) == EPS
    cout << point.x << "; " << point.y << "; " << point.z << ";  " << ((3 - 3)== 0)<< endl;
    manager.getRoadHeader()->printData();
    manager.getLane()->printData();

    manager.setInertialPos(6776.5,-85.91, 0);
    manager.inertial2lane();
    manager.getRoadHeader()->printData();
    manager.getLane()->printData();*/


    double step = 1;

    PositionInfo posInfo;
    GetPositionInfo posInfoManager(manager, xodrPath, startPoint,posInfo);
    //posInfoManager.myPosInfo.lane->printData();
    posInfoManager.UpdatePosInfoByRoadLane(1335, -1);

    /*vector<Point> inputPoints = {{5915.00, -2937.76, 0}, {6031.85, -3539.57, 0}};

    vector<pair<int, int>> inputLaneIDRoadID = {{-2, 11}, {-1, 22}};
    vector<pair<int, int>> guidePathsAdded;
    guidePathsAdded = posInfoManager.AddJuncLane2GuidePaths(inputLaneIDRoadID, guidePathsAdded);
    for(int i = 0; i< guidePathsAdded.size(); i++)
    {
        cout << "laneId: " << guidePathsAdded.at(i).first;
        cout << " ;roadId: " << guidePathsAdded.at(i).second << endl;
    }


    vector<PointLaneInfo> guidePathPoint;
    guidePathPoint = posInfoManager.GetPointLaneInfo(inputPoints, guidePathPoint);
    vector<PointLaneInfo> guidePathPointAdded;
    guidePathPointAdded = posInfoManager.AddJuncLane2GuidePaths(guidePathPoint, guidePathPointAdded);
    vector<Point> detailedPoint;
    shareMessage.GetLineAndCurLinePoint(posInfoManager, step, guidePathPointAdded, detailedPoint);*/


    //挑战赛点集测试
    /***
     * 1、输入起点的直接调用即可
     * 2、无起始点，则直接输入一个属于该road的有效点激活地图，再输入roadID和laneID
     */
    vector<Point> detailedPoint;
    cout << "Step = " << step << endl;
    //shareMessage.GetPointPathInRampRoadAll(posInfoManager, step, posInfo.roadId, posInfo.laneId, detailedPoint);
    //posInfoManager.Print(detailedPoint);
   //cout << detailedPoint.size() << endl;

    detailedPoint.clear();
    cout << "roadID,laneID" << posInfoManager.myPosInfo.roadId << "; " << posInfoManager.myPosInfo.laneId << endl;
    shareMessage.GetPointPathInRoadLineSpecial(posInfoManager, step, posInfoManager.myPosInfo.roadId, posInfoManager.myPosInfo.laneId, detailedPoint);
    posInfoManager.Print(detailedPoint);
    cout << detailedPoint.size() << endl;

    return 0;
}







