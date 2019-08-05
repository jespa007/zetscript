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

file(GLOB_RECURSE INCS "*.h")

message("-- Dest.dir: " ${CMAKE_INSTALL_PREFIX})

set(ZETSCRIPT_LIB_SRCS "")

if(MSVC)
	MESSAGE ("-- Plataform: MSVC" )
	
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)

	#disable C4103 warning because is perfectly valid. Only MSVC complains about it
	add_definitions(/wd4103)
	add_definitions(/wd4800)
	add_definitions(/wd4244)

else()

	SET( COMMON_PARAMS " ${COMMON_PARAMS} -D__STDC_CONSTANT_MACROS -std=c++1y -I -Wall")
   
	if (MINGW)
		MESSAGE("-- Plataform: MINGW" )
		message("-- Mingw path: $ENV{MINGWDIR} ")
		
		set(CMAKE_LEGACY_MINGW_WIN32 0)
		set(CMAKE_PREFIX_PATH "$ENV{MINGWDIR}/local32/lib")
 
		SET( COMMON_PARAMS " ${COMMON_PARAMS} ")
  
	elseif(GNU)
    	MESSAGE ("-- Plataform: GNU" )
    else()
    	MESSAGE ("-- Plataform: Unknown" )
	endif()
	
	IF (MEMMANAGER)
		message("-- Memmanager: ON (Warning execution time could be slow)")
		SET( COMMON_PARAMS " ${COMMON_PARAMS} -D__MEMMANAGER__")
		include_directories (
			../memmanager
		)
		
	   	set(ZETSCRIPT_LIB_SRCS ${ZETSCRIPT_LIB_SRCS} ../memmanager/MEM_ram.cpp)
		
	ELSE()
		message("-- Memmanager: OFF")
	ENDIF()
	

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

set( POST_NAME "")
IF ( "${TARGET}" STREQUAL "debug")
	set( POST_NAME "_d")
ENDIF()
 
MESSAGE ("-- Target : " ${TARGET})

IF ( "${TARGET}" STREQUAL "debug")
	SET( COMMON_PARAMS " ${COMMON_PARAMS} -D__ZETSCRIPT_DEBUG__ ")
ENDIF()


if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")

	MESSAGE ("-- Toolchain : GNU")

	set(TARGET_FLAGS " -O2 ")

	IF ( "${TARGET}" STREQUAL "debug")
		set(TARGET_FLAGS " -O0 ")
		set(TARGET_FLAGS " ${TARGET_FLAGS} -g -fomit-frame-pointer ")
	ENDIF ()
	
	IF ( "${32BIT}" STREQUAL "ON")
		MESSAGE ("-- Compile 32bit : ON")
		set(TARGET_FLAGS " ${TARGET_FLAGS} -m32 ")
	ELSE ()
		MESSAGE ("-- Compile 32bit : OFF")
	ENDIF()

	SET(CMAKE_C_FLAGS " ")
	SET(CMAKE_CXX_FLAGS   " ${TARGET_FLAGS} ${COMMON_PARAMS}  " )
	

	#Redefine output dir ...
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${TARGET})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${TARGET})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${TARGET})
	LINK_DIRECTORIES(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
	
endif()


#
# RELEASE PARAMS
#
#----------------------------------------------------------------------------------------------------------------------------------------------------
#
# SOURCE UTILS
#

# Library
IF ( "${FCOMPILE}" STREQUAL "ON")

	MESSAGE ("-- Fast compile: ON (Only generates library | Install process is disabled)" )

	set( ZETSCRIPT_LIB_SRCS ${ZETSCRIPT_LIB_SRCS} ${INCS}
		include_all.cpp
	)
ELSE()

	MESSAGE ("-- Fast compile: OFF" )

	set( ZETSCRIPT_LIB_SRCS ${ZETSCRIPT_LIB_SRCS} ${INCS}
	
		#-- Exception --
		exception.cpp
	
	    #-- Utils --
		util/io.cpp
		util/log.cpp
		util/path.cpp
		util/rtti.cpp
		util/string.cpp
		
		#-- Variables --
		var/CScriptVariable.cpp
	
		var/CStringScriptVariable.cpp
		var/CStructScriptVariable.cpp
		var/CVectorScriptVariable.cpp
		var/CFunctorScriptVariable.cpp
		
		#-- Script Core --
		CEval.cpp
		CScope.cpp
		CScopeFactory.cpp
		CNativeFunctionFactory.cpp
		CScriptClassBase.cpp
		CScriptFunction.cpp
		CScriptFunctionFactory.cpp
		CScriptClass.cpp
		CScriptClassFactory.cpp
		
		CVirtualMachine.cpp
		
	
		#-- Script Core --
		CZetScript.cpp	
	
	)
ENDIF()

# zetscriptlib
add_library( zetscript${POST_NAME} STATIC ${ZETSCRIPT_LIB_SRCS} )
if(MSVC)
	#only for msvc...
	set_target_properties(zetscript${POST_NAME} PROPERTIES COMPILE_DEFINITIONS ZETSCRIPT_EXPORTS)
endif()



IF ( "${FCOMPILE}" STREQUAL "ON")
	# helloworld
	add_executable(helloworld${POST_NAME}
		${PROJECT_SOURCE_DIR}/test/helloworld.cpp
	)
	target_link_libraries(helloworld${POST_NAME} zetscript${POST_NAME})
ELSE()

	SET(
		HS
		
		CEval.h
		
		var/CStringScriptVariable.h
		var/CVectorScriptVariable.h
		var/CFunctorScriptVariable.h
		var/CStructScriptVariable.h
		var/CScriptVariable.h
		
		helper/register_function.h
		helper/cpp_binding.h
		Common.h
		CScope.h
		CScopeFactory.h
		CScriptClassBase.h
		CScriptClass.h
		CScriptClassFactory.h
		CScriptFunction.h
		CScriptFunctionFactory.h
		CVirtualMachine.h
		CNativeFunctionFactory.h
		
		CZetScript.h
		zetscript_exception.h
		
		CVirtualMachine.inc
		CZetScript.inc
		CScriptClassFactory.inc
	)
	
	
	
	INSTALL_HEADERS_WITH_DIRECTORY(HS)

	#zs console
	add_executable(zs${POST_NAME}
		${PROJECT_SOURCE_DIR}/zs.cpp
	)
	
	target_link_libraries(zs${POST_NAME} zetscript${POST_NAME})
	
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
	
	
	# register_constants
	add_executable(register_constant${POST_NAME}
		${PROJECT_SOURCE_DIR}/test/register_constant.cpp
	)
	
	target_link_libraries(register_constant${POST_NAME} zetscript${POST_NAME})
	
	INSTALL(TARGETS zetscript${POST_NAME} zs${POST_NAME}
		ARCHIVE DESTINATION  	${CMAKE_INSTALL_PREFIX}/lib
		RUNTIME DESTINATION  	${CMAKE_INSTALL_PREFIX}/bin
    	LIBRARY DESTINATION 	${CMAKE_INSTALL_PREFIX}/lib
    	PUBLIC_HEADER DESTINATION	${CMAKE_INSTALL_PREFIX}/includes
	)

ENDIF()


