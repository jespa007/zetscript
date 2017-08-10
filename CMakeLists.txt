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
	MESSAGE ("********* MSVC ************" )
	
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -DZETSCRIPT_EXPORTS)

	#disable C4103 warning because is perfectly valid. Only MSVC complains about it
	add_definitions(/wd4103)

else()

	SET( COMMON_PARAMS " ${COMMON_PARAMS} -D__STDC_CONSTANT_MACROS -std=c++1y -I -pthread -Wall")
   
	if (MINGW)
		MESSAGE ("********* MINGW CONF ************" )
		message("mingw path: $ENV{MINGWDIR} ")
		
		set(CMAKE_LEGACY_MINGW_WIN32 0)
		set(CMAKE_PREFIX_PATH "$ENV{MINGWDIR}/local32/lib")

		MESSAGE ("include" ${include_directories})
 
		SET( COMMON_PARAMS " ${COMMON_PARAMS} ")
  
	endif()

	include_directories (
		$ENV{MINGWDIR}/local32/include
	)

	LINK_DIRECTORIES (
		$ENV{MINGWDIR}/local32/lib
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
	IF ( "${TARGET}" STREQUAL "debug")
		add_definitions(/bigobj)
	ENDIF()


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
	set(TARGET_FLAGS " ${TARGET_FLAGS} ")


	if ( "${EMSCRIPTEN}" STREQUAL "yes" )

		message("emscripten path: $ENV{EMSCRIPTENDIR} ")
		

		MESSAGE ("********* EMSCRIPTEN CONF ************" )
		set(CMAKE_C_COMPILER "$ENV{EMSCRIPTENDIR}/emcc")
		set(CMAKE_CXX_COMPILER "$ENV{EMSCRIPTENDIR}/em++")

		SET( COMMON_PARAMS " ${COMMON_PARAMS} -I$ENV{EMSCRIPTENDIR}/system/lib/libcxxabi/include/ ")
		
		set( POST_NAME ${POST_NAME}.html)
	
	endif()

	
	SET(CMAKE_C_FLAGS " ")
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
add_library( zetscript${POST_NAME} STATIC ${ZETSCRIPT_LIB_SRCS} )
target_link_libraries(zetscript${POST_NAME} )

if ( "${EMSCRIPTEN}" STREQUAL "yes" )

	add_executable(zs${POST_NAME}
		${PROJECT_SOURCE_DIR}/zs.cpp
	)

 	
else()

	#zs console
	add_executable(zs${POST_NAME}
		${PROJECT_SOURCE_DIR}/zs.cpp
	)
	
	# zs_test
	add_executable(zs_test${POST_NAME}
		${PROJECT_SOURCE_DIR}/zs_test.cpp
	)
	
	
	add_executable(engine${POST_NAME}
		${PROJECT_SOURCE_DIR}/demo/engine/main.cpp
		${PROJECT_SOURCE_DIR}/demo/engine/CRender.cpp
		${PROJECT_SOURCE_DIR}/demo/engine/CInput.cpp
		${PROJECT_SOURCE_DIR}/demo/engine/CImage.cpp
		${PROJECT_SOURCE_DIR}/demo/engine/CSprite.cpp
		${PROJECT_SOURCE_DIR}/demo/engine/CFont.cpp
		${PROJECT_SOURCE_DIR}/demo/engine/CSound.cpp
		${PROJECT_SOURCE_DIR}/demo/engine/CSoundPlayer.cpp
	)
	
	configure_file(${PROJECT_SOURCE_DIR}/demo/engine/invader.zs ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/invader.zs COPYONLY)
	configure_file(${PROJECT_SOURCE_DIR}/demo/engine/font.bmp ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/font.bmp COPYONLY)
	configure_file(${PROJECT_SOURCE_DIR}/demo/engine/laser.wav ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/laser.wav COPYONLY)
	
	
	
	target_link_libraries(zs_test${POST_NAME} zetscript${POST_NAME})
	
	if (MINGW)
		find_library(SDL2_LIB  NAMES SDL2  )
		target_link_libraries(engine${POST_NAME} mingw32 zetscript${POST_NAME} SDL2main SDL2)
	else()
		target_link_libraries(engine${POST_NAME} zetscript${POST_NAME} SDL2main SDL2)
	endif()
	
endif()

target_link_libraries(zs${POST_NAME} zetscript${POST_NAME})



