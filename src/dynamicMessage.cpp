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
    //默认同向
    if (!getPosInfo.myPosInfo.road) {
        return reinterpret_cast<vector<CASpecialArea> &>(mSpecialArea); //输入点没有激活，未知
    }

    //默认同向
    double tempLength = getPosInfo.myPosInfo.roadlength - getPosInfo.myPosInfo.trackS;
    RoadHeader* nextRoad = guidePaths.at(1).second;
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

    RoadHeader* currentRoad;
    //int iter = 1;
    for(int j = 0; j < mSpecialArea->size(); j++){
        //某一种不存在，则从下一个road开始遍历
        if (0 == mSpecialArea->at(j).relatedType)
        {
            //2.1从索引1处开始遍历
            cout << "Now traversal keyWord : " << j <<getPosInfo.myKeyWords[j] << endl;
            //更新条件那里+1了
            for(int i = 1; i < guidePaths.size() - 1; i++)
            {
                currentRoad = nextRoad;
                specialArea = getPosInfo.TraversalSpecialAreaInRoad(currentRoad, tempObject,getPosInfo.myKeyWords[j], guidePaths.at(i).first, specialArea);
                tempLength += currentRoad->mLength;
                if(0 != specialArea.relatedType){
                    cout << "SpecialArea existed in roadID:" << currentRoad->getId() << endl;
                    continue;
                }
                nextRoad = guidePaths.at(i+1).second;
                //iter ++;
            }
            cout << "Now traversal keyWord End: " << j <<getPosInfo.myKeyWords[j] << endl;
            //2.2 某个特殊区域存在，返回对应类型，其枚举值和循环匹配上
            if (0 != specialArea.relatedType) {
                //存在特殊区域但是距离超过2000米
                if((tempLength - currentRoad->mLength + specialArea.specialAreaLength) > getPosInfo.myRange)
                {
                    mSpecialArea->at(j).relatedType = static_cast<CaCurrentRelatedSpecialArea>(kSATUnkown);
                    mSpecialArea->at(j).specialAreaLength = 1;
                    mSpecialArea->at(j).toSpecialArea = -1;
                }else{
                    mSpecialArea->at(j).relatedType = specialArea.relatedType;
                    mSpecialArea->at(j).specialAreaLength = specialArea.specialAreaLength;
                    mSpecialArea->at(j).toSpecialArea = tempLength - currentRoad->mLength + specialArea.specialAreaLength;
                }

            }
        }
    }
    Print(mSpecialArea);
    return reinterpret_cast<vector<CASpecialArea> &>(mSpecialArea);
}

