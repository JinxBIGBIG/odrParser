//
// Created by JinxBIGBIG on 2022/6/27.
//

#include "ShareMessage.h"
#include "OdrBbox.hh"

using namespace std;
using namespace OpenDrive;

const double stepLine = 30;

ShareMessage::ShareMessage(){}

void Print(IEvalSignal &signal) {
    cout << "ID: " << signal.id << endl;
    cout << "st: " << signal.s << "; " << signal.t<<endl;
    cout << "type: " << signal.type << endl;
    cout << "roadID: " << signal.roadID << endl;
    cout << "laneIDs: " ;
    for(int i = 0; i < signal.laneIDs.size(); i++)
    {
        cout << signal.laneIDs.at(i) << ";";
    }
    cout << endl;
}


void ShareMessage::GetSignals(GetPositionInfo &getPosInfo) {
    getPosInfo.myManager.setLanePos(124, -1, 0, 0);
    getPosInfo.myManager.track2inertial();
    RoadHeader* road = getPosInfo.myManager.getRoadHeader();
    OpenDrive::Signal* signal = road->getFirstSignal();
    IEvalSignal signalInfo;
    while(signal != nullptr){
        if((1000001 == signal->mType || 1000002 == signal->mType)){
            signalInfo.id = signal->mId;
            signalInfo.s = signal->mS;
            signalInfo.t = signal->mT;
            signalInfo.type = signal->mType;
            if(-99 != signal->mMinLane){
                for(int i = signal->mMinLane; i <= signal->mMaxLane; i++)
                    signalInfo.laneIDs.push_back(i);
            }
            signalInfo.roadID = getPosInfo.myPosInfo.roadId;
            mySignals.push_back(signalInfo);
            signalInfo.laneIDs.clear();
        }
        signal = (Signal*)signal->getRight();
    }


}

void operator<<(ostream &os, const ShareMessage &shareMessage) {
    for(int i = 0; i < shareMessage.mySignals.size(); i++){
        os << "id: " << shareMessage.mySignals.at(i).id << endl << "st: " << shareMessage.mySignals.at(i).s << "; " <<
        shareMessage.mySignals.at(i).t << endl << "type: " << shareMessage.mySignals.at(i).type << endl << "roadID: " <<
        shareMessage.mySignals.at(i).roadID << endl;
        os << "laneID: ";
        for(int j = 0; j < shareMessage.mySignals.at(i).laneIDs.size(); j++)
        {
            os << shareMessage.mySignals.at(i).laneIDs.at(j) << "; ";
        }
        os << endl << endl;
    }
}

vector<Point> ShareMessage::GetPointInSection(GetPositionInfo &getPosInfo, double curve, int indexEnd, int iterInSectionInner,
                                        double step, vector<PointLaneInfo> &inputPointLaneST, Point &point, vector<Point> &detailedPoints){
    if(0 == curve){
        //直线
        //本次循环的起始点的前一个点(iterInSectionInner-1)
        point = getPosInfo.GetMiddlePointCoord(inputPointLaneST.at(iterInSectionInner-1).point, inputPointLaneST.at(indexEnd-1).point, point);
        detailedPoints.push_back(point);
        detailedPoints.push_back(inputPointLaneST.at(indexEnd-1).point);
    }else{
        //曲线
        //本次循环的起始点的前一个点(iterInSectionInner-1)
        double tempS = inputPointLaneST.at(iterInSectionInner-1).sectionStart;
        double tempSEnd = inputPointLaneST.at(indexEnd-1).sectionStart;//存疑具体给end，or start
        double tempT = inputPointLaneST.at(iterInSectionInner-1).t;
        int numberStep = abs(tempS - tempSEnd)/step + 0.5;//四舍五入
        double stepT = abs( tempT- inputPointLaneST.at(indexEnd-1).t)/numberStep;
        while(tempS < tempSEnd){
            getPosInfo.myManager.setTrackPos(inputPointLaneST.at(iterInSectionInner-1 ).roadID, tempS, tempT);
            getPosInfo.myManager.track2inertial();
            point = {getPosInfo.myManager.getInertialPos().getX(), getPosInfo.myManager.getInertialPos().getY(), getPosInfo.myManager.getInertialPos().getZ()};
            detailedPoints.push_back(point);
            tempS += step;
            tempT += stepT;
        }
        detailedPoints.push_back(inputPointLaneST.at(indexEnd-1).point);

    }
    return detailedPoints;
}

