//
// Created by JinxBIGBIG on 2022/5/30.
//

//
// Created by JinxBIGBIG on 2022/4/14.
//

#include "dynamiciMessage.h"
#include "OpenDRIVE.hh"
#include "BaseNodes/OdrTunnel.hh"
#include "AdditionNodes/OdrReaderXML.hh"
#include "tinyxml2.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace OpenDrive;
using namespace tinyxml2;


DynamicMap::DynamicMap() {}



//dynamic
static void PrintAnchorPosRelatedInfo(CAAnchorPosRelatedInfo &anchorPosRelatedInfo){
    cout << "AnchorPosRelatedInfo information: " << endl;
    cout << "numOfLanes = " << anchorPosRelatedInfo.numOfLanes << ", "
         << "currentLaneNum = " << anchorPosRelatedInfo.currentLaneNum << ", "
         << "withinGeofence = " << anchorPosRelatedInfo.withinGeofence << ", "
         << "currentLinkId = " << anchorPosRelatedInfo.currentLinkId <<","
         << "toLinkEnd = " << anchorPosRelatedInfo.toLinkEnd << ", "
         << "toGeoFenceEnd = " << anchorPosRelatedInfo.toGeoFenceEnd << endl;
}
//jl10.获取AnchorPosRelatedInfo(请求地图的坐标点相关信息)
CAAnchorPosRelatedInfo DynamicMap::GetCAAnchorPosRelatedInfo(GetPositionInfo &getPosInfo){
    //如果是无效点
    if(!getPosInfo.myPosInfo.road)
        return mAnchorPosRelatedInfo;
    mAnchorPosRelatedInfo.numOfLanes = getPosInfo.GetLaneNum();
    mAnchorPosRelatedInfo.currentLaneNum = getPosInfo.myPosInfo.laneId;
    mAnchorPosRelatedInfo.currentLinkId = 0;
    //默认同向
    mAnchorPosRelatedInfo.toLinkEnd = getPosInfo.myPosInfo.section->mSEnd - getPosInfo.myPosInfo.trackS;
    PrintAnchorPosRelatedInfo(mAnchorPosRelatedInfo);
    return mAnchorPosRelatedInfo;
}

static void Print(vector<CASpecialArea>* specialAreas){
    cout << "SpecialArea information: " << endl;
    for(int i = 0; i < specialAreas->size(); i++){
        cout << "type = " << specialAreas->at(i).type << ", "
             << "specialAreaLength = " << specialAreas->at(i).specialAreaLength << ", "
             << "relatedType = " << specialAreas->at(i).relatedType << ", "
             << "toSpecialArea = " <<specialAreas->at(i).toSpecialArea << endl;
    }

}

std::vector<CASpecialArea> DynamicMap::GetCASpecialArea(GetPositionInfo &getPosInfo, GuidePaths &guidePaths){
    if (!getPosInfo.myPosInfo.road) {
        return reinterpret_cast<vector<CASpecialArea> &>(mSpecialArea); //输入点没有激活，未知
    }
    //默认同向
    double tempLength = getPosInfo.myPosInfo.roadlength - getPosInfo.myPosInfo.trackS;
    //如果反向，长安不存在这种情况
    if (getPosInfo.DrivingDirRelToRoad(getPosInfo.myPosInfo.road->mRHT) > 1)
    {
        tempLength = getPosInfo.myPosInfo.trackS;
    }
    Object* tempObject = getPosInfo.myPosInfo.road->getFirstObject();
    //1、第一个road特殊处理
    //遍历第一个road，特殊区位于起始点之后才生效
    CASpecialArea specialArea;
    for(int i = 0; i < getPosInfo.myKeyWords.size(); i++)
    {
        cout << "Traversal :" << getPosInfo.myKeyWords.at(i) << endl;
        specialArea = getPosInfo.TraversalSpecialAreaInCurrentRoad(getPosInfo.myKeyWords.at(i), specialArea);
        //当前road存在特殊区但是距离超过2000直接返回
        if (specialArea.toSpecialArea > getPosInfo.myRange){
            mSpecialArea->push_back(specialArea);
            return reinterpret_cast<vector<CASpecialArea> &>(mSpecialArea);
        }
        mSpecialArea->push_back(specialArea);
    }
    cout << "First Road: " << endl;
    Print(mSpecialArea);
    //三种特殊区域都存在了，直接返回
    if(((0 != mSpecialArea->at(0).relatedType) && (0 != mSpecialArea->at(1).relatedType) &&
    (0 != mSpecialArea->at(2).relatedType)) || tempLength >= getPosInfo.myRange)
        return reinterpret_cast<vector<CASpecialArea> &>(mSpecialArea);
    //2、当前road不存在某一个有效特殊区域，从下一个road开始遍历
    //vector<int>* roadDirs = getPosInfo.GetRoadDirs(rangeRamp);

    RoadHeader* nextRoad;
    //int iter = 1;
    for(int j = 0; j < mSpecialArea->size(); j++){
        //某一种不存在，则从下一个road开始遍历
        if (0 == mSpecialArea->at(j).relatedType)
        {
            //2.1从索引1处开始遍历
            cout << "Now traversal keyWord : " << j <<getPosInfo.myKeyWords[j] << endl;
            for(int i = 1; i < guidePaths.size(); i++)
            {
                nextRoad = guidePaths.at(i).second;
                specialArea = getPosInfo.TraversalSpecialAreaInRoad(nextRoad, tempObject,getPosInfo.myKeyWords[j], guidePaths.at(i).first, specialArea);
                tempLength += nextRoad->mLength;
                if(0 != specialArea.relatedType){
                    cout << "SpecialArea existed in roadID:" << nextRoad->getId() << endl;
                    continue;
                }
                //iter ++;
            }
            cout << "Now traversal keyWord End: " << j <<getPosInfo.myKeyWords[j] << endl;
            //2.2 某个特殊区域存在，返回对应类型，其枚举值和循环匹配上
            if (0 != specialArea.relatedType) {
                //存在特殊区域但是距离超过2000米
                if((tempLength - nextRoad->mLength + specialArea.specialAreaLength) > getPosInfo.myRange)
                {
                    mSpecialArea->at(j).relatedType = static_cast<CaCurrentRelatedSpecialArea>(kSATUnkown);
                    mSpecialArea->at(j).specialAreaLength = 1;
                    mSpecialArea->at(j).toSpecialArea = -1;
                }else{
                    mSpecialArea->at(j).relatedType = specialArea.relatedType;
                    mSpecialArea->at(j).specialAreaLength = specialArea.specialAreaLength;
                    mSpecialArea->at(j).toSpecialArea = tempLength - nextRoad->mLength + specialArea.specialAreaLength;
                }
            }
        }
    }
    Print(mSpecialArea);
    return reinterpret_cast<vector<CASpecialArea> &>(mSpecialArea);
}

