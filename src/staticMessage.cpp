//
// Created by JinxBIGBIG on 2022/5/30.
//
#include "staticMessage.h"
#include "OpenDRIVE.hh"
#include "BaseNodes/OdrTunnel.hh"
#include "BaseNodes/OdrLaneWidth.hh"
#include "AdditionNodes/OdrReaderXML.hh"
#include "tinyxml2.h"
#include <iostream>
#include <vector>


using namespace std;
using namespace OpenDrive;
using namespace tinyxml2;

StaticMap::StaticMap() {}



static void Print(const Curvatures &curvatures)
{
    for(int i = 0; i < curvatures.size(); i++)
    {
        cout << "CurvaturesRoad Information:" << endl;
        cout << "curvatures.dis: " << curvatures.at(i).dis << endl;
        cout << "curvatures.value: " << curvatures.at(i).value << endl;
        //cout << "curvatures.valueEnd: " << curvatures.at(i).valueEnd << endl;
    }
}

vector<CALink> StaticMap::GetCALink(GetPositionInfo &getPosInfo, GuidePaths &guidePaths) {

    cout << "GetCALink begins..." << endl;
    vector<LaneSection *> originalLaneSections;
    getPosInfo.GetLaneSectionsByEndPoint(guidePaths,originalLaneSections);
    //1.默认同向,第一个road的laneSection则进行右遍历
    double length = getPosInfo.myPosInfo.roadlength - getPosInfo.myPosInfo.trackS;
    if(getPosInfo.myPosInfo.laneId > 0)
        length = getPosInfo.myPosInfo.trackS;
    //1.1、计算给定起始点之间的距离
    double totalLength = length;
    for(int i = 0; i < originalLaneSections.size() -1; i++)
        totalLength += abs(originalLaneSections.at(i)->mS - originalLaneSections.at(i)->mSEnd);
    cout << "Rotation begins..." << endl;

    CALink link;
    int roadType;
    double speed;
    LaneSection *tempLaneSection = getPosInfo.myPosInfo.section;

    int iterRoad = 0;
    int iterLaneSection = 0;
    int originalDir = guidePaths.at(0).first;
    //2.设定循环条件，linkNum<=6(针对给定导航终点范围内包含laneSection数大于6的情况，且始终计算当前已计算距离是否已超过终点距离)，
    // 且在给定road里边进行计算(防止不足6个再进行计算出错)
    double curveLength;
    RoadHeader *roadHeader;
    Curvatures curvaturesLaneSection;
    //iterRoad < guidePaths.size()
    while(iterLaneSection < linkNum && iterRoad < guidePaths.size()){
        //2.2.1、road层级，speed可在此层级获取,曲率也在此层级获取
        cout << " Rotation iterRoad "<< iterRoad << " begins..." << endl;
        //源码里边是TrackCoord生效，故还需添加一步track2inertial
        getPosInfo.myManager.setLanePos(guidePaths.at(iterRoad).second->mId, guidePaths.at(iterRoad).first, 0, 0);
        getPosInfo.myManager.track2inertial();
        roadHeader = getPosInfo.myManager.getRoadHeader();
        //cout << "guidePaths.at(iterRoad).second->mId: " << guidePaths.at(iterRoad).second->mId << endl;
        cout << "RoadHeader->getId: " << roadHeader->getId() << endl;

        speed = getPosInfo.myManager.getLaneSpeed();

        //默认同向，曲率获取，每次都是新的曲率集合
        Curvatures curvaturesRoad;
        curvaturesRoad = getPosInfo.GetCurvatureInRoad(originalDir, iterRoad, roadHeader, curvaturesRoad);
        //vector<GeoHeader*> *curvaturesRoad = new vector<GeoHeader*>;
        //curvaturesRoad = getPosInfo.GetGeoHeadersRoad(originalDir, iterRoad, roadHeader, curvaturesRoad);
        cout << "curvaturesRoad.size: " << curvaturesRoad.size() << endl;
        //Print(curvaturesRoad);
        cout << "GetCurvatureInRoad rotation " << iterRoad << " over..." << endl;

        while(tempLaneSection != NULL && length <= totalLength)
        {
            //2.2.2、laneSection层级，车道连接关系中linkID可在此处获取(注意获取结束后，对其实laneSection的前驱进行单独处理)
            link.linkId = iterLaneSection;
            link.linkLength = abs(tempLaneSection->mSEnd -tempLaneSection->mS);

            //roadType获取
            getPosInfo.TraversalLane(tempLaneSection->getLaneFromId(0), roadType);
            roadType = getPosInfo.RoadTypeMapping(roadType);

            //获取laneSection层级的点集合，后期可优化为lane层级
            vector<vector<Gnss>> cLinePointsList;
            vector<vector<Gnss>> linePointsList;
            cout << "GetLaneLinePoints rotation " << iterRoad << " begins..." << endl;
            getPosInfo.GetLaneLinePoints(guidePaths.at(iterRoad).second, tempLaneSection, linePointsList, cLinePointsList);
            cout << "GetLaneLinePoints rotation " << iterRoad << " over..." << endl;
            //cout << "cLinePointsList.size()" << cLinePointsList.size() << endl;

            link.laneAttributes = GetCALaneAttribute(getPosInfo, speed, link.linkId, curvaturesRoad, cLinePointsList, guidePaths.at(iterRoad), tempLaneSection);

            /*if(iterRoad == 3){
                Print(link.laneAttributes.at(0).laneCurvatureList);
                for(int k = 0; k<link.laneAttributes.size();k++){
                    cout << " laneConnectivityList rotation :" << k << endl;
                    for(int x =0; x<link.laneAttributes.at(k).laneConnectivityList.size();x++)
                    {
                        cout << "laneConnectivityList message :" << endl;
                        cout <<link.laneAttributes.at(k).laneConnectivityList.at(x).linkId << " ; ";
                        cout <<link.laneAttributes.at(k).laneConnectivityList.at(x).laneNum << endl;
                    }
                }
            }*/
            link.line = GetCALine(getPosInfo, linePointsList, tempLaneSection);
            /*if(iterRoad == 3){
                for(int k = 0; k<link.line.size();k++){
                    cout << " markings rotation :" << k << endl;
                    for(int x =0; x<link.line.at(k).markings.size();x++)
                    {
                        cout << "markings message :" << endl;
                        cout <<link.line.at(k).markings.at(x).type << " ; ";
                        cout <<link.line.at(k).markings.at(x).color << " ; ";
                        cout <<link.line.at(k).markings.at(x).dis << endl;
                    }
                }
            }*/
            mLink.push_back(link);

            iterLaneSection ++;
            length += link.linkLength;
            //后续无laneSection直接返回
            tempLaneSection = dynamic_cast<LaneSection *>(tempLaneSection->getRight());
        }
        //更新
        //默认同向，若是反向要获取lastLaneSection
        if((guidePaths.size() - 1) == iterRoad)
            break;
        curveLength += roadHeader->mLength;
        iterRoad ++;
        tempLaneSection = guidePaths.at(iterRoad).second->getFirstLaneSection();
        cout << "iterLaneSection " << iterLaneSection << " ; iterRoad "<< iterRoad << "over..." << endl;
    }

    //3、不足6段填默认值补充
    if (mLink.size() < linkNum){
        for(int i = mLink.size() - 1; i < linkNum - 1; i++)
        {
            CALink nullLink;
            mLink.push_back(nullLink);
        }
    }
    return mLink;
}