vector<Point> ShareMessage::GetPointInSectionJunc(GetPositionInfo &getPosInfo, GeoHeader *geo, int iterInRoad, double tempT,
                                        double step, vector<PointLaneInfo> &inputPointLaneST, Point &point,
                                        vector<Point> &detailedPoints) {
    //直线取点
    if(0 == (geo->mCurv || geo->mCurvEnd)){
        double tempS = geo->mS;
        while(tempS <= geo->mSEnd){
            getPosInfo.myManager.setTrackPos(inputPointLaneST.at(iterInRoad).roadID, tempS, tempT);
            getPosInfo.myManager.track2inertial();
            point = {getPosInfo.myManager.getInertialPos().getX(), getPosInfo.myManager.getInertialPos().getY(), getPosInfo.myManager.getInertialPos().getZ()};
            detailedPoints.push_back(point);
            tempS += stepLine;
        }
    }
        //曲线取点
    else{
        double tempS = geo->mS;
        while(tempS <= geo->mSEnd){
            getPosInfo.myManager.setTrackPos(inputPointLaneST.at(iterInRoad).roadID, tempS, tempT);
            getPosInfo.myManager.track2inertial();
            point = {getPosInfo.myManager.getInertialPos().getX(), getPosInfo.myManager.getInertialPos().getY(), getPosInfo.myManager.getInertialPos().getZ()};
            detailedPoints.push_back(point);
            tempS += step;
        }
    }
    return detailedPoints;
}

