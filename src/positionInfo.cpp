//
// Created by JinxBIGBIG on 2022/4/14.
//


#include "positionInfo.h"
#include "OpenDRIVE.hh"
#include "OdrManager.hh"
#include "OdrManagerLite.hh"
#include "OdrProjection.hh"
#include "BaseNodes/OdrTunnel.hh"
#include "BaseNodes/OdrLaneWidth.hh"
#include "BaseNodes/OdrHeader.hh"
#include "AdditionNodes/OdrReaderXML.hh"
#include "BaseNodes/OdrRoadData.hh"
#include "OdrCoord.hh"
#include "tinyxml2.h"
#include <iostream>
#include <vector>



using namespace std;
using namespace OpenDrive;
using namespace tinyxml2;

const double EPS = 1e-8;
const char *OdrGeoReference = "+proj=utm +zone=32 +datum=WGS84";

GetPositionInfo::GetPositionInfo() {}

GetPositionInfo::GetPositionInfo(OpenDrive::OdrManager &manager, std::string &xodrPath, Point &point, double range, PositionInfo &posInfo) {
    myManager = manager;
    myXodrPath = xodrPath;
    myPoint = point;
    myRange = range;
    myKeyWords = {"tunnel","toll" ,"construction"};

    bool result = InitialPosition(point, myManager);
    if(result){
        myPosInfo = GetInertialPosInfo(myManager, posInfo);
    } else{
        myPosInfo = posInfo;
    }
}

GetPositionInfo::GetPositionInfo(OpenDrive::OdrManager &manager, std::string &xodrPath, Point &point, Point &endPoint,
                                 double range, PositionInfo &posInfo) {
    myManager = manager;
    myXodrPath = xodrPath;
    myPoint = point;
    myEndPoint = endPoint;
    myRange = range;
    myKeyWords = {"tunnel","toll" ,"construction"};

    bool result = InitialPosition(point, myManager);
    if(result){
        myPosInfo = GetInertialPosInfo(myManager, posInfo);
        GetEndPosInfo(myPoint, endPoint,myManager);
        //以下两行非必要，
        myManager.setInertialPos(point.x, point.y, point.z);
        myManager.inertial2lane();
    } else{
        myPosInfo = posInfo;
        myEndPosInfo = posInfo;
    }
}



bool GetPositionInfo::InitXodr()
{
    bool xodrLoad = myManager.loadFile(myXodrPath);
    //cout << "xodrLoad: " << xodrLoad << endl;
    return xodrLoad;
}



bool GetPositionInfo::InitialPosition(Point point, OdrManager& manager){
    bool xodrLoad = InitXodr();
    OpenDrive::Position* pos = manager.createPosition();
    manager.activatePosition(pos);
    manager.setInertialPos(point.x, point.y, point.z);
    bool result = manager.inertial2lane();
    cout << "resultInertial2lane: " << result << endl;
    if (result){
        if (!manager.getRoadHeader()) return false;
        if (!manager.getLane()) return false;
        if (!manager.getLaneSection()) return false;
    }
    else {
        cout << "Given point is not an road!." << endl;
    }
    return result;
};


/*bool GetPositionInfo::GetInertialPosInfo() {
    myPosInfo = GetInertialPosInfo(myPoint, myManager, myPosInfo);
    bool result2 = InitialPosition(myEndPoint, myEndManager);
    if (result) {

        OpenDrive::RoadHeader* header = myManager.getRoadHeader();
        myPosInfo.roadId = header->getId();
        myPosInfo.road = header;
        myPosInfo.roadlength = header->mLength;

        OpenDrive::Lane* lane = myManager.getLane();
        myPosInfo.lane = lane;


        double speed = myManager.getLaneSpeed();
        myPosInfo.laneId = lane->getId();
        myPosInfo.laneSpeed = speed;
        myPosInfo.laneType = lane->getType();
        myPosInfo.laneWidth = myManager.getLaneWidth();

        OpenDrive::LaneSection* section = myManager.getLaneSection();
        myPosInfo.section = section;
        myPosInfo.sectionS = section->mS;

        myPosInfo.junctionId = myManager.getJunctionId();

        OpenDrive::TrackCoord posTrack = myManager.getTrackPos();
        myPosInfo.trackS = posTrack.getS();
        myPosInfo.trackT = posTrack.getT();
        myPosInfo.roadheading = posTrack.getH();

        OpenDrive::Elevation* posElevation = myManager.getElevation();
        myPosInfo.ElvmS = posElevation->mS;
        myPosInfo.ElvmSEnd = posElevation->mSEnd;
        myPosInfo.ElvmA = posElevation->mA;
        myPosInfo.ElvmB = posElevation->mB;
        myPosInfo.ElvmC = posElevation->mC;
        myPosInfo.ElvmD = posElevation->mD;
    } else {
        // std::cout << "the point is not on road." << std::endl;
    }
    return result;
}*/



PositionInfo GetPositionInfo::GetInertialPosInfo(OdrManager& manager, PositionInfo &posInfo)
{
    OpenDrive::RoadHeader* header = manager.getRoadHeader();
    posInfo.roadId = header->getId();
    posInfo.road = header;
    posInfo.roadlength = header->mLength;

    OpenDrive::Lane* lane = manager.getLane();
    posInfo.lane = lane;


    double speed = manager.getLaneSpeed();
    posInfo.laneId = lane->getId();
    posInfo.laneSpeed = speed;
    posInfo.laneType = lane->getType();
    posInfo.laneWidth = manager.getLaneWidth();

    OpenDrive::LaneSection* section = manager.getLaneSection();
    posInfo.section = section;
    posInfo.sectionS = section->mS;

    posInfo.junctionId = manager.getJunctionId();

    OpenDrive::TrackCoord posTrack = manager.getTrackPos();
    posInfo.trackS = posTrack.getS();
    posInfo.trackT = posTrack.getT();
    posInfo.roadheading = posTrack.getH();

    OpenDrive::Elevation* posElevation = manager.getElevation();
    posInfo.ElvmS = posElevation->mS;
    posInfo.ElvmSEnd = posElevation->mSEnd;
    posInfo.ElvmA = posElevation->mA;
    posInfo.ElvmB = posElevation->mB;
    posInfo.ElvmC = posElevation->mC;
    posInfo.ElvmD = posElevation->mD;

    posInfo.geoReference = manager.getGeoReference();
    return posInfo;
}
void GetPositionInfo::GetEndPosInfo(Point &point, Point &endPoint, OdrManager& manager)
{
    manager.setInertialPos(endPoint.x, endPoint.y, endPoint.z);
    manager.inertial2lane();
    myEndPosInfo = GetInertialPosInfo(manager, myEndPosInfo);
}

void printRoadLink(OpenDrive::RoadLink* roadLink){
    cout << "RoadLink information :" << endl
         << "type : " << roadLink->mType << endl
         << "elementType : " << roadLink->mElemType << endl
         << "elementId : " << roadLink->mElemId << endl
         << "elementIdAsString : " << roadLink->mElemIdAsString << endl;
}

void GetPositionInfo::GetGeoHeader(Point point){
    vector<OpenDrive::GeoHeader*> geoHeaders;
    cout << "loading geoHeader begins ..." << endl;
    OpenDrive::RoadHeader* header = myManager.getRoadHeader();
    if (!header) return;

    // 当前车道所有geometry读取
    OpenDrive::GeoHeader* geoLastGeoHeader = header ->getLastGeoHeader();
    OpenDrive::GeoHeader* myGeoHeader = header->getFirstGeoHeader();
    while (myGeoHeader->getRight() != NULL){
        OpenDrive::GeoHeader* tempGeoHeader = (OpenDrive::GeoHeader*)myGeoHeader->getRight();
        geoHeaders.push_back(tempGeoHeader);
        myGeoHeader = tempGeoHeader;
    }
    cout << "loading geoHeader ends ..." << endl;
    myGeoHeader->printData();
    //geoLastGeoHeader->printData();

    OpenDrive::RoadLink* sucLink = header->getSuccessorLink();
    if (!sucLink){
        cout <<"No successorLink existed" <<endl;
    }else{
        myPosInfo.sucRoadLink = sucLink;
        //posInfo.sucRoadLink->printData();
    }
};

//测试xy2st and st2xy
void GetPositionInfo::coordXYtoST(Point point) {
    OpenDrive::TrackCoord t = myManager.getTrackPos();
    cout << "inertial2track method : " << endl;
    t.print();

    cout << "segmentation..." <<endl;

    myManager.setTrackPos(t);
    OpenDrive::Coord i = myManager.getInertialPos();
    cout << "track2inertial method : " << endl;
    i.print();
}


//static


/*static int RampStatus(const int &laneType){
    int rampStatus = 0;
    switch (laneType)
    {
        case ODR_LANE_TYPE_ENTRY: rampStatus = 4;
            break;
        case ODR_LANE_TYPE_EXIT: rampStatus = 5;
            break;
        case ODR_LANE_TYPE_ON_RAMP: rampStatus = 6;
            break;
        case ODR_LANE_TYPE_OFF_RAMP: rampStatus = 7;
            break;
        case ODR_LANE_TYPE_CONNECTING_RAMP: rampStatus = 8;
    }
    return rampStatus;
}*/
int GetPositionInfo::GetLaneNum()
{
    if (!myPosInfo.road) {
        return false;
    }
    RoadHeader *my_road = myPosInfo.road;
    LaneSection* my_section = my_road->getFirstLaneSection();
    while (myPosInfo.trackS>my_section->mSEnd){
        my_section =(LaneSection*) my_section->getRight();
//        cout<<my_section<<endl;
    }
    Lane* myfind = my_section->getLaneFromId ( 0 );
    int drivable_lane_num=0;
    while (myfind->getLeft()) {
        Lane* leftLane = (Lane*)(myfind->getLeft());
        if (leftLane->getType() == ODR_LANE_TYPE_DRIVING) {
            drivable_lane_num++;
        }
        myfind = leftLane;
    }

    myfind =  my_section->getLaneFromId ( 0 );
    while (myfind->getRight()) {
        Lane* rightLane = (Lane*)(myfind->getRight());
        if (rightLane->getType() == ODR_LANE_TYPE_DRIVING) {
            drivable_lane_num++;
        }
        myfind = rightLane;
    }
//    cout<<"drivable_lane_num:"<<drivable_lane_num<<endl;
    return drivable_lane_num;
}

