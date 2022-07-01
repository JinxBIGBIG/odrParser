//
// Created by JinxBIGBIG on 2022/4/21.
//

#ifndef ODRPARSER_PUREODRPARSE_H
#define ODRPARSER_PUREODRPARSE_H

#include <vector>
#include "tinyxml2.h"
#include "OpenDRIVE.hh"
#include "OdrManager.hh"
#include "OdrGeoCoord.hh"
#include "OdrManagerLite.hh"
#include "BaseNodes/OdrNode.hh"
#include "BaseNodes/OdrRoadHeader.hh"
#include "BaseNodes/OdrObject.hh"
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
#include "OdrTrackCoord.hh"
#include "OdrLaneCoord.hh"
#include "BaseNodes/OdrPosition.hh"
#include "BaseNodes/OdrRoadData.hh"
#include <iostream>
#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;
//using namespace OpenDrive;


struct OpenDriveFile{
    OpenDrive::RoadHeader* roadHeader;
    OpenDrive::Lane* lane;
    OpenDrive::RoadLink* preRoadLink;
    OpenDrive::RoadLink* sucRoadLink;
    OpenDrive::LaneSection* section;
    OpenDrive::EnRoadMarkType roadmarkType;
    OpenDrive::EnRoadMarkColor roadmarkColor;
    OpenDrive::Elevation* posElevation;
    OpenDrive::GeoHeader* geoHeader;
};


struct TempOdrInfo{
    OpenDrive::GeoHeader* geoHeader;

};

class GetOpenDrive{

private:
    //OpenDrive::OdrManager p_manager;



public:
    GetOpenDrive();   //构造函数

    bool loadFile(string xodrFilepath);

    OpenDrive::RoadHeader* GetRoadHeader(OpenDriveFile &openDriveFile);




    //注意此处有无后患
    XMLDocument *Document = new XMLDocument();


};

#endif //ODRPARSER_PUREODRPARSE_H