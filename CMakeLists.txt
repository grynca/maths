cmake_minimum_required(VERSION 3.2)
project(maths)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -ffast-math" )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGLM_FORCE_RADIANS -DGLM_PRECISION_MEDIUMP_FLOAT" )

IF (CMAKE_BUILD_TYPE STREQUAL "Release")
    #disable profiling
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNPROFILE -Wall -Werror -Wno-strict-aliasing" )
ENDIF()

IF (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    IF (WIN32)
        # for profiling
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -gdwarf-2" )
    ENDIF()
ENDIF()

IF (WIN32)
    include_directories(c:/DEV/ENV/msys64/mingw64/include/)
    link_directories(c:/DEV/ENV/msys64/mingw64/lib/)
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
        set(LIBS "-lglew32 -lglu32 -lopengl32 -lmingw32 -lSDL2main -lSDL2 -lSDL2_image")
    ELSE()
        set(LIBS "-lGLEW -lGL -lGLU -lSDL2 -lSDL2_image")
    ENDIF()
ENDIF()

include_directories(include/)

set(INC_FILES
        include/maths.h
        include/maths/glm_incl.h
        include/maths/maths_config.h
        include/maths/common.h
        include/maths/maths_funcs.h
        include/maths/Angle.h
        include/maths/Angle.inl
        include/maths/Vec2.h
        include/maths/Vec2.inl
        include/maths/Mat3.h
        include/maths/Mat3.inl
        include/maths/Interval.h
        include/maths/Interval.inl
        include/maths/shapes/shapes_fw.h
        include/maths/shapes/shapes_h.h
        include/maths/shapes/ClipVertex.h
        include/maths/shapes/OverlapTmp.h
        include/maths/shapes/ContactManifold.h
        include/maths/shapes/ShapeTypes.h
        include/maths/shapes/Shape.h
        include/maths/shapes/Shape.inl
        include/maths/shapes/OverlapHelper.h
        include/maths/shapes/OverlapHelper.inl
        include/maths/shapes/Circle.h
        include/maths/shapes/Circle.inl
        include/maths/Transform.h
        include/maths/Transform.inl
        include/maths/shapes/ARect.h
        include/maths/shapes/ARect.inl
        include/maths/shapes/Rect.h
        include/maths/shapes/Rect.inl
        include/maths/shapes/Ray.h
        include/maths/shapes/Ray.inl
        include/maths/shapes/Pgon.h
        include/maths/shapes/Pgon.inl
        include/maths/shapes/GJK.h
        include/maths/shapes/GJK.inl
        include/maths/debug_draw.h
        include/maths/debug_draw.inl
        )
set(SOURCE_FILES
        test/main.cpp
        test/test_polygons.h
        test/test_overlaps.h)

add_executable(maths ${SOURCE_FILES} ${INC_FILES} )
target_link_libraries(maths ${LIBS})