static void Print(CARampInfoOut& rampInfoOut){
    cout << "CARampInfoOut information: " << endl;
    cout << "rampInfoStatus = " << rampInfoOut.rampInfoStatus << ", "
         << "enterRampInfo = " << rampInfoOut.enterRampInfo << ", "
         << "toDis = " << rampInfoOut.toDis << ", "
         << "splitStartDis = " << rampInfoOut.splitStartDis << ", "
         << "splitEndDis = " << rampInfoOut.splitEndDis << ", "
         << "mergeStartDis = " << rampInfoOut.mergeStartDis << ", "
         << "mergeEndDis = " << rampInfoOut.mergeEndDis << ", "
         << "splitRampLaneId = " << rampInfoOut.splitRampLaneId << ", "
         << "mergeLaneId = " << rampInfoOut.mergeLaneId << endl;
}

//switch提出
CARampInfoOut DynamicMap::GetRampCase(GetPositionInfo &getPosInfo, GuidePaths &guidePathsRange,GuidePaths &guidePathsRangeAdded,RoadHeader *currentRoad,RoadHeader *nextRoad,double currentLength,int id,int rampStatus){
    LaneSection *nextRoadLaneSection = nextRoad->getFirstLaneSection();
    RoadHeader *nextNextRoad;
    LaneSection *nextNextRoadLaneSection;
    int rotationTimes;
    if(1 == guidePathsRange.size()) rotationTimes = 1;
    else rotationTimes = guidePathsRange.size() - 1;
    switch (getPosInfo.myPosInfo.laneType) {
        case ODR_LANE_TYPE_ON_RAMP:
            rampStatus = getPosInfo.TraversalLane(nextRoadLaneSection->getLaneFromId(0)).first;   //默认road方向和行驶方向一致
//                    id = getPosInfo.TraversalLane(nextRoadLaneSection->getLaneFromId(0)).second;
            if (ODR_LANE_TYPE_ENTRY == rampStatus) {
                mRampInfoOut.mergeStartDis = currentLength;
                mRampInfoOut.mergeEndDis =mRampInfoOut.mergeStartDis + (nextRoadLaneSection->mSEnd - nextRoadLaneSection->mS);
                mRampInfoOut.mergeLaneId = pow(2, 17 - abs(id));
            } else {
                nextNextRoad = guidePathsRangeAdded.at(2).second;
                nextNextRoadLaneSection = nextNextRoad->getFirstLaneSection(); //默认是entry
                mRampInfoOut.mergeStartDis = currentLength + nextRoad->mLength;
                mRampInfoOut.mergeEndDis = mRampInfoOut.mergeStartDis +(nextNextRoadLaneSection->mSEnd - nextNextRoadLaneSection->mS);
                mRampInfoOut.mergeLaneId = pow(2, 17 - abs(id));
            }
            break;
        case ODR_LANE_TYPE_OFF_RAMP:
            for (int i = 0; i < rotationTimes; i++) {
                //currentRoad = nextRoad;
                currentRoad = guidePathsRangeAdded.at(i+1).second;
                currentLength += currentRoad->mLength;
                //判断当前road是否属于junction
                if (currentRoad->mJuncNo > 0)      //offramp-junction
                {
                    mRampInfoOut.splitStartDis = 0;
                    mRampInfoOut.splitEndDis = currentLength - currentRoad->mLength;
                    mRampInfoOut.splitRampLaneId = pow(2, 17 - abs(id));
                    break;
                }
                //offramp-onramp
                rampStatus = getPosInfo.TraversalLane((currentRoad->getFirstLaneSection())->getLaneFromId(0)).first;
                if (ODR_LANE_TYPE_ON_RAMP == rampStatus) // 等同onramp
                {
                    nextRoad = guidePathsRangeAdded.at(i + 2).second;
                    nextRoadLaneSection = nextRoad->getFirstLaneSection();
                    rampStatus = getPosInfo.TraversalLane(nextRoadLaneSection->getLaneFromId(0)).first;
                    if (ODR_LANE_TYPE_ENTRY == rampStatus) {
                        mRampInfoOut.mergeStartDis = currentLength;
                        mRampInfoOut.mergeEndDis = mRampInfoOut.mergeStartDis +(nextRoadLaneSection->mSEnd - nextRoadLaneSection->mS);
                        mRampInfoOut.mergeLaneId = pow(2, 17 - abs(id));
                    } else {
                        nextNextRoad = guidePathsRangeAdded.at(i + 3).second;
                        nextNextRoadLaneSection = nextNextRoad->getFirstLaneSection(); //默认是entry
                        mRampInfoOut.mergeStartDis = currentLength + nextRoad->mLength;
                        mRampInfoOut.mergeEndDis = mRampInfoOut.mergeStartDis +
                                                   (nextNextRoadLaneSection->mSEnd -
                                                    nextNextRoadLaneSection->mS);
                        mRampInfoOut.mergeLaneId = pow(2, 17 - abs(id));
                    }
                    break;
                }
            }
            break;
        case ODR_LANE_TYPE_CONNECTING_RAMP:
//            currentLength += nextRoad->mLength;
            for (int i = 0; i < rotationTimes; i++) {
//                currentRoad = nextRoad;
                currentRoad = guidePathsRangeAdded.at(i+1).second;
                currentLength += currentRoad->mLength;
                //判断当前road是否属于junction
                if (currentRoad->mJuncNo > 0) {
                    //判断junction属于哪种（34or长安）
                    nextRoad = guidePathsRangeAdded.at(i + 2).second;
                    nextRoadLaneSection = nextRoad->getFirstLaneSection();
                    rampStatus = getPosInfo.TraversalLane(nextRoadLaneSection->getLaneFromId(0)).first;
//                            id = getPosInfo.TraversalLane(nextRoadLaneSection->getLaneFromId(0)).second;
                    if (ODR_LANE_TYPE_ENTRY == rampStatus)    //属于3.4，计算汇流
                    {
                        mRampInfoOut.mergeStartDis = currentLength;
                        mRampInfoOut.mergeEndDis = mRampInfoOut.mergeStartDis +
                                                   (nextRoadLaneSection->mSEnd - nextRoadLaneSection->mS);
                        mRampInfoOut.mergeLaneId = pow(2, 17 - abs(id));
                    } else  //计算分流
                    {
                        mRampInfoOut.splitStartDis = 0;
                        mRampInfoOut.splitEndDis = currentLength - currentRoad->mLength;
                        mRampInfoOut.splitRampLaneId = pow(2, 17 - abs(id));
                    }
                    break;
                }
            }
            break;
        case ODR_LANE_TYPE_ENTRY:
            mRampInfoOut.mergeStartDis = 0;
            mRampInfoOut.mergeEndDis = getPosInfo.myPosInfo.section->mSEnd - getPosInfo.myPosInfo.trackS;
//                rampInfoOut.mergeEndDis = (getPosInfo.myManager.getLaneSection()->mSEnd-getPosInfo.myPosInfo.trackS);
            mRampInfoOut.mergeLaneId = pow(2, 17 - abs(id));
            break;
        case ODR_LANE_TYPE_EXIT:
            mRampInfoOut.splitStartDis = 0;
            mRampInfoOut.splitEndDis = getPosInfo.myPosInfo.section->mSEnd - getPosInfo.myPosInfo.trackS;
//                rampInfoOut.splitEndDis = (getPosInfo.myManager.getLaneSection()->mSEnd-getPosInfo.myPosInfo.trackS);
            mRampInfoOut.splitRampLaneId = pow(2, 17 - abs(id));
            break;
    }

}