int GetPositionInfo::TraversalLane(Lane *tempLane,int &laneStatus)
{
    Lane *currentLane;
    while(tempLane->getRight() != NULL)
    {
        currentLane = tempLane;
        if(ODR_LANE_TYPE_ENTRY == currentLane->getType() || ODR_LANE_TYPE_EXIT == currentLane->getType()
        || ODR_LANE_TYPE_ON_RAMP == currentLane->getType()|| ODR_LANE_TYPE_OFF_RAMP == currentLane->getType()
        || ODR_LANE_TYPE_DRIVING == currentLane->getType() || ODR_LANE_TYPE_CONNECTING_RAMP == currentLane->getType())
        {
            //待写详细代码
            laneStatus = currentLane->getType();
            break;
        }
        else
            laneStatus = 0;
        tempLane = (Lane *)(tempLane->getRight());
    }
    /* //左边遍历结束，如果已经存在ramp则不进行右遍历
     if(rampStatus == 0)
     {
         while(tempLane->getLeft() != NULL)
         {
             currentLane = tempLane;
             tempLane = (Lane *)(tempLane->getLeft());
             if(ODR_LANE_TYPE_ON_RAMP == currentLane->getType() || ODR_LANE_TYPE_OFF_RAMP == currentLane->getType()
                || ODR_LANE_TYPE_CONNECTING_RAMP == currentLane->getType()){
                 rampStatus = RampStatus(currentLane->getType());
                 break;
             }
             else
                 rampStatus = 0;
         }
     }*/
    return laneStatus;
}


pair<int,int> GetPositionInfo::TraversalLane(Lane *tempLane)
{
    Lane *currentLane;
    int laneStatus=0,laneId=tempLane->mId;
    while(tempLane->getRight() != NULL)
    {
        tempLane = (Lane *)(tempLane->getRight());
        currentLane = tempLane;
        if(ODR_LANE_TYPE_ENTRY == currentLane->getType() || ODR_LANE_TYPE_EXIT == currentLane->getType()
           || ODR_LANE_TYPE_ON_RAMP == currentLane->getType()|| ODR_LANE_TYPE_OFF_RAMP == currentLane->getType()
           || ODR_LANE_TYPE_CONNECTING_RAMP== currentLane->getType())
        {
            cout<<"ramp is here"<<currentLane->mId<<endl;
            //待写详细代码
            laneStatus = currentLane->getType();
            laneId = currentLane->mId;
            break;
        }
        else{
            laneId = currentLane->mId;
        }
    }
    //左边遍历结束，如果已经存在ramp则不进行右遍历
    if(laneStatus == 0)
    {
        while(tempLane->getLeft() != NULL)
        {
            tempLane = (Lane *)(tempLane->getLeft());
            currentLane = tempLane;
            if(ODR_LANE_TYPE_ENTRY == currentLane->getType() || ODR_LANE_TYPE_EXIT == currentLane->getType()
               || ODR_LANE_TYPE_ON_RAMP == currentLane->getType()|| ODR_LANE_TYPE_OFF_RAMP == currentLane->getType()
               || ODR_LANE_TYPE_CONNECTING_RAMP== currentLane->getType())
            {
                cout<<"ramp is here"<<currentLane->mId<<endl;
                //待写详细代码
                laneStatus = currentLane->getType();
                laneId = currentLane->mId;
                break;
            }
            else{
                laneId = currentLane->mId;
            }
        }
    }
    return make_pair(laneStatus,laneId);
}

int GetPositionInfo::DrivingDirRelToRoad(bool isRHT) {
    if (myPosInfo.laneId < 0 && isRHT)
        return -1;    //0代表同向，
    if (myPosInfo.laneId > 0 && !isRHT)
        return -1;
    return 1;     //1反向
}

//以下下四个均不考虑junction
vector<int> *GetPositionInfo::GetRoadDirs(const int range, vector<int> *roadDirs)
{
    int iter = 1;
    int roadDir;
    //cout << "First laneID is " << myPosInfo.laneId << endl;
    roadDir = DrivingDirRelToRoad(myPosInfo.road->mRHT);
    //至此，roadDir = -1,同方向，找后继successor
    //否则找前驱predecessor
    RoadHeader* tempRoad = myPosInfo.road;
    double tempS;
    roadDirs->push_back(roadDir);
    //初始方向不一样，若给范围就在当前road直接返回
    if(roadDir < 0 ){
        if(range <= myPosInfo.roadlength - myPosInfo.trackS)
            return roadDirs;
        tempS = myPosInfo.trackS;
        //就在当前road里边，直接返回
    }else{
        if(range <= myPosInfo.trackS)
            return roadDirs;
        tempS = myPosInfo.roadlength - myPosInfo.trackS;
    }
    //road = {roadDir, tempRoad};
    cout << "First length : " << tempS << endl;
    //roads.push_back(road);
    while(tempS < range){
        //判断方向来寻找前驱后继
        if((roadDir <= 0) && (tempRoad->getSuccessor()))
        {
            roadDir = tempRoad->getSuccessorDir();
            tempRoad = tempRoad->getSuccessor();
            tempS += tempRoad->mLength;
        }
        else if ((roadDir > 0) && (tempRoad->getPredecessor()))
        {
            roadDir = tempRoad->getPredecessorDir();
            tempRoad = tempRoad->getPredecessor();
            tempS  += tempRoad->mLength;
        }
        else
        {
            cout << "The" << iter << "st road has no predecessor or successor." << endl;
            break;
        }
        //cout << "tempRoadId" << tempRoad->getId()<< endl;
        //cout << "tempRoad : " << tempRoad->getLength() << endl;
        roadDirs->push_back(roadDir);
        iter ++;
    }
    //cout << "tempS " << tempS << endl;
    cout << "roads judging over..." << endl;
    return roadDirs;
}
vector<RoadHeader*>& GetPositionInfo::GetRoads(const int range, vector<RoadHeader*> &roads)
{
    int iter = 1;
    int roadDir = 1;
    RoadHeader* firstRoad = myPosInfo.road;
    cout << "First laneID is " << myPosInfo.laneId << endl;
    roadDir = DrivingDirRelToRoad(firstRoad->mRHT);
    //至此，roadDir = 0,同方向，找后继successor
    //否则找前驱predecessor
    RoadHeader* tempRoad = firstRoad->getSuccessor();
    double tempS = myPosInfo.trackS;
    //就在当前road里边，直接返回
    roads.push_back(myPosInfo.road);
    if(range <= myPosInfo.roadlength - myPosInfo.trackS)
        return roads;
    //roadDir = 1, 找前驱，此处设置为同后续找Dir(即odrMGR中dir寻找前驱or后继)的逻辑
    if (1 == roadDir)
    {
        tempRoad = firstRoad->getPredecessor();
        tempS = myPosInfo.roadlength - myPosInfo.trackS;
        if(range <= myPosInfo.trackS)
            return roads;
    }
    cout << "Second tempRoadId" << tempRoad->getId()<< endl;
    roads.push_back(tempRoad);

    while(tempS < range){
        //判断方向来寻找前驱后继
        if((roadDir == 0) && (tempRoad->getSuccessorNode()))
        {
            //cout << "tempRoadId" << tempRoad->getId();
            roadDir = tempRoad->getSuccessorDir();
            if(tempRoad->getSuccessor() != NULL){
                tempRoad = tempRoad->getSuccessor();
            } else{
                tempRoad = tempRoad->getSuccessor(1);
                //tempRoad->printData();
            }
            //tempRoad = tempRoad->getSuccessor(1);
            //tempRoad->printData();
            tempS += tempRoad->mLength;
        }
        else if ((roadDir == 1) && (tempRoad->getPredecessorNode()))
        {

            roadDir = tempRoad->getPredecessorDir();
            if(tempRoad->getPredecessor() != NULL){
                tempRoad = tempRoad->getPredecessor();
            } else{
                tempRoad = tempRoad->getPredecessor(1);
                //tempRoad->printData();
            }
            //tempRoad = tempRoad->getPredecessor(1);
            //tempRoad->printData();
            tempS  += tempRoad->mLength;
        }
        else
        {
            cout << "The" << iter << "st road has no predecessor or successor." << endl;
            break;
        }
        cout << "tempRoadId" << tempRoad->getId()<< endl;
        tempRoad->printData();
        roads.push_back(tempRoad);
        iter ++;
    }
    cout << "tempS " << tempS << endl;
    cout << "roads judging over..." << endl;
    return roads;
}
vector<pair<int, RoadHeader*>> &GetPositionInfo::GetRoads(const int range, vector<pair<int, RoadHeader*>> &roads){
    int iter = 1;
    int roadDir;
    pair<int, RoadHeader *> road;
    //cout << "First laneID is " << myPosInfo.laneId << endl;
    roadDir = DrivingDirRelToRoad(myPosInfo.road->mRHT);
    //至此，roadDir = -1,同方向，找后继successor
    //否则找前驱predecessor
    RoadHeader* tempRoad = myPosInfo.road;
    double tempS;
    road = {roadDir, myPosInfo.road};

    roads.push_back(road);
    //初始方向不一样，若给范围就在当前road直接返回
    if(roadDir < 0 ){
        if(range <= myPosInfo.roadlength - myPosInfo.trackS)
            return roads;
        tempS = myPosInfo.trackS;
        //就在当前road里边，直接返回
    }else{
        if(range <= myPosInfo.trackS)
            return roads;
        tempS = myPosInfo.roadlength - myPosInfo.trackS;
    }
    cout << "First length : " << tempS << endl;
    cout << "First laneId : " << myPosInfo.laneId << endl;
    cout << "First roadId : " << myPosInfo.roadId << endl;
    cout <<"First tracks: " << myPosInfo.trackS << endl;
    //roads.push_back(road);
    while(tempS < range){
        //判断方向来寻找前驱后继
        if((roadDir <= 0) && (tempRoad->getSuccessor()))
        {
            roadDir = tempRoad->getSuccessorDir();
            tempRoad = tempRoad->getSuccessor();
            tempS += tempRoad->mLength;
        }
        else if ((roadDir > 0) && (tempRoad->getPredecessor()))
        {
            roadDir = tempRoad->getPredecessorDir();
            tempRoad = tempRoad->getPredecessor();
            tempS  += tempRoad->mLength;
        }
        else
        {
            cout << "The" << iter << "st road has no predecessor or successor." << endl;
            break;
        }
        //cout << "tempRoadId: " << tempRoad->getId()<< endl;
        //cout << "roadDir: " << roadDir<< endl;
        //cout << "tempRoad : " << tempRoad->getLength() << endl;
        road = {roadDir, tempRoad};
        roads.push_back(road);
        iter ++;
    }
    //cout << "tempS " << tempS << endl;
    cout << "roads judging over..." << endl;
    return roads;
}