vector<CALineMarking> &StaticMap::GetCALineMarking(GetPositionInfo &getPosInfo, Lane *lane, LaneSection* laneSection){
    CALineMarking lineMarking;
    RoadMark *roadMark = lane->getFirstRoadMark();
    //默认同向
    mLineMarking.clear();
    while(roadMark != NULL)
    {
        lineMarking.type = getPosInfo.RoadMarkTypeMapping(roadMark->mType);
        lineMarking.color = getPosInfo.RoadMarkColorMapping(roadMark->mColor);
        lineMarking.dis = roadMark->mS - laneSection->mS;
        mLineMarking.push_back(lineMarking);
        roadMark = dynamic_cast<RoadMark *>(roadMark->getRight());
    }
    return mLineMarking;
}

vector<CALine> StaticMap::GetCALine(GetPositionInfo &getPosInfo, vector<vector<Gnss>> &linePointsList, LaneSection* laneSection) {
    CALine line;
    Lane *tempLane = laneSection->getLaneFromId(0);
    //参考线、车道中心线、车道线共享一套索引
    int iter = 2;
    mLine.clear();
    int iterLine = 0;
    int kLine = 0;
    while(tempLane->getRight() != NULL && iter <= lineNum*2)
    {
        tempLane = dynamic_cast<Lane *>(tempLane->getRight());
        line.index = iter;
        //roadMark = tempLane->getFirstRoadMark();
        //注意最后一条是否是边界线
        line.lineType = kLineTLaneMarking;

        //line.markings;
        line.markings = GetCALineMarking(getPosInfo, tempLane, laneSection);

        //line.linePoints;
        //line.linePoints = &linePointsList.at(iter + 1);
        //line.linePointsNum = line.linePoints->size();

        iterLine ++;
        mLine.push_back(line);
    }
    return mLine;

}