/*CASpecialArea DynamicMap::GetCASpecialArea(CASpecialArea &specialArea, GetPositionInfo &getPosInfo) {
    //odrMGR目前只有tunnel部分的输出，如果需要输出其它object信息需要其对应的模型和opendrive中的模型计算距离进行匹配
    //OpenDrive::RoadHeader* roadHeader = getPosInfo.myManager.getRoadHeader();
    cout << " here! " << endl;
    if(!getPosInfo.myPosInfo.road) return specialArea;
    OpenDrive::Tunnel* tunnel = getPosInfo.myPosInfo.road->getFirstTunnel();
    OpenDrive::Lane* lane = getPosInfo.myManager.getLane();
    if(!tunnel){
        PrintSpecialArea(specialArea);
        return specialArea;
        cout << "No tunnel here! " << endl;
    } else {
        cout << "Tunnel data : " << endl;
        tunnel->printData();
        specialArea.type = kSATTunnel; //存在隧道，设置特殊区域类型为隧道
        specialArea.specialAreaLength = tunnel->mLength;  //存在隧道，设置隧道长度
        //specialArea.toSpecialArea = abs()
        OpenDrive::TrackCoord trackCoord = getPosInfo.myManager.getTrackPos();
        trackCoord.print();
        //行驶方向和道路方向同向
        if (lane->getType() < 0){
            double tunnelPos = tunnel->mS + tunnel->mLength;
            if(trackCoord.getS() <= tunnel->mS)
            {
                specialArea.relatedType = kCRSAEnter;  //入口前
                specialArea.toSpecialArea = abs(trackCoord.getS() - tunnel->mS);
            }
            else if(trackCoord.getS() >= tunnelPos)
            {
                specialArea.relatedType = kCRSALeave; //出口外
                specialArea.toSpecialArea = abs(trackCoord.getS() - tunnelPos);
            }
            else if(trackCoord.getS() > tunnel->mS && trackCoord.getS() < tunnelPos )
            {
                specialArea.relatedType = kCRSAIn;  //隧道内部
                specialArea.toSpecialArea = abs(trackCoord.getS() - tunnelPos);
            }
            else
            {
                specialArea.relatedType = kCRSAUnkown; //未知
            }

        }
            //行驶方向和道路方向反向
        else
        {
            double tunnelPos = tunnel->mS + tunnel->mLength;
            if(trackCoord.getS() <= tunnel->mS)
            {
                specialArea.relatedType = kCRSAEnter;  //出口外
                specialArea.toSpecialArea = abs(trackCoord.getS() - tunnel->mS);
            }
            else if(trackCoord.getS() >= tunnelPos)
            {
                specialArea.relatedType = kCRSALeave; //入口处
                specialArea.toSpecialArea = abs(trackCoord.getS() - tunnelPos);
            }
            else if(trackCoord.getS() > tunnel->mS && trackCoord.getS() < tunnelPos )
            {
                specialArea.relatedType = kCRSAIn;  //隧道内部
                specialArea.toSpecialArea = abs(trackCoord.getS() - tunnel->mS);
            }
            else
            {
                specialArea.relatedType = kCRSAUnkown; //未知
            }
        }

    }
    PrintSpecialArea(specialArea);
    return specialArea;


}*/



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


