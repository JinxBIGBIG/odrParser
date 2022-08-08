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

    friend std::ostream &operator<<(std::ostream &os, const ShareMessage &shareMessage);

    Point &
    GetInertialPointBySTPos(GetPositionInfo &getPosInfo, double tempS, double tempT, int roadID,  Point &point);

    void GetSignals(GetPositionInfo &getPosInfo);


    std::vector<Point>
    GetPointInSection(GetPositionInfo &getPosInfo, double curve, int indexEnd, int iterInSectionInner, double step,
                      std::vector<PointLaneInfo> &inputPointLaneST, Point &point, std::vector<Point> &detailedPoints);

    std::vector<Point>
    GetPointInSectionJunc(GetPositionInfo &getPosInfo, OpenDrive::GeoHeader *geo, int iterInRoad, double tempT,
                          double step,
                          std::vector<PointLaneInfo> &inputPointLaneST, Point &point,
                          std::vector<Point> &detailedPoints);

    std::vector<Point>
    GetPointInRoadWithoutChangeLane(GetPositionInfo &getPosInfo, OpenDrive::GeoHeader *geo, double tempT, double step,
                                    int roadID, Point &point, std::vector<Point> &detailedPoints);

    void
    GetLineAndCurLinePoint(GetPositionInfo &getPosInfo, double step, std::vector<PointLaneInfo> &inputPointLaneInfo,
                           std::vector<Point> &detailedPoints);

    void
    GetPointPathInRampRoad(GetPositionInfo &getPosInfo, double step, int roadID, int laneID, std::vector<Point> &detailedPoints);
    void
    GetPointPathInRampRoadAll(GetPositionInfo &getPosInfo, double step, int roadID, int laneID, std::vector<Point> &detailedPoints);
    void
    GetPointPathInRoadLineSpecial(GetPositionInfo &getPosInfo, double step, int roadID, int laneID, std::vector<Point> &detailedPoints);

    void GetPointPathInJunc(GetPositionInfo &getPosInfo, double step, std::vector<Point> &inputPoint,
                            std::vector<Point> &detailedPoints);

    void GetPointInCommonRoad(GetPositionInfo &getPosInfo, double step, std::vector<Point> &inputPoint,
                              std::vector<Point> &detailedPoints);



};


#endif //ODRPARSER_SHAREMESSAGE_H
