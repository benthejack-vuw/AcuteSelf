cmake_minimum_required(VERSION 3.12.0)
project(AcuteSelf)

find_package(OpenCV CONFIG REQUIRED)

#get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../Cinder" ABSOLUTE)
get_filename_component( APP_PATH    "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
set(CINDER_PATH "" CACHE PATH "Location of cinder folder")

message(STATUS "Cinder path: ${CINDER_PATH} App path: ${APP_PATH}" )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

set( ACUTESELF_TARGET "AcuteSelf" )

set( ACUTESELF_SOURCES
    ${APP_PATH}/src/AcuteMesh.cpp
    ${APP_PATH}/src/AcuteSelfApp.cpp
    ${APP_PATH}/src/BackgroundPlane.cpp
    ${APP_PATH}/src/BlurShadow.cpp
    ${APP_PATH}/src/FrameSubtractor.cpp
    ${APP_PATH}/src/MeshCollection.cpp
    ${APP_PATH}/src/PointDetector.cpp
)

set( ACUTESELF_RESOURCES
    ${APP_PATH}/assets/background.frag
    ${APP_PATH}/assets/background.vert
    ${APP_PATH}/assets/blurFrag.frag
    ${APP_PATH}/assets/emptyVert.vert
    ${APP_PATH}/assets/inverseAlpha.frag
    ${APP_PATH}/assets/mainFrag.frag
    ${APP_PATH}/assets/mainVert.vert
)

set ( INCLUDE_PATHS
      ${APP_PATH}/include
)

ci_make_app(
    APP_NAME    ${ACUTESELF_TARGET}
    CINDER_PATH ${CINDER_PATH}
    SOURCES     ${ACUTESELF_SOURCES}
    INCLUDES    ${INCLUDE_PATHS}
    RESOURCES   ${ACUTESELF_RESOURCES}
  )

target_include_directories(${ACUTESELF_TARGET} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${ACUTESELF_TARGET} ${OpenCV_LIBS})