vector<int>* GetPositionInfo::GetRoadDirs(const Point &endPoint)
{
    int iter = 1;
    int roadDir;
    vector<int>* roadDirs = new vector<int>[iter];
//    endPosInfo = GetInertialPosInfo(endPoint, endManager, endPosInfo);
//    int endRoadId = endPosInfo.roadId;
    OdrManager endManager;
    bool endActivate = InitialPosition(endPoint, endManager);
    int endRoadId = endManager.getRoadHeader()->getId();
    RoadHeader* firstRoad = myPosInfo.road;
    RoadHeader* tempRoad = firstRoad->getSuccessor();
    roadDir = 0;//默认同向
    // 否则找前驱predecessor
    if (1 == roadDir){
        tempRoad = firstRoad->getPredecessor();
        roadDir = 1;
    }
    roadDirs->push_back(roadDir);
    cout << "Second tempRoadId" << tempRoad->getId()<< endl;
    //如果给定点就在当前road，直接返回
    if (endRoadId == myPosInfo.roadId)
        return roadDirs;
    cout << "First laneID is " << myPosInfo.laneId << endl;
    int tempRoadId = myPosInfo.roadId;
    roadDir = DrivingDirRelToRoad(firstRoad->mRHT);
    //至此，roadDir = 0,同方向，找后继successor
    //roadDir = 1, 找前驱，此处设置为同后续找Dir(即odrMGR中dir寻找前驱or后继)的逻辑

    roadDirs->push_back(roadDir);
    while(tempRoadId != endRoadId){
        //判断方向来寻找前驱后继
        if((roadDir == 0) && (tempRoad->getSuccessor()))
        {
            //cout << "tempRoadId" << tempRoad->getId();
            roadDir = tempRoad->getSuccessorDir();
            tempRoad = tempRoad->getSuccessor();
            tempRoadId = tempRoad->getId();
        }
        else if ((roadDir == 1) && (tempRoad->getPredecessor()))
        {

            roadDir = tempRoad->getPredecessorDir();
            tempRoad = tempRoad->getPredecessor();
            tempRoadId = tempRoad->getId();
        }
        else
        {
            cout << "The" << iter << "st road has no predecessor or successor." << endl;
            break;
        }
        cout << "tempRoadId" << tempRoad->getId()<< endl;
        roadDirs->push_back(roadDir);
        iter ++;
    }
    /*for(int i = 0; i<roadDirs->size(); i++)
    {
        cout << "roadDirs " << roadDirs->at(i) << endl;
    }*/
    cout << "roads judging over..." << endl;
    return roadDirs;

}
vector<RoadHeader*>& GetPositionInfo::GetRoads(vector<RoadHeader*> &roads)
{
    int iter = 1;
    int roadDir;
    OdrManager endManager;
    int endRoadId = endManager.getRoadHeader()->getId();
    RoadHeader* firstRoad = myPosInfo.road;
    roads.push_back(firstRoad);
    //如果给定点就在当前road，直接返回
    if (endRoadId == myPosInfo.roadId)
        return roads;
    cout << "First laneID is " << myPosInfo.laneId << endl;
    //int tempRoadId = myPosInfo.roadId;
    roadDir = DrivingDirRelToRoad(firstRoad->mRHT);
    //至此，roadDir = 0,同方向，找后继successor
    //否则找前驱predecessor
    RoadHeader* tempRoad = firstRoad->getSuccessor();
    //roadDir = 1, 找前驱，此处设置为同后续找Dir(即odrMGR中dir寻找前驱or后继)的逻辑
    if (1 == roadDir)
        tempRoad = firstRoad->getPredecessor();
    //cout << "Second tempRoadId" << tempRoad->getId()<< endl;
    roads.push_back(tempRoad);
    while(tempRoad->getId() != endRoadId){
        //判断方向来寻找前驱后继
        if((roadDir == 0) && (tempRoad->getSuccessor()))
        {
            //cout << "tempRoadId" << tempRoad->getId();
            roadDir = tempRoad->getSuccessorDir();
            tempRoad = tempRoad->getSuccessor();
        }
        else if ((roadDir == 1) && (tempRoad->getPredecessor()))
        {

            roadDir = tempRoad->getPredecessorDir();
            tempRoad = tempRoad->getPredecessor();
        }
        else
        {
            cout << "The" << iter << "st road has no predecessor or successor." << endl;
            break;
        }
        cout << "tempRoadId" << tempRoad->getId()<< endl;
        roads.push_back(tempRoad);;
        iter ++;
    }

    return roads;
}
vector<pair<int, RoadHeader*>> &GetPositionInfo::GetRoads(vector<pair<int, RoadHeader*>> &roads){
    int iter = 1;
    int roadDir;
    pair<int, RoadHeader *> road;
    //cout << "First laneID is " << myPosInfo.laneId << endl;

    //int endRoadId = 509;
    int endRoadId = myEndPosInfo.roadId;

    cout << "endRoadId: " << endRoadId << endl;
    roadDir = DrivingDirRelToRoad(myPosInfo.road->mRHT);
    cout << "roadDir" << roadDir <<endl;
    //至此，roadDir = -1,同方向，找后继successor
    //否则找前驱predecessor
    RoadHeader* tempRoad = myPosInfo.road;
    road = {roadDir, myPosInfo.road};
    roads.push_back(road);
    //初始方向不一样，若给范围就在当前road直接返回
    while(tempRoad->getId() != endRoadId){
        //判断方向来寻找前驱后继
        if((roadDir <= 0) && (tempRoad->getSuccessor()))
        {
            cout << "tempRoadId" << tempRoad->getId();
            roadDir = tempRoad->getSuccessorDir();
            tempRoad = tempRoad->getSuccessor();
        }
        else if ((roadDir > 0) && (tempRoad->getPredecessor()))
        {

            roadDir = tempRoad->getPredecessorDir();
            tempRoad = tempRoad->getPredecessor();
        }
        else
        {
            cout << "The" << iter << "st road has no predecessor or successor." << endl;
            break;
        }
        //cout << "tempRoadId" << tempRoad->getId()<< endl;
        road = {roadDir, tempRoad};
        roads.push_back(road);
        iter ++;
    }
    //cout << "tempS " << tempS << endl;
    cout << "roads judging over..." << endl;
    return roads;
}
//待给了导航路径再用此方法替代上边的方法

RoadHeader* GetPositionInfo::GetShortestRoadInJunction(RoadHeader* currentRoad, RoadHeader* juncRoad){
    JuncHeader *juncHeader;
    JuncLink *juncLink;
    int juncRoadLength;
    juncHeader = dynamic_cast<JuncHeader *>(juncRoad->getJunction());
    juncLink = juncHeader->getFirstLink();
    juncRoadLength = juncLink->mConnectingRoad->mLength;
    while (juncLink->getRight() != NULL){
        juncLink = dynamic_cast<JuncLink *>(juncLink->getRight());
        if(juncLink->mIncomingRoad->getId() == currentRoad->getId() && juncRoadLength < juncLink->mConnectingRoad->mLength)
        {
            juncRoadLength = juncLink->mConnectingRoad->mLength;
            juncRoad = juncLink->mConnectingRoad;
        }
    }
    return juncRoad;
}

pair<int, RoadHeader*> GetPositionInfo::AddNextRoad(int &dir, RoadHeader* currentRoad, pair<int, RoadHeader*> &nextRoadMessage){
    OpenDrive::RoadHeader* nextRoad;
    if(dir > 0){
        if(currentRoad->getPredecessor() != NULL){
            nextRoadMessage = {currentRoad->getPredecessorDir(), currentRoad->getPredecessor()};
        }else if((!currentRoad->getPredecessor()) && currentRoad->getPredecessor(1)){
            nextRoad = currentRoad->getPredecessor(1);
            nextRoad = GetShortestRoadInJunction(currentRoad, nextRoad);
            nextRoadMessage = {-1, nextRoad};
        }
    }
    else{
        if(currentRoad->getSuccessor() != NULL){
            nextRoadMessage = {currentRoad->getSuccessorDir(), currentRoad->getSuccessor()};
        }else if((!currentRoad->getSuccessor()) && currentRoad->getSuccessor(1)){
            nextRoad = currentRoad->getSuccessor(1);
            nextRoad = GetShortestRoadInJunction(currentRoad, nextRoad);
            nextRoadMessage = {-1, nextRoad};
        }
    }
    return nextRoadMessage;
}

GuidePaths GetPositionInfo::AddJuncRoad2GuidePathsWithTwoRoads(GuidePaths guidePaths) {
    RoadHeader *lastRoad = guidePaths.at(guidePaths.size() - 1).second;
    std::pair<int, OpenDrive::RoadHeader*> nextRoadMessage;
    int dir = guidePaths.at(guidePaths.size() - 1).first;
    nextRoadMessage = AddNextRoad(dir, lastRoad, nextRoadMessage);
    guidePaths.push_back(nextRoadMessage);
    nextRoadMessage = AddNextRoad(nextRoadMessage.first, nextRoadMessage.second, nextRoadMessage);
    guidePaths.push_back(nextRoadMessage);
    return guidePaths;
}


vector<int> GetPositionInfo::GetNextRoadLaneIDs(int formerLaneID, RoadHeader* nextRoad, vector<int> &nextRoadLaneId){
    myManager.setLanePos(nextRoad->mId, formerLaneID, 0, 0);
    myManager.inertial2lane();
    LaneSection* laneSection = nextRoad->getFirstLaneSection();

    Lane* lane = myManager.getLane();
    int laneID = formerLaneID;
    while(laneSection != NULL){
        if(formerLaneID > 0){
            if(lane->getPredecessor() != NULL){
                lane = lane->getPredecessor();
                laneID = lane->getPredecessor()->mId;
            }
            else{
                lane = laneSection->getLaneFromId(0);
                while(lane->getLeft() != NULL){
                    lane = (Lane*)lane->getLeft();
                    if( laneID == lane->getSuccessor()->mId){
                        laneID = lane->mId;
                        break;
                    }
                }
            }
        }else{
            if(lane->getSuccessor() != NULL){
                lane = lane->getSuccessor();
                laneID = lane->getSuccessor()->mId;
            }
            else{
                lane = laneSection->getLaneFromId(0);
                while(lane->getRight() != NULL){
                    lane = (Lane*)lane->getRight();
                    if( laneID == lane->getPredecessor()->mId){
                        laneID = lane->mId;
                        break;
                    }
                }
            }
        }
        nextRoadLaneId.push_back(laneID);
        laneSection = (LaneSection*)laneSection->getRight();
    }
    return nextRoadLaneId;
}

