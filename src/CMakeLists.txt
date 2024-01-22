cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)
project(zetscript)


MESSAGE ("-- Target: " ${CMAKE_BUILD_TYPE})

file(READ "zetscript.h" zetscript_h)

string(REGEX MATCH "ZETSCRIPT_VERSION_MAJOR ([0-9]*)" _ ${zetscript_h})
set(ZETSCRIPT_VERSION_MAJOR ${CMAKE_MATCH_1})

string(REGEX MATCH "ZETSCRIPT_VERSION_MINOR ([0-9]*)" _ ${zetscript_h})
set(ZETSCRIPT_VERSION_MINOR ${CMAKE_MATCH_1})

string(REGEX MATCH "ZETSCRIPT_VERSION_PATCH ([0-9]*)" _ ${zetscript_h})
set(ZETSCRIPT_VERSION_PATCH ${CMAKE_MATCH_1})



MESSAGE("-- zetscript version ${ZETSCRIPT_VERSION_MAJOR}.${ZETSCRIPT_VERSION_MINOR}.${ZETSCRIPT_VERSION_PATCH}" )

set(ZETSCRIPT_LIB_NAME "zetscript-${ZETSCRIPT_VERSION_MAJOR}-${ZETSCRIPT_VERSION_MINOR}-${ZETSCRIPT_VERSION_PATCH}")


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

#-------------------------------------------------------------------------------------------------
#
# CONFIGURE FILES
#


if((CMAKE_SIZEOF_VOID_P EQUAL 8) OR "${FLOAT_PRECISION}" STREQUAL "DOUBLE")
	set( FLOAT_TYPE 		"double" )
	MESSAGE("-- Float precision: double"  )
else()
	set( FLOAT_TYPE		"float" )
	MESSAGE("-- Float precision: single"  )
endif()

configure_file(${PROJECT_SOURCE_DIR}/Config.h.in ${PROJECT_SOURCE_DIR}/base/Config.h)

set( ZS_TEST_ALL_SCRIPT_TEST_PATH		"${PROJECT_SOURCE_DIR}/../test/language" )
MESSAGE("-- Script test path: " ${ZS_TEST_ALL_SCRIPT_TEST_PATH}  )

configure_file(${PROJECT_SOURCE_DIR}/../test/test_all_config.h.in ${PROJECT_SOURCE_DIR}/../test/test_all_config.h)

#-------------------------------------------------------------------------------------------------
#
# COMPILER PARAMS
#

include_directories (
	${PROJECT_SOURCE_DIR}
)

file(GLOB_RECURSE INCS "${PROJECT_SOURCE_DIR}/*.h,${PROJECT_SOURCE_DIR}/*.tcc")


message("-- Install directory: " ${CMAKE_INSTALL_PREFIX})

set(ZETSCRIPT_LIB_SRCS "")

if(MSVC)
	MESSAGE ("-- Plataform: " msvc)
	SET(PLATFORM "msvc")
	
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)

	#disable C4103 warning because is perfectly valid. Only MSVC complains about it
	add_definitions(/wd4103)
	add_definitions(/wd4800)
	add_definitions(/wd4244)
	
	# Warning all
	add_definitions(/W4)
	
	# warning C4530: C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc
	add_definitions(/EHsc)
	
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	
	if(BUILD_SHARED_LIBS)
		set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
	else()
		set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	endif()

	# Provisional to export all symbols when shared 
	if(BUILD_SHARED_LIBS)
		set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	endif()
	
