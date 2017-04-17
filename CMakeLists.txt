cmake_minimum_required(VERSION 3.2)
project(maths)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGLM_FORCE_RADIANS -std=c++14 -ffast-math -Wall -Werror" )

IF (CMAKE_BUILD_TYPE STREQUAL "Release")
    #disable profiling
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNPROFILE" )
ENDIF()

IF (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    IF (WIN32)
        # for profiling
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -gdwarf-2" )
    ENDIF()
ENDIF()

IF (WIN32)
    include_directories(c:/DEV/libs/mingw/glm/)
    include_directories(c:/DEV/gamedev/base/include/)
    include_directories(c:/DEV/gamedev/assets/include/)
ELSE()
    include_directories(/home/grynca/DEV/gamedev/base/include/)
    include_directories(/home/grynca/DEV/gamedev/assets/include/)
ENDIF()

option(USE_SDL2 "Use SDL2" ON)
IF (USE_SDL2)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_SDL2=1" )
    IF (WIN32)
        include_directories(c:/DEV/libs/mingw/SDL2-2.0.4/x86/include/)
        link_directories(c:/DEV/libs/mingw/SDL2-2.0.4/x86/lib)
        include_directories(c:/DEV/libs/mingw/glew-1.13.0/include/)
        link_directories(c:/DEV/libs/mingw/glew-1.13.0/lib/)
        set(LIBS "-lglew32 -lglu32 -lopengl32 -lmingw32 -lSDL2main -lSDL2 -lSDL2_image")
    ELSE()
        set(LIBS "-lGLEW -lGL -lGLU -lSDL2 -lSDL2_image")
    ENDIF()
ENDIF()

include_directories(include/)

set(INC_FILES
        include/maths/glm_incl.h
        include/maths.h
        include/maths/common.h
        include/maths/Angle.h
        include/maths/Angle.inl
        include/maths/Vec2.h
        include/maths/Vec2.inl
        include/maths/Mat3.h
        include/maths/Mat3.inl
        include/maths/Interval.h
        include/maths/Interval.inl
        include/maths/shapes/Shape.h
        include/maths/shapes/Shape.inl
        include/maths/shapes/Shape_internal.h
        include/maths/shapes/Circle.h
        include/maths/shapes/Circle.inl
        include/maths/Transform.h
        include/maths/Transform.inl
        include/maths/shapes/OverlapInfo.h
        include/maths/shapes/OverlapInfo.inl
        include/maths/shapes/ARect.h
        include/maths/shapes/ARect.inl
        include/maths/shapes/ShapeTypes.h
        include/maths/shapes/Rect.h
        include/maths/shapes/Rect.inl
        include/maths/shapes/Ray.h
        include/maths/shapes/Ray.inl
        include/maths/shapes/Pgon.h
        include/maths/shapes/Pgon.inl
        include/maths/shapes/NoShape.h)
set(SOURCE_FILES
        test/main.cpp
        test/test_polygons.h)

add_executable(maths ${SOURCE_FILES} ${INC_FILES} )
target_link_libraries(maths ${LIBS})
