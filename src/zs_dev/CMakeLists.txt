cmake_minimum_required(VERSION 2.8)
 
project(zg_base)



#--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#
# COMMON PARAMS
#
  SET( COMMON_PARAMS " -D__STDC_CONSTANT_MACROS -std=c++1y -I${PROJECT_SOURCE_DIR} -I${PROJECT_SOURCE_DIR}/../zetscript -pthread -Wall")
   
if (MINGW)
	set(CMAKE_LEGACY_MINGW_WIN32 0)
	set(CMAKE_PREFIX_PATH "$ENV{MINGWDIR}/local32/lib")

	MESSAGE ("include" ${include_directories})
 
	#find_package(GLU REQUIRED)
	SET( COMMON_PARAMS " ${COMMON_PARAMS} -I/local32/include -I/usr/local/include ")
	
 
  
endif()


include_directories (
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

# -D_GLIBCXX_DEBUG

IF ( "${TARGET}" STREQUAL "debug")
	set( POST_NAME "_d")
	set(TARGET_FLAGS " -O0 ")
	set(TARGET_FLAGS " ${TARGET_FLAGS} -g -fomit-frame-pointer -D__DEBUG__ ")
ENDIF ()

set(TARGET_FLAGS " ${TARGET_FLAGS} -m32 ")

if ( "${MEMMANAGER}" STREQUAL "yes" )
	MESSAGE ("********* ¡¡ Warning memmanager enabled. Be care about performance !!! ************" )
	set(TARGET_FLAGS " ${TARGET_FLAGS} -D__MEMMANAGER__ ")
endif()


if ( "${EMMSCRIPTEN}" STREQUAL "yes" )

	SET( EMMSCRIPTEN_DIR "/opt/emsdk/emscripten/master")

	MESSAGE ("********* ¡¡ EMMSCRIPTEN RELEASE !!! ************" )
	set(CMAKE_C_COMPILER "${EMMSCRIPTEN_DIR}/emcc")
	set(CMAKE_CXX_COMPILER "${EMMSCRIPTEN_DIR}/em++")

	SET( COMMON_PARAMS " ${COMMON_PARAMS} -I${EMMSCRIPTEN_DIR}/system/lib/libcxxabi/include/ ")
	
endif()

SET(CMAKE_CXX_FLAGS   " ${TARGET_FLAGS} ${COMMON_PARAMS}  " )
SET(CMAKE_C_FLAGS " -I/local32/include ")


#
# RELEASE PARAMS
#
#--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


# Select flags.



MESSAGE ("Target : " ${TARGET})

#Redefine output dir ...
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${TARGET})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${TARGET})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${TARGET})
LINK_DIRECTORIES(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})



#----------------------------------------------------------------------------------------------------------------------------------------------------
#
# SOURCE UTILS
#



# Library

set( ZG_BASE_LIB_SRCS
    
    
	#-- System --
	system/zs_system.cpp
	
    #-- Utils --
	../libzetscript/utils/zs_utils.cpp

	
	#-- Script Core --
	../libzetscript/core/zs_core.cpp	

	#-- Script Core --
	../libzetscript/CZetScript.cpp	

	
)


 
if ( "${EMMSCRIPTEN}" STREQUAL "yes" )

	add_executable(zs${POST_NAME}.html
		${ZG_BASE_LIB_SRCS}
		${PROJECT_SOURCE_DIR}/main.cpp
	)

 	#target_link_libraries(zg_main${POST_NAME}.html "-Wl,--no-as-needed" )
 	
else()


	add_library( zetscript_dev${POST_NAME} SHARED ${ZG_BASE_LIB_SRCS} )
	
	add_executable(zs_dev${POST_NAME}
		${PROJECT_SOURCE_DIR}/main.cpp
	)

	IF (MINGW)
	
	 target_link_libraries(zetscript_dev${POST_NAME} Rpcrt4 kernel32 "-Wl,--no-as-needed" )
	 target_link_libraries(zs${POST_NAME} mingw32  zetscript_dev${POST_NAME} "-Wl,--no-as-needed" )
	  
	ELSE(MINGW)
		target_link_libraries(zetscript_dev${POST_NAME}  "-Wl,--no-as-needed" )
		target_link_libraries(zs_dev${POST_NAME} zetscript_dev${POST_NAME} "-Wl,--no-as-needed" )
	ENDIF(MINGW)

endif()