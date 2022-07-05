//
// Created by JinxBIGBIG on 2022/5/30.
//

#ifndef ODRPARSER_DYNAMICIMESSAGE_H
#define ODRPARSER_DYNAMICIMESSAGE_H
#include <vector>
#include "positionInfo.h"

class DynamicMap{

private:
    CAAnchorPosRelatedInfo mAnchorPosRelatedInfo;
    std::vector<CASpecialArea> mSpecialArea[3];
    CARampInfoOut mRampInfoOut;
    CAPathPlanningOut mPathPlanningOut;
    CADynamicAddition mDynamicAddition;
    CADynamicHDMapErc mDynamicHdMapErc;

public:
    DynamicMap();

    //长安相关
    //动态
    CAAnchorPosRelatedInfo GetCAAnchorPosRelatedInfo(GetPositionInfo &getPositionInfo);

    //typedef std::vector<std::pair<int, OpenDrive::RoadHeader* >> GuidePaths;
    std::vector<CASpecialArea> GetCASpecialArea(GetPositionInfo &getPosInfo, GuidePaths &guidePaths);


    //int GetRampLaneId(int odrLaneId);
    CARampInfoOut GetCARampInfoOut(GetPositionInfo &getPosInfo, GuidePaths &guidePaths,GuidePaths &guidePathsAdded);

    std::vector<CARoutingPath>* GetCARoutingPath(GetPositionInfo &getPosInfo, GuidePaths &guidePaths,
                                                 std::vector<CARoutingPath>* routingPaths);

    CAPathPlanningOut GetCAPathPlanningOut(GetPositionInfo &getPosInfo, GuidePaths &guidePaths, GuidePaths &guidePathsAdded);

    CADynamicAddition GetCADynamicAddition(GetPositionInfo &getPositionInfo, GuidePaths &guidePaths);


    CADynamicHDMapErc GetCADynamicHDMapErc(GetPositionInfo &getPosInfo, GuidePaths &guidePathsRange, GuidePaths &guidePathsRangeAdded,
                                           GuidePaths &guidePathsEndPoint, GuidePaths &guidePathsEndPointAdd);


    void GetLaneChangeInfo(GetPositionInfo &getPosInfo, GuidePaths &guidePaths);
};
#endif //ODRPARSER_DYNAMICIMESSAGE_H