cmake_minimum_required(VERSION 2.8)
 
project(zetscript)

#--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#
# MACRO UTILS
#


MACRO(INSTALL_HEADERS_WITH_DIRECTORY HEADER_LIST)

    FOREACH(HEADER ${${HEADER_LIST}})
        get_filename_component(DIR ${HEADER} DIRECTORY)
        INSTALL(FILES ${HEADER} DESTINATION include/zetscript/${DIR})
    ENDFOREACH(HEADER)

ENDMACRO(INSTALL_HEADERS_WITH_DIRECTORY)

#--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#
# COMMON PARAMS
#

include_directories (
	${PROJECT_SOURCE_DIR}
	
)

message("dest dir:" ${CMAKE_INSTALL_PREFIX})

if(MSVC)
	MESSAGE ("********* MSVC ************" )
	
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -DZETSCRIPT_EXPORTS)

	#disable C4103 warning because is perfectly valid. Only MSVC complains about it
	add_definitions(/wd4103)
	add_definitions(/wd4800)
	add_definitions(/wd4244)

else()

	SET( COMMON_PARAMS " ${COMMON_PARAMS} -D__STDC_CONSTANT_MACROS -std=c++1y -I -Wall")
   
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


IF ( "${TARGET}" STREQUAL "")
   SET ( TARGET "release" )
ENDIF ()
 

if(MSVC)
	IF ( "${TARGET}" STREQUAL "debug")
		add_definitions(/bigobj)
	ENDIF()


else()


	set(TARGET_FLAGS " -O2 ")
	set( POST_NAME "")

	# -D_GLIBCXX_DEBUG

	IF ( "${TARGET}" STREQUAL "debug")
		set( POST_NAME "_d")
		set(TARGET_FLAGS " -O0 ")
		set(TARGET_FLAGS " ${TARGET_FLAGS} -g -fomit-frame-pointer -D__ZETSCRIPT_DEBUG__ ")
	ENDIF ()

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
	
	#-- Registered Class --
	extra/zs_extra.cpp	

	#-- Script Core --
	CZetScript.cpp	

)

# zetscriptlib
add_library( zetscript${POST_NAME} SHARED ${ZETSCRIPT_LIB_SRCS} )
target_link_libraries(zetscript${POST_NAME} )

SET(
	HS
	utils/CIO_Utils.h
	utils/CColorTerm.h
	utils/zs_utils.h
	utils/CStringUtils.h
	utils/CLog.h
	core/CASTNode.h
	core/var/CStringScriptVariable.h
	core/var/CVectorScriptVariable.h
	core/var/CFunctorScriptVariable.h
	core/var/CStructScriptVariable.h
	core/var/zs_variable.h
	core/var/CScriptVariable.h
	core/CCompiler.h
	helpers/RegisterFunctionHelper.h
	helpers/CppBindingHelper.h
	core/Common.h
	core/CState.h
	core/CScope.h
	core/CScriptClass.h
	core/vm/CVirtualMachine.h
	core/CNativeFunction.h
	core/CScriptFunctionObject.h
	core/zs_core.h
	extra/zs_extra.h
	extra/CJSON.h
	CZetScript.h
)



INSTALL_HEADERS_WITH_DIRECTORY(HS)


#zs console
add_executable(zs${POST_NAME}
	${PROJECT_SOURCE_DIR}/zs.cpp
)

# zs_test
add_executable(zs_test${POST_NAME}
	${PROJECT_SOURCE_DIR}/test/zs_test.cpp
)

target_link_libraries(zs_test${POST_NAME} zetscript${POST_NAME})

# helloworld
add_executable(helloworld${POST_NAME}
	${PROJECT_SOURCE_DIR}/test/helloworld.cpp
)
target_link_libraries(helloworld${POST_NAME} zetscript${POST_NAME})

# register_variables
add_executable(register_variable${POST_NAME}
	${PROJECT_SOURCE_DIR}/test/register_variable.cpp
)	

target_link_libraries(register_variable${POST_NAME} zetscript${POST_NAME})

# register_functions
add_executable(register_function${POST_NAME}
	${PROJECT_SOURCE_DIR}/test/register_function.cpp
)	

target_link_libraries(register_function${POST_NAME} zetscript${POST_NAME})	


# register_class
add_executable(register_class${POST_NAME}
	${PROJECT_SOURCE_DIR}/test/register_class.cpp
)	

target_link_libraries(register_class${POST_NAME} zetscript${POST_NAME})

# function_script_call
add_executable(script_call_from_cpp${POST_NAME}
	${PROJECT_SOURCE_DIR}/test/script_call_from_cpp.cpp
)	

target_link_libraries(script_call_from_cpp${POST_NAME} zetscript${POST_NAME})
target_link_libraries(zs${POST_NAME} zetscript${POST_NAME})



INSTALL(TARGETS zetscript${POST_NAME} zs${POST_NAME}
		ARCHIVE DESTINATION  	${CMAKE_INSTALL_PREFIX}/lib
		RUNTIME DESTINATION  	${CMAKE_INSTALL_PREFIX}/bin
    	LIBRARY DESTINATION 	${CMAKE_INSTALL_PREFIX}/lib
    	PUBLIC_HEADER DESTINATION	${CMAKE_INSTALL_PREFIX}/includes
)
