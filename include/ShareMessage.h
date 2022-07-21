//
// Created by JinxBIGBIG on 2022/6/27.
//

#ifndef ODRPARSER_SHAREMESSAGE_H
#define ODRPARSER_SHAREMESSAGE_H

#include "positionInfo.h"
#include "IStruct.h"
#include <iostream>

class ShareMessage {

public:
    std::vector<IEvalSignal> mySignals;
    //IEvalSignal signal0;

public:
    ShareMessage();

    friend void Print(IEvalSignal &signal);

    friend void operator<<(std::ostream &os, const ShareMessage &shareMessage);

    void GetSignals(GetPositionInfo &getPosInfo);


    std::vector<Point>
    GetPointInSection(GetPositionInfo &getPosInfo, double curve, int indexEnd, int iterInSectionInner, double step,
                      std::vector<PointLaneInfo> &inputPointLaneST, Point &point, std::vector<Point> &detailedPoints);

    std::vector<Point>
    GetPointInSectionJunc(GetPositionInfo &getPosInfo, OpenDrive::GeoHeader *geo, int iterInRoad, double tempT,
                          double step,
                          std::vector<PointLaneInfo> &inputPointLaneST, Point &point,
                          std::vector<Point> &detailedPoints);

    void
    GetLineAndCurLinePoint(GetPositionInfo &getPosInfo, double step, std::vector<PointLaneInfo> &inputPointLaneInfo,
                           std::vector<Point> &detailedPoints);
};


#endif //ODRPARSER_SHAREMESSAGE_H
