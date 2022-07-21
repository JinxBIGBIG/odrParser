//
// Created by JinxBIGBIG on 2022/4/12.
//
#include "OdrManager.hh"
#include "positionInfo.h"
#include "CA/staticMessage.h"
#include "CA/dynamiciMessage.h"
#include <iostream>

using namespace std;
using namespace OpenDrive;


int main(int argc, char** argv) {
    OpenDrive::OdrManager manager;
//    string xodrPath = "..\\data\\Crossing8Course.xodr";
    string xodrPath = "..\\data\\Germany_2018.xodr";
    Point startPoint(5915.00, -2937.76, 0); //roadId:40,laneId:-1
    Point endPoint(6031.85, -3539.57, 0); //roadId:22,laneId:-2
//    Point startPoint(2996.44, -3023.28, 0);
//    Point endPoint(3111.44, -3003.28, 0);

//    string xodrPath = "..\\data\\Crossing8Course.xodr";
//    Point startPoint(21.86, -26.1, 0);
//    Point endPoint(68.03, 77.12, 0);

//    string xodrPath = "..\\data\\CAexample.xodr";
//    Point startPoint(634.46, 594.85, 0);
////    Point startPoint(1120.4, 591.41, 0);
//    Point endPoint(1238.77, 584.29, 0);

    double range = 2000;

    PositionInfo posInfo;

    GetPositionInfo posInfoManager(manager, xodrPath, startPoint, endPoint, range, posInfo);
    //posInfoManager.myPosInfo.road->printData();
    //posInfoManager.myEndPosInfo.road->printData();



    //经纬度获取测试
//    posInfoManager.GetGeographicCoordinate();
//    cout << "Longitude and latitude over..." << endl;
    //posInfoManager.ProjTest();

    //posInfoManager.GetGuidePaths(rangeRamp);

    //posInfoManager.Test();
    vector<pair<int, RoadHeader* >> guidePathsRange;
    vector<pair<int, RoadHeader* >> guidePathsEndPoint;
    vector<pair<int, RoadHeader* >> guidePathsRangeAdded;
    vector<pair<int, RoadHeader* >> guidePathsEndPointAdded;
    //vector<pair<int, int>> inputPaths = {{1, 54}, {-1, 63}, {-1, 53}, {-1, 135}, {-1, 91}};
    //vector<pair<int, int>> inputPaths = {{-1, 110}, {-1, 102}, {-1, 94}, {-1, 15}, {-1, 3}};
    vector<pair<int, int>> inputPathsEndPoint = {{-1, 40}, {-1, 20}, {-1, 11}, {-1, 22}};
    vector<pair<int, int>> inputPathsRange = {{-1, 40}, {-1, 20}, {-1, 11}, {-1, 22}};
    guidePathsEndPoint = posInfoManager.AddJuncRoad2GuidePaths(inputPathsEndPoint, guidePathsEndPoint);
    guidePathsRange = posInfoManager.AddJuncRoad2GuidePaths(inputPathsRange, guidePathsRange);
    guidePathsRangeAdded = posInfoManager.AddJuncRoad2GuidePathsWithTwoRoads(guidePathsRange);
    guidePathsEndPointAdded = posInfoManager.AddJuncRoad2GuidePathsWithTwoRoads(guidePathsEndPoint);

    for(int i = 0; i< guidePathsRangeAdded.size(); i++)
    {
        cout << "laneId: " << guidePathsRangeAdded.at(i).first;
        cout << " ;roadId: " << guidePathsRangeAdded.at(i).second->getId() << endl;
    }
    //posInfoManager.GetRoadLinkInfos(manager, point, posInfo);
    //cout << "getRoadLinkInfos over." << endl;
    //posInfoManager.GetGeoHeader(manager, point, posInfo);
    //posInfoManager.coordXYtoST(manager, point, stPoint);

    //vector<IdAndLength>* ids = posInfoManager.GetIdAndLengths(manager, point, 1400);

    //vector<int> ids ={1,88};
    //posInfoManager.GetGeoInfos(manager, point, posInfo, ids);

//    //长安相关测试


      //动态信息获取
    DynamicMap dynamicMap;


//    vector<LaneSection *> laneSections;
//    laneSections = posInfoManager.GetLaneSectionsByEndPoint(endPoint, laneSections);
//

    /*CAAnchorPosRelatedInfo anchorPosRelatedInfo;
    anchorPosRelatedInfo = dynamicMap.GetCAAnchorPosRelatedInfo(posInfoManager);

    vector<CASpecialArea> specialAreas;
    specialAreas = dynamicMap.GetCASpecialArea( posInfoManager, guidePathsEndPoint);

    CARampInfoOut rampInfoOut;
    dynamicMap.GetCARampInfoOut(posInfoManager, guidePathsRangeAdded);

    vector<CARoutingPath> *routingPath = new vector<CARoutingPath>;
    routingPath = dynamicMap.GetCARoutingPath(posInfoManager, guidePathsEndPoint, routingPath);

    CADynamicAddition dynamicAddition;
    dynamicAddition = dynamicMap.GetCADynamicAddition(posInfoManager, guidePathsEndPoint);

    CAPathPlanningOut pathPlanningOut;
    dynamicMap.GetCAPathPlanningOut(posInfoManager, guidePathsEndPoint, guidePathsEndPointAdded);*/

    CADynamicHDMapErc dynamicHdMapErc;
    dynamicHdMapErc = dynamicMap.GetCADynamicHDMapErc(posInfoManager, guidePathsRange, guidePathsRangeAdded, guidePathsEndPoint, guidePathsEndPointAdded);
     cout << " GetCADynamicHDMapErc over ... " << endl;





    //静态信息获取
    StaticMap staticMap;
    //vector<CALink> caLinks;
    CAStaticHDMapErc caStaticHdMapErc;
    //caLinks = staticMap.GetCALink(posInfoManager, guidePathsEndPoint);
    caStaticHdMapErc = staticMap.GetCAStaticHDMapErc(posInfoManager, guidePathsEndPoint);


//

    //vector<int>* roadDirs = posInfoManager.GetRoadDirs(2000);
    //vector<RoadHeader*> roads;
   // roads = posInfoManager.GetRoads(2000, roads);


    return 0;
}