vector<pair<int, int >> GetPositionInfo::AddJuncLane2GuidePaths(vector<pair<int, int>> &inputPaths, vector<pair<int, int>> &guidePaths)
{
    //RoadHeader *tempRoad = myPosInfo.road;
    myManager.setLanePos(inputPaths.at(0).second, inputPaths.at(0).first, 0, 0);
    myManager.track2inertial();
    RoadHeader *tempRoad = myManager.getRoadHeader();
    RoadHeader *juncRoadNext;
    JuncHeader *juncHeader;
    JuncLink *juncLink;
    int nextDir;
    guidePaths.push_back({inputPaths.at(0).first, inputPaths.at(0).second});
    if (1 == inputPaths.size())
        return guidePaths;
    //只需要找到倒数第二个判断后继是否为junction
    vector<int> nextRoadLaneId;
    vector<int>::iterator iter;
    for(int i =  0; i < inputPaths.size() - 1; i++)
    {
        //均同向,后续若需要考虑反向则改为前驱
        nextDir = inputPaths.at(i).first;
        if(nextDir <= 0)
        {
            if((!tempRoad->getSuccessor()) && tempRoad->getSuccessor(1))
            {
                tempRoad = tempRoad->getSuccessor(1);
                juncHeader = dynamic_cast<JuncHeader *>(tempRoad->getJunction());
                juncLink = juncHeader->getFirstLink();
                while(juncLink != NULL)
                {
                    juncRoadNext = juncLink->mConnectingRoad->getSuccessor();
                    nextDir = juncLink->mConnectingRoad->getSuccessorDir();
                    if(juncLink->mIncomingRoad->getId() == inputPaths.at(i).second && inputPaths.at( i+1 ).second == juncRoadNext->getId())
                    {
                        nextRoadLaneId = GetNextRoadLaneIDs(inputPaths.at(i).first, juncLink->mConnectingRoad, nextRoadLaneId);
                        for(iter = nextRoadLaneId.begin(); iter != nextRoadLaneId.end(); iter++)
                            guidePaths.push_back({*iter, juncLink->mConnectingRoad->mId});
                        break;
                    }
                    juncLink = dynamic_cast<JuncLink *>(juncLink->getRight());
                }
            } else{
                nextDir = tempRoad->getSuccessorDir();
            }
        }
        else{
            if((!tempRoad->getPredecessor()) && tempRoad->getPredecessor(1))
            {
                tempRoad = tempRoad->getPredecessor(1);
                juncHeader = dynamic_cast<JuncHeader *>(tempRoad->getJunction());
                juncLink = juncHeader->getFirstLink();
                while(juncLink != NULL)
                {
                    juncRoadNext = juncLink->mConnectingRoad->getSuccessor();
                    nextDir = juncLink->mConnectingRoad->getSuccessorDir();
                    if(juncLink->mIncomingRoad->getId() == inputPaths.at(i).second && inputPaths.at( i+1 ).second == juncRoadNext->getId())
                    {
                        nextRoadLaneId = GetNextRoadLaneIDs(inputPaths.at(i).first, juncLink->mConnectingRoad, nextRoadLaneId);
                        for(iter = nextRoadLaneId.begin(); iter != nextRoadLaneId.end(); iter++)
                            guidePaths.push_back({*iter, juncLink->mConnectingRoad->mId});
                        break;
                    }
                    juncLink = dynamic_cast<JuncLink *>(juncLink->getRight());
                }
            } else{
                nextDir = tempRoad->getPredecessorDir();
            }
        }

        //i从0开始
        myManager.setLanePos(inputPaths.at(i+1).second, inputPaths.at(i+1).first, 0, 0);
        myManager.track2inertial();
        tempRoad = myManager.getRoadHeader();
        //guidePaths.push_back({inputPaths.at(i + 1).first, tempRoad});
        guidePaths.push_back({inputPaths.at(i+1).first, tempRoad->mId});
    }
    return guidePaths;
}

vector<pair<int, RoadHeader* >> GetPositionInfo::AddJuncRoad2GuidePaths(vector<pair<int, int>> &inputPaths, GuidePaths &guidePaths)
{
    //RoadHeader *tempRoad = myPosInfo.road;
    myManager.setLanePos(inputPaths.at(0).second, inputPaths.at(0).first, 0, 0);
    myManager.track2inertial();
    RoadHeader *tempRoad = myManager.getRoadHeader();
    RoadHeader *juncRoadNext;
    JuncHeader *juncHeader;
    JuncLink *juncLink;
    int nextDir = inputPaths.at(0).first;
    guidePaths.push_back({inputPaths.at(0).first, tempRoad});
    if (1 == inputPaths.size())
        return guidePaths;
    //只需要找到倒数第二个判断后继是否为junction
    for(int i =  0; i < inputPaths.size() - 1; i++)
    {
        //均同向,后续若需要考虑反向则改为前驱
        nextDir = inputPaths.at(i).first;
        if(nextDir <= 0)
        {
            if((!tempRoad->getSuccessor()) && tempRoad->getSuccessor(1))
            {
                tempRoad = tempRoad->getSuccessor(1);
                juncHeader = dynamic_cast<JuncHeader *>(tempRoad->getJunction());
                juncLink = juncHeader->getFirstLink();
                while(juncLink != NULL)
                {
                    juncRoadNext = juncLink->mConnectingRoad->getSuccessor();
                    nextDir = juncLink->mConnectingRoad->getSuccessorDir();
                    if(juncLink->mIncomingRoad->getId() == inputPaths.at(i).second && inputPaths.at( i+1 ).second == juncRoadNext->getId())
                    {
                        guidePaths.push_back({-1, juncLink->mConnectingRoad});
                        break;
                    }
                    juncLink = dynamic_cast<JuncLink *>(juncLink->getRight());
                }
            } else{
                nextDir = tempRoad->getSuccessorDir();
            }
        }
        else{
            if((!tempRoad->getPredecessor()) && tempRoad->getPredecessor(1))
            {
                tempRoad = tempRoad->getPredecessor(1);
                juncHeader = dynamic_cast<JuncHeader *>(tempRoad->getJunction());
                juncLink = juncHeader->getFirstLink();
                while(juncLink != NULL)
                {
                    juncRoadNext = juncLink->mConnectingRoad->getSuccessor();
                    nextDir = juncLink->mConnectingRoad->getSuccessorDir();
                    if(juncLink->mIncomingRoad->getId() == inputPaths.at(i).second && inputPaths.at( i+1 ).second == juncRoadNext->getId())
                    {
                        guidePaths.push_back({-1, juncLink->mConnectingRoad});
                        break;
                    }
                    juncLink = dynamic_cast<JuncLink *>(juncLink->getRight());
                }
            } else{
                nextDir = tempRoad->getPredecessorDir();
            }
            //cout << "nextDir: " << nextDir << endl;
        }

        //i从0开始
        myManager.setLanePos(inputPaths.at(i+1).second, inputPaths.at(i+1).first, 0, 0);
        myManager.track2inertial();
        tempRoad = myManager.getRoadHeader();
        //guidePaths.push_back({inputPaths.at(i + 1).first, tempRoad});
        guidePaths.push_back({nextDir, tempRoad});
    }
    return guidePaths;
}


//若需考虑
/*vector<int>* GetPositionInfo::GetRoadDirs(const int range)
{
    int iter = 1;
    int roadDir;
    vector<int>* roadDirs = new vector<int>[iter];
    RoadHeader* firstRoad = myPosInfo.road;
    //cout << "First laneID is " << myPosInfo.laneId << endl;
    //cout << "First tempRoadId" << firstRoad->getId()<< endl;
    roadDir = DrivingDirRelToRoad(firstRoad->mRHT);
    //至此，roadDir = 0,同方向，找后继successor
    //否则找前驱predecessor
    double tempS = myPosInfo.trackS;
    RoadHeader* tempRoad = firstRoad->getSuccessor();
    //roadDir = 1, 找前驱，此处设置为同后续找Dir(即odrMGR中dir寻找前驱or后继)的逻辑
    if (1 == roadDir)
    {
        tempRoad = firstRoad->getPredecessor();
        tempS = myPosInfo.roadlength - myPosInfo.trackS;
    }
//    cout << "First direction: " << roadDir << endl;
//    cout << "Second tempRoadId: " << tempRoad->getId()<< endl;
    roadDirs->push_back(roadDir);

    while(tempS < range){
        //判断方向来寻找前驱后继
        //cout << "tempRoadId : " << tempRoad->getId()<< endl;

        if((roadDir == 0) && (tempRoad->getSuccessorNode()))
        {
            //cout << "tempRoadId" << tempRoad->getId();
            roadDir = tempRoad->getSuccessorDir();
            tempRoad = tempRoad->getSuccessor();   //注意此处是否需要改变
            tempS += tempRoad->mLength;
        }
        else if ((roadDir == 1) && (tempRoad->getPredecessorNode()))
        {

            roadDir = tempRoad->getPredecessorDir();
            tempRoad = tempRoad->getPredecessor();//注意此处是否需要改变
            tempS  += tempRoad->mLength;
        }
        else
        {
            cout << "The" << iter << "st road has no predecessor or successor." << endl;
            break;
        }
        cout << "tempRoadId = " << tempRoad->mId << endl;
        roadDirs->push_back(roadDir);
        iter ++;
    }
    /*for(int i = 0; i<roadDirs->size(); i++)
    {
        cout << "roadDirs " << roadDirs->at(i) << endl;
    }
    cout << "tempS " << tempS << endl;
    cout << "roads judging over..." << endl;
    return roadDirs;
}*/

vector<pair<int, RoadHeader* >> GetPositionInfo::GetLastPaths(const Point &startPoint, vector<pair<int, int>> &inputPaths,
                                                               vector<pair<int, OpenDrive::RoadHeader *>> &lastPaths)
{
    int iter = 1;
    int roadDir;
    pair<int, RoadHeader *> lastPath;
//    endPosInfo = GetInertialPosInfo(endPoint, endManager, endPosInfo);
//    int endRoadId = endPosInfo.roadId;

    int endRoadId = myEndPosInfo.roadId;
    RoadHeader* tempRoad = myPosInfo.road;
    roadDir = DrivingDirRelToRoad(tempRoad->mRHT);
    lastPath.first = 0;
    lastPath.second = tempRoad;
    lastPaths.push_back(lastPath);
    //如果给定点就在当前road，直接返回
    if (1 == inputPaths.size())
        return lastPaths;
    cout << "First laneID is " << myPosInfo.laneId << endl;
    int tempRoadId = myPosInfo.roadId;
    //至此，roadDir = 0,同方向，找后继successor; 否则找前驱predecessor
    //cout << "Second tempRoadId" << tempRoad->getId()<< endl;
    OpenDrive::JuncHeader juncHeader;
    while(tempRoadId != endRoadId){
        //判断方向来寻找前驱后继
        if((roadDir == 0) && (tempRoad->getSuccessor()))
        {
            //cout << "tempRoadId" << tempRoad->getId();
            roadDir = tempRoad->getSuccessorDir();
            if(tempRoad->getSuccessor() != NULL)
            {
                tempRoad = tempRoad->getSuccessor();
            }else{
                tempRoad = tempRoad->getSuccessor(1);

            }
            tempRoadId = tempRoad->getId();

        }
        else if ((roadDir == 1) && (tempRoad->getPredecessor()))
        {

            roadDir = tempRoad->getPredecessorDir();
            tempRoad = tempRoad->getPredecessor();
            tempRoadId = tempRoad->getId();
        }
        else
        {
            cout << "The" << iter << "st road has no predecessor or successor." << endl;
            break;
        }
        cout << "tempRoadId" << tempRoad->getId()<< endl;
        //roads.push_back(tempRoad);;
        iter ++;
    }
    return lastPaths;
}