CARampInfoOut DynamicMap::GetCARampInfoOut(GetPositionInfo &getPosInfo, GuidePaths &guidePaths)
{
    int rampStatus = 0;
    int id =0;
    if (!getPosInfo.myPosInfo.road) {
        mRampInfoOut.rampInfoStatus = 2; //输入点没有激活，未知
        return mRampInfoOut;
    }
//    vector<int>* roadDirs = getPosInfo.GetRoadDirs(getPosInfo.myRange);
      vector<int>* roadDirs = new vector<int>;
//    getPosInfo.GetRoadDirs(getPosInfo.myRange, roadDirs);

    for(int i = 0; i < guidePaths.size(); i++){
        roadDirs->push_back(guidePaths.at(i).first);
    }

    RoadHeader *nextRoad = guidePaths.at(1).second;
    double currentLength = getPosInfo.myPosInfo.roadlength - getPosInfo.myPosInfo.trackS;
    RoadHeader* currentRoad;
    pair<int, int> rampStatusAndId;
    //1、当前lane就在ramp中
    if (ODR_LANE_TYPE_ON_RAMP == getPosInfo.myPosInfo.laneType || ODR_LANE_TYPE_OFF_RAMP == getPosInfo.myPosInfo.laneType
        || ODR_LANE_TYPE_CONNECTING_RAMP == getPosInfo.myPosInfo.laneType|| ODR_LANE_TYPE_ENTRY == getPosInfo.myPosInfo.laneType
        || ODR_LANE_TYPE_EXIT == getPosInfo.myPosInfo.laneType)
    {
        cout << "Ramp existed in given lane. " << endl;
        //都存在匝道，且在匝道中，当前位置到匝道距离为0
        rampStatus = 1;
        mRampInfoOut.rampInfoStatus = rampStatus;
        mRampInfoOut.enterRampInfo = 2;
        mRampInfoOut.toDis = 0;
        id =  getPosInfo.myPosInfo.laneId;
        LaneSection *nextRoadLaneSection = nextRoad->getFirstLaneSection();
        RoadHeader *nextNextRoad;
        LaneSection *nextNextRoadLaneSection;
        //汇流起点/终点距离
        switch (getPosInfo.myPosInfo.laneType)
        {
            case ODR_LANE_TYPE_ON_RAMP:
//                getPosInfo.TraversalLane(nextRoadLaneSection->getLaneFromId(0), rampStatus);
                rampStatus=getPosInfo.TraversalLane(nextRoadLaneSection->getLaneFromId(0)).first;
                id=getPosInfo.TraversalLane(nextRoadLaneSection->getLaneFromId(0)).second;
                if(ODR_LANE_TYPE_ENTRY == rampStatus)
                {
                    mRampInfoOut.mergeStartDis = currentLength;
                    mRampInfoOut.mergeEndDis = mRampInfoOut.mergeStartDis + (nextRoadLaneSection->mSEnd-nextRoadLaneSection->mS);
                    mRampInfoOut.mergeLaneId = pow(2, 17-abs(id));

                } else{
                    nextNextRoad = guidePaths.at(2).second;
                    nextNextRoadLaneSection = nextNextRoad->getFirstLaneSection(); //默认是entry
                    mRampInfoOut.mergeStartDis = currentLength + nextRoad->mLength;
                    mRampInfoOut.mergeEndDis = mRampInfoOut.mergeStartDis + (nextNextRoadLaneSection->mSEnd-nextNextRoadLaneSection->mS);
                    mRampInfoOut.mergeLaneId = pow(2, 17-abs(id));
                    break;
                }
                break;
            case ODR_LANE_TYPE_OFF_RAMP:
                currentLength += nextRoad->mLength;
                for(int i = 1; i < roadDirs->size(); i++) {
                    currentRoad = guidePaths.at(i).second;
//                    currentLength += nextRoad->mLength;
//                    //判断当前road是否属于junction
                    if (currentRoad->mJuncNo > 0)      //offramp-junction
                    {
                        mRampInfoOut.splitStartDis = 0;
                        mRampInfoOut.splitEndDis = currentLength-nextRoad->mLength;
                        mRampInfoOut.splitRampLaneId = pow(2, 17-abs(id));
                        break;
                    }
                    //offramp-onramp
//                    getPosInfo.TraversalLane((currentRoad->getFirstLaneSection())->getLaneFromId(0), rampStatus);
                    rampStatus = getPosInfo.TraversalLane((currentRoad->getFirstLaneSection())->getLaneFromId(0)).first;
                    id = getPosInfo.TraversalLane((currentRoad->getFirstLaneSection())->getLaneFromId(0)).second;
                    if (ODR_LANE_TYPE_ON_RAMP == rampStatus) // 等同onramp
                    {
                        // 方向>0, 找后继；否则，找前驱(参考ordMGR中的Dir逻辑)
                        /*if (roadDirs->at(i) == 0)
                            nextRoad = currentRoad->getSuccessor();
                        else
                            nextRoad = currentRoad->getPredecessor();*/

                        nextRoad = guidePaths.at(i+1).second;
                        nextRoadLaneSection = nextRoad->getFirstLaneSection();
                        //getPosInfo.TraversalLane(nextRoadLaneSection->getLaneFromId(0), rampStatus);
                        rampStatusAndId =getPosInfo.TraversalLane(nextRoadLaneSection->getLaneFromId(0));

                        if(ODR_LANE_TYPE_ENTRY == rampStatusAndId.first)
                        {
                            mRampInfoOut.mergeStartDis = currentLength;
                            mRampInfoOut.mergeEndDis = mRampInfoOut.mergeStartDis + (nextRoadLaneSection->mSEnd-nextRoadLaneSection->mS);
                            mRampInfoOut.mergeLaneId = pow(2, 17-abs(rampStatusAndId.second));
                        } else{
                            /*if (roadDirs->at(i+1) == 0)
                                nextNextRoad = nextRoad->getSuccessor(1);
                            else
                                nextNextRoad = nextRoad->getPredecessor(1);*/
                            nextNextRoad = guidePaths.at(i+1).second;
                            nextNextRoadLaneSection = nextNextRoad->getFirstLaneSection(); //默认是entry
                            mRampInfoOut.mergeStartDis = currentLength + nextRoad->mLength;
                            mRampInfoOut.mergeEndDis = mRampInfoOut.mergeStartDis + (nextNextRoadLaneSection->mSEnd-nextNextRoadLaneSection->mS);
                            mRampInfoOut.mergeLaneId = pow(2, 17-abs(rampStatusAndId.second));
                            break;
                        }
                    }
                    // 方向>0, 找后继；否则，找前驱(参考ordMGR中的Dir逻辑)
                    /*if (roadDirs->at(i) == 0)
                        nextRoad = currentRoad->getSuccessor();
                    else
                        nextRoad = currentRoad->getPredecessor();*/
                    currentLength += nextRoad->mLength;
                }
                break;
            case ODR_LANE_TYPE_CONNECTING_RAMP:
                currentLength += nextRoad->mLength;
                for(int i = 1; i < roadDirs->size(); i++)
                {
                    currentRoad = guidePaths.at(i).second;
                    //判断当前road是否属于junction
                    if (currentRoad->mJuncNo > 0)
                    {
                        //判断junction属于哪种（34or长安）
                        nextRoad = guidePaths.at(i+1).second;
//                        currentLength += nextRoad->mLength;
                        nextRoadLaneSection = nextRoad->getFirstLaneSection();
//                        getPosInfo.TraversalLane(nextRoadLaneSection->getLaneFromId(0), rampStatus);
                        rampStatusAndId =getPosInfo.TraversalLane(nextRoadLaneSection->getLaneFromId(0));
                        if(ODR_LANE_TYPE_ENTRY == rampStatusAndId.first)    //属于3.4，计算汇流
                        {
                            mRampInfoOut.mergeStartDis = currentLength;
                            mRampInfoOut.mergeEndDis = mRampInfoOut.mergeStartDis + (nextRoadLaneSection->mSEnd-nextRoadLaneSection->mS);
                            mRampInfoOut.mergeLaneId = pow(2, 17-abs(rampStatusAndId.second));
                        }else  //计算分流
                        {
                            mRampInfoOut.splitStartDis = 0;
                            mRampInfoOut.splitEndDis = currentLength-currentRoad->mLength;
                            mRampInfoOut.splitRampLaneId = pow(2, 17-abs(rampStatusAndId.second));
                        }
                        break;
                    }
                    // 方向>0, 找后继；否则，找前驱(参考ordMGR中的Dir逻辑)
                    /*if (roadDirs->at(i) == 0)
                        nextRoad = currentRoad->getSuccessor();
                    else
                        nextRoad = currentRoad->getPredecessor();*/
                    currentLength += nextRoad->mLength;
                }
                break;
            case ODR_LANE_TYPE_ENTRY:
                mRampInfoOut.mergeStartDis = 0;
                mRampInfoOut.mergeEndDis = getPosInfo.myPosInfo.section->mSEnd - getPosInfo.myPosInfo.trackS;
//                rampInfoOut.mergeEndDis = (getPosInfo.myManager.getLaneSection()->mSEnd-getPosInfo.myPosInfo.trackS);
                mRampInfoOut.mergeLaneId = pow(2, 17-abs(id));

                break;
            case ODR_LANE_TYPE_EXIT:
                mRampInfoOut.splitStartDis = 0;
                mRampInfoOut.splitEndDis = getPosInfo.myPosInfo.section->mSEnd - getPosInfo.myPosInfo.trackS;
//                rampInfoOut.splitEndDis = (getPosInfo.myManager.getLaneSection()->mSEnd-getPosInfo.myPosInfo.trackS);
                mRampInfoOut.splitRampLaneId = pow(2, 17-abs(id));

                break;
        }
        /*rampInfoOut.mergeStartDis = myPosInfo.roadlength - myPosInfo.trackS;
        double nextRoadLength = (myPosInfo.road->getSuccessor())->mLength;
        rampInfoOut.mergeEndDis = rampInfoOut.mergeStartDis + nextRoadLength;*/
        return mRampInfoOut;
    }
    //2、当前位置不在ramp中，遍历当前road的后继是否存在entry/exit
    //2.1、定位当前road
//    nextRoad = getPosInfo.myPosInfo.road;
    //记录当前位置到entry（exit）出现的road终点的距离
//    double roadLength = getPosInfo.myPosInfo.trackS;
//    cout << "Ramp judging in other roads begins....." << endl;
//    cout << "roadDirs.size : " << roadDirs->size() << endl;

    for(int i = 1; i < roadDirs->size(); i++)
    {
        //cout << i << "st circle..." << endl;
        /*currentRoad = nextRoad;
        // 方向>0, 找后继；否则，找前驱(参考ordMGR中的Dir逻辑)。先更新，再判断匝道情况。
        if (roadDirs->at(i) == 0)
            nextRoad = currentRoad->getSuccessor();
        else
            nextRoad = currentRoad->getPredecessor();*/
        currentRoad = guidePaths.at(i).second;
//        getPosInfo.TraversalLane((currentRoad->getFirstLaneSection())->getLaneFromId(0), rampStatus);
        rampStatusAndId = getPosInfo.TraversalLane((currentRoad->getFirstLaneSection())->getLaneFromId(0));


        currentLength += currentRoad->mLength;
//        cout<<"rampstatus"<<rampStatus<<endl;
        if(0 != rampStatusAndId.first){
            cout << "Ramp existed in roadId = " << currentRoad->getId() <<endl;
            break;
        }
        cout << "Traversal roadId = " << currentRoad->getId() << " ends"<< endl;
        //判断匝道结束，在此处结束。
    }
    // 分流汇流各种距离详细代码
//    cout << "Finally rampStatus: " << rampStatus << endl;
//    cout<<"is exit"<<currentRoad->mLength<<endl;
    //2.2.1、给定范围内不存在ramp，直接返回
    switch (rampStatusAndId.first)
    {
        case 0: return mRampInfoOut;
        case ODR_LANE_TYPE_ENTRY:
            mRampInfoOut.rampInfoStatus = 1;
            mRampInfoOut.enterRampInfo = 0;  //存疑
            mRampInfoOut.toDis = -1;       //存疑
            mRampInfoOut.mergeStartDis = currentLength - currentRoad->mLength;
            mRampInfoOut.mergeEndDis = mRampInfoOut.mergeStartDis + (currentRoad->getFirstLaneSection()->mSEnd-currentRoad->getFirstLaneSection()->mS);
            mRampInfoOut.mergeLaneId = pow(2, 17-abs(id));
            return mRampInfoOut;
        case ODR_LANE_TYPE_EXIT:
            mRampInfoOut.rampInfoStatus = 1;
            mRampInfoOut.enterRampInfo = 1;//存疑
            mRampInfoOut.toDis = currentLength- currentRoad->mLength; //存疑
            mRampInfoOut.splitStartDis = currentLength - currentRoad->mLength;
            mRampInfoOut.splitEndDis = currentLength;  //默认整条road都是exit
            mRampInfoOut.splitRampLaneId=pow(2, 17-abs(id));
            return mRampInfoOut;
    }

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

CAPathPlanningOut DynamicMap::GetCAPathPlanningOut(GetPositionInfo &getPosInfo, GuidePaths &guidePaths)
{
    if (!getPosInfo.myPosInfo.road) return mPathPlanningOut; //输入点没有激活
    vector<CARoutingPath> *routingPath = new vector<CARoutingPath>;
    routingPath = GetCARoutingPath(getPosInfo, guidePaths, routingPath);
    mPathPlanningOut.routingPath = routingPath;
    //mPathPlanningOut.routingPath = GetCARoutingPath(getPosInfo, guidePaths,mPathPlanningOut.routingPath);

    //仿照rampInfoOut lll
    int rampStatus = 0;
    RoadHeader *nextRoad = guidePaths.at(1).second;
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
//                    if (roadDirs->at(1) == 0)  //下面涉及roadDirs全部换为guidePaths.first
                    /*if(guidePaths.at(1).first==0)
                        nextNextRoad = nextRoad->getSuccessor(1);
                    else
                        nextNextRoad = nextRoad->getPredecessor(1);*/
                    nextNextRoad = guidePaths.at(2).second;
                    nextNextRoadLaneSection = nextNextRoad->getFirstLaneSection(); //默认是entry
                    mPathPlanningOut.pathPlanningToRampStartDis = currentLength + nextRoad->mLength;
                    mPathPlanningOut.pathPlanningToRampEndDis = mPathPlanningOut.pathPlanningToRampStartDis+ (nextNextRoadLaneSection->mSEnd-nextNextRoadLaneSection->mS);
                    break;
                }
                break;
            case ODR_LANE_TYPE_OFF_RAMP:
                currentLength += nextRoad->mLength;
//                for(int i = 1; i < roadDirs->size(); i++) {
                for(int i = 1; i < guidePaths.size() - 1; i++) {
                    currentRoad = guidePaths.at(i).second;
//                    //判断当前road是否属于junction
                    if (currentRoad->mJuncNo > 0)      //offramp-junction
                    {
                        mPathPlanningOut.pathPlanningToRampStartDis = 0;
                        mPathPlanningOut.pathPlanningToRampEndDis = currentLength-nextRoad->mLength;
                        break;
                    }
                    //offramp-onramp
                    getPosInfo.JudgeRampAbout((currentRoad->getFirstLaneSection())->getLaneFromId(0), rampStatus);
                    if (ODR_LANE_TYPE_ON_RAMP == rampStatus) // 等同onramp
                    {
                        // 方向>0, 找后继；否则，找前驱(参考ordMGR中的Dir逻辑)
                        nextRoad = guidePaths.at(i+1).second;
                        nextRoadLaneSection = nextRoad->getFirstLaneSection();
                        getPosInfo.JudgeRampAbout(nextRoadLaneSection->getLaneFromId(0), rampStatus);
                        if(ODR_LANE_TYPE_ENTRY == rampStatus)
                        {
                            mPathPlanningOut.pathPlanningToRampStartDis = currentLength;
                            mPathPlanningOut.pathPlanningToRampEndDis= mPathPlanningOut.pathPlanningToRampStartDis + (nextRoadLaneSection->mSEnd-nextRoadLaneSection->mS);
                        } else{
                            if (guidePaths.at(i+1).first == 0)
                                nextNextRoad = nextRoad->getSuccessor(1);
                            else
                                nextNextRoad = nextRoad->getPredecessor(1);
                            nextNextRoadLaneSection = nextNextRoad->getFirstLaneSection(); //默认是entry
                            mPathPlanningOut.pathPlanningToRampStartDis = currentLength + nextRoad->mLength;
                            mPathPlanningOut.pathPlanningToRampEndDis = mPathPlanningOut.pathPlanningToRampStartDis + (nextNextRoadLaneSection->mSEnd-nextNextRoadLaneSection->mS);
                            break;
                        }
                    }
                    // 方向>0, 找后继；否则，找前驱(参考ordMGR中的Dir逻辑)
                    /*if (guidePaths.at(i).first == 0)
                        nextRoad = currentRoad->getSuccessor();
                    else
                        nextRoad = currentRoad->getPredecessor();*/
                    currentLength += nextRoad->mLength;
                }
                break;
            case ODR_LANE_TYPE_CONNECTING_RAMP:
                currentLength += nextRoad->mLength;
                for(int i = 1; i < guidePaths.size() - 1; i++)
                {
                    currentRoad = guidePaths.at(i).second;
                    //判断当前road是否属于junction
                    if (currentRoad->mJuncNo > 0)
                    {
                        //判断junction属于哪种（34or长安）
                        nextRoad = guidePaths.at(i+1).second;
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
                    // 方向>0, 找后继；否则，找前驱(参考ordMGR中的Dir逻辑)
                    /*if (guidePaths.at(i).first == 0)
                        nextRoad = currentRoad->getSuccessor();
                    else
                        nextRoad = currentRoad->getPredecessor();*/
                    currentLength += nextRoad->mLength;
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
        /* mPathPlanningOut.pathPlanningToRampStartDis =getPosInfo.myPosInfo.roadlength - getPosInfo.myPosInfo.trackS;
         double nextRoadLength = (getPosInfo.myPosInfo.road->getSuccessor())->mLength;
         mPathPlanningOut.pathPlanningToRampEndDis =  mPathPlanningOut.pathPlanningToRampStartDis + nextRoadLength;*/
        return mPathPlanningOut;

    }
    //2、当前位置不在ramp中，遍历当前road的后继是否存在entry/exit
    //2.1、定位当前road
    //记录当前位置到entry（exit）出现的road终点的距离
    currentLength += nextRoad->mLength;
    for(int i = 1; i < guidePaths.size(); i++)
    {
        currentRoad = guidePaths.at(i).second;
        getPosInfo.JudgeRampAbout((currentRoad->getFirstLaneSection())->getLaneFromId(0), rampStatus);
        if(ODR_LANE_TYPE_ENTRY== rampStatus ||ODR_LANE_TYPE_EXIT== rampStatus){
            cout << "Ramp existed in roadId = " << currentRoad->getId() <<endl;
            break;
        }
        // 方向>0, 找后继；否则，找前驱(参考ordMGR中的Dir逻辑)
        /*if (guidePaths.at(i).first == 0)
            nextRoad = currentRoad->getSuccessor();
        else
            nextRoad = currentRoad->getPredecessor();*/
        currentLength += nextRoad->mLength;
    }
    // 分流/汇流起点和终点距离详细代码
    //2.2.1、给定范围内不存在ramp，直接返回默认值
    switch (rampStatus)
    {
        case 0: return mPathPlanningOut;
        case ODR_LANE_TYPE_ENTRY:
            mPathPlanningOut.pathPlanningType = kInSpecialArea;
            mPathPlanningOut.pathPlanningToRampStartDis = currentLength - currentRoad->mLength;
            mPathPlanningOut.pathPlanningToRampEndDis = mPathPlanningOut.pathPlanningToRampStartDis + (currentRoad->getFirstLaneSection()->mSEnd-currentRoad->getFirstLaneSection()->mS);;
            return mPathPlanningOut;
        case ODR_LANE_TYPE_EXIT:
            mPathPlanningOut.pathPlanningType = kInSpecialArea;
            mPathPlanningOut.pathPlanningToRampStartDis = currentLength - currentRoad->mLength;
            mPathPlanningOut.pathPlanningToRampEndDis = currentLength;  //默认整条road都是exit
            return mPathPlanningOut;
    }
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
    for(int i = 1; i < routingPaths->size(); i++)
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

CADynamicHDMapErc DynamicMap::GetCADynamicHDMapErc(GetPositionInfo &getPosInfo, GuidePaths &guidePathsRange, GuidePaths &guidePathsEndPoint) {

    cout << " GetCAAnchorPosRelatedInfo begins ... " << endl;
    GetCAAnchorPosRelatedInfo(getPosInfo);
    cout << " GetCAAnchorPosRelatedInfo over ... " << endl;
    cout << " GetCASpecialArea begins ... " << endl;
    GetCASpecialArea(getPosInfo, guidePathsRange);
    cout << " GetCASpecialArea over ... " << endl;
    cout << " GetCARampInfoOut begins ... " << endl;
    GetCARampInfoOut(getPosInfo, guidePathsRange);
    cout << " GetCARampInfoOut over ... " << endl;
    GetCAPathPlanningOut(getPosInfo,guidePathsEndPoint);
    GetCADynamicAddition(getPosInfo, guidePathsRange);
    mDynamicHdMapErc.anchorPosRelatedInfo = mAnchorPosRelatedInfo;
    mDynamicHdMapErc.specialArea = mSpecialArea;
    mDynamicHdMapErc.rampInfo = mRampInfoOut;
    mDynamicHdMapErc.pathPlanningOut = mPathPlanningOut;
    mDynamicHdMapErc.dynamicAddition = mDynamicAddition;

    return mDynamicHdMapErc;
}








