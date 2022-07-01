//
// Created by JinxBIGBIG on 2022/5/6.
//

#ifndef ODRPARSER_HDENUM_H
#define ODRPARSER_HDENUM_H

#endif //ODRPARSER_HDENUM_H

/**
* @brief 道路类型
* @details  abbr:RoadT
*/
enum CaRoadType
{
    ///<无效
    kRoadTInvalid = 0,

    ///<高速
    kRoadTHighway = 1,

    ///<匝道
    kRoadTRamp = 2,

    ///<城区道路
    kRoadTUrbanRoad = 3
};


/**
* @brief 道路等级
* @details  abbr:RoadG
*/
enum CaRoadGrade
{
    ///<无效
    kRoadGInvalid = 0,

    ///<国道
    kRoadGNationHighway = 1,

    ///<省道
    kRoadGProvincialHighway = 2,

    ///<主要道路
    kRoadGMianRoad = 3,

    ///<次要道路
    kRoadGMinorRoad = 4,

    ///<普通道路
    kRoadGOrdinaryRoad = 5,

    ///<县道
    kRoadGCountyRoad = 6,

    ///<乡道
    kRoadGVillageRoad = 7,

    ///<县乡村内部道路
    kRoadGCountyInternalRoad = 8,

    ///<其他
    kRoadGCountyOther = 99
};

/**
* @brief 特殊区类型
* @details  区分隧道、收费站、施工区；特殊地区类型abbr:SAT
*/
enum CaSpecialAreaType
{
    ///<未知
    kSATUnkown = 0,

    ///<隧道
    kSATTunnel = 1,

    ///<收费站
    kSATToll = 2,

    ///<施工区
    ///<高精地图施工区时效性取决于审图时间，建议参考导航信息
    kSATConstruction = 3,
};

/**
* @brief 当前位置与特殊地区位置关系
* @details  abbr：CRSA
*/
enum CaCurrentRelatedSpecialArea
{
    ///<未知
    kCRSAUnkown = 0,

    ///<出口
    kCRSALeave = 1,

    ///<入口
    kCRSAEnter = 2,

    ///<内部
    kCRSAIn = 3,
};

/**
* @brief 车道线类型
* @details  缩写：LMT
*/
enum CaLineMarkingType
{
    ///<无车道线
    kLMTNone = 0,

    ///<未知
    kLMTUnknown = 1,

    ///<单实线
    kLMTSolidLine = 2,

    ///<单虚线
    kLMTDashedLine = 3,

    ///<双实线
    kLMTDoubleSolidLine = 4,

    ///<双虚线
    kLMTDoubleDashedLine = 5,

    ///<左实右虚
    kLMTLeftSolidRightDashed = 6,

    ///<左虚右实
    kLMTRightSolidLeftDashed = 7,

    ///<短虚线
    kLMTDashedBlocks = 8,

    ///<导流线
    kLMTShadedArea = 9,

    ///<物理分隔
    kLMTPhysicalDivider = 10
};

/**
* @brief 颜色
* @details  对应车道线颜色、车道标志颜色
*/
enum CaColor
{
    ///<未知
    kUnknown = 0,

    ///<其它
    kOther = 1,

    ///<白色
    kWhite = 2,

    ///<黄色
    kYellow = 3,

    ///<橙色
    kOrange = 4,

    ///<红色
    kRed = 5,

    ///<蓝色
    kBlue = 6,

    ///<绿色
    kGreen = 7,

    ///<灰色
    kGrey = 8
};
/**
* @brief 车道线类型
* @details  abbr：LineT
*/
enum CaLineType
{
    ///<无
    kLineTNone = 0,

    ///<车道中心线
    kLineTCenterLine = 1,

    ///<车道边线
    kLineTLaneMarking = 2,

    ///<边界线(护栏、路沿等)
    kLineTBoundary = 3,

    ///<边界线--护栏
    kLineTGuardrail = 4,

    ///<边界线--篱笆
    kLineTFence = 5,

    ///<边界线--路缘
    kLineTKerb = 6,

    ///<边界线--墙
    kLineTWall = 7
};
/**
* @brief 车道限速类型
* @details  abbr:SLT
*/
enum CaSpeedLimitType
{
    ///<无
    kSLTNone = 0,

    ///<未知
    kSLTUnknown = 1,

    ///<无限制
    kSLTImplicit = 2,

    ///<交通灯
    kSLTExplicitOnTrafficSign = 3,

    ///<夜间
    kSLTExplicitNight = 4,

    ///<白天
    kSLTExplicitDay = 5,

    ///<时间段
    kSLTExplicitTimeOrDay = 6,

    ///<下雨
    kSLTExplicitRain = 7,

    ///<雪
    kSLTExplicitSnow = 8,

    ///<雾
    kSLTExplicitFog = 9
};

/**
* @brief 导航规划与匝道位置关系
* @details  根据导航路径规划判断是否进出匝道
*/
enum CaPathPlanningType
{
    ///<无特殊区
    kNone = 0,

    ///<进入
    kInSpecialArea = 1,

    ///<离开
    kOutSpecialArea = 2
};
enum CaLaneType
{
    ///<未知
    kLaneTUnknown = 0,
    ///<正常
    kLaneTNormal = 1,

    ///<出口
    kLaneTExit = 2,

    ///<入口
    kLaneTEntry = 3,

    ///<辅路
    kLaneTAuxiliary = 4,

    ///<应急车道
    kLaneTEmergency = 5,

    ///<受限禁止，多为公交车道
    kLaneTRestrictedForbidden = 6,

    ///<受限可用，通常定义超车道
    kLaneTRestrictedUsable = 7,

    ///<HighOccupancyVehicleLane大容量车道
    ///<多见于美国、加拿大等国
    kLaneTHOV = 8,

    ///<加速车道
    kLaneTExpress = 9,

    ///<掉头车道
    kLaneTReversible = 10,

    ///<减速车道
    kLaneTSlow = 11,

    ///<可行驶路肩
    kLaneTDrivableShoulder = 12,

    ///<可变导向或死亡车道
    ///<主见潮汐车道，又称可逆车道、反流车道；在早晚高峰时期临时调整车道方向
    kLaneTTurnOrSuicide = 13,

    ///<加速减速车道
    kLaneExpressAndSlow = 14,

    ///<非机动车道
    kLaneNonMotorVehicle = 15,

    ///<机非混合车道
    kLaneMixedLane = 16,

    ///<人行道
    kLaneSidewalk = 17,

    ///<爬坡车道
    kLaneClimbingLane = 18
};