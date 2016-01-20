

cmake_minimum_required(VERSION 2.8)
project(tunify)

# Select flags.
SET(CMAKE_CXX_FLAGS "-Wall -fno-rtti -std=c++0x")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -fno-rtti -std=c++0x")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -fno-rtti -std=c++0x")
SET(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g -fno-rtti -std=c++0x -D__DEBUG__")

IF ( "${TARGET}" STREQUAL "")
   SET ( TARGET "debug" )
ENDIF ( "${TARGET}" STREQUAL "")  

set(CMAKE_CSS_FLAGS "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
IF ( "${TARGET}" STREQUAL "debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
ENDIF ( "${TARGET}" STREQUAL "debug")

MESSAGE ("Target : "${TARGET})

#Redefine output dir ...
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../bin/${TARGET})
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../bin/${TARGET})
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../bin/${TARGET})
LINK_DIRECTORIES(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

#tunify executable ...

find_package(Qt4 REQUIRED)
include_directories(
# ${PROJECT_SOURCE_DIR}/../engine
/usr/local/qwt-6.1.0/include
${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR}
)

set(japp_tunify_SRCS 
# ${PROJECT_SOURCE_DIR}/CTreeView.cpp
# ${PROJECT_SOURCE_DIR}/CResourceViewer.cpp
# ${PROJECT_SOURCE_DIR}/QGraphPlot.cpp
# ${PROJECT_SOURCE_DIR}/canvaspicker.cpp
${PROJECT_SOURCE_DIR}/CMainWindow.cpp
${PROJECT_SOURCE_DIR}/CGraphViewer.cpp
${PROJECT_SOURCE_DIR}/main.cpp
)
qt4_automoc(${japp_tunify_SRCS})


add_executable(tunify 

${japp_tunify_SRCS}
#${PROJECT_SOURCE_DIR}/src/apps/gamebox/machine/cdemo.cpp
)



target_link_libraries( tunify  SDL SDL_mixer ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} )


