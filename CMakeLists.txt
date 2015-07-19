cmake_minimum_required(VERSION 3.2)
project(maths)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -std=gnu++0x" )
# for profiling
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fno-omit-frame-pointer -gdwarf-2" )

include_directories(c:/DEV/libs/mingw/glm/)
include_directories(c:/DEV/gamedev/Base/src/)
include_directories(src/)

set(SOURCE_FILES main.cpp src/maths/Angle.h src/maths.h)

add_executable(maths ${SOURCE_FILES} src/maths/Vec2.h src/maths/Mat3.h src/maths/Interval.h src/maths/shapes/Shape.h src/maths/shapes/Circle.h src/maths/Frame.h src/maths/shapes/OverlapInfo.h src/maths/shapes/ARect.h src/maths/shapes/Shape_internal.h src/maths/shapes/ShapeTypes.h src/maths/shapes/Rect.h)
