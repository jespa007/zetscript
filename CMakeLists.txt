# CMakeLists.txt
#
# Copyright 2021 by
# Jordi Espada
#
# Say
#
#   cmake CMakeLists.txt
#
# to create a Makefile that builds a static version of the library.  For a
# dynamic library, use
#
#   cmake CMakeLists.txt -DBUILD_SHARED_LIBS:BOOL=true
#
# instead.  Please refer to the cmake manual for further options, in
# particular, how to modify compilation and linking parameters.
#

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

if("${FLOAT_PRECISION}" STREQUAL "DOUBLE")
	set( ZS_FLOAT_TYPE 			"double" )
	MESSAGE("-- Float precision: double"  )
else()
	set( ZS_FLOAT_TYPE		"float" )
	MESSAGE("-- Float precision: single"  )
endif()

configure_file(${PROJECT_SOURCE_DIR}/config.h.in ${PROJECT_SOURCE_DIR}/config.h)


include_directories (
	${PROJECT_SOURCE_DIR}
)

file(GLOB_RECURSE INCS "*.h,*.tcc")


message("-- Install drectory: " ${CMAKE_INSTALL_PREFIX})

set(ZETSCRIPT_LIB_SRCS "")

if(MSVC)
	MESSAGE ("-- Plataform: MSVC" )
	
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)

	#disable C4103 warning because is perfectly valid. Only MSVC complains about it
	add_definitions(/wd4103)
	add_definitions(/wd4800)
	add_definitions(/wd4244)

else()

	SET( COMPILE_COMMON_FLAGS " ${COMPILE_COMMON_FLAGS} -D__STDC_CONSTANT_MACROS -std=gnu++0x -I -Wall -Wextra")
   
	if (MINGW)
		MESSAGE("-- Plataform: MINGW" )
		message("-- Mingw path: $ENV{MINGWDIR} ")
		
		set(CMAKE_LEGACY_MINGW_WIN32 0)
		set(CMAKE_PREFIX_PATH "$ENV{MINGWDIR}/local32/lib")
 
		SET( COMPILE_COMMON_FLAGS " ${COMPILE_COMMON_FLAGS} ")
  
	elseif(UNIX)
    	MESSAGE ("-- Plataform: UNIX" )
    else()
    	MESSAGE ("-- Plataform: Unknown" )
	endif()
	
	IF ( "${TARGET}" STREQUAL "debug")
		message("-- Memmanager: ON (Warning execution time could be slow)")
		SET( COMPILE_COMMON_FLAGS " ${COMPILE_COMMON_FLAGS} -D__MEMMANAGER__")
		include_directories (
			../memmgr
		)
		
	   	set(ZETSCRIPT_LIB_SRCS ${ZETSCRIPT_LIB_SRCS} ../memmgr/memmgr.cpp)
		
	ELSE()
		message("-- Memmanager: OFF")
	ENDIF()
	

endif()


#set library generation as static by default
set( LIBRARY_TYPE_CREATION "STATIC" )
if(BUILD_SHARED_LIBS)
	set( LIBRARY_TYPE_CREATION "SHARED" )
endif()

message("-- Library type: " ${LIBRARY_TYPE_CREATION})

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
 
MESSAGE ("-- Target: " ${TARGET})

IF ( "${TARGET}" STREQUAL "debug")
	SET( COMPILE_COMMON_FLAGS " ${COMPILE_COMMON_FLAGS} -D__DEBUG__ ")
ENDIF()


if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")

	MESSAGE ("-- Toolchain: GNU")

	set(COMPILE_TARGET_FLAGS " -O2")

	IF ( "${TARGET}" STREQUAL "debug")
		set(COMPILE_TARGET_FLAGS " -O0 -g ")
	ENDIF ()
	
	IF ( "${32BIT}" STREQUAL "ON")
		MESSAGE ("-- Architecture: 32bit")
		set(COMPILE_TARGET_FLAGS " ${COMPILE_TARGET_FLAGS} -m32 ")
	ELSE ()
		MESSAGE ("-- Architecture: 64bit")
	ENDIF()

	SET(CMAKE_C_FLAGS " ")
	SET(CMAKE_CXX_FLAGS   " ${COMPILE_TARGET_FLAGS} ${COMPILE_COMMON_FLAGS}  " )
	

	#Redefine output dir ...
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${TARGET})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${TARGET})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${TARGET})
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

	MESSAGE ("-- Fast compile: ON" )

	set( ZETSCRIPT_LIB_SRCS ${ZETSCRIPT_LIB_SRCS} ${INCS}
		include_all.cpp
	)
ELSE()

	MESSAGE ("-- Fast compile: OFF" )

	set( ZETSCRIPT_LIB_SRCS ${ZETSCRIPT_LIB_SRCS} ${INCS}
	
		# Utils
 		util/zs_file.cpp
 		util/zs_log.cpp
 		util/zs_path.cpp
 		util/zs_rtti.cpp
 		util/zs_strutils.cpp
 		util/zs_vector.cpp
 		util/zs_map.cpp
 		util/zs_timespan.cpp
		util/zs_datetime.cpp
 		
 		
 		# Modules
 		modules/ModuleMathWrap.cpp
		modules/ModuleSystemWrap.cpp
		modules/ModuleJsonWrap.cpp
 		
 		# VM
 		vm/VirtualMachine.cpp
 		
		# Eval		
		eval/eval.cpp
		
		# Function
		function_proxy/FunctionProxyFactory.cpp 		
		
 		# Scope
 		scope/Scope.cpp
		scope/ScopeFactory.cpp
				
		 # Script
		script/ScriptObject.cpp
		script/ScriptObjectVarRef.cpp
		script/ScriptObjectString.cpp
		script/ScriptObjectStringWrap.cpp
		script/ScriptObjectStringIterator.cpp
		script/ScriptObjectStringIteratorWrap.cpp
		script/ScriptObjectVector.cpp
		script/ScriptObjectVectorWrap.cpp
		script/ScriptObjectVectorIterator.cpp
		script/ScriptObjectVectorIteratorWrap.cpp
		script/ScriptObjectObject.cpp
		script/ScriptObjectObjectIterator.cpp
		script/ScriptObjectObjectIteratorWrap.cpp
		script/ScriptObjectClass.cpp
		script/ScriptObjectDateTime.cpp
		script/ScriptObjectDateTimeWrap.cpp
				
		script/ScriptClass.cpp
		script/ScriptFunction.cpp
		script/ScriptFunctionArg.cpp
		script/ScriptFunctionFactory.cpp
		script/ScriptClassFactory.cpp
		 		 		
 		# Core
 		StackElement.cpp
 		Instruction.cpp
 		ByteCode.cpp
 		exception.cpp
 		Symbol.cpp
		zetscript.cpp
	
	)
