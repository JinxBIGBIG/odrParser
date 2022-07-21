//
// Created by JinxBIGBIG on 2022/6/9.
//

#ifndef ODRPARSER_CASTRUCT_H
#define ODRPARSER_CASTRUCT_H
#include <vector>
//dynamic
struct CAAnchorPosRelatedInfo{
    int numOfLanes = -1;
    int currentLaneNum = -1;
    int withinGeofence = 0;  //此处值待定
    int currentLinkId = 0;
    double toLinkEnd = -1;
    double toGeoFenceEnd = 0;
};

struct CASpecialArea{
    CaSpecialAreaType type = kSATUnkown;
    double specialAreaLength = -1;
    CaCurrentRelatedSpecialArea relatedType = kCRSAUnkown;
    double toSpecialArea = -1;
};

struct CARampInfoOut{
    int rampInfoStatus = 0;
    int enterRampInfo = 0;
    double toDis = -1;
    double splitStartDis = -1;
    double splitEndDis = -1;
    double mergeStartDis = -1;
    double mergeEndDis = -1;
    int splitRampLaneId = 0;
    int mergeLaneId = 0;
};
struct CARoutingPath{
    int linkId = 0;
    std::vector<int> *availableLanes;
    int availableLaneNum = 0;
};
struct CAPathPlanningOut{
    CaPathPlanningType pathPlanningType = kNone;
    double pathPlanningToRampStartDis = -1;
    double pathPlanningToRampEndDis = -1;
    std::vector<CARoutingPath> *routingPath = new std::vector<CARoutingPath>;
    int rampDirectionToMainRoad = 0;
};

struct CADynamicAddition{
    double utc = 0;
    bool isInMap = 0;
    double toNumChangeOfLaneDis = -1;
    int toNumChangeOfLanesType = 0;
    int numChangeOfLaneId = 0;
    double laneCurvature100 = 0;
    double laneCurvature200 = 0;
    double laneCurvature300 = 0;
    double latitude = 0;
    double longitude = 0;
    double azimuth = 0;  //航向角
    double xAcc = 0;   //加速度z、y、z，RDB直接输出
    double yAcc = 0;  //
    double zAcc = 0;  //
};

//动态信息集（整体动态信息的综合）
struct CADynamicHDMapErc{
    CAAnchorPosRelatedInfo anchorPosRelatedInfo;
    std::vector<CASpecialArea>* specialArea;
    CAAnchorPosRelatedInfo relatedInfo;
    CARampInfoOut rampInfo;
    CAPathPlanningOut pathPlanningOut;
    CADynamicAddition dynamicAddition;
};

struct CADyFinalParameter{
    CAAnchorPosRelatedInfo anchorPosRelatedInfo;
    std::vector<CASpecialArea> specialAreas;
    CARampInfoOut rampInfoOut;
    CAPathPlanningOut pathPlanningOut;
};


//static
struct Gnss{
    Gnss(double lng, double lat){
        this->lng = lng;
        this->lat = lat;
    }
    double lng = 0;  //经度，(0,-180,180),东经为正
    double lat = -0;   //纬度，(-0,-90,90),北纬为正
};


struct CASpeedLimit{
    int value = 0;  //车道限速值
    CaSpeedLimitType type = kSLTUnknown;  //限速类型
};
struct CALaneWidthCurvature{
    double dis = -1;  //距离link开始端的距离
    double value = 0;  //值，当其值为999时表示是螺旋线
    double valueEnd = 0;
};

struct CALaneConnectivityInfo
{
    int linkId = 0;   //link编号
    int laneNum = -1;   //车道索引
};
struct CALaneAttribute{
    int index = -1;   //车道索引
    int type= 0;  // 车道类型 ，，不确定
    CASpeedLimit speedLimit ;  //车道限速
    int leftIndex = 0;  //左车道线索引
    int centerIndex = 0;  //中心线索引
    int rightIndex = 0;   //右车道线索引
    std::vector<Gnss> *laneClinePoints  ;  //车道中心线上点集合
//    struct CALaneWidthCurvature lane_curvature_list ;  //车道曲率集合，取值范围±1
    std::vector<CALaneWidthCurvature> laneCurvatureList;
//    struct CALaneConnectivityInfo lane_connectivity_list ;  //车道连接关系
    std::vector<CALaneConnectivityInfo> laneConnectivityList ;  //车道连接关系
    int curvatureNum = 0;  //车道曲率段数量
    double laneWidth = 0;  //车道宽度
    int clinePointsNum = 0;  //车道中心线点的数量


};
struct CALineMarking{
    CaLineMarkingType type = kLMTNone;   //车道线类型
    CaColor color = kUnknown;   //车道线颜色
    double dis = -1;   //车道线类型变化偏移量
};

struct CALine{
    int index = -1;   //车道线索引
    CaLineType lineType = kLineTNone;  //车道线判断
    std::vector<CALineMarking> markings ;  //车道线属性集合
    std::vector<Gnss> *linePoints ;  //车道线轨迹点集合
    int linePointsNum = 0;  //车道线轨迹点数量

};

struct CAHDMapStatus{
    ///< 本地地图是否可使用:[/],(0,0,1),[/],(1,0),判断地图是否正常使用
    ///< 0:本地地图不可使用，需要更新
    ///< 1:本地地图可使用，不需要更新
    bool mapStatus = 0;

    ///< 地图更新状态:[/],(0,0,1),[/],(1,0),判断高精地图是否正在更新
    ///< 0:标识地图没有处于更新状态
    ///< 1:标识地图正在更新
    bool updateStatus = 0;
};


struct CALink{
    int linkId = 0;
    double linkLength = 0;
    CaRoadType type;
    const int laneNum = 7;
    std::vector<CALaneAttribute> laneAttributes;
    std::vector<CALine> line;
    const int lineNum = 8;
    const CaRoadGrade grade = kRoadGNationHighway;
};

struct CAStaticHDMapErc{
    ///< 路段总数:[/],(0,0,6),[/],(1,0),links.size()
    int linkNum = 6;
    ///<高精地图状态信息:[/],(/,/,/),[1],(/,/),/
    CAHDMapStatus hdMapStatus;
    ///< 路段相关的信息:[/],(/,/,/),[12],(/,/),/
    std::vector<CALink> links;

};




#endif //ODRPARSER_CASTRUCT_H