CASpecialArea GetPositionInfo::TraversalSpecialAreaInCurrentRoad(const string keyWord, CASpecialArea &specialArea){
    Object* tempObject = myPosInfo.road->getFirstObject();
    int status = 0;
    double objectS = 0;
    //Object* currentObject;
    while(tempObject != NULL){
        string tempObjectName = tempObject->mName;
        //此处默认同向
        objectS = tempObject->mS + tempObject->mLength;
        if(tempObjectName.find(keyWord) != tempObjectName.npos && (myPosInfo.trackS <= objectS) )
        {

            cout << keyWord <<" == " << tempObjectName << endl;
            status = 1;
            break;
        }
        //长安不包含此情况
        if(myPosInfo.laneId > 0){
            if(tempObjectName.find(keyWord) == tempObjectName.npos && myPosInfo.trackS >= objectS )
            {
                status = 1;
                break;
            }
        }
        //更新
        tempObject = (Object *)tempObject->getRight();
    }
    //1.2、第一个road里边就存在有效特殊地区
    if (myKeyWords.at(0) == keyWord) specialArea.type = kSATTunnel;
    else if (myKeyWords.at(1) == keyWord) specialArea.type = kSATToll;
    else if (myKeyWords.at(2) == keyWord) specialArea.type = kSATConstruction;
    else specialArea.type = kSATUnkown;//存疑
    if(1 == status){
        specialArea.specialAreaLength = tempObject->mLength;
        //默认同向
        if(myPosInfo.trackS == tempObject->mS + tempObject->mLength)
        {
            specialArea.relatedType = kCRSALeave;
            specialArea.toSpecialArea = 0;
        } else if(myPosInfo.trackS <= tempObject->mS){
            specialArea.relatedType = kCRSAEnter;
            specialArea.toSpecialArea = tempObject->mS - myPosInfo.trackS;
        } else{
            specialArea.relatedType = kCRSAIn;
            specialArea.toSpecialArea = tempObject->mS + tempObject->mLength - myPosInfo.trackS;
        }
        return specialArea;
    }
    return specialArea;
}

//在下一个road开始遍历，若有则都为入口
CASpecialArea GetPositionInfo::TraversalSpecialAreaInRoad(RoadHeader *tempRoad, Object* tempObject, const string keyWord,
                                                         const int dir, CASpecialArea &specialArea) {
    tempObject = tempRoad->getFirstObject();
    while(tempObject != NULL){
        string tempObjectName = tempObject->mName;
        if(tempObjectName.find(keyWord) != tempObjectName.npos)
        {
            specialArea.specialAreaLength = tempObject->mLength;
            specialArea.relatedType = kCRSAEnter;
            //在此处设置为距离当前road起始点的距离
            specialArea.toSpecialArea = tempObject->mS;
            //如果反向，特殊区在当前road的距离需要修改
            if (dir > 0)
                specialArea.toSpecialArea = tempRoad->mLength - tempObject->mS;
            break;
        }
        tempObject = (Object *)tempObject->getRight();
    }
    return specialArea;
}
//ll3.计算主车前一定距离的曲率
double GetPositionInfo::GetForwardCurvature(double viewRange) {
    // 输入：
    // （1）p:车辆的x,y,z坐标
    // (2) viewRange:车辆前方的距离
    // 输出：
    // (1)curve: 车辆前方viewRange处的曲率；若点不在road上，curve=9999.
    double curve;
    if (!myPosInfo.road) {
        curve = 9999;
        return curve;
    }
    RoadHeader *road = myPosInfo.road;
//    int roadid=m_mapInfo.roadId;
    int laneid = myPosInfo.laneId;
//    cout<<"laneidyuanshi:"<<laneid<<endl;
    double curS = myPosInfo.trackS;
    int drivingDir = DrivingDirRelToRoad(road->mRHT);  //判断主车行驶方向与road方向是否相同
    //同向，主车前方找路的后继
    if (drivingDir == 0) {
//        cout << "drivingDir1:" << drivingDir << endl;
        double Slength = road->getLength() - curS;
        double targetS = curS;
        int nextRoadDir = -1;
        bool getNextFlag = 1;  //判断是找前驱还是后继 0-前驱，1-后继
//        cout << "curS" << curS << endl;
        RoadHeader *nextRoad = NULL;
        while (Slength < viewRange) {
//            cout << "Slength:" << Slength << endl;
            if ((getNextFlag == 1) && (road->getSuccessorNode())) {
                nextRoad = road->getSuccessor();
                nextRoadDir = (int) road->getSuccessorDir();
                road = nextRoad;
                Slength += road->mLength;
//                cout << "Slength11:" << Slength << endl;
//                cout << "id:" << nextRoad->getId() << endl;
            }
            else if ((getNextFlag == 0) && (road->getPredecessorNode())) {
                nextRoad = road->getPredecessor();
                nextRoadDir = (int) road->getPredecessorDir();
                road = nextRoad;
                Slength += road->mLength;
//                cout << "Slength11:" << Slength << endl;
//                cout << "id:" << nextRoad->getId() << endl;
            } else {
                break;
//                return 0;
            }
        }
        if (nextRoadDir == 0) {
//            cout << "111111" << endl;
            targetS = road->mLength - (Slength - viewRange);
//            cout << "targetS" << targetS << endl;
            getNextFlag = 1;
        } else if (nextRoadDir == 1) {
//            cout << "22222222" << endl;
            targetS = Slength - viewRange;
            getNextFlag = 0;
        } else {
//            cout << "3333333" << endl;
            targetS += viewRange;
        }

//        cout << "zuizhongid:" << road->getId() << endl;
//        cout << "zuizhongS:" << targetS << endl;
        //TODO 此处设置t为0，还需优化
        myManager.setTrackPos(road->getId(), targetS, 0);
        myManager.track2curvature();
        curve = myManager.getCurvature();
//        cout << "curve:" << curve << std::endl;
//        double curve_lane = m_manager.getLaneCurvature();
//        cout << "curve_lane:" << curve_lane << std::endl;
    }
//反向，主车前方找路的前驱
    if (drivingDir == 1){
//        cout<<"drivingDir1:"<<drivingDir<<endl;
        double Slength = curS;
        double targetS = curS;
        int nextRoadDir = -1;
        bool getNextFlag = 0;
//        cout<<"curS"<<curS<<endl;
        RoadHeader *nextRoad = NULL;
        while (Slength<viewRange){
//            cout << "Slength:" << Slength << endl;
            if ((getNextFlag == 0) && (road->getPredecessorNode())) {
                nextRoad = road->getPredecessor();
                nextRoadDir = (int) road->getPredecessorDir();
                road = nextRoad;
                Slength += road->mLength;
//                cout << "Slength11:" << Slength << endl;
//                cout << "id:" << nextRoad->getId() << endl;
            }
            else if ((getNextFlag == 1) && (road->getSuccessorNode())) {
                nextRoad = road->getSuccessor();
                nextRoadDir = (int) road->getSuccessorDir();
                road = nextRoad;
                Slength += road->mLength;
//                cout << "Slength11:" << Slength << endl;
//                cout << "id:" << nextRoad->getId() << endl;
            }
            else {
                break;
//                return 0;
            }
        }
        if (nextRoadDir == 0) {
//            cout << "111111,butongyu myroad" << endl;
            targetS = road->mLength - (Slength - viewRange);
//            cout << "targetS" << targetS << endl;
            getNextFlag = 1;
        }
        else if (nextRoadDir == 1) {
//            cout << "22222222" << endl;
            targetS = Slength - viewRange;
            getNextFlag = 0;
        }
        else{
//            cout<<"3333333"<<endl;
            targetS -=viewRange;
        }
//        cout << "zuizhongid:" << road->getId() << endl;
//        cout << "zuizhongS:" << targetS << endl;
        //TODO 此处设置t为0，还需优化
        myManager.setTrackPos(road->getId(), targetS, 0);
        myManager.track2curvature();
        curve = myManager.getCurvature();
//        cout << "curve:" << curve << std::endl;
    }
    return curve;
}

pair<int, vector<int>*> GetPositionInfo::GetLaneIdNumInLaneSection(LaneSection *laneSection) {
    vector<int> *laneIds = new vector<int>;
    //初始laneID
    Lane *lane = laneSection->getLaneFromId(0);
    //首先从左边开始遍历
    int iter = 1;
    /*while(lane->getLeft() != NULL)
    {
        //后续标准opendrive需要再写
        *//*laneIds->push_back(lane->getId());
        lane = (Lane *)lane->getRight();
        iter ++;*//*
    }*/
    while(lane->getRight() != NULL)
    {
        laneIds->push_back(lane->getId());
        lane = (Lane *)lane->getRight();
        iter ++;
    }
    return make_pair(laneIds->size(), laneIds);
}

vector<pair<int, LaneSection *>> &GetPositionInfo::GetLaneSectionsByEndPoint(GuidePaths &guidePaths,
                                                                             vector<pair<int, LaneSection *>> &laneSections){

    //默认同向
    RoadHeader *tempRoad = myPosInfo.road->getSuccessor(1);
    pair<int, LaneSection*> laneSection;
    laneSection.second = myPosInfo.section;
    //1.1、给定终点就在当前road,默认同向
    while (laneSection.second != NULL)
    {
        laneSection.first = -1;
        laneSections.push_back(laneSection);
        if (laneSection.second->mS == myEndPosInfo.sectionS && 1 == guidePaths.size())
            return laneSections;
        laneSection.second = (LaneSection *)laneSection.second->getRight();
    }
    //1.2、反向
    if (myPosInfo.laneId > 0){
        while (laneSection.second != NULL)
        {
            laneSection.first = -1;
            laneSections.push_back(laneSection);
            if (laneSection.second->mS == myEndPosInfo.sectionS && 1 == guidePaths.size())
                return laneSections;
            laneSection.second = (LaneSection *)laneSection.second->getLeft();
        }
        tempRoad = myPosInfo.road->getPredecessor(1);
    }
    //2、给定终点不止当前road
    for(int i = 1; i < guidePaths.size(); i++)
    {
        laneSection.second = tempRoad->getFirstLaneSection();
        //第一个road和最后road需要特殊处理
        //最后一个需特殊处理
        if(i == guidePaths.size() - 1)
        {
            while (laneSection.second != NULL)
            {
                laneSection.first = guidePaths.at(i).first;
                laneSections.push_back(laneSection);
                if (laneSection.second->mS == myEndPosInfo.sectionS)
                    return laneSections;
                laneSection.second = (LaneSection *)laneSection.second->getRight();
            }
        }
        while (laneSection.second!= NULL)
        {
            laneSection.first = guidePaths.at(i).first;
            laneSections.push_back(laneSection);
            laneSection.second = (LaneSection *)laneSection.second->getRight();
        }

        tempRoad = tempRoad->getSuccessor();
        if(guidePaths.at(i).first > 0)
            tempRoad = tempRoad->getPredecessor();
    }
    return laneSections;

}
vector<LaneSection *> &GetPositionInfo::GetLaneSectionsByEndPoint(GuidePaths &guidePaths, vector<LaneSection *> &laneSections){
    //默认同向
    RoadHeader *tempRoad = guidePaths.at(1).second;
    LaneSection *laneSection = myPosInfo.section;
    laneSections.push_back(laneSection);
    //1.1、给定终点就在当前road,默认同向
    while (laneSection->getRight() != NULL)
    {
        if (laneSection->mS == myEndPosInfo.sectionS && 1 == guidePaths.size())
            return laneSections;
        laneSection = (LaneSection *)laneSection->getRight();
        laneSections.push_back(laneSection);
    }//
    //1.2、反向
    if (myPosInfo.laneId > 0){
        while (laneSection->getLeft() != NULL)
        {

            laneSections.push_back(laneSection);
            if (laneSection->mS == myEndPosInfo.sectionS && 1 == guidePaths.size())
                return laneSections;
            laneSection = (LaneSection *)laneSection->getLeft();
        }
        tempRoad = guidePaths.at(1).second;
    }
    //2、给定终点不止当前road
    for(int i = 1; i < guidePaths.size(); i++)
    {
       // cout << "rotateNum begin: " << i << endl;
       tempRoad = guidePaths.at(i).second;
        laneSection = tempRoad->getFirstLaneSection();
        laneSections.push_back(laneSection);
        //第一个road和最后road需要特殊处理
        //最后一个特殊处理//此处针对的初始方向是同向,laneID<0
        if(i == guidePaths.size() - 1 && guidePaths.at(0).first < 0)
        {
            while (laneSection->getRight() != NULL)
            {
                if (laneSection->mS == myEndPosInfo.sectionS)
                    return laneSections;
                laneSection = (LaneSection *)laneSection->getRight();
                laneSections.push_back(laneSection);
            }
        }

        /*//暂时不考虑反向，，
         * if(i == guidePaths.size() - 1 && guidePaths.at(0).first > 0)
        {
            while (laneSection->getLeft() != NULL)
            {
                laneSections.push_back(laneSection);
                if (laneSection->mS == endPosInfo.sectionS)
                    return laneSections;
                laneSection = (LaneSection *)laneSection->getRight();
            }
        }*/
        //普通的完整road
        while (laneSection->getRight()!= NULL)
        {
            laneSection= (LaneSection *)laneSection->getRight();
            laneSections.push_back(laneSection);
        }

       // cout << "rotateNum end: " << i << endl;
    }
    return laneSections;

}



