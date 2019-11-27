cmake_minimum_required(VERSION 3.15)
project(LeapDroneController)

set(CMAKE_CXX_STANDARD 14)
set(THREADING_LINKER_FLAG "-lpthread  -pthread")

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${THREADING_LINKER_FLAG}")


## Add leap motion library
include_directories(LeapSDK/include)
link_directories(LeapSDK/lib/x64/)


 add_executable(LeapDroneController src/main.cpp src/process/process.cpp src/process/process.h src/main.cpp)
 target_link_libraries(LeapDroneController Leap)


# Test Cases
add_executable(TestProcessClass
        src/process/process.cpp
        src/process/main.cpp)


add_executable(TestConfigurationClass
        src/configuration/config.h
        src/configuration/parser.cpp
        src/configuration/main.cpp)


add_executable(
        TestMamboClient
        src/configuration/parser.cpp
        src/droneclient/mamboclient.cpp
        src/process/process.cpp
        src/droneclient/main.cpp)