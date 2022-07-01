//
// Created by JinxBIGBIG on 2022/4/21.
//
#include "OdrManager.hh"
#include "BaseNodes/OdrNode.hh"
#include "BaseNodes/OdrRoadHeader.hh"
#include "BaseNodes/OdrLane.hh"
#include "BaseNodes/OdrLaneSection.hh"
#include <iostream>
#include <pureOdrParse.h>
#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;

GetOpenDrive::GetOpenDrive() {}

bool GetOpenDrive::loadFile(string xodrFilepath){
    if(Document->LoadFile(xodrFilepath.c_str()) != XML_SUCCESS)
    {
        cout << "Loading xodr file failed." << endl;
        return false;
    } else{
        cout << "Loading xodr file succeed." << endl;
        return true;
    }
}

OpenDrive::RoadHeader* GetOpenDrive::GetRoadHeader(OpenDriveFile &openDriveFile) {


    XMLElement *RootElement = Document->RootElement();
    XMLElement *RoadElement = RootElement->FirstChildElement("road");
    cout << RoadElement << endl;
    int iRoadNum = 0;

    for (RoadElement; RoadElement->NextSiblingElement() != NULL; RoadElement=RoadElement->NextSiblingElement())
    {
        if ((RoadElement->Attribute("id")!=NULL)&(RoadElement->Attribute("length")!=NULL))
        {
            iRoadNum++;
           /* cout << "Road ID is " << RoadElement->Attribute("id") << endl;
            cout << "Road length is " << RoadElement->Attribute("length") << endl;*/
           openDriveFile.roadHeader->mName = RoadElement->Attribute("name");
           openDriveFile.roadHeader->mId = RoadElement->IntAttribute("id");
           cout << "mId" << endl;

        }
    }

    vector<OpenDrive::RoadHeader> roadHeaders(iRoadNum);
    cout << "Road Number is" << iRoadNum << endl;
    iRoadNum = 0;
    return openDriveFile.roadHeader;
}










