cmake_minimum_required(VERSION 3.0)
project(odrParser)

# set(CMAKE_CXX_STANDARD 14)
add_definitions("-std=c++11")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(ODRMANGER_LIB ${PROJECT_SOURCE_DIR}/lib/libProj)
#set(SOURCESLIST ${PROJECT_SOURCE_DIR}../src/positionInfo.cpp ../include/pureOdrParse.h ../src/pureOdrParse.cpp)
#[[
file(GLOB_RECURSE
        sources
        ../src/positionInfo.cpp)
]]

include_directories(OdrManager include)

add_subdirectory(test)
