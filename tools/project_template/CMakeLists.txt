cmake_minimum_required(VERSION 2.8)

project(jeme_video)


  SET( COMMON_PARAMS " ")
  
#SET( COMMON_PARAMS "${COMMON_PARAMS} -D__USE_CHAISCRIPT__")


include_directories (
	/usr/local/include/freetype2
	/usr/local/include
	${OPENGL_INCLUDE_DIRS}  
	${GLU_INCLUDE_DIRS}
)


LINK_DIRECTORIES (
	/usr/local/lib
	/loca32/lib
)



# MINGW support
if (MINGW)
  set(CMAKE_LEGACY_MINGW_WIN32 0)
  set(CMAKE_PREFIX_PATH "c:/apps/mingw/local32/lib")
  find_package(OpenGL REQUIRED)
	 find_library(SDL2_LIB  NAMES SDL2  )
	 find_library(SDL2_NET_LIB  NAMES SDL2_net  )
	 find_library(SDL2_MAIN_LIB  NAMES SDL2main  )
	 
	MESSAGE ("SDL2 " ${SDL2_LIB})
	
	MESSAGE ("include" ${include_directories})
	 
  #find_package(GLU REQUIRED)
  # MINGW does not yet support C++11's concurrency features
  option(MULTITHREAD_SUPPORT_ENABLED "Multithreaded Support Enabled" FALSE)
  
  SET( EXTRA_PARAMS "-I/local32/include -I/local32/include/freetype2 -I/usr/local/include -I/usr/local/include/freetype2")
  
  
else()
  option(MULTITHREAD_SUPPORT_ENABLED "Multithreaded Support Enabled" TRUE)
endif()

# Select flags.
#SET(CMAKE_CXX_FLAGS "-Wall -fno-rtti -std=c++0x")

SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -D__STDC_CONSTANT_MACROS -pthread -std=gnu++0x ${EXTRA_PARAMS} ${COMMON_PARAMS}" )
SET(CMAKE_CXX_FLAGS_RELEASE "-O3  -D__STDC_CONSTANT_MACROS -pthread -std=gnu++0x ${EXTRA_PARAMS} ${COMMON_PARAMS}")
SET(CMAKE_CXX_FLAGS_DEBUG   "-O0  -fomit-frame-pointer -D__STDC_CONSTANT_MACROS -pthread -g  -std=gnu++0x -D__DEBUG__  ${EXTRA_PARAMS} ${COMMON_PARAMS}" )



IF ( "${TARGET}" STREQUAL "")
   SET ( TARGET "debug" )
ENDIF ( "${TARGET}" STREQUAL "")  

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
IF ( "${TARGET}" STREQUAL "debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
	
	
ENDIF ( "${TARGET}" STREQUAL "debug")

MESSAGE ("Target : " ${TARGET})

#Redefine output dir ...
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../bin/${TARGET})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../bin/${TARGET})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../bin/${TARGET})
LINK_DIRECTORIES(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})


#----------------------------------------------------------------------------------------------------------------------------------------------------
#
# SOURCE ENGINE
#

 
set( JEME_VIDEO_LIB_SRCS
    
	jeme_video.cpp
	
)

add_library( jeme_video SHARED ${jeme_LIB_SRCS} )

add_executable(jeme_player 
	${PROJECT_SOURCE_DIR}/jeme_player.cpp
)



IF (MINGW)

	target_link_libraries(jeme_video  mingw32 ${SDL2_MAIN_LIB}  ${SDL2_LIB}  opengl32   "-Wl,--no-as-needed" )
	

ELSE(MINGW)

  #target_link_libraries(jeme_player  jeme_video jpeg SDL2  SDL2_gfx SDL2_image SDL2_mixer SDL2_net GL GLU freetype glttf avformat avcodec avutil swscale mp3lame z gif xmp readline dl uuid "-Wl,--no-as-needed" )
	# target_link_libraries(jeme jpeg SDL2  SDL2_gfx SDL2_image SDL2_mixer GL GLU glttf freetype  avformat avcodec avutil swscale mp3lame z gif xmp readline dl  )

ENDIF(MINGW)

