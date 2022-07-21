//
// Created by JinxBIGBIG on 2022/7/15.
//

#ifndef ODRPARSER_ISTRUCT_H
#define ODRPARSER_ISTRUCT_H

#include "positionInfo.h"

struct IEvalSignal{
    int id;
    double s;
    double t;
    int type;
    int roadID;
    std::vector<int> laneIDs;
};
struct Point{
    double x{0.};
    double y{0.};
    double z{0.};

    Point() = default;

    Point(double _x, double _y, double _z)
            : x(_x)
            , y(_y)
            , z(_z)
    {}
};
struct PointLaneInfo{
    Point point;
    int laneID;
    int roadID;
    int juncID;
    double curve;
    double sectionStart;
    double sectionEnd;
    double t;
};

#endif //ODRPARSER_ISTRUCT_H