CaLaneType GetPositionInfo::LaneTypeMapping(const int odrLaneType){
    if(odrLaneType == ODR_LANE_TYPE_EXIT)
        return kLaneTExit;
    else if(odrLaneType == ODR_LANE_TYPE_ENTRY)
        return kLaneTEntry;
    else if(odrLaneType == ODR_LANE_TYPE_SIDEWALK)
        return kLaneSidewalk;
    else if(odrLaneType == ODR_LANE_TYPE_BIKING)
        return kLaneNonMotorVehicle;
    else
        return kLaneTUnknown;
}

CaLineMarkingType GetPositionInfo::RoadMarkTypeMapping(const int odrType){
    if(odrType == ODR_ROAD_MARK_TYPE_SOLID)
        return kLMTSolidLine;
    else if(odrType == ODR_ROAD_MARK_TYPE_BROKEN)
        return kLMTDashedLine;
    else if(odrType == ODR_ROAD_MARK_TYPE_SOLID_SOLID)
        return kLMTDoubleSolidLine;
    else if(odrType == ODR_ROAD_MARK_TYPE_BROKEN_BROKEN)
        return  kLMTDoubleDashedLine;
    else if(odrType == ODR_ROAD_MARK_TYPE_SOLID_BROKEN)
        return kLMTLeftSolidRightDashed;
    else if(odrType == ODR_ROAD_MARK_TYPE_BROKEN_SOLID)
        return  kLMTRightSolidLeftDashed;
    //待改进
    else
        return kLMTUnknown;
}

CaColor GetPositionInfo::RoadMarkColorMapping(const int odrColor){
    if(odrColor == ODR_ROAD_MARK_COLOR_WHITE)
        return kWhite;
    else if(odrColor == ODR_ROAD_MARK_COLOR_YELLOW)
        return kYellow;
    else if(odrColor == ODR_ROAD_MARK_COLOR_ORANGE)
        return kOrange;
    else if(odrColor == ODR_ROAD_MARK_COLOR_RED)
        return kRed;
    else if(odrColor == ODR_ROAD_MARK_COLOR_BLUE)
        return kBlue;
    else if(odrColor == ODR_ROAD_MARK_COLOR_GREEN)
        return kGreen;
    else
        return kUnknown;
}
CaRoadType GetPositionInfo::RoadTypeMapping(const int &roadType){
    if(ODR_LANE_TYPE_DRIVING == roadType)
        return kRoadTHighway;
    else if(ODR_LANE_TYPE_ENTRY == roadType || ODR_LANE_TYPE_EXIT == roadType
            || ODR_LANE_TYPE_ON_RAMP == roadType|| ODR_LANE_TYPE_OFF_RAMP == roadType)
        return kRoadTRamp;
    else
        return kRoadTInvalid;
}

CALaneWidthCurvature GetPositionInfo::CurvatureReparse(GeoHeader* geoHeader, CALaneWidthCurvature &curvature){
    if(geoHeader->mCurv == geoHeader->mCurvEnd)
        curvature.value = geoHeader->mCurv;
    else
        curvature.value = 999;
    //此处若为999(螺旋线)怎么计算后边再调整
    //curvature.value = geoHeader->mCurv;
    //curvature.valueEnd = geoHeader->mCurvEnd;
    curvature.dis = geoHeader->mS;
    curvature.valueEnd = geoHeader->mSEnd;
    return curvature;
}

Curvatures GetPositionInfo::GetCurvatureInRoad(int originalDir, int iter, OpenDrive::RoadHeader*roadHeader,
                                               Curvatures &curvaturesRoad){
    //默认同向
    CALaneWidthCurvature curvature;
    double curEnd;
    GeoHeader *geoHeader = roadHeader->getFirstGeoHeader();
    if(originalDir <= 0){
        //第一个road特殊处理,要从第一个road的起始点开始计算曲率，在起点之前的曲率都无效
        /*while(0 == iter && geoHeader != NULL)
        {
            if(geoHeader->mSEnd >= myPosInfo.trackS)
            {
                curvature = CurvatureReparse(geoHeader, curvature);
                curvaturesRoad.push_back(curvature);
            }
            geoHeader = (GeoHeader *)geoHeader->getRight();
        }*/

        while( iter >= 0 && geoHeader != NULL)
        {
            curvature = CurvatureReparse(geoHeader, curvature);
            curvaturesRoad.push_back(curvature);
            geoHeader = (GeoHeader *)geoHeader->getRight();
        }
    }else{
        /*while(0 == iter && geoHeader != NULL)
        {
            if(geoHeader->mSEnd <= myPosInfo.trackS)
            {
                curvature = CurvatureReparse(geoHeader, curvature);
                curvaturesRoad.push_back(curvature);
            }
            geoHeader = (GeoHeader *)geoHeader->getLeft();
        }*/

        while( iter >= 1 && geoHeader != NULL)
        {
            curvature = CurvatureReparse(geoHeader, curvature);
            curvaturesRoad.push_back(curvature);
            geoHeader = (GeoHeader *)geoHeader->getLeft();
        }
    }

    return curvaturesRoad;
}

std::vector<OpenDrive::GeoHeader*> *GetPositionInfo::GetGeoHeadersRoad(int originalDir, int iter, OpenDrive::RoadHeader*roadHeader,
                                                       std::vector<OpenDrive::GeoHeader*> *geoHeadersRoad){
    //默认同向
    CALaneWidthCurvature curvature;
    double curEnd;
    GeoHeader *geoHeader = roadHeader->getFirstGeoHeader();
    if(originalDir <= 0){
        //第一个road特殊处理,要从第一个road的起始点开始计算曲率，在起点之前的曲率都无效
        while(0 == iter && geoHeader != NULL)
        {
            if(geoHeader->mSEnd >= myPosInfo.trackS)
            {
                geoHeadersRoad->push_back(geoHeader);
            }
            geoHeader = (GeoHeader *)geoHeader->getRight();
        }

        while( iter >= 0 && geoHeader != NULL)
        {
            geoHeadersRoad->push_back(geoHeader);
            geoHeader = (GeoHeader *)geoHeader->getRight();
        }
    }else{
        while(0 == iter && geoHeader != NULL)
        {
            if(geoHeader->mSEnd <= myPosInfo.trackS)
            {
                geoHeadersRoad->push_back(geoHeader);
            }
            geoHeader = (GeoHeader *)geoHeader->getLeft();
        }

        while( iter >= 1 && geoHeader != NULL)
        {
            geoHeadersRoad->push_back(geoHeader);
            geoHeader = (GeoHeader *)geoHeader->getLeft();
        }
    }

    return geoHeadersRoad;
}

Curvatures GetPositionInfo::GetCurLaneSection(LaneSection *laneSection, Curvatures &curvaturesRoad,
                                               Curvatures &curLaneSection){
    //曲率获取

    int curveStart = -1;
    int curveEnd = -1;
    CALaneWidthCurvature curvature;
    if(1 == curvaturesRoad.size()){
        for(int i = 0; i < curvaturesRoad.size(); i++)
        {
            //相对每个laneSection的位置
            curvature.dis = abs(curvaturesRoad.at(i).dis - laneSection->mS);
            curvature.value = curvaturesRoad.at(i).value;
//            cout << " curvature.dis: " << curvature.dis << endl;
//            cout << " curvature.value: " << curvature.value << endl;
            curLaneSection.push_back(curvature);
        }
        curLaneSection.at(0).dis = 0;
        return curLaneSection;
    }
    for(int i = 0; i < curvaturesRoad.size(); i++ )
    {
        //cout << " laneSection->mS " <<i <<" ; "<< laneSection->mS << endl;
        //cout << " ccurvaturesRoad.at(i).dis " <<i <<" ; "<< curvaturesRoad.at(i).dis << endl;
        if(abs(laneSection->mS - curvaturesRoad.at(i).dis) <= EPS){
            curveStart = i;
            break;
        }
        else if(laneSection->mS < curvaturesRoad.at(i).dis){
            curveStart = i -1 ;
            break;
        }
        else{
            curveStart = i; //表示异常
        }
    }
    //cout << "curveStart  : " << curveStart << endl;
    for(int j = curveStart; j < curvaturesRoad.size(); j++)
    {

        //cout << " laneSection->mSEnd " <<j <<" ; "<< laneSection->mSEnd << endl;
        //cout << " curvaturesRoad.at(j).valueEnd " <<j <<" ; "<< curvaturesRoad.at(j).valueEnd << endl;
        if(abs(laneSection->mSEnd - curvaturesRoad.at(j).valueEnd) <= EPS ){
            curveEnd = j;
            break;
        }
        else if(laneSection->mSEnd < curvaturesRoad.at(j).valueEnd){
            curveEnd = j - 1;
            break;
        }
        else{
            curveEnd = j; //表示异常，未找到合适区间
        }
    }
    //cout << "curveStart  : " << curveStart << endl;
    /*for(int j = curveStart; j < curvaturesRoad.size(); j++)
    {

        //cout << " laneSection->mSEnd " <<j <<" ; "<< laneSection->mSEnd << endl;
        //cout << " curvaturesRoad.at(j).valueEnd " <<j <<" ; "<< curvaturesRoad.at(j).valueEnd << endl;
        if(abs(laneSection->mSEnd - curvaturesRoad.at(j).valueEnd) <= EPS ){
            curveEnd = j;
            break;
        }
        else if(laneSection->mSEnd < curvaturesRoad.at(j).valueEnd){
            curveEnd = j;
            break;
        }
        else{
            curveEnd = -5; //表示异常，未找到合适区间
        }
    }*/
    //cout << "curveEnd  : " << curveEnd << endl;
    for(int j = curveStart; j <= curveEnd; j++)
    {
        //相对每个laneSection的位置
        curvature.dis = abs(curvaturesRoad.at(j).dis - laneSection->mS);
        curvature.value = curvaturesRoad.at(j).value;
        //cout << " curvature.dis: " << curvature.dis << endl;
        //cout << " curvature.value: " << curvature.value << endl;
        curLaneSection.push_back(curvature);
    }
    //初始处dis都为0
    curLaneSection.at(0).dis = 0;

    return curLaneSection;
}