CARampInfoOut DynamicMap::GetCARampInfoOut(GetPositionInfo &getPosInfo, GuidePaths &guidePathsRange,GuidePaths &guidePathsRangeAdded)
{
    if (!getPosInfo.myPosInfo.road) {
        mRampInfoOut.rampInfoStatus = 2; //输入点没有激活，未知
        return mRampInfoOut;
    }
    int rampStatus = 0;
    int id;
    int rotationTimes;
    if(1 == guidePathsRange.size()) rotationTimes = 1;
    else rotationTimes = guidePathsRange.size() - 1;
    RoadHeader *nextRoad;
//    if(guidePaths.size()>=3) {
    nextRoad = guidePathsRangeAdded.at(1).second;
    cout << "nextRoad:" << nextRoad->mId << endl;
    double currentLength = getPosInfo.myPosInfo.roadlength - getPosInfo.myPosInfo.trackS;
    RoadHeader *currentRoad;
    //1、当前lane就在ramp中
    if (ODR_LANE_TYPE_ON_RAMP == getPosInfo.myPosInfo.laneType || ODR_LANE_TYPE_OFF_RAMP == getPosInfo.myPosInfo.laneType
        || ODR_LANE_TYPE_CONNECTING_RAMP == getPosInfo.myPosInfo.laneType || ODR_LANE_TYPE_ENTRY == getPosInfo.myPosInfo.laneType
        || ODR_LANE_TYPE_EXIT == getPosInfo.myPosInfo.laneType) {
        cout << "Ramp existed in given lane. " << endl;
        //都存在匝道，且在匝道中，当前位置到匝道距离为0
        rampStatus = 1;
        mRampInfoOut.rampInfoStatus = rampStatus;
        mRampInfoOut.enterRampInfo = 2;
        mRampInfoOut.toDis = 0;
        id = getPosInfo.myPosInfo.laneId;
        //汇流起点/终点距离
        GetRampCase(getPosInfo, guidePathsRange,guidePathsRangeAdded,currentRoad,nextRoad, currentLength,id, rampStatus);

        return mRampInfoOut;
    }
    //2、当前位置不在ramp中，遍历当前road的后继是否存在entry/exit
    //2.1、定位当前road
    currentLength = getPosInfo.myPosInfo.section->mSEnd-getPosInfo.myPosInfo.trackS;
    LaneSection *tempLaneSection =getPosInfo.myPosInfo.section ;
    LaneSection *currentLaneSection;
    pair<int,int> rampStatusId;
    tuple<int,int,double,LaneSection*> rampStatusIdCurLength ;
    //遍历当前road所在laneSection 后是否存在entry/exit
    while(tempLaneSection->getRight()!=NULL){
        tempLaneSection = (LaneSection*)(tempLaneSection->getRight());
        currentLaneSection = tempLaneSection;
        rampStatusId =getPosInfo.TraversalLane(currentLaneSection->getLaneFromId(0));
        rampStatus = rampStatusId.first;
        id=rampStatusId.second;
        currentLength += currentLaneSection->mSEnd-currentLaneSection->mS;
        if(0!=rampStatus || currentLength>getPosInfo.myRange) break;
    }
    //当前不存在entry/exit
    if(0==rampStatus){
        for (int i = 0; i < rotationTimes; i++) {
            //cout << i << "st circle..." << endl;
//            currentRoad = nextRoad;
            currentRoad = guidePathsRangeAdded.at(i+1).second;
            rampStatusIdCurLength = getPosInfo.TraversalRoad(currentRoad,currentLength,rampStatusId,rampStatusIdCurLength);  //遍历road中所有laneSection
//                currentLength += currentRoad->mLength;
            rampStatus = get<0>(rampStatusIdCurLength);
            id=get<1>(rampStatusIdCurLength);
            currentLength = get<2>(rampStatusIdCurLength);
//        cout<<"rampstatus"<<rampStatus<<endl;
            if (0 != rampStatus ||currentLength>getPosInfo.myRange) {
                cout << "Ramp existed in roadId = " << currentRoad->getId() << endl;
                break;
            }
            cout << "Traversal roadId = " << currentRoad->getId() << " ends" << endl;
            //判断匝道结束，在此处结束。
        }
    }
    // 计算分流汇流距离
//    cout << "Finally rampStatus: " << rampStatus << endl;
//    cout<<"is exit"<<currentRoad->mLength<<endl;
    //2.2.1、给定范围内不存在ramp，直接返回
    switch (rampStatus) {
        case 0:
            return mRampInfoOut;
        case ODR_LANE_TYPE_ENTRY:
            mRampInfoOut.rampInfoStatus = 1;
            mRampInfoOut.enterRampInfo = 0;  //存疑
            mRampInfoOut.toDis = -1;       //存疑
//                mRampInfoOut.mergeStartDis = currentLength - currentRoad->mLength;
            mRampInfoOut.mergeStartDis = currentLength -(get<3>(rampStatusIdCurLength)->mSEnd-get<3>(rampStatusIdCurLength)->mS);
//                mRampInfoOut.mergeEndDis = mRampInfoOut.mergeStartDis + (currentRoad->getFirstLaneSection()->mSEnd -
//                                                                         currentRoad->getFirstLaneSection()->mS);
            mRampInfoOut.mergeEndDis = currentLength;
            mRampInfoOut.mergeLaneId = pow(2, 17 - abs(id));
            return mRampInfoOut;
        case ODR_LANE_TYPE_EXIT:
            mRampInfoOut.rampInfoStatus = 1;
            mRampInfoOut.enterRampInfo = 1;//存疑
//                mRampInfoOut.toDis = currentLength - currentRoad->mLength; //存疑
            mRampInfoOut.toDis = currentLength -(get<3>(rampStatusIdCurLength)->mSEnd-get<3>(rampStatusIdCurLength)->mS); //存疑
            mRampInfoOut.splitStartDis =currentLength -(get<3>(rampStatusIdCurLength)->mSEnd-get<3>(rampStatusIdCurLength)->mS);
            mRampInfoOut.splitEndDis = currentLength;  //默认整条road都是exit
            mRampInfoOut.splitRampLaneId = pow(2, 17 - abs(id));
            return mRampInfoOut;
    }
//}
    return mRampInfoOut;
}



