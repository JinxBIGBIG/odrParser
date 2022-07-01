//
// Created by JinxBIGBIG on 2022/4/21.
//
#include "tinyxml2.h"
#include "pureOdrParse.h"
#include <iostream>
using namespace tinyxml2;
using namespace std;

int main(int argc, char** argv) {
    string xodrFilepath = "..\\data\\Germany_2018.xodr";

    OpenDrive::RoadHeader* roadHeader;
    OpenDriveFile openDriveFile;
    GetOpenDrive getOpenDrive;
    bool signal = getOpenDrive.loadFile(xodrFilepath);
    // cout << signal << endl;
    if(signal == false){
        cout << "Please check xodrFile." <<endl;
    } else{
        // roadHeader = getOpenDrive.GetRoadHeader(openDriveFile);


    }

    return 0;
}