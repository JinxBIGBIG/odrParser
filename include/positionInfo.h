//
// Created by JinxBIGBIG on 2022/4/14.
//

#ifndef ODRPARSER_POSITIONINFO_H
#define ODRPARSER_POSITIONINFO_H
#include "tinyxml2.h"
#include "OpenDRIVE.hh"
#include "OdrManager.hh"
#include "OdrGeoCoord.hh"
#include "OdrManagerLite.hh"
#include "BaseNodes/OdrNode.hh"
#include "BaseNodes/OdrRoadHeader.hh"
#include "BaseNodes/OdrObject.hh"
#include "BaseNodes/OdrSignal.hh"
#include "BaseNodes/OdrJuncHeader.hh"
#include "BaseNodes/OdrBridge.hh"
#include "BaseNodes/OdrElevation.hh"
#include "BaseNodes/OdrLane.hh"
#include "BaseNodes/OdrLaneAccess.hh"
#include "BaseNodes/OdrLaneSection.hh"
#include "BaseNodes/OdrRoadLink.hh"
#include "BaseNodes/OdrJuncLink.hh"
#include "BaseNodes/OdrJuncLaneLink.hh"
#include "BaseNodes/OdrSignal.hh"
#include "BaseNodes/OdrRoadMark.hh"
#include "BaseNodes/OdrRoadType.hh"
#include "BaseNodes/OdrGeoHeader.hh"
#include "BaseNodes/OdrLaneOffset.hh"
#include "OdrTrackCoord.hh"
#include "OdrLaneCoord.hh"
#include "proj.h"
#include "CaEnum.h"
#include "CaStruct.h"
#include <vector>
#include <math.h>
#include <algorithm>

struct PositionInfo {
    int laneId;
    int roadId;
    int junctionId;
    int roadlength;

    double sectionS;
    unsigned int laneType;
    double laneSpeed;
    double trackS;
    double trackT;
    double roadheading;

    // evaluationProfile
    double ElvmS;
    double ElvmSEnd;
    double ElvmA;
    double ElvmB;
    double ElvmC;
    double ElvmD;
    double laneWidth;

    std::string geoReference;

    OpenDrive::Lane* lane;
    OpenDrive::RoadHeader* road;
    OpenDrive::RoadLink* preRoadLink;
    OpenDrive::RoadLink* sucRoadLink;
    OpenDrive::LaneSection* section;
    OpenDrive::EnRoadMarkType roadmarkType;
    OpenDrive::EnRoadMarkColor roadmarkColor;
    OpenDrive::Elevation* posElevation;

    OpenDrive::GeoHeader* geoHeader;
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

typedef std::vector<std::pair<int, OpenDrive::RoadHeader* >> GuidePaths;
typedef std::vector<CALaneWidthCurvature> Curvatures;

class GetPositionInfo {

public:
    OpenDrive::OdrManager myManager;
    std::string myXodrPath;
    Point myPoint;
    Point myEndPoint;
    double myRange;
    std::vector<std::string> myKeyWords;
    PositionInfo myPosInfo;
    PositionInfo myEndPosInfo;

public:

    GetPositionInfo();

    GetPositionInfo(OpenDrive::OdrManager &manager, std::string &xodrPath, Point &myStartPoint, Point &myEndPoint,
                    double myRange, PositionInfo &posInfo);

    GetPositionInfo(OpenDrive::OdrManager &manager, std::string &xodrPath, Point &myStartPoint, double myRange, PositionInfo &posInfo);

    bool InitXodr();

    bool InitialPosition(Point point, OpenDrive::OdrManager& manager);


    bool GetInertialPosInfo(Point &point, OpenDrive::OdrManager &manager);


    //bool GetInertialPosInfo();

    PositionInfo GetInertialPosInfo(OpenDrive::OdrManager& manager, PositionInfo &posInfo);