vector<CALaneConnectivityInfo> StaticMap::GetCALaneConnectivityInfo(int dir, Lane *lane, const int linkID){
    mLaneConnectivityInfo.clear();
    CALaneConnectivityInfo laneConnectivityInfo;
    if(dir < 0){
        //1、同向
        //前驱写入
        laneConnectivityInfo.linkId = linkID - 1;
        if (lane->getPredecessor() != NULL)
            laneConnectivityInfo.laneNum = lane->getPredecessor()->mId;
        mLaneConnectivityInfo.push_back(laneConnectivityInfo);
        //后继写入
        laneConnectivityInfo.linkId = linkID + 1;
        if (lane->getSuccessor() != NULL)
            laneConnectivityInfo.laneNum = lane->getSuccessor()->mId;
        mLaneConnectivityInfo.push_back(laneConnectivityInfo);
    }else if(dir > 0){
        //2、反向
        //前驱写入
        laneConnectivityInfo.linkId = linkID + 1;
        if (lane->getSuccessor() != NULL)
            laneConnectivityInfo.laneNum = abs(lane->getSuccessor()->mId) - 1;
        mLaneConnectivityInfo.push_back(laneConnectivityInfo);
        //后继写入
        laneConnectivityInfo.linkId = linkID - 1;
        if (lane->getPredecessor() != NULL)
            laneConnectivityInfo.laneNum = abs(lane->getPredecessor()->mId) - 1;
        mLaneConnectivityInfo.push_back(laneConnectivityInfo);
    }
    return mLaneConnectivityInfo;
}
static void Print(vector<CALaneWidthCurvature> &curvatureList){
    cout << "curvatureList: " << endl;
    for(int i = 0; i< curvatureList.size(); i++)
    {
        cout << curvatureList.at(i).dis << "; " << curvatureList.at(i).value << endl;
    }
}

vector<CALaneAttribute> StaticMap::GetCALaneAttribute(GetPositionInfo &getPosInfo, double speed, int linkId,
                                                      Curvatures &curvaturesRoad, vector<vector<Gnss>> &cLinePointsList,
                                                      pair<int, OpenDrive::RoadHeader*> &road, LaneSection* laneSection) {

    CALaneAttribute laneAttribute;
    Lane *tempLane = laneSection->getLaneFromId(0);
    CASpeedLimit speedLimit;
    //1、长安只有同向的lane
    int laneID = 2;
    //1.1
    int iter =0;

    //曲率是laneSection层级的，计算一次即可
    Curvatures curLaneSection;
    curLaneSection = getPosInfo.GetCurLaneSection(laneSection, curvaturesRoad, curLaneSection);
    mLaneAttribute.clear();
    while(tempLane->getRight() != NULL && iter < laneNum)
    {
        tempLane = dynamic_cast<Lane *>(tempLane->getRight());
        laneAttribute.type = getPosInfo.LaneTypeMapping(tempLane->getType());
        speedLimit.value = speed;//此处只给value值，限速类型无法获取，由vtd输出
        laneAttribute.speedLimit = speedLimit;
        laneAttribute.leftIndex = laneID ;
        laneAttribute.centerIndex = laneID + 1;
        laneAttribute.rightIndex = laneID + 2;

        //laneAttribute.laneClinePoints;
        //laneAttribute.laneClinePoints = &cLinePointsList.at(iter + 1);

        //laneAttribute.laneCurvatureList;
        laneAttribute.laneCurvatureList = curLaneSection;



        //laneAttribute.laneConnectivityList;
        laneAttribute.laneConnectivityList = GetCALaneConnectivityInfo(road.first, tempLane, linkId);

        //laneAttribute.curvatureNum;
        laneAttribute.curvatureNum = laneAttribute.laneCurvatureList.size();

        //laneAttribute.laneWidth;
        laneAttribute.laneWidth = tempLane->getFirstWidth()->mA;

        //laneAttribute.clinePointsNum
        //laneAttribute.clinePointsNum = laneAttribute.laneClinePoints->size();

        mLaneAttribute.push_back(laneAttribute);
        iter ++;
    }

    return mLaneAttribute;
}

CAStaticHDMapErc StaticMap::GetCAStaticHDMapErc(GetPositionInfo &getPosInfo,
                                                vector<std::pair<int, OpenDrive::RoadHeader *>> &guidePaths) {
    mStaticHdMapErc.linkNum = linkNum;
    //mStaticHdMapErc.hdMapStatus;
    mStaticHdMapErc.links = GetCALink(getPosInfo, guidePaths);
    return mStaticHdMapErc;
}




/*
static void PrintCALinkInfo(CALink& caLink){
    cout << "CALink information: " << endl;
    cout << "link_id = " << caLink.link_id << ", "
         << "link_length = " << caLink.link_length << ", "
         << "type = " << caLink.type << ", "
         << "lane_num = " << caLink.lane_num << ", "
         << "line_num = " << caLink.line_num << ", "
         << "grade = " << caLink.grade << endl;
}*/
