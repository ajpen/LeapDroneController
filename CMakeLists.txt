cmake_minimum_required(VERSION 3.15)
project(LeapDroneController)

set(CMAKE_CXX_STANDARD 14)

## Add leap motion library
include_directories(LeapSDK/include)
link_directories(LeapSDK/lib/x64/)

## add python development libraries
include_directories("C:\\Users\\anfer\\Anaconda3\\include")
link_directories("C:\\Users\\anfer\\Anaconda3\\libs")

add_executable(LeapDroneController src/main.cpp src/process.cpp src/process.h)
target_link_libraries(LeapDroneController Leap)