else()

	IF( NOT CMAKE_BUILD_TYPE )
	   SET( CMAKE_BUILD_TYPE Release )
	ENDIF()
	
	set(POST_NAME "")
	set(IS_DEBUG FALSE)
	if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
		set(IS_DEBUG TRUE)
		SET( CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -D__DEBUG__ ")
		set( POST_NAME "_d")
	endif()

	SET( CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -D__STDC_CONSTANT_MACROS -std=gnu++0x -Wall -Wextra -pedantic")
	SET(PLATFORM "gcc")
		
	if (MINGW)
		MESSAGE("-- Plataform: mingw" )
	else()
		SET( CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -fPIC ")
		
    	if(RASPBERRY)
    		MESSAGE ("-- Plataform: Raspberry" )
    		SET(PLATFORM "rpi")
    	elseif(APPLE)
    		MESSAGE ("-- Plataform: Apple" )
		elseif(EMSCRIPTEN)
			MESSAGE ("-- Plataform: Emscripten" )
			set(PLATFORM "em")
			MESSAGE ("-- Emscripten path: $ENV{EMSCRIPTEN_ROOT_PATH} ")
			SET( CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -I$ENV{EMSCRIPTEN_ROOT_PATH}/system/lib/libcxxabi/include/ ")
    	elseif(UNIX)
    		MESSAGE ("-- Plataform: Unix" )
    		file(READ "/etc/issue" ETC_ISSUE )
			string(REGEX MATCH "Debian|Ubuntu|Alpine" DIST ${ETC_ISSUE})
			MESSAGE ("-- Dist: " ${DIST} )
    	else()
    		MESSAGE ("-- Plataform: Unknown" )
    	endif()
	endif()
	
	IF ( IS_DEBUG)
		set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -O0 -g ")
		
		IF (EXISTS ${PROJECT_SOURCE_DIR}/../../memmgr)
		
			message("-- Memmanager: Yes")
			SET( CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -D__MEMMANAGER__")
			include_directories (
				${PROJECT_SOURCE_DIR}/../../memmgr
			)
			
	   		set(ZETSCRIPT_LIB_SRCS ${ZETSCRIPT_LIB_SRCS} ${PROJECT_SOURCE_DIR}/../../memmgr/memmgr.cpp)
		
		ELSE()
	
			if(UNIX)
				if( NOT "${DIST}" STREQUAL "Alpine")
					set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -fsanitize=address ")
					message("-- Memmanager: -fsanitize=address")
				else()
					message("-- Memmanager: -fsanitize NOT supported")
				endif()
			 else()
			 	message("-- Memmanager: OFF (../../memmgr not exist)")
			 endif()
		ENDIF()
	ELSE()
		set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -O2 ")
	ENDIF()
	
	
endif()


#Redefine output dir ...
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin/${PLATFORM}/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin/${PLATFORM}/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin/${PLATFORM}/${CMAKE_BUILD_TYPE})
LINK_DIRECTORIES(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})	


if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	# 64 bits
	MESSAGE ("-- Architecture: 64bit")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    # 32 bits
    MESSAGE ("-- Architecture: 32bit")
endif()

#
# COMPILER PARAMS
#
#-------------------------------------------------------------------------------------------------
#
# SOURCE PARAMS
#

# Library


set( ZETSCRIPT_LIB_SRCS ${ZETSCRIPT_LIB_SRCS} ${INCS}

	# Utils
	base/@base.cpp
	
	# Core
	core/@core.cpp
	
	# Eval		
	compiler/compiler.cpp
	
	# Modules
	module/@module.cpp
	 
	# Virtual machine
	vm/vm.cpp

	# ScriptEngine
	ScriptEngine.cpp
)

#-------------------------------------------------------------------------------------------------------------------------------------
# LINKING PARAMS

#set library generation as static by default
set( LIBRARY_TYPE_CREATION "STATIC" )
if(BUILD_SHARED_LIBS)
	set( LIBRARY_TYPE_CREATION "SHARED" )
else()
	set( ZETSCRIPT_LIB_NAME ${ZETSCRIPT_LIB_NAME}-static)
endif()

message("-- Library type: " ${LIBRARY_TYPE_CREATION})

# zetscriptlib
add_library( ${ZETSCRIPT_LIB_NAME}${POST_NAME} ${LIBRARY_TYPE_CREATION} ${ZETSCRIPT_LIB_SRCS} )
if(MSVC)
	#only for msvc...
	#set_target_properties(${ZETSCRIPT_LIB_NAME}${POST_NAME} PROPERTIES COMPILE_DEFINITIONS ZETSCRIPT_EXPORTS)
endif()

# zs script
add_executable(zs${POST_NAME}
	${PROJECT_SOURCE_DIR}/zs.cpp
)
target_link_libraries(zs${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME} )

if (SAMPLES)
	MESSAGE ("-- Samples: Yes")

	# helloworld
	add_executable(helloworld${POST_NAME}
		${PROJECT_SOURCE_DIR}/../samples/api/helloworld.cpp
	)
	
	target_link_libraries(helloworld${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})
	
	# register_functions
	add_executable(register_function${POST_NAME}
		${PROJECT_SOURCE_DIR}/../samples/api/register_function.cpp
	)	
	
	target_link_libraries(register_function${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})	
	
	# test_register_class
	add_executable(register_class${POST_NAME}
		${PROJECT_SOURCE_DIR}/../samples/api/register_class.cpp
	)	
	
	target_link_libraries(register_class${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})
	
	#-----------------------------------------------
	# print_object_sizes
	add_executable(print_object_sizes${POST_NAME}
		${PROJECT_SOURCE_DIR}/../test/api/print_object_sizes.cpp
	)
	target_link_libraries(print_object_sizes${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})
			

else()
	MESSAGE ("-- Suit samples: No")
endif()


if (TESTS)
	MESSAGE ("-- Tests: Yes")
	
	
	#-----------------------------------------------
	# test benchmark_std
	add_executable(test_benchmark_std${POST_NAME}
		${PROJECT_SOURCE_DIR}/../test/api/test_benchmark_std.cpp
	)
	target_link_libraries(test_benchmark_std${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})
		

	#-----------------------------------------------
	# test arithmetic vars
	add_executable(test_arithmetic_vars${POST_NAME}
		${PROJECT_SOURCE_DIR}/../test/api/test_arithmetic_common.cpp
		${PROJECT_SOURCE_DIR}/../test/api/test_arithmetic_vars.cpp
	)
		
	target_compile_definitions(test_arithmetic_vars${POST_NAME} PUBLIC -D__MAIN__ )
	target_link_libraries(test_arithmetic_vars${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})
	
	#-----------------------------------------------
	# test arithmetic constants
	add_executable(test_arithmetic_constants${POST_NAME}
		${PROJECT_SOURCE_DIR}/../test/api/test_arithmetic_common.cpp
		${PROJECT_SOURCE_DIR}/../test/api/test_arithmetic_constants.cpp
	)
	target_compile_definitions(test_arithmetic_constants${POST_NAME} PUBLIC -D__MAIN__ )
	target_link_libraries(test_arithmetic_constants${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})
	
	#-----------------------------------------------
	# test_consecutive_evals
	add_executable(test_consecutive_evals${POST_NAME}
		${PROJECT_SOURCE_DIR}/../test/api/test_consecutive_evals.cpp
	)
	target_compile_definitions(test_consecutive_evals${POST_NAME} PUBLIC -D__MAIN__ ) 
	target_link_libraries(test_consecutive_evals${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})	
	
	#-----------------------------------------------
	# test arithmetic metamethods
	add_executable(test_arithmetic_metamethods${POST_NAME}
		${PROJECT_SOURCE_DIR}/../test/api/test_arithmetic_common.cpp
		${PROJECT_SOURCE_DIR}/../test/api/test_arithmetic_metamethods.cpp
	)
	target_compile_definitions(test_arithmetic_metamethods${POST_NAME} PUBLIC -D__MAIN__ )
	target_link_libraries(test_arithmetic_metamethods${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})
	#-----------------------------------------------
	# test_call_native_functions
	add_executable(test_call_native_functions${POST_NAME}
		${PROJECT_SOURCE_DIR}/../test/api/test_call_native_functions.cpp
	)
	target_compile_definitions(test_call_native_functions${POST_NAME} PUBLIC -D__MAIN__ )
	target_link_libraries(test_call_native_functions${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})	
	#-----------------------------------------------
	# test_callbacks
	add_executable(test_callbacks${POST_NAME}
		${PROJECT_SOURCE_DIR}/../test/api/test_callbacks.cpp
	)
	target_compile_definitions(test_callbacks${POST_NAME} PUBLIC -D__MAIN__ )
	target_link_libraries(test_callbacks${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})
	#-----------------------------------------------
	# test_register_constants
	add_executable(test_register_constants${POST_NAME}
		${PROJECT_SOURCE_DIR}/../test/api/test_register_constants.cpp
	)
	target_compile_definitions(test_register_constants${POST_NAME} PUBLIC -D__MAIN__ )
	target_link_libraries(test_register_constants${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})
		
		
	#-----------------------------------------------
	# test_all
	add_executable(test_all${POST_NAME}
		${PROJECT_SOURCE_DIR}/../test/test_all.cpp
	)

	target_link_libraries(test_all${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})
	
else()
	MESSAGE ("-- Suit test: No")
endif()



SET(
	HS
	# Base
	base/Common.h
	base/Config.h
	base/DateTime.h
	base/Directory.h
	base/File.h
	base/Console.h
	base/Log.h
	base/MapStringIterator.h
	base/MapString.h
	base/MapInt.h
	base/MapIntIterator.h
	base/Memory.h
	base/Path.h
	base/Rtti.h
	base/String.h
	base/TimeSpan.h
	base/@base.h
	base/Vector.h
	base/Vector.tcc
	base/List.tcc
	base/Buffer.h
	base/System.h
	base/List.h
	base/Exception.h
	base/Character.h
	base/Integer.h
	base/Float.h
	base/Boolean.h
	

	#eval
	compiler/compiler_keyword.h
	compiler/compiler_operator.h
	compiler/compiler.h
	
	
	# module
	module/ConsoleModule.h
	module/DateTimeModule.h
	module/TimeSpanModule.h
	module/JsonModule.h
	module/MathModule.h
	module/SystemModule.h
	module/@module.h
	
	# core
	core/@core.h
	
	core/MemberProperty.h
	core/MetamethodMembers.h
	core/StackElementMemberProperty.h
	core/ByteCode.h
	core/ByteCodeHelper.h
	core/Metamethod.h
	core/MetamethodHelper.h	
	core/Instruction.h
	core/StackElement.h
	core/Symbol.h
	
			
	# core/function	
	core/function/ScriptFunction.h
	core/function/ScriptFunctionsFactory.h
	core/function/ScriptFunctionParam.h
	core/function/FunctionTraits.h
	
	# core/type
	core/type/ScriptType.h
	core/type/ScriptTypeId.h
	core/type/ScriptType.tcc
	core/type/ScriptTypesFactory.h
	core/type/ScriptTypesFactory.tcc
	
	# core/object
	core/object/RefScriptObject.h
	core/object/ScriptObject.h
	core/object/ClassScriptObject.h
	core/object/ClassScriptObject.tcc
	core/object/MemberFunctionScriptObject.h
	core/object/ContainerScriptObject.h
	core/object/ContainerSlot.h
	core/object/ObjectScriptObject.h
	core/object/ObjectScriptObject.tcc
	core/object/ObjectScriptObjectZs.h
	core/object/ObjectIteratorScriptObject.h
	core/object/ObjectIteratorScriptObjectZs.h
	core/object/StringScriptObject.h
	core/object/StringScriptObjectZs.h
	core/object/StringIteratorScriptObject.h
	core/object/StringIteratorScriptObjectZs.h
	core/object/VarRefScriptObject.h
	core/object/ArrayScriptObject.h
	core/object/ArrayScriptObject.tcc
	core/object/ArrayScriptObjectZs.h
	core/object/ArrayIteratorScriptObject.h
	core/object/ArrayIteratorScriptObjectZs.h
	core/scope/ScriptScope.h
	core/scope/ScriptScopesFactory.h
	
	# vm
	vm/vm_operation_assign.h
	vm/vm_operation.h
	vm/vm_pop_stk.h
	vm/vm_push_stk.h
	vm/vm_scope.h
	vm/vm.cpp
	vm/vm.h
	vm/vm.tcc
	vm/vm_common.h
	vm/vm_share_manager.h
	
	# main
	zetscript.h
	ScriptEngine.h
	ScriptEngine.tcc
)
	

INSTALL_HEADERS_WITH_DIRECTORY(HS)
	
	
INSTALL(TARGETS ${ZETSCRIPT_LIB_NAME}${POST_NAME} zs${POST_NAME}
	ARCHIVE DESTINATION  	${CMAKE_INSTALL_PREFIX}/lib
	RUNTIME DESTINATION  	${CMAKE_INSTALL_PREFIX}/bin
	LIBRARY DESTINATION 	${CMAKE_INSTALL_PREFIX}/lib
	PUBLIC_HEADER DESTINATION	${CMAKE_INSTALL_PREFIX}/includes
)


