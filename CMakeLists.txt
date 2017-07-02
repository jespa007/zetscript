cmake_minimum_required(VERSION 2.8)
 
project(libzetscript)



#--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#
# COMMON PARAMS
#

include_directories (
	${PROJECT_SOURCE_DIR}
	
)

if(MSVC)

	add_definitions(/bigobj)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -DZETSCRIPT_EXPORTS)

	#disable C4103 warning because is valid. Only MSVC complains about it
	add_definitions(/wd4103)

else()

	SET( COMMON_PARAMS " ${COMMON_PARAMS} -D__STDC_CONSTANT_MACROS -std=c++1y -I -pthread -Wall")
   
	if (MINGW)

		set(CMAKE_LEGACY_MINGW_WIN32 0)
		set(CMAKE_PREFIX_PATH "$ENV{MINGWDIR}/local32/lib")

		MESSAGE ("include" ${include_directories})
 
		SET( COMMON_PARAMS " ${COMMON_PARAMS} -I/local32/include -I/usr/local/include ")
  
	endif()

	include_directories (
		/usr/local/include

	)

	LINK_DIRECTORIES (
		/usr/local/lib
		/loca32/lib
	)


endif()




#
# COMMON PARAMS
#
#--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#
# RELEASE PARAMS
#


if(MSVC)

else()

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


	# Compile for x86 target by default ...
	set(TARGET_FLAGS " ${TARGET_FLAGS} -m32 ")


	if ( "${EMMSCRIPTEN}" STREQUAL "yes" )

		SET( EMMSCRIPTEN_DIR "/opt/emsdk/emscripten/master")

		MESSAGE ("********* ¡¡ EMMSCRIPTEN RELEASE !!! ************" )
		set(CMAKE_C_COMPILER "${EMMSCRIPTEN_DIR}/emcc")
		set(CMAKE_CXX_COMPILER "${EMMSCRIPTEN_DIR}/em++")

		SET( COMMON_PARAMS " ${COMMON_PARAMS} -I${EMMSCRIPTEN_DIR}/system/lib/libcxxabi/include/ ")
	
	endif()

	
	SET(CMAKE_C_FLAGS "  -I/local32/include ")
	SET(CMAKE_CXX_FLAGS   " ${TARGET_FLAGS} ${COMMON_PARAMS}  " )
	
	
	MESSAGE ("Target : " ${TARGET})

	#Redefine output dir ...
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${TARGET})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${TARGET})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${TARGET})
	LINK_DIRECTORIES(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
	
endif()




#
# RELEASE PARAMS
#
#--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


# Select flags.







#----------------------------------------------------------------------------------------------------------------------------------------------------
#
# SOURCE UTILS
#



# Library

set( ZETSCRIPT_LIB_SRCS

    #-- Utils --
	utils/zs_utils.cpp

	
	#-- Script Core --
	core/zs_core.cpp	

	#-- Script Core --
	CZetScript.cpp	

)

# zetscriptlib
add_library( zetscript${POST_NAME} SHARED ${ZETSCRIPT_LIB_SRCS} )
target_link_libraries(zetscript${POST_NAME} )

if ( "${EMMSCRIPTEN}" STREQUAL "yes" )

	add_executable(zs${POST_NAME}.html
		${ZG_BASE_LIB_SRCS}
		${PROJECT_SOURCE_DIR}/main.cpp
	)

 	
else()

	#zs console
	add_executable(zs${POST_NAME}
		${PROJECT_SOURCE_DIR}/zs.cpp
	)
	
	target_link_libraries(zs${POST_NAME} zetscript${POST_NAME} "-Wl,--no-as-needed")
	
endif()


