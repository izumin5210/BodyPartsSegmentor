cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(BodyPartsSegmentor)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CINDER_VERBOSE ON)

if(APPLE)
  include_directories(/usr/local/include/ni2)
  set(OpenCV_DIR /usr/local/opt/opencv3/share/OpenCV)
  set(Qt5_DIR /usr/local/opt/qt5/lib/cmake/Qt5)
elseif(LINUX)
  include_directories(/usr/include/openni2)
endif()

find_package        (CUDA QUIET)
include_directories (/usr/local/cuda/include)

find_package(PCL 1.8 REQUIRED)
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(OpenCV 3.0 REQUIRED)
find_package(Boost COMPONENTS system filesystem program_options thread REQUIRED)

set(NITE2 TRUE)
if(NITE2)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_NITE2")
  set (NiTE2_LIBRARIES "NiTE2")
  include_directories (/usr/local/include/nite2)
  link_directories(/usr/local/lib/ni2)
endif()

get_filename_component(CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../cinder/Cinder" ABSOLUTE)
get_filename_component(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)

include_directories("${PROJECT_DIR}/blocks/OpenCV3/include")

set(PROJECT_SOURCES
  ${PROJECT_DIR}/src/BodyPartsSegmentorApp.cpp
  ${PROJECT_DIR}/src/SensorDevice.cpp
  ${CINDER_IMGUI_SOURCES}
)

include("${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake")

ci_make_app(
    SOURCES     ${PROJECT_SOURCES}
    INCLUDES    ${PROJECT_DIR}/include ${PCL_INCLUDE_DIRS}
    CINDER_PATH ${CINDER_PATH}
    LIBRARIES   ${Boost_LIBRARIES} ${PCL_LIBRARIES} ${OpenCV_LIBS} freenect2 ${NiTE2_LIBRARIES} yaml-cpp
)