bool GetPositionInfo::GetGeographicCoordinate(PJ_COORD &coord) {
    if(myPosInfo.geoReference.empty()){
        cout << "GeoReference missing..." << endl;
        return false;
    }
    char *targetProj = const_cast<char *>(myPosInfo.geoReference.c_str());
    //cout << "targetProj: " << targetProj << endl;

    PJ_CONTEXT *C;
    PJ *P;
    PJ *norm;
    PJ_COORD a;

    //a = proj_coord(point.x, point.y, point.z, 0);
    //cout << "Original coordX: " << coord.xy.x << "; Original coordY: " << coord.xy.y <<endl;
    C = proj_context_create();

    //"+proj=longlat +ellps=GRS80 +no_defs" = "EPSG:4326"
    P = proj_create_crs_to_crs (C, targetProj,OdrGeoReference, NULL);
    if (0 == P) {
        cout << "Failed to create transformation object." << stderr << endl;
        return false;
    }
    norm = proj_normalize_for_visualization(C, P);
    if (0 == norm) {
        cout << " Failed to normalize transformation object." << stderr << endl;
        return false;
    }
    proj_destroy(P);//释放投影
    P = norm;//投影赋值

    coord = proj_trans(P, PJ_INV, coord);
    //cout << "East: " << coord.xy.x << "; North:" << coord.xy.y <<endl;
    /*cout << "After the second transformation(recover): "<< endl;
    b = proj_trans(P, PJ_INV, b);
    cout << "Longitude: " << b.lp.lam << "; Latitude: " << b.lp.phi <<endl;*/

    proj_destroy(P);
    proj_context_destroy(C);
    return true;
}
bool GetPositionInfo::GetGeographicCoordinate(PJ_COORD &coord, char *targetProj) {
    //char *targetProj = const_cast<char *>(myPosInfo.geoReference.c_str());
    if(!targetProj){
        cout << "GeoReference missing..." << endl;
        return false;
    }
    cout << "targetProj: " << targetProj << endl;

    PJ_CONTEXT *C;
    PJ *P;
    PJ *norm;
    //PJ_COORD a;

    /* a coordinate union representing Copenhagen: 55d N, 12d E    */
    //a = proj_coord(12, 55, 0, 0);//设定待转换的投影坐标，此处分别为经度，纬度，高程，时间
    cout << "Original coordX: " << coord.xy.x << "; Original coordY: " << coord.xy.y <<endl;
    C = proj_context_create();

    //"+proj=longlat +ellps=GRS80 +no_defs" = "EPSG:4326"
    P = proj_create_crs_to_crs (C, targetProj,OdrGeoReference, NULL);

    if (0 == P) {
        cout << "Failed to create transformation object." << stderr << endl;
        return false;
    }
    norm = proj_normalize_for_visualization(C, P);
    if (0 == norm) {
        cout << " Failed to normalize transformation object." << stderr << endl;
        return false;
    }
    proj_destroy(P);//释放投影
    P = norm;//投影赋值

    //cout << "After the first transformation: "<< endl;
    //b = proj_trans(P, PJ_FWD, a);
    coord = proj_trans(P, PJ_INV, coord);
    cout << "East: " << coord.xy.x << "; North:" << coord.xy.y <<endl;
    /*cout << "After the second transformation(recover): "<< endl;
    b = proj_trans(P, PJ_INV, b);
    cout << "Longitude: " << b.lp.lam << "; Latitude: " << b.lp.phi <<endl;*/

    proj_destroy(P);
    proj_context_destroy(C);
    return true;
}


void GetPositionInfo::GetSDsOffsetSet(OpenDrive::RoadHeader* road, OpenDrive::LaneSection *laneSection, double step, vector<double> &sSectionSet,
                                      vector<double> &sToSectionDisSet, vector<double> &refSSet){
    Lane* tempLane = laneSection->getLaneFromId(0);
    double s = laneSection->mS;
    double targetS = s;
    double ds = 0;
    double ref;

    while(targetS<laneSection->mSEnd)
    {
        ds = targetS-laneSection->mS;
        ref = GetLaneOffset(road,targetS);
        sSectionSet.push_back(targetS);
        sToSectionDisSet.push_back(ds);
        refSSet.push_back(ref);
        targetS += step;
    }
}

void GetPositionInfo::GetWidthSSetList(OpenDrive::RoadHeader* road, OpenDrive::LaneSection *laneSection, double step,
                                       vector<double> &sSectionSet, vector<vector<double>> &widthSetList){
    Lane* tempLane = laneSection->getLaneFromId(0);
//    Lane *currentLane;
//只获取了右侧车道

    vector<double> v1(sSectionSet.size(),0);
    vector<double> widthSet;
    widthSetList.push_back(v1);
    double laneWidth;
    while(tempLane->getRight() != NULL)
    {
        tempLane = (Lane *)(tempLane->getRight());
        for(int i =0;i<sSectionSet.size();i++){
            laneWidth = GetLaneWidth(tempLane,sSectionSet.at(i));
            widthSet.push_back(laneWidth);
        }
        widthSetList.push_back(widthSet);
        widthSet.clear();
    }
}

void GetPositionInfo::GetSTSetList(vector<double> &sSectionSet, vector<double> &refSSet, vector<vector<double>> &widthSetList, vector<vector<double>> &tSetList,
                                   vector<vector<double>> &tClineSetList){
    //cout<<"widthSetList.size()"<<widthSetList.size()<<endl;
//    vector<double>t1(dsSet.size(),0);
    double t;
    vector<double> tSet;
    tSetList.push_back(refSSet);
    double tCline;
    vector<double>tClineSet;
    tClineSetList.push_back(refSSet);
    int j,k;
    for( j=1;j<widthSetList.size();j++){
//        cout<<"j="<<j<<endl;
        for(k=0;k<sSectionSet.size();k++){
//            cout<<"k="<<k<<endl;
            t=tSetList[j-1][k]-widthSetList[j][k];
            tSet.push_back(t);
            tCline = tSetList[j-1][k]-(1.0/2)*widthSetList[j][k];
            tClineSet.push_back(tCline);
        }
        tSetList.push_back(tSet);
        tSet.clear();
        tClineSetList.push_back(tClineSet);
        tClineSet.clear();
    }
}

Point GetPositionInfo::GetLanePointList(OpenDrive::RoadHeader* road, double s, double t, Point &point, bool InertialGeo = false){
    PJ_COORD geoCoord;
    myManager.setTrackPos(road->getId(), s, t);
    bool result = myManager.track2inertial();
    point = {myManager.getInertialPos().getX(), myManager.getInertialPos().getY(),myManager.getInertialPos().getZ()};
    if(InertialGeo) {
        //cout << "Original coord line: " << geoCoord.xy.x << " ; " << geoCoord.xy.y << endl;
        GetGeographicCoordinate(geoCoord);
        //cout << "Ll coord line: " << geoCoord.lp.lam << " ; " <<geoCoord.lp.phi << endl;
        point = {geoCoord.lp.lam, geoCoord.lp.phi, 0};
    }
    return point;
}

void GetPositionInfo::GetLaneLinePoints(OpenDrive::RoadHeader* road, OpenDrive::LaneSection *laneSection, double step, vector<vector<Gnss>>& linePointsList,
                                        vector<vector<Gnss>>& cLinePointsList){
    //获取laneSection 中lane的车道线轨迹点集，车道中心线轨迹点集
    vector<Gnss> linePoints;
    vector<Gnss> cLinePoints;
    vector<double> sSectionSet;
    vector<double> sToSectionDisSet;
    vector<double> refSSet;
    vector<vector<double>> widthSetList;
    vector<vector<double>> tSetList;
    vector<vector<double>> tClineSetList;

    GetSDsOffsetSet(road, laneSection, step, sSectionSet, sToSectionDisSet, refSSet);
    GetWidthSSetList(road, laneSection, step, sSectionSet, widthSetList);
    GetSTSetList(sSectionSet, refSSet, widthSetList, tSetList, tClineSetList);

    Point point;
    for( int n =0;n<tSetList.size();n++) {
        for (int m = 0; m < sSectionSet.size(); m++) {
//            cout<<"m="<<m<<",n="<<n;
            point = GetLanePointList(road, sSectionSet.at(m), tSetList[n][m], point, false);
            Gnss caPoint(point.x, point.y);
            linePoints.push_back(caPoint);

            double curve = myManager.getCurvature();
            bool result = myManager.track2curvature();

            point = GetLanePointList(road, sSectionSet.at(m), tClineSetList[n][m], point, false);
            caPoint = {point.x, point.y};
            cLinePoints.push_back(caPoint);
            if (curve == 0)
                m = m + 1;
        }
        //cout<<"linePoints"<<linePoints.size()<<endl;
        linePointsList.push_back(linePoints);
        linePoints.clear();
        cLinePointsList.push_back(cLinePoints);
        cLinePoints.clear();
    }
}