static void Print(const vector<CARoutingPath> *routingPaths){
    for(int i = 0; i<routingPaths->size(); i++)
    {
        cout << "routingPath.linkId = " << routingPaths->at(i).linkId << endl;
        cout << "routingPath.availableLanes : ";
        for(int j = 0; j < routingPaths->at(i).availableLanes->size(); j++)
        {
            cout << routingPaths->at(i).availableLanes->at(j) <<" ; ";
        }
        cout << endl;
        cout << "routingPath.availableLaneNum = " << routingPaths->at(i).availableLaneNum << endl;
    }
}
vector<CARoutingPath> *DynamicMap::GetCARoutingPath(GetPositionInfo &getPosInfo, GuidePaths &guidePaths,
                                                    vector<CARoutingPath> *routingPaths) {
    cout << " GetCARoutingPath begins..." << endl;
    vector<LaneSection *> laneSections;
    //cout << "guidePaths.size: " << guidePaths.size() << endl;
    laneSections = getPosInfo.GetLaneSectionsByEndPoint(guidePaths, laneSections);
    //cout << "laneSections.size: " << laneSections.size() <<endl;
    //cout << "GetLaneSectionsByEndPoint over..." << endl;
    CARoutingPath routingPath;
    pair<int, vector<int>*> laneIdAndNum;
    for(int i = 0; i < laneSections.size(); i++)
    {
       // cout << "rotateNum begin: " << i << endl;
        laneIdAndNum = getPosInfo.GetLaneIdNumInLaneSection(laneSections.at(i));
        routingPath.linkId = i;
        routingPath.availableLanes = laneIdAndNum.second;
        routingPath.availableLaneNum = laneIdAndNum.first;
        routingPaths->push_back(routingPath);
       // cout << "rotateNum end: " << i << endl;
    }
    //Print(routingPaths);
    cout << " GetCARoutingPath over..." << endl;
    return routingPaths;
}

