//
// Created by JinxBIGBIG on 2022/5/30.
//

#ifndef ODRPARSER_STATICMESSAGE_H
#define ODRPARSER_STATICMESSAGE_H
#include <vector>
#include "tinyxml2.h"
#include "OpenDRIVE.hh"
#include "OdrManager.hh"
#include "OdrGeoCoord.hh"
#include "OdrManagerLite.hh"
#include "BaseNodes/OdrNode.hh"
#include "OdrTrackCoord.hh"
#include "OdrLaneCoord.hh"
#include "positionInfo.h"



//长安相关

class StaticMap{

private:
    const int linkNum = 6;
    const int laneNum = 7;
    const int lineNum = 8;

    std::vector<CALink> mLink;
    std::vector<CALaneAttribute> mLaneAttribute;
    std::vector<CALaneWidthCurvature> mLaneWidthCurvature;
    std::vector<CALaneConnectivityInfo> mLaneConnectivityInfo;
    std::vector<CALine> mLine;
    std::vector<CALineMarking> mLineMarking;
    CAStaticHDMapErc mStaticHdMapErc;

public:
    StaticMap();
    //长安相关

    //静态
    //静态
    std::vector<CALink> GetCALink(GetPositionInfo &getPosInfo, std::vector<std::pair<int, OpenDrive::RoadHeader* >> &guidePaths);

    std::vector<CALaneConnectivityInfo> GetCALaneConnectivityInfo(int dir, OpenDrive::Lane *lane, const int linkId);

    std::vector<CALaneAttribute> GetCALaneAttribute(GetPositionInfo &getPosInfo, double speed, int linkID,
                                                    std::vector<OpenDrive::GeoHeader*> *geoHeadersRoad,
                                                    std::vector<std::vector<Gnss>> &linePointsList,
                                                    std::pair<int, OpenDrive::RoadHeader*> &road, OpenDrive::LaneSection* laneSection);
    std::vector<CALaneAttribute> GetCALaneAttribute(GetPositionInfo &getPosInfo, double speed, int linkID,
                                                    Curvatures &curvaturesRoad, std::vector<std::vector<Gnss>> &linePointsList,
                                                    std::pair<int, OpenDrive::RoadHeader*> &road, OpenDrive::LaneSection* laneSection);
    void GetCASpeedLimit();

    std::vector<CALaneWidthCurvature> GetCALaneWidthCurvature(GetPositionInfo &getPosInfo,OpenDrive::LaneSection *laneSection,
                                                              std::pair<int, OpenDrive::RoadHeader*> road);

    std::vector<CALineMarking> &GetCALineMarking(GetPositionInfo &getPosInfo, OpenDrive::Lane *lane, OpenDrive::LaneSection* laneSection);

    std::vector<CALine> GetCALine(GetPositionInfo &getPosInfo, std::vector<std::vector<Gnss>> &linePointsList, OpenDrive::LaneSection* laneSection);

    CAStaticHDMapErc GetCAStaticHDMapErc(GetPositionInfo &getPosInfo, std::vector<std::pair<int, OpenDrive::RoadHeader* >> &guidePaths);


};

#endif //ODRPARSER_STATICMESSAGE_H