    //获取终点的相关信息并将manager恢复到起点，
    void GetEndPosInfo(Point &point, Point &endPoint, OpenDrive::OdrManager& manager);

    int DrivingDirRelToRoad(bool isRHT);


    void GetGeoHeader(Point point);

    void coordXYtoST(Point point);

    void getElavationInfos();


    //功能函数

    int GetLaneNum();
    int TraversalLane(OpenDrive::Lane *tempLane,int &laneStatus);
    std::pair<int,int> TraversalLane(OpenDrive::Lane *tempLane);

    int JudgeRampAbout(OpenDrive::Lane *tempLane, int &laneStatus);
    int JudgeRampAbout(OpenDrive::Lane *tempLane);

    //获取给定范围内的所有road的方向标识
    std::vector<int>* GetRoadDirs(const int range, std::vector<int> *roadDirs);
    std::vector<OpenDrive::RoadHeader*>& GetRoads(const int range, std::vector<OpenDrive::RoadHeader*> &roads);
    std::vector<std::pair<int, OpenDrive::RoadHeader*>> &GetRoads(const int range, std::vector<std::pair<int, OpenDrive::RoadHeader*>> &roads);
    //获取给定终点的所有road的方向标识
    std::vector<int>* GetRoadDirs(const Point &endPoint);
    std::vector<OpenDrive::RoadHeader*>& GetRoads(std::vector<OpenDrive::RoadHeader*> &roads);
    std::vector<std::pair<int, OpenDrive::RoadHeader*>> &GetRoads(std::vector<std::pair<int, OpenDrive::RoadHeader*>> &roads);


    std::vector<std::pair<int, OpenDrive::RoadHeader* >> GetLastPaths(const Point &startPoint, std::vector<std::pair<int, int>> &inputPaths,
                                                                  std::vector<std::pair<int, OpenDrive::RoadHeader *>> &lastPaths);

    OpenDrive::RoadHeader *GetShortestRoadInJunction(OpenDrive::RoadHeader* currentRoad, OpenDrive::RoadHeader *juncRoad);

    std::pair<int, OpenDrive::RoadHeader*> AddNextRoad(int &dir, OpenDrive::RoadHeader* currentRoad, std::pair<int, OpenDrive::RoadHeader*> &nextRoadMessage);

    //待给了导航路径后采用此方法进行验证

    std::vector<std::pair<int, OpenDrive::RoadHeader* >> AddJuncRoad2GuidePathsWithTwoRoads(GuidePaths guidePaths);
    std::vector<std::pair<int, OpenDrive::RoadHeader* >> AddJuncRoad2GuidePaths(std::vector<std::pair<int, int>> &inputPaths, GuidePaths &guidePaths);
    std::vector<std::pair<int, int >> AddJuncLane2GuidePaths(std::vector<std::pair<int, int>> &inputPaths, std::vector<std::pair<int, int>> &guidePaths);

    //用此替代验证
    std::vector<std::pair<int, OpenDrive::RoadHeader* >> GetGuidePaths(const int range);


    CASpecialArea TraversalSpecialAreaInCurrentRoad(const std::string keyWord, CASpecialArea &specialArea);

    CASpecialArea TraversalSpecialAreaInRoad(OpenDrive::RoadHeader *tempRoad, OpenDrive::Object* tempObject,
                                              const std::string keyWord, const int dir,CASpecialArea &specialArea);
    //void FindRampInRoad(OpenDrive::RoadHeader* road, int& rampStatus);

    std::pair<int, std::vector<int>*> GetLaneIdNumInLaneSection(OpenDrive::LaneSection *laneSection);

    std::vector<std::pair<int, OpenDrive::LaneSection *>> &GetLaneSectionsByEndPoint(GuidePaths &guidePaths,
                                                                                     std::vector<std::pair<int, OpenDrive::LaneSection *>> &laneSections);