CAPathPlanningOut DynamicMap::GetCAPathPlanningOut(GetPositionInfo &getPosInfo, GuidePaths &guidePathsEndPoint, GuidePaths &guidePathsEndPointAdded)
{
    if (!getPosInfo.myPosInfo.road) return mPathPlanningOut; //输入点没有激活
    vector<CARoutingPath> *routingPath = new vector<CARoutingPath>;
    routingPath = GetCARoutingPath(getPosInfo, guidePathsEndPoint, routingPath);
    mPathPlanningOut.routingPath = routingPath;
    int rotationTimes = 0;
    if(1 == guidePathsEndPoint.size()) rotationTimes = 1;
    else rotationTimes = guidePathsEndPoint.size() - 1;
    //mPathPlanningOut.routingPath = GetCARoutingPath(getPosInfo, guidePaths,mPathPlanningOut.routingPath);

    //仿照rampInfoOut lll
    int rampStatus = 0;
    RoadHeader *nextRoad = guidePathsEndPointAdded.at(1).second;
    double currentLength = getPosInfo.myPosInfo.roadlength - getPosInfo.myPosInfo.trackS;
    RoadHeader* currentRoad;
    //1、当前lane就在ramp中
    if (ODR_LANE_TYPE_ON_RAMP == getPosInfo.myPosInfo.laneType || ODR_LANE_TYPE_OFF_RAMP == getPosInfo.myPosInfo.laneType
        || ODR_LANE_TYPE_CONNECTING_RAMP == getPosInfo.myPosInfo.laneType|| ODR_LANE_TYPE_ENTRY == getPosInfo.myPosInfo.laneType
        || ODR_LANE_TYPE_EXIT == getPosInfo.myPosInfo.laneType)
    {
        cout << "Ramp existed in given lane. " << endl;
        //都存在匝道,除exit外与匝道关系均为离开
        mPathPlanningOut.pathPlanningType = kOutSpecialArea;
        LaneSection *nextRoadLaneSection = nextRoad->getFirstLaneSection();
        RoadHeader *nextNextRoad;
        LaneSection *nextNextRoadLaneSection;
        //汇流起点/终点距离
        switch (getPosInfo.myPosInfo.laneType)
        {
            case ODR_LANE_TYPE_ON_RAMP:
                getPosInfo.JudgeRampAbout(nextRoadLaneSection->getLaneFromId(0), rampStatus);
                if(ODR_LANE_TYPE_ENTRY == rampStatus)
                {
                    mPathPlanningOut.pathPlanningToRampStartDis = currentLength;
                    mPathPlanningOut.pathPlanningToRampEndDis = mPathPlanningOut.pathPlanningToRampStartDis + (nextRoadLaneSection->mSEnd-nextRoadLaneSection->mS);
                } else{
                    nextNextRoad = guidePathsEndPointAdded.at(2).second;
                    nextNextRoadLaneSection = nextNextRoad->getFirstLaneSection(); //默认是entry
                    mPathPlanningOut.pathPlanningToRampStartDis = currentLength + nextRoad->mLength;
                    mPathPlanningOut.pathPlanningToRampEndDis = mPathPlanningOut.pathPlanningToRampStartDis+ (nextNextRoadLaneSection->mSEnd-nextNextRoadLaneSection->mS);
                }
                break;
            case ODR_LANE_TYPE_OFF_RAMP:
                for(int i = 0; i < rotationTimes; i++) {
                    currentRoad = guidePathsEndPointAdded.at(i+1).second;
                    currentLength += currentRoad->mLength;
//                    //判断当前road是否属于junction
                    if (currentRoad->mJuncNo > 0)      //offramp-junction
                    {
                        mPathPlanningOut.pathPlanningToRampStartDis = 0;
                        mPathPlanningOut.pathPlanningToRampEndDis = currentLength-currentRoad->mLength;
                        break;
                    }
                    //offramp-onramp
                    getPosInfo.JudgeRampAbout((currentRoad->getFirstLaneSection())->getLaneFromId(0), rampStatus);
                    if (ODR_LANE_TYPE_ON_RAMP == rampStatus) // 等同onramp
                    {
                        // 方向>0, 找后继；否则，找前驱(参考ordMGR中的Dir逻辑)
                        nextRoad = guidePathsEndPointAdded.at(i+2).second;
                        nextRoadLaneSection = nextRoad->getFirstLaneSection();
                        getPosInfo.JudgeRampAbout(nextRoadLaneSection->getLaneFromId(0), rampStatus);
                        if(ODR_LANE_TYPE_ENTRY == rampStatus)
                        {
                            mPathPlanningOut.pathPlanningToRampStartDis = currentLength;
                            mPathPlanningOut.pathPlanningToRampEndDis= mPathPlanningOut.pathPlanningToRampStartDis + (nextRoadLaneSection->mSEnd-nextRoadLaneSection->mS);
                        } else{
                            nextNextRoad = guidePathsEndPointAdded.at(i+3).second;
                            nextNextRoadLaneSection = nextNextRoad->getFirstLaneSection(); //默认是entry
                            mPathPlanningOut.pathPlanningToRampStartDis = currentLength + nextRoad->mLength;
                            mPathPlanningOut.pathPlanningToRampEndDis = mPathPlanningOut.pathPlanningToRampStartDis + (nextNextRoadLaneSection->mSEnd-nextNextRoadLaneSection->mS);

                        }
                        break;
                    }
                }
                break;
            case ODR_LANE_TYPE_CONNECTING_RAMP:
                for(int i = 0; i < rotationTimes; i++)
                {
                    currentRoad = guidePathsEndPointAdded.at(i+1).second;
                    currentLength += currentRoad->mLength;
                    //判断当前road是否属于junction
                    if (currentRoad->mJuncNo > 0)
                    {
                        //判断junction属于哪种（34or长安）
                        nextRoad = guidePathsEndPointAdded.at(i+2).second;
                        nextRoadLaneSection = nextRoad->getFirstLaneSection();
                        getPosInfo.JudgeRampAbout(nextRoadLaneSection->getLaneFromId(0), rampStatus);
                        if(ODR_LANE_TYPE_ENTRY == rampStatus)    //属于3.4，计算汇流
                        {
                            mPathPlanningOut.pathPlanningToRampStartDis = currentLength;
                            mPathPlanningOut.pathPlanningToRampEndDis = mPathPlanningOut.pathPlanningToRampStartDis + (nextRoadLaneSection->mSEnd-nextRoadLaneSection->mS);

                        }else  //计算分流
                        {
                            mPathPlanningOut.pathPlanningToRampStartDis = 0;
                            mPathPlanningOut.pathPlanningToRampEndDis = currentLength-currentRoad->mLength;
                        }
                        break;
                    }
                }
                break;
            case ODR_LANE_TYPE_ENTRY:
                mPathPlanningOut.pathPlanningToRampStartDis = 0;
                mPathPlanningOut.pathPlanningToRampEndDis = getPosInfo.myPosInfo.section->mSEnd - getPosInfo.myPosInfo.trackS;
                break;
            case ODR_LANE_TYPE_EXIT:
                mPathPlanningOut.pathPlanningType = kInSpecialArea;
                mPathPlanningOut.pathPlanningToRampStartDis = 0;
                mPathPlanningOut.pathPlanningToRampEndDis = getPosInfo.myPosInfo.section->mSEnd - getPosInfo.myPosInfo.trackS;
                break;
        }
        return mPathPlanningOut;

    }
    //2、当前位置不在ramp中，遍历当前road的后继是否存在entry/exit
    //2.1、定位当前road
    //记录当前位置到entry（exit）出现的road终点的距离
    currentLength = getPosInfo.myPosInfo.section->mSEnd-getPosInfo.myPosInfo.trackS;
    LaneSection *tempLaneSection =getPosInfo.myPosInfo.section ;
    LaneSection *currentLaneSection;
    pair<int,int> rampStatusId;
    tuple<int,int,double,LaneSection*> rampStatusIdCurLength ;
    //遍历当前road所在laneSection 后是否存在entry/exit
    while(tempLaneSection->getRight()!=NULL){
        tempLaneSection = (LaneSection*)(tempLaneSection->getRight());
        currentLaneSection = tempLaneSection;
        getPosInfo.JudgeRampAbout(currentLaneSection->getLaneFromId(0), rampStatus);
        if(ODR_LANE_TYPE_ENTRY== rampStatus ||ODR_LANE_TYPE_EXIT== rampStatus){
            cout << "Ramp existed in roadId = " << currentRoad->getId() <<endl;
            break;
        }
    }
    if(0==rampStatus){
        for (int i = 0; i < rotationTimes; i++) {
            currentRoad = guidePathsEndPointAdded.at(i+1).second;
            rampStatusIdCurLength = getPosInfo.TraversalRoad(currentRoad,currentLength,rampStatusId,rampStatusIdCurLength);  //遍历road中所有laneSection
            rampStatus = get<0>(rampStatusIdCurLength);
            currentLength = get<2>(rampStatusIdCurLength);
            if(ODR_LANE_TYPE_ENTRY== rampStatus ||ODR_LANE_TYPE_EXIT== rampStatus){
                cout << "Ramp existed in roadId = " << currentRoad->getId() <<endl;
                break;
            }
            cout << "Traversal roadId = " << currentRoad->getId() << " ends" << endl;
            //判断匝道结束，在此处结束。
        }
    }
    // 分流/汇流起点和终点距离详细代码
    //2.2.1、给定范围内不存在ramp，直接返回默认值
    if(rampStatus){
        mPathPlanningOut.pathPlanningType = kInSpecialArea;
        mPathPlanningOut.pathPlanningToRampStartDis = currentLength -(get<3>(rampStatusIdCurLength)->mSEnd-get<3>(rampStatusIdCurLength)->mS);
        mPathPlanningOut.pathPlanningToRampEndDis = currentLength;  //默认整条road都是exit
        return mPathPlanningOut;
    }
    return mPathPlanningOut;
}