void ShareMessage::GetLineAndCurLinePoint(GetPositionInfo &getPosInfo, double step, vector<PointLaneInfo> &inputPointLaneInfo, vector<Point> &detailedPoints){

    int iter = 0;
    int iterInRoad = 1;
    int iterInSection = 1;
    int iterInSectionInner = 1;
    int roadID = inputPointLaneInfo.at(0).roadID;
    int juncID = inputPointLaneInfo.at(0).juncID;
    int curve = inputPointLaneInfo.at(0).curve;
    double sectionS = inputPointLaneInfo.at(0).sectionStart;

    detailedPoints.push_back(inputPointLaneInfo.at(0).point);
    Point point = inputPointLaneInfo.at(0).point;

    while(iter < inputPointLaneInfo.size()){
        for(int i = iterInRoad; i < inputPointLaneInfo.size(); i++){
            //1、同一个road层级
            //1.1、存在不同的road
            if(roadID != inputPointLaneInfo.at(i).roadID){
                //2、同一个laneSection层级
                //2.1、road非junction
                if(juncID < 0){
                    for(int k = iterInSection; k < i; k++){
                        //2.1.1、存在不同的laneSection
                        if(sectionS != inputPointLaneInfo.at(k).sectionStart){
                            for(int j = iterInSectionInner; j < k; j++){
                                //2.1.1.1、出现曲率变化的情况
                                if(curve != inputPointLaneInfo.at(j).curve){
                                    GetPointInSection(getPosInfo, curve, j, iterInSectionInner, step, inputPointLaneInfo, point,detailedPoints);
                                    curve = inputPointLaneInfo.at(j).curve;
                                    iterInSectionInner = j+1;
                                    continue;
                                }
                                    //2.1.1.2、曲率值不变(或全直或全曲)
                                else{
                                    GetPointInSection(getPosInfo, curve, k, iterInSectionInner, step, inputPointLaneInfo, point,detailedPoints);
                                }
                            }
                            sectionS = inputPointLaneInfo.at(k).sectionStart;
                            iterInSection = k+1;
                            continue;
                        }
                            //2.1.2、均在一个laneSection中
                        else{
                            for(int j = iterInSectionInner-1; j < i; j++){
                                //2.1.2.1、出现曲率变化的情况
                                if(curve != inputPointLaneInfo.at(j).curve){
                                    GetPointInSection(getPosInfo, curve, j, iterInSectionInner, step, inputPointLaneInfo, point,detailedPoints);
                                    curve = inputPointLaneInfo.at(j).curve;
                                    iterInSectionInner = j+1;
                                    continue;
                                }
                                    //2.1.2.2、曲率值不变(或全直或全曲)
                                else{
                                    GetPointInSection(getPosInfo, curve, i, iterInSectionInner, step, inputPointLaneInfo, point,detailedPoints);
                                }
                            }
                        }
                    }
                }
                //2.2、road为junction,junc中不考虑变道
                else{
                    getPosInfo.myManager.setLanePos(inputPointLaneInfo.at(iterInRoad).roadID, inputPointLaneInfo.at(iterInRoad).laneID, 0, 0);
                    getPosInfo.myManager.track2inertial();
                    RoadHeader *road = getPosInfo.myManager.getRoadHeader();
                    double tempT = getPosInfo.myManager.getTrackPos().getT();
                    GeoHeader *geo = road->getFirstGeoHeader();
                    while(geo != nullptr){
                        //curve的start和end均为0时表示是直线；除此为曲线
                        //直线只取起点和终点
                        GetPointInSectionJunc(getPosInfo, geo, iterInRoad, tempT, step, inputPointLaneInfo, point, detailedPoints);
                        geo = (GeoHeader *)geo->getRight();
                    }
                }

                roadID = inputPointLaneInfo.at(i).roadID;
                juncID = inputPointLaneInfo.at(i).juncID;
                //roadID给i，进行比较则应从下一个开始
                iterInRoad = i+1;
                continue;
            }
            //1.2、均在同一个road中
            else{
                //1.2.1、road非junc
                if(juncID < 0){
                    for(int k = iterInSection-1; k < inputPointLaneInfo.size(); k++){
                        //2.1、存在不同的laneSection
                        if(sectionS != inputPointLaneInfo.at(k).sectionStart){
                            for(int j = iterInSectionInner; j < k; j++){
                                //2.1.1、出现曲率变化的情况
                                if(curve != inputPointLaneInfo.at(j).curve){
                                    GetPointInSection(getPosInfo, curve, j, iterInSectionInner, step, inputPointLaneInfo, point,detailedPoints);
                                    curve = inputPointLaneInfo.at(j).curve;
                                    iterInSectionInner = j+1;
                                    continue;
                                }
                                    //2.1.2、曲率值不变(或全直或全曲)
                                else{
                                    GetPointInSection(getPosInfo, curve, k, iterInSectionInner, step, inputPointLaneInfo, point,detailedPoints);
                                }
                            }
                            sectionS = inputPointLaneInfo.at(k).sectionStart;
                            iterInSection = k+1;
                            continue;
                        }
                            //2.2、均在一个laneSection中
                        else{
                            for(int j = iterInSectionInner; j < k; j++){
                                //2.1.1、出现曲率变化的情况
                                if(curve != inputPointLaneInfo.at(j).curve){
                                    GetPointInSection(getPosInfo, curve, j, iterInSectionInner, step, inputPointLaneInfo, point,detailedPoints);
                                    curve = inputPointLaneInfo.at(j).curve;
                                    iterInSectionInner = j+1;
                                    continue;
                                }
                                    //2.1.2、曲率值不变(或全直或全曲)
                                else{
                                    GetPointInSection(getPosInfo, curve, k, iterInSectionInner, step, inputPointLaneInfo, point,detailedPoints);
                                }
                            }
                        }
                    }
                }
                //1.2.2、road为junc
                else{
                    getPosInfo.myManager.setLanePos(inputPointLaneInfo.at(iterInRoad).roadID, inputPointLaneInfo.at(iterInRoad).laneID, 0, 0);
                    getPosInfo.myManager.track2inertial();
                    RoadHeader *road = getPosInfo.myManager.getRoadHeader();
                    double tempT = getPosInfo.myManager.getTrackPos().getT();
                    GeoHeader *geo = road->getFirstGeoHeader();
                    while(geo != nullptr){
                        //curve的start和end均为0时表示是直线；除此为曲线
                        //直线只取起点和终点
                        GetPointInSectionJunc(getPosInfo, geo, iterInRoad, tempT, step, inputPointLaneInfo, point, detailedPoints);
                        geo = (GeoHeader *)geo->getRight();
                    }
                }
            }
        }
        iter = iter + iterInRoad;
    }
}