    std::vector<OpenDrive::LaneSection *> &GetLaneSectionsByEndPoint(GuidePaths &guidePaths,
                                                                     std::vector<OpenDrive::LaneSection *> &laneSections);
    double GetLaneOffset(OpenDrive::RoadHeader* road,  double s);

    double GetLaneWidth(OpenDrive::Lane *lane, double ds);

    void getRampInfo(const int laneType);

    CaLaneType LaneTypeMapping(const int odrLaneType);

    CaLineMarkingType RoadMarkTypeMapping(const int odrType);

    CaColor RoadMarkColorMapping(const int odrColor);

    CaRoadType RoadTypeMapping(const int &roadType);

    double GetForwardCurvature(double viewRange);

    CALaneWidthCurvature CurvatureReparse(OpenDrive::GeoHeader *geoHeader, CALaneWidthCurvature &curvature);
    Curvatures GetCurvatureInRoad(int originalDir, int iter, OpenDrive::RoadHeader*road, Curvatures &curvaturesRoad);
    std::vector<OpenDrive::GeoHeader *> *GetGeoHeadersRoad(int originalDir, int iter, OpenDrive::RoadHeader *roadHeader,
                                                           std::vector<OpenDrive::GeoHeader *> *geoHeadersRoad);
    Curvatures GetCurLaneSection(OpenDrive::LaneSection *laneSection, Curvatures &curvaturesRoad, Curvatures &curLaneSection);

    void getNextRoadInfo();

    bool GetGeographicCoordinate(PJ_COORD &coord);
    bool GetGeographicCoordinate(PJ_COORD &coord, char *targetProj);

    void GetLaneLinePoints(OpenDrive::RoadHeader* road, OpenDrive::LaneSection *laneSection, double step,
                           std::vector<std::vector<Gnss>>& linePointsList, std::vector<std::vector<Gnss>>& cLinePointsList);
    void GetLaneLinePoints(OpenDrive::RoadHeader* road, OpenDrive::LaneSection *laneSection, double step,
                           std::vector<std::vector<Point>>& linePointsList, std::vector<std::vector<Point>>& cLinePointsList);
    bool ProjTest();


    void GetCustomMessage(const Point point);

    std::vector<std::pair<int, int >> ConvertPoints2Paths(std::vector<Point> &inputPoints, std::vector<std::pair<int, int >>&paths);

    void ReaderXMLTest();

    void Test();


    std::tuple<int, int, double, OpenDrive::LaneSection *>
    TraversalRoad(OpenDrive::RoadHeader *tempRoad, double currentLength, std::pair<int, int> &traversalResult,
                  std::tuple<int, int, double, OpenDrive::LaneSection *> &traversalRoadResult);

    double GetEndPointLength(GuidePaths &guidePathsEndPoint, Point &endPoint);

    double GetSpiralCurvature(OpenDrive::GeoHeader *geoHeader, double s);


    void GetSDsOffsetSet(OpenDrive::RoadHeader *road, OpenDrive::LaneSection *laneSection, double step,
                         std::vector<double> &sSectionSet, std::vector<double> &sToSectionDisSet,
                         std::vector<double> &refSSet);

    void GetWidthSSetList(OpenDrive::RoadHeader *road, OpenDrive::LaneSection *laneSection, double step,
                          std::vector<double> &sSectionSet, std::vector<std::vector<double>> &widthSetList);

    void GetSTSetList(std::vector<double> &sSectionSet, std::vector<double> &refSSet, std::vector<std::vector<double>> &widthSetList,
                      std::vector<std::vector<double>> &tSetList,std::vector<std::vector<double>> &tClineSetList);


    Point GetLanePointList(OpenDrive::RoadHeader *road, double s, double t, Point &point, bool InertialGeo);

    std::vector<int> GetNextRoadLaneIDs(int laneID, OpenDrive::RoadHeader *nextRoad, std::vector<int> &nextRoadLaneId);
};

#endif //ODRPARSER_POSITIONINFO_H