bool GetPositionInfo::ProjTest() {

    PJ_CONTEXT *C;//用于处理多线程程序
    PJ *P;//初始化投影目标
    PJ *norm;//初始化投影目标
    PJ_COORD a, b;//初始化投影坐标

    /* or you may set C=PJ_DEFAULT_CTX if you are sure you will     */
    /* use PJ objects from only one thread                          */
    C = proj_context_create();//创建多线程，由于本示例为单线程，此处为展示作用

    //"+proj=longlat +ellps=GRS80 +no_defs" = "EPSG:4326"
    P = proj_create_crs_to_crs (C,
                                "EPSG:4326",//源投影
                                "+proj=utm +zone=32 +datum=WGS84", //目标投影
                                NULL);//创建在线程C内两个投影关系之间的相互转换

    if (0 == P) {
        cout << "Failed to create transformation object." << stderr << endl;
        return 0;
    }//如果P中两个投影的字符串不符合proj定义，提示转换失败

    norm = proj_normalize_for_visualization(C, P);//在线程C内使投影目标P和norm拥有相同的坐标格式，此处为经纬度
    if (0 == norm) {
        cout << " Failed to normalize transformation object." << stderr << endl;
        return 0;
    }//norm为0，说明格式同步失败
    proj_destroy(P);//释放投影
    P = norm;//投影赋值

    /* a coordinate union representing Copenhagen: 55d N, 12d E    */
    a = proj_coord(12, 55, 0, 0);//设定待转换的投影坐标，此处分别为经度，纬度，高程，时间
    cout << "Original longitude: " << a.xy.x << "; Original latitude: " << a.xy.y <<endl;
    cout << "After the first transformation: "<< endl;
    b = proj_trans(P, PJ_FWD, a);
    cout << "East: " << b.xy.x << "; North:" << b.xy.y <<endl;
    cout << "After the second transformation(recover): "<< endl;
    b = proj_trans(P, PJ_INV, b);
    cout << "Longitude: " << b.lp.lam << "; Latitude: " << b.lp.phi <<endl;

    proj_destroy(P);
    proj_context_destroy(C);
    return 1;
}
int GetPositionInfo::JudgeRampAbout(OpenDrive::Lane *tempLane, int &laneStatus) {
    Lane *currentLane;
    while(tempLane->getRight() != NULL)
    {
        currentLane = tempLane;
        tempLane = (Lane *)(tempLane->getRight());
        if(ODR_LANE_TYPE_ENTRY == currentLane->getType() || ODR_LANE_TYPE_EXIT == currentLane->getType()
           || ODR_LANE_TYPE_ON_RAMP == currentLane->getType()|| ODR_LANE_TYPE_OFF_RAMP == currentLane->getType()
           || ODR_LANE_TYPE_CONNECTING_RAMP == currentLane->getType())
        {
            //待写详细代码
            laneStatus = currentLane->getType();
            break;
        }
        else
            laneStatus = 0;
    }
     //左边遍历结束，如果已经存在ramp则不进行右遍历
     if(0 == laneStatus)
     {
         while(tempLane->getLeft() != NULL)
         {
             currentLane = tempLane;
             tempLane = (Lane *)(tempLane->getLeft());
             if(ODR_LANE_TYPE_ENTRY == currentLane->getType() || ODR_LANE_TYPE_EXIT == currentLane->getType()
                || ODR_LANE_TYPE_ON_RAMP == currentLane->getType()|| ODR_LANE_TYPE_OFF_RAMP == currentLane->getType()
                || ODR_LANE_TYPE_CONNECTING_RAMP == currentLane->getType()){
                 laneStatus = currentLane->getType();
                 break;
             }
             else
                 laneStatus = 0;
         }
     }
    return laneStatus;
}

int GetPositionInfo::JudgeRampAbout(OpenDrive::Lane *tempLane) {
    Lane *currentLane;
    int laneStatus;
    while(tempLane->getRight() != NULL)
    {
        currentLane = tempLane;
        tempLane = (Lane *)(tempLane->getRight());
        if(ODR_LANE_TYPE_ENTRY == currentLane->getType() || ODR_LANE_TYPE_EXIT == currentLane->getType()
           || ODR_LANE_TYPE_ON_RAMP == currentLane->getType()|| ODR_LANE_TYPE_OFF_RAMP == currentLane->getType()
           || ODR_LANE_TYPE_CONNECTING_RAMP == currentLane->getType())
        {
            //待写详细代码
            laneStatus = currentLane->getType();
            break;
        }
        else
            laneStatus = 0;
    }
    //左边遍历结束，如果已经存在ramp则不进行右遍历
    if(0 == laneStatus)
    {
        while(tempLane->getLeft() != NULL)
        {
            currentLane = tempLane;
            tempLane = (Lane *)(tempLane->getLeft());
            if(ODR_LANE_TYPE_ENTRY == currentLane->getType() || ODR_LANE_TYPE_EXIT == currentLane->getType()
               || ODR_LANE_TYPE_ON_RAMP == currentLane->getType()|| ODR_LANE_TYPE_OFF_RAMP == currentLane->getType()
               || ODR_LANE_TYPE_CONNECTING_RAMP == currentLane->getType()){
                laneStatus = currentLane->getType();
                break;
            }
            else
                laneStatus = 0;
        }
    }
    return laneStatus;
}

double GetPositionInfo::GetLaneOffset(OpenDrive::RoadHeader* road,  double s){
    double offset=0;
    LaneOffset* laneOffset =  reinterpret_cast<LaneOffset *>(road->getChild(ODR_OPCODE_LANE_OFFSET));
//    laneOffset->printData();
    if(laneOffset){
        LaneOffset* curLaneOffset = laneOffset;
        while(s>curLaneOffset->mSEnd){
            curLaneOffset = (LaneOffset*)curLaneOffset->getRight();
        }
        offset = curLaneOffset->s2offset(s);
    }
    return offset;
}

double GetPositionInfo::GetLaneWidth(OpenDrive::Lane *lane,  double ds){
    LaneWidth *curLaneWidth = lane->getFirstWidth();
    LaneWidth *tempLaneWidth;
    double width;
    //后期需要计算的时候再添加，
    /*width = laneWidth->mA - laneWidth->mB * laneWidth->mOffset + laneWidth->mC* (pow(laneWidth->mOffset, 2))
            - laneWidth->mD * pow(-laneWidth->mOffset, 3);*/
//    width = laneWidth->mA;
    while(curLaneWidth != NULL && ds > curLaneWidth->mOffsetEnd){
        tempLaneWidth = curLaneWidth;
        curLaneWidth = (LaneWidth*)curLaneWidth->getRight();
    }
    width = tempLaneWidth->ds2width(ds);
//    cout<<"width"<<width<<endl;
    return width;
}


vector<pair<int, int >> GetPositionInfo::ConvertPoints2Paths(vector<Point> &inputPoints, vector<pair<int, int >>&paths){
    int laneId,roadId;
    for(int i=0;i<inputPoints.size();i++){
        myManager.setInertialPos(inputPoints.at(i).x,inputPoints.at(i).y,inputPoints.at(i).z);
        bool result = myManager.inertial2lane();
        if (result){
            laneId = myManager.getLane()->mId;
            roadId = myManager.getRoadHeader()->mId;
            paths.push_back(make_pair(laneId,roadId));
        }
    }
    return paths;
}

void GetPositionInfo::ReaderXMLTest(){
    ReaderXML *l = new ReaderXML();

    l->setFilename("..\\data\\Germany_2018.xodr");
    l->read();
    RoadData *r = new RoadData(l->getRootNode());
    //GeoReference *g = dynamic_cast<GeoReference *>(r->findFirstNode(ODR_OPCODE_GEO_REFERENCE));
    //g->printData();
    //cout << g->getCDATA();

    Projection * projection = r->getProjection();
    cout << r->getProjection();
    //projection->setOGCWKT(g->getCDATA());
    projection->print();
    Header *h = r->getOdrHeader();
    h->printData();
    Header *header = dynamic_cast<Header *>(r->findFirstNode(ODR_OPCODE_HEADER));
    header->printData();

    Coord coord{365, -3023.28, 0};
    GeoCoord geoCoord;
    int a = projection->inertial2geo(coord, geoCoord);
    cout << "a: " << a <<endl;
    geoCoord.print();
}

double GetPositionInfo::GetEndPointLength(GuidePaths &guidePathsEndPoint, Point &endPoint){
    double length = 0;
    if(guidePathsEndPoint.at(0).first > 0) length = myPosInfo.trackS;
    else length = myPosInfo.roadlength - myPosInfo.trackS;
    for(int i = 1; i < guidePathsEndPoint.size() - 1; i++ ){
        length += guidePathsEndPoint.at(i).second->mLength;
    }
    if(guidePathsEndPoint.at(guidePathsEndPoint.size() - 1).first > 0) length = myEndPosInfo.trackS;
    else length = myEndPosInfo.roadlength - myEndPosInfo.trackS;
    return length;
}

//计算s处螺旋线曲率,默认输入的geoHeader就是螺旋线
double GetPositionInfo::GetSpiralCurvature(GeoHeader* geoHeader,double s){
    double curvatureS = geoHeader->mCurv + ((geoHeader->mCurvEnd-geoHeader->mCurv)/(geoHeader->mSEnd-geoHeader->mS))*(s - geoHeader->mS);
    return curvatureS;
}

void GetPositionInfo::GetCustomMessage(const Point point) {


}

void GetPositionInfo::Test() {
    myManager.setLanePos(54, -1, 0, 0);
    myManager.track2inertial();
    RoadHeader *roadHeader = myManager.getRoadHeader();
    //roadHeader->printData();
   // cout << "lll" << roadHeader->getPredecessor();

    PJ_COORD geoCoord;
    geoCoord = {2577.44, -3315.51,
                0, 0};
    cout << "Original coord: " << geoCoord.xy.x << " ; " << geoCoord.xy.y << endl;
    GetGeographicCoordinate(geoCoord);
    cout << "Ll coord: " << geoCoord.xy.x << " ; " << geoCoord.xy.y << endl;

    geoCoord = {2996.44, -3023.28, 0, 0};
    cout << "Original coord2: " << geoCoord.xy.x << " ; " << geoCoord.xy.y << endl;
    GetGeographicCoordinate(geoCoord);
    cout << "Ll coord2: " << geoCoord.xy.x << " ; " << geoCoord.xy.y << endl;


}

tuple<int,int,double,LaneSection*> GetPositionInfo::TraversalRoad(RoadHeader *tempRoad, double currentLength,pair<int,int> &rampStatusId,
                                                                  tuple<int,int,double,LaneSection*> &rampStatusIdCurLength ){
//    int rampStatus = 0,id;
    LaneSection* currentLaneSection;
    LaneSection* tempLaneSection = tempRoad->getFirstLaneSection();
    rampStatusId = TraversalLane(tempLaneSection->getLaneFromId(0));
    currentLength +=tempLaneSection->mSEnd-tempLaneSection->mS;
    if(0!=rampStatusId.first||currentLength>myRange)
        return make_tuple(rampStatusId.first,rampStatusId.second,currentLength,tempLaneSection);
    while(tempLaneSection->getRight()!=NULL){
        tempLaneSection = (LaneSection*)(tempLaneSection->getRight());
        //cout<<"iii"<<tempLaneSection->mS<<endl;
        currentLaneSection = tempLaneSection;
        rampStatusId = TraversalLane(currentLaneSection->getLaneFromId(0));
        currentLength +=currentLaneSection->mSEnd-currentLaneSection->mS;
//        rampStatus = TraversalLane(currentLaneSection->getLaneFromId(0)).first;
//        id = TraversalLane(currentLaneSection->getLaneFromId(0)).second;
        if(0!=rampStatusId.first||currentLength>myRange) break;

    }
    return make_tuple(rampStatusId.first,rampStatusId.second,currentLength,currentLaneSection);
}




static int RangeStatus(const string currentRamp){
    const string stringOnRamp = "on_ramp";
    const string stringOffRamp = "off_ramp";
    const string stringConnectingRamp = "connecting_ramp";
    int rampStatus = 0;//未知、无
    if (stringOnRamp == currentRamp )
    {
        rampStatus = 1;
    }
    if (stringOffRamp == currentRamp)
    {
        rampStatus = 2;
    }
    if (stringConnectingRamp == currentRamp)
    {
        rampStatus = 3;
    }
    return rampStatus;

}










