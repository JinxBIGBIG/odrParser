//
// Created by JinxBIGBIG on 2022/6/27.
//

#ifndef ODRPARSER_SHAREMESSAGE_H
#define ODRPARSER_SHAREMESSAGE_H

#include "positionInfo.h"
#include <iostream>
struct IEvalSignal{
    int id;
    double s;
    double t;
    int type;
    int roadID;
    std::vector<int> laneIDs;
};

class ShareMessage {

public:
    std::vector<IEvalSignal> mySignals;
    //IEvalSignal signal0;

public:
    ShareMessage();

    friend void Print(IEvalSignal &signal);

    friend void operator<<(std::ostream &os, const ShareMessage &shareMessage);

    void GetSignals(GetPositionInfo &getPosInfo);







};


#endif //ODRPARSER_SHAREMESSAGE_H