static void Print(const CADynamicAddition &dynamicAddition){
    cout << "dynamicAddition.toNumChangeOfLaneDis = " << dynamicAddition.toNumChangeOfLaneDis << endl;
    cout << "dynamicAddition.toNumChangeOfLanesType = " << dynamicAddition.toNumChangeOfLanesType << endl;
    cout << "dynamicAddition.numChangeOfLaneId = " << dynamicAddition.numChangeOfLaneId << endl;
    cout << "dynamicAddition.longitude =" << dynamicAddition.longitude << endl;
    cout << "dynamicAddition.latitude =" << dynamicAddition.latitude << endl;
}

void DynamicMap::GetLaneChangeInfo(GetPositionInfo &getPosInfo, GuidePaths &guidePaths){

    vector<CARoutingPath> *routingPaths = new vector<CARoutingPath>;
    GetCARoutingPath(getPosInfo,guidePaths, routingPaths);

    int originalLaneNum = routingPaths->at(0).availableLaneNum;
    int linkID;
    vector<int>::iterator  it,st;
    double length;
    vector<int>::iterator iterator;
    //默认没有变化,有变化才更改
    for(int i = 1; i < routingPaths->size()-2; i++)
    {
        //只查找第一个变化的距离
        if(originalLaneNum != routingPaths->at(i).availableLaneNum)
        {
            //cout << "LaneChangeOfLinkID = " << i << endl;
            linkID = routingPaths->at(i).linkId;
            vector<LaneSection *> laneSections;
            getPosInfo.GetLaneSectionsByEndPoint(guidePaths, laneSections);
            //默认同向
            length = abs(getPosInfo.myPosInfo.section->mSEnd - getPosInfo.myPosInfo.sectionS);
            //cout << length << ";";
            for(int i = 1; i < linkID; i++)
            {

                double sectionLength = abs(laneSections.at(i)->mSEnd - laneSections.at(i)->mS);
                length += sectionLength;
                //cout << sectionLength << ";";
            }

            int rampStatusFormer = getPosInfo.JudgeRampAbout(laneSections.at(linkID-1)->getLaneFromId(0));
            int rampStatusLater = getPosInfo.JudgeRampAbout(laneSections.at(linkID)->getLaneFromId(0));
            //cout << "rampStatusFormer: rampStatusLater" << rampStatusFormer << "; " << rampStatusLater << endl;

            //增加
            //查找不同的laneID
            if (originalLaneNum < routingPaths->at(i).availableLaneNum)
            {
                //变化的laneID
                //1、迭代数据对比查找
                /*for(int j = 0; j < routingPaths->at(i).availableLanes->size(); j++)
                {
                    iterator = find(routingPaths->at(i).availableLanes->begin(), routingPaths->at(i).availableLanes->end(),
                                    (routingPaths->at(i-1).availableLanes)->at(j));
                    if (iterator == routingPaths->at(i).availableLanes->end())
                        mDynamicAddition.numChangeOfLaneId = (routingPaths->at(i-1).availableLanes)->at(j) ;
                    break;
                }*/
                //2、根据opendrive数据格式进行获取，直接取多的数组的最末尾的laneID
                mDynamicAddition.numChangeOfLaneId = routingPaths->at(i).availableLanes->at(routingPaths->at(i).availableLaneNum - 1);
                mDynamicAddition.toNumChangeOfLanesType = 1;
                //若存在匝道相关lane
                if(rampStatusFormer || rampStatusLater)
                    mDynamicAddition.toNumChangeOfLanesType = 3;
            }
                //减少
            else
            {
                //变化的laneID
                /*for(int j = 0; j < routingPaths->at(i-1).availableLanes->size(); j++)
                {
                    iterator = find(routingPaths->at(i-1).availableLanes->begin(), routingPaths->at(i-1).availableLanes->end(),
                                    (routingPaths->at(i).availableLanes)->at(j));
                    if (iterator == routingPaths->at(i-1).availableLanes->end())
                        mDynamicAddition.numChangeOfLaneId = (routingPaths->at(i).availableLanes)->at(j) ;
                    break;
                }*/
                mDynamicAddition.numChangeOfLaneId = routingPaths->at(i - 1).availableLanes->at(routingPaths->at(i-1).availableLaneNum-1);
                mDynamicAddition.toNumChangeOfLanesType = 2;
                //若存在匝道相关lane
                if(rampStatusFormer || rampStatusLater)
                    mDynamicAddition.toNumChangeOfLanesType = 4;
            }
            mDynamicAddition.toNumChangeOfLaneDis = length;
            break;
        }
    }
    //Print(mDynamicAddition);

}

