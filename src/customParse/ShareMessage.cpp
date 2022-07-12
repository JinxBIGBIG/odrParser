//
// Created by JinxBIGBIG on 2022/6/27.
//

#include "ShareMessage.h"
using namespace std;
using namespace OpenDrive;

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



