cmake_minimum_required(VERSION 2.8)
 
project(zg_base)



#--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#
# COMMON PARAMS
#
  SET( COMMON_PARAMS " -D__STDC_CONSTANT_MACROS -std=c++1y -I${PROJECT_SOURCE_DIR} -pthread -Wall")
   
if (MINGW)
	set(CMAKE_LEGACY_MINGW_WIN32 0)
	set(CMAKE_PREFIX_PATH "c:/apps/mingw/local32/lib")

	
	MESSAGE ("include" ${include_directories})
 
	#find_package(GLU REQUIRED)
	SET( COMMON_PARAMS " ${COMMON_PARAMS} -I/local32/include -I/usr/local/include ")
	
	  
else()
  option(MULTITHREAD_SUPPORT_ENABLED "Multithreaded Support Enabled" TRUE)
endif()  
  


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


include_directories (
	/usr/local/include
)


#
# COMMON PARAMS
#
#--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#
# RELEASE PARAMS
#


IF ( "${TARGET}" STREQUAL "")
   SET ( TARGET "release" )
ENDIF () 

set(TARGET_FLAGS " -O3 ")
set( POST_NAME "")

IF ( "${TARGET}" STREQUAL "debug")
	set( POST_NAME "_d")
	set(TARGET_FLAGS " -O0 ")
	set(TARGET_FLAGS " ${TARGET_FLAGS} -g -fomit-frame-pointer -D__DEBUG__ ")
ENDIF ()

if ( "${MEMMANAGER}" STREQUAL "yes" )
	MESSAGE ("********* ¡¡ Warning memmanager enabled. Be care about performance !!! ************" )
	set(TARGET_FLAGS " ${TARGET_FLAGS} -D__MEMMANAGER__ ")
endif()

if ( "${FFMPEG}" STREQUAL "yes" )
	MESSAGE ("-FFMPEG Enabled" )
	set(TARGET_FLAGS " ${TARGET_FLAGS} -D__WITH_FFMPEG__ ")
endif()



#
# RELEASE PARAMS
#
#--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


# Select flags.
SET(CMAKE_CXX_FLAGS   " ${TARGET_FLAGS} ${COMMON_PARAMS}  " )
SET(CMAKE_C_FLAGS " -I/local32/include ")


MESSAGE ("Target : " ${TARGET})

#Redefine output dir ...
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/${TARGET})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/${TARGET})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/${TARGET})
LINK_DIRECTORIES(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})



#----------------------------------------------------------------------------------------------------------------------------------------------------
#
# SOURCE UTILS
#


# Library

set( ZG_BASE_LIB_SRCS
    
    
	#-- System --
	system/zg_system.cpp
	
    #-- Utils --
	utils/zg_utils.cpp

	#-- Object --
	object/zg_object.cpp
    
	
	#-- Script Core --
	script/zg_script.cpp	
	
	#-- Factory --
	factory/zg_factory.cpp

)


add_executable(zg_main${POST_NAME}
	${PROJECT_SOURCE_DIR}/main.cpp
)

add_library( zg_script${POST_NAME} SHARED ${ZG_BASE_LIB_SRCS} )
 


IF (MINGW)

 target_link_libraries(zg_script${POST_NAME} SDL2main SDL2 ${UUID_LIB} re2 Rpcrt4 kernel32 "-Wl,--no-as-needed" )
 target_link_libraries(zg_main${POST_NAME} mingw32 zg_script${POST_NAME} "-Wl,--no-as-needed" )
  
ELSE(MINGW)
	target_link_libraries(zg_script${POST_NAME}  "-Wl,--no-as-needed" )
	target_link_libraries(zg_main${POST_NAME} zg_script${POST_NAME} SDL2 uuid re2 "-Wl,--no-as-needed" )
ENDIF(MINGW)
