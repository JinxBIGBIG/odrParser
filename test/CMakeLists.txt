
#(get_lane_info getLaneInfo.cpp ../src/positionInfo.cpp)

#set(PROJ "D:/MCworkspace/PROJ/cmake-build-debug/cmake_install")
#find_package(PROJ REQUIRED CONFIG)
#target_link_libraries(MyApp PRIVATE PROJ::proj)

#[[add_executable(get_lane_info getLaneInfo.cpp ../src/positionInfo.cpp ../src/CA/dynamicMessage.cpp ../src/CA/staticMessage.cpp ../include/IStruct.h)
target_link_libraries(get_lane_info ${ODRMANGER_LIB}/libproj.dll.a
         ${ODRMANGER_LIB}/libODrive.1.5.5.a)]]

add_executable(get_ISim ISimTest.cpp ../src/positionInfo.cpp ../src/customParse/ShareMessage.cpp ../src/jinx/DataOperation.cpp)
target_link_libraries(get_ISim ws2_32 ${ODRMANGER_LIB}/libproj_9_1.dll
        ${ODRMANGER_LIB}/libODrive.1.5.5.a)

#[[add_executable(get_V3 getV3.cpp ../src/positionInfo.cpp ../src/customParse/ShareMessage.cpp getV3.cpp)
target_link_libraries(get_V3 ${ODRMANGER_LIB}/libproj_9_1.dll
        ${ODRMANGER_LIB}/libODrive.1.5.5.a)]]

#[[add_executable(get_IEval IEvalTest.cpp ../src/positionInfo.cpp ../src/customParse/ShareMessage.cpp)
target_link_libraries(get_IEval ${ODRMANGER_LIB}/libproj.dll.a
        ${ODRMANGER_LIB}/libODrive.1.5.5.a)]]


#[[add_executable(get_jinx_info ../src/jinx/DataOperation.cpp)
target_link_libraries(get_jinx_info ${ODRMANGER_LIB}/libproj_9_1.dll
        ${ODRMANGER_LIB}/libODrive.1.5.5.a)]]

# add_executable(pos_info getPosInfoClassTest.cpp ../src/posInfo.cpp)
# target_link_libraries(pos_info ${ODRMANGER_LIB})


# add_executable(pure_odrInfo getPureOdrInfo.cpp ../src/pureOdrParse.cpp ../src/tinyxml2.cpp)
# target_link_libraries(pure_odrInfo ${ODRMANGER_LIB})

#[[add_executable(get_udp receiveAndSend.cpp ../src/positionInfo.cpp ../src/customParse/ShareMessage.cpp ../src/jinx/DataOperation.cpp)
target_link_libraries(get_udp ws2_32 ${ODRMANGER_LIB}/libproj_9_1.dll
        ${ODRMANGER_LIB}/libODrive.1.5.5.a)]]

#[[add_executable(get_sendOnly sendServer.cpp ../src/positionInfo.cpp ../src/customParse/ShareMessage.cpp ../src/jinx/DataOperation.cpp)
target_link_libraries(get_sendOnly ws2_32 ${ODRMANGER_LIB}/libproj_9_1.dll
        ${ODRMANGER_LIB}/libODrive.1.5.5.a)]]

#[[add_executable(get_receiveOnly receiveOnly.cpp ../src/positionInfo.cpp ../src/customParse/ShareMessage.cpp ../src/jinx/DataOperation.cpp)
target_link_libraries(get_receiveOnly ws2_32 ${ODRMANGER_LIB}/libproj_9_1.dll
        ${ODRMANGER_LIB}/libODrive.1.5.5.a)]]