ENDIF()

# zetscriptlib
add_library( zetscript${POST_NAME} ${LIBRARY_TYPE_CREATION} ${ZETSCRIPT_LIB_SRCS} )
if(MSVC)
	#only for msvc...
	set_target_properties(zetscript${POST_NAME} PROPERTIES COMPILE_DEFINITIONS ZETSCRIPT_EXPORTS)
endif()


IF ( "${FCOMPILE}" STREQUAL "ON")
	# zs script
	add_executable(zs${POST_NAME}
		${PROJECT_SOURCE_DIR}/zs.cpp
	)
	target_link_libraries(zs${POST_NAME} zetscript${POST_NAME} )
	
	# test_script_call
	#add_executable(test_script_call${POST_NAME}
	#	${PROJECT_SOURCE_DIR}/test/test_script_call.cpp
	#)	
	#target_link_libraries(test_script_call${POST_NAME} zetscript${POST_NAME})
	
	# zs_test
	#add_executable(zs_test${POST_NAME}
	#	${PROJECT_SOURCE_DIR}/test/zs_test.cpp
	#)	
	#target_link_libraries(zs_test${POST_NAME} zetscript${POST_NAME})
	
	# register_class
	#add_executable(test_register_class${POST_NAME}
	#	${PROJECT_SOURCE_DIR}/test/test_register_class.cpp
	#)	
	#target_link_libraries(test_register_class${POST_NAME} zetscript${POST_NAME})	
	
	# register_c_module
	#add_executable(test_register_c_module${POST_NAME}
	#	${PROJECT_SOURCE_DIR}/test/test_register_c_module.cpp
	#	${PROJECT_SOURCE_DIR}/test/CModule.c
	#)	
	#target_link_libraries(test_register_c_module${POST_NAME} zetscript${POST_NAME})	
	
	# test_register_constant
	#add_executable(test_register_constant${POST_NAME}
	#	${PROJECT_SOURCE_DIR}/test/test_register_constant.cpp
	#)	
	#target_link_libraries(test_register_constant${POST_NAME} zetscript${POST_NAME})		
		
ELSE()

	SET(
		HS
		# Utils
 		util/zs_file.h
 		util/zs_log.h
 		util/zs_path.h
 		util/zs_rtti.h
 		util/zs_strutils.h
 		util/zs_vector.h
 		util/zs_map.h
 		util/zs_uuid.h
 		util/zs_map_iterator.h
 		util/zs_timespan.h
		util/zs_datetime.h
 		
 		
 		# extra
		modules/ModuleMathWrap.h
		modules/ModuleSystemWrap.h
		modules/ModuleConsoleWrap.h
		modules/ModuleJsonDeserializeWrap.h
		modules/ModuleJsonSerializeWrap.h
		
		# script
		script/ScriptClass.h
		script/ScriptClass.tcc
		script/ScriptClassFactory.h
		script/ScriptClassFactory.tcc
		script/ScriptFunction.h
		script/ScriptFunction.tcc
		script/ScriptFunctionArg.h
		script/ScriptFunctionFactory.h
		script/ScriptFunctionTraits.h
		script/ScriptObject.h
		script/ScriptObjectObject.h
		script/ScriptObjectObjectWrap.h
		script/ScriptObjectObjectIterator.h
		script/ScriptObjectObjectIteratorwrap.h
		script/ScriptObjectClass.h
		script/ScriptObjectString.h
		script/ScriptObjectStringWrap.h
		script/ScriptObjectStringIterator.h
		script/ScriptObjectStringIteratorWrap.h
		script/ScriptObjectVarRef.h
		script/ScriptObjectVector.h
		script/ScriptObjectVector.tcc
		script/ScriptObjectVectorwrap.h
		script/ScriptObjectVectorIterator.h
		script/ScriptObjectVectorIteratorWrap.h
		script/ScriptObjectDateTime.h
		script/ScriptObjectDateTimeWrap.h
		
		# function proxy
		function_proxy/FunctionProxyFactory.h
		function_proxy/FunctionProxyFactory.h
		function_proxy/FunctionProxyFactory.tcc
		function_proxy/FunctionTraits.h
		
		# scope
		scope/Scope.h
		scope/ScopeFactory.h
		
		# vm
		vm/VirtualMachine.h
		vm/VirtualMachine.tcc
		
		
		# main
		BuiltinType.h
		ByteCode.h
		common.h
		config.h
		exception.h
		Instruction.h
		StackElement.h
		Symbol.h
		zetscript.h
		zetscript.tcc
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
	add_executable(test_script_call${POST_NAME}
		${PROJECT_SOURCE_DIR}/test/test_script_call.cpp
	)
	
	target_link_libraries(test_script_call${POST_NAME} zetscript${POST_NAME})
	
	
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


