//
// Created by JinxBIGBIG on 2022/4/12.
//
#include <iostream>
#include "OdrManager.hh"
#include "positionInfo.h"
#include "BaseNodes/OdrRoadHeader.hh"
#include "BaseNodes/OdrRoadLink.hh"
using namespace OpenDrive;
using namespace std;

void getLaneInfoFromInertialPos(OpenDrive::OdrManager& manager,
                                double x, double y, double z
                                ,posInfo& posinfo){
    OpenDrive::Position* pos = manager.createPosition();
    manager.activatePosition(pos);

    manager.setInertialPos(x, y, z);
    bool result = manager.inertial2lane();

    if (result){
        OpenDrive::RoadHeader* header = manager.getRoadHeader();
        if (!header) return;
        posinfo.roadId = header->getId();

        // 主车道路ID相关信息读取
        OpenDrive::Lane* lane = manager.getLane();
        if (!lane) return;
        // double speed = manager.getLaneSpeed();
        posinfo.laneId = lane->getId();
        posinfo.laneSpeed = manager.getLaneSpeed();
        posinfo.laneType = lane->getType();

        // 前驱、后继信息读取
        OpenDrive::RoadLink* roadLink = header->getFirstLink();
    }


}

int main(int agrc, char** argv){
    OpenDrive::OdrManager manager;
    bool flag = manager.loadFile("..\\data\\MT_intersection_ego_1lane.xodr");
    if (flag)
        cout << "load file successful." << endl;
    else
        cout << "load file wrong." << endl;

    OpenDrive::Position* pos = manager.createPosition();
    manager.activatePosition(pos);

    //定义自车坐标点
    Point p1(114.798, 5.182, 0);

    OpenDrive::RoadHeader* header = manager.getRoadHeader();
    // header相关数据的输出
    cout << "header->printData(): " << endl;
      header->printData();
      cout << endl;

      cout << "track od as string " << header->mIdAsString << endl;

      cout << "track id is " << header->mId << endl;




    return 0;
}
