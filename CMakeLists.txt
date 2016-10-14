cmake_minimum_required(VERSION 3.2)
project(maths)

option(VISUAL_UPDATE "Visual Updating" OFF)

IF (VISUAL_UPDATE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVISUAL_UPDATE" )
ENDIF()


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -std=gnu++0x -ffast-math" )

include_directories(include/)
IF(WIN32)
    # for profiling
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fno-omit-frame-pointer -gdwarf-2" )

    include_directories(c:/DEV/gamedev/base/src/)
    include_directories(c:/DEV/libs/mingw/glm/)

    IF (VISUAL_UPDATE)
        include_directories(c:/DEV/gamedev/assets/include/)
        include_directories(c:/DEV/libs/mingw/SDL2-2.0.4/x86/include/)
        link_directories(c:/DEV/libs/mingw/SDL2-2.0.4/x86/lib)
        include_directories(c:/DEV/libs/mingw/glew-1.13.0/include/)
        link_directories(c:/DEV/libs/mingw/glew-1.13.0/lib/)

        set(LIBS "-lglew32 -lglu32 -lopengl32 -lmingw32 -lSDL2main -lSDL2 -lSDL2_image")
    ENDIF()
ELSE()
   include_directories(/home/grynca/DEV/gamedev/base/src/)
   set(LIBS "-lGLEW -lGL -lGLU -lfreetype -lSDL2 -lSDL2_image")
ENDIF(WIN32)

set(INC_FILES
        include/maths.h
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
        )
set(SOURCE_FILES
        src/main.cpp
        src/test_gon.h src/test_polygons.h)

add_executable(maths ${SOURCE_FILES} ${INC_FILES} )
target_link_libraries(maths ${LIBS})