CADynamicAddition DynamicMap::GetCADynamicAddition(GetPositionInfo &getPosInfo, GuidePaths &guidePaths)
{
    cout << " GetCADynamicAddition begins..." << endl;
     mDynamicAddition.laneCurvature100 = getPosInfo.GetForwardCurvature(100);
    mDynamicAddition.laneCurvature200 = getPosInfo.GetForwardCurvature(200);
    mDynamicAddition.laneCurvature300 = getPosInfo.GetForwardCurvature(300);

    GetLaneChangeInfo(getPosInfo,guidePaths);

    PJ_COORD coord = {getPosInfo.myPoint.x, getPosInfo.myPoint.y, getPosInfo.myPoint.z};
    if(!getPosInfo.myPosInfo.geoReference.empty())
    {
        char *targetProj = const_cast<char *>(getPosInfo.myPosInfo.geoReference.c_str());
        getPosInfo.GetGeographicCoordinate(coord, targetProj);
    }

    mDynamicAddition.longitude = coord.xy.x;
    mDynamicAddition.latitude = coord.xy.y;
    cout << " GetCADynamicAddition over..." << endl;
    return mDynamicAddition;
}

CADynamicHDMapErc DynamicMap::GetCADynamicHDMapErc(GetPositionInfo &getPosInfo, GuidePaths &guidePathsRange, GuidePaths &guidePathsRangeAdded,
                                                   GuidePaths &guidePathsEndPoint, GuidePaths &guidePathsEndPointAdd) {

    cout << " GetCAAnchorPosRelatedInfo begins ... " << endl;
    GetCAAnchorPosRelatedInfo(getPosInfo);
    cout << " GetCAAnchorPosRelatedInfo over ... " << endl;
    cout << " GetCASpecialArea begins ... " << endl;
    GetCASpecialArea(getPosInfo, guidePathsRange);
    cout << " GetCASpecialArea over ... " << endl;
    cout << " GetCARampInfoOut begins ... " << endl;
    GetCARampInfoOut(getPosInfo,guidePathsRange,guidePathsRangeAdded);
    cout << " GetCARampInfoOut over ... " << endl;
    GetCAPathPlanningOut(getPosInfo,guidePathsEndPoint, guidePathsEndPointAdd);
    GetCADynamicAddition(getPosInfo, guidePathsEndPoint);
    mDynamicHdMapErc.anchorPosRelatedInfo = mAnchorPosRelatedInfo;
    mDynamicHdMapErc.specialArea = mSpecialArea;
    mDynamicHdMapErc.rampInfo = mRampInfoOut;
    mDynamicHdMapErc.pathPlanningOut = mPathPlanningOut;
    mDynamicHdMapErc.dynamicAddition = mDynamicAddition;

    return mDynamicHdMapErc;
}








