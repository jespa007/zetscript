cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)
project(zetscript)

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

# By default double precision, but user can explicity set SINGLE precision doing -DFLOAT_PRECISION=SINGLE. Is not recommended due precision lose. 
# For example,
# Take a float of single precision using System::clock() that gives a large numbers, the difference between lower interval could give a same value (i.e a difference of 0)
#
if("${FLOAT_PRECISION}" STREQUAL "SINGLE")
	set( ZS_FLOAT_TYPE		"float" )
	MESSAGE("-- Float precision: single"  )
else()
	set( ZS_FLOAT_TYPE 		"double" )
	MESSAGE("-- Float precision: double"  )
endif()

configure_file(${PROJECT_SOURCE_DIR}/config.h.in ${PROJECT_SOURCE_DIR}/config.h)

set( ZS_TEST_ALL_SCRIPT_TEST_PATH		"${PROJECT_SOURCE_DIR}/../test/script" )
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
	MESSAGE ("-- Plataform: MSVC" )
	
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
	
else()

	SET( CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -D__STDC_CONSTANT_MACROS -std=gnu++0x -I -Wall -Wextra -pedantic")
   
	if (MINGW)
		MESSAGE("-- Plataform: MINGW" )
	else()
		SET( CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -fPIC ")
		
    	if(RASPBERRY)
    		MESSAGE ("-- Plataform: RASPBERRY" )
    	elseif(UNIX)
    		MESSAGE ("-- Plataform: UNIX" )
    	else()
    		MESSAGE ("-- Plataform: Unknown" )
    	endif()
	endif()
	
	IF ( "${TARGET}" STREQUAL "debug")
	
		set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -O0 -g ")
	
		IF( EXISTS ${PROJECT_SOURCE_DIR}/../../memmgr )
			message("-- Memmanager: Yes")
			SET( CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -D__MEMMANAGER__")
			include_directories (
				${PROJECT_SOURCE_DIR}/../../memmgr
			)
		
	   		set(ZETSCRIPT_LIB_SRCS ${ZETSCRIPT_LIB_SRCS} ${PROJECT_SOURCE_DIR}/../../memmgr/memmgr.cpp)
	   	else()
	   		message("-- Memmanager: OFF (../../memmgr not exist)")
	   	endif()
		
	ELSE()
		set(TARGET "release")
		set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -O2 ")
		message("-- Memmanager: No")
	ENDIF()
	

	#Redefine output dir ...
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${TARGET})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${TARGET})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${TARGET})
	LINK_DIRECTORIES(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})	
	

endif()


if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	# 64 bits
	MESSAGE ("-- Architecture: 64bit")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    # 32 bits
    MESSAGE ("-- Architecture: 32bit")
endif()


IF ( "${TARGET}" STREQUAL "")
   SET ( TARGET "release" )
ENDIF ()

set( POST_NAME "")
IF ( "${TARGET}" STREQUAL "debug")
	set( POST_NAME "_d")
ENDIF()
 
MESSAGE ("-- Target: " ${TARGET})

IF ( "${TARGET}" STREQUAL "debug")
	SET( CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -D__DEBUG__ ")
ENDIF()

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
	util/zs_util.cpp
	
	# Modules
	module/zs_module.cpp
	
	# VM
	vm/vm.cpp
	
	# Eval		
	eval/eval.cpp
		
	# Scope
	scope/zs_scope.cpp
			
	# Script
	script/zs_script.cpp 

	# Core
	zetscript.cpp
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

if ((SUITSAMPLES) OR (NOT DEFINED SUITSAMPLES))

	# helloworld
	add_executable(helloworld${POST_NAME}
		${PROJECT_SOURCE_DIR}/../samples/helloworld.cpp
	)
	
	target_link_libraries(helloworld${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})
	
	# register_functions
	add_executable(register_function${POST_NAME}
		${PROJECT_SOURCE_DIR}/../samples/register_function.cpp
	)	
	
	target_link_libraries(register_function${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})	
	
	# test_register_class
	add_executable(register_class${POST_NAME}
		${PROJECT_SOURCE_DIR}/../samples/register_class.cpp
	)	
	
	target_link_libraries(register_class${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})
	
	#-----------------------------------------------
	# test_map
	add_executable(test_benchmark_std${POST_NAME}
		${PROJECT_SOURCE_DIR}/../test/test_benchmark_std.cpp
	)
	target_link_libraries(test_benchmark_std${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})
	
	#-----------------------------------------------
	# print_object_sizes
	add_executable(print_object_sizes${POST_NAME}
		${PROJECT_SOURCE_DIR}/../test/print_object_sizes.cpp
	)
	target_link_libraries(print_object_sizes${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})
			

endif()


if ((SUITTEST) OR (NOT DEFINED SUITTEST))
	MESSAGE ("-- Suit test: Yes")

	if(false)
	
		#-----------------------------------------------
		# test register_constants
		add_executable(test_register_constant${POST_NAME}
			${PROJECT_SOURCE_DIR}/../test/test_register_constant.cpp
		)
		target_compile_definitions(test_register_constant${POST_NAME} PUBLIC -D__MAIN__ )
		target_link_libraries(test_register_constant${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})
		
	
		#-----------------------------------------------
		# test_map
		add_executable(test_benchmark_std${POST_NAME}
			${PROJECT_SOURCE_DIR}/../test/test_benchmark_std.cpp
		)
		target_link_libraries(test_benchmark_std${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})
		

	endif()
	
	#-----------------------------------------------
	# test arithmetic vars
	add_executable(test_arithmetic_vars${POST_NAME}
		${PROJECT_SOURCE_DIR}/../test/test_arithmetic_common.cpp
		${PROJECT_SOURCE_DIR}/../test/test_arithmetic_vars.cpp
	)
		
	target_compile_definitions(test_arithmetic_vars${POST_NAME} PUBLIC -D__MAIN__ )
	target_link_libraries(test_arithmetic_vars${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})
	
	#-----------------------------------------------
	# test arithmetic constants
	add_executable(test_arithmetic_constants${POST_NAME}
		${PROJECT_SOURCE_DIR}/../test/test_arithmetic_common.cpp
		${PROJECT_SOURCE_DIR}/../test/test_arithmetic_constants.cpp
	)
	target_compile_definitions(test_arithmetic_constants${POST_NAME} PUBLIC -D__MAIN__ )
	target_link_libraries(test_arithmetic_constants${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})
	
	#-----------------------------------------------
	# test_consecutive_evals
	add_executable(test_consecutive_evals${POST_NAME}
		${PROJECT_SOURCE_DIR}/../test/test_consecutive_evals.cpp
	)
	target_compile_definitions(test_consecutive_evals${POST_NAME} PUBLIC -D__MAIN__ ) 
	target_link_libraries(test_consecutive_evals${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})	
	
	#-----------------------------------------------
	# test arithmetic metamethods
	add_executable(test_arithmetic_metamethods${POST_NAME}
		${PROJECT_SOURCE_DIR}/../test/test_arithmetic_common.cpp
		${PROJECT_SOURCE_DIR}/../test/test_arithmetic_metamethods.cpp
	)
	target_compile_definitions(test_arithmetic_metamethods${POST_NAME} PUBLIC -D__MAIN__ )
	target_link_libraries(test_arithmetic_metamethods${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})
	#-----------------------------------------------
	# test_call_native_functions
	add_executable(test_call_native_functions${POST_NAME}
		${PROJECT_SOURCE_DIR}/../test/test_call_native_functions.cpp
	)
	target_compile_definitions(test_call_native_functions${POST_NAME} PUBLIC -D__MAIN__ )
	target_link_libraries(test_call_native_functions${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})	
	#-----------------------------------------------
	# test_callbacks
	add_executable(test_callbacks${POST_NAME}
		${PROJECT_SOURCE_DIR}/../test/test_callbacks.cpp
	)
	target_compile_definitions(test_callbacks${POST_NAME} PUBLIC -D__MAIN__ )
	target_link_libraries(test_callbacks${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})
	#-----------------------------------------------
	# test_register_constants
	add_executable(test_register_constants${POST_NAME}
		${PROJECT_SOURCE_DIR}/../test/test_register_constants.cpp
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
	# Utils
	util/zs_datetime.h
	util/zs_dir.h
	util/zs_file.h
	util/zs_io.h
	util/zs_log.h
	util/zs_map_iterator.h
	util/zs_map.h
	util/zs_map_int.h
	util/zs_map_int_iterator.h
	util/zs_mem.h
	util/zs_path.h
	util/zs_rtti.h
	util/zs_string.h
	util/zs_strutils.h
	util/zs_timespan.h
	util/zs_util.h
	util/zs_vector.h
	util/zs_vector.tcc
	util/zs_list.tcc
	util/zs_buffer.h
	util/zs_system.h
	util/zs_list.h
	
	#eval
	eval/eval_keyword.h
	eval/eval_operator.h
	eval/eval.h
	
	
	# module
	module/ConsoleModule.h
	module/DateTimeModule.h
	module/JsonModule.h
	module/MathModule.h
	module/SystemModule.h
	module/zs_module.h
	
	# script
	script/zs_script.h
	script/MemberProperty.h
	script/MetamethodMembers.h
		
	# script/function	
	script/function/ScriptFunction.h
	script/function/ScriptFunctionFactory.h
	script/function/ScriptFunctionParam.h
	script/function/ScriptFunctionTraits.h
	
	# script/type
	script/type/ScriptType.h
	script/type/ScriptType.tcc
	script/type/ScriptTypeFactory.h
	script/type/ScriptTypeFactory.tcc
	
	# script/object
	script/object/RefObjectScriptObject.h
	script/object/ScriptObject.h
	script/object/ClassScriptObject.h
	script/object/ClassScriptObject.tcc
	script/object/MemberFunctionScriptObject.h
	script/object/ContainerScriptObject.h
	script/object/ContainerSlot.h
	script/object/ContainerSlot.h
	script/object/ObjectScriptObject.h
	script/object/ObjectScriptObjectWrap.h
	script/object/ObjectIteratorScriptObject.h
	script/object/ObjectIteratorScriptObjectWrap.h
	script/object/StringScriptObject.h
	script/object/StringScriptObjectWrap.h
	script/object/StringIteratorScriptObject.h
	script/object/StringIteratorScriptObjectWrap.h
	script/object/VarRefScriptObject.h
	script/object/VectorScriptObject.h
	script/object/VectorScriptObjectWrap.h
	script/object/VectorIteratorScriptObject.h
	script/object/VectorIteratorScriptObjectWrap.h
	
			
	# scope
	scope/Scope.h
	scope/ScopeFactory.h
	scope/zs_scope.h
	
	# vm
	vm/vm_byte_code_operation_set.h
	vm/vm_byte_code_operation.h
	vm/vm_pop_stk.h
	vm/vm_push_stk.h
	vm/vm_scope.h
	vm/vm.cpp
	vm/vm.h
	vm/vm.tcc
	vm/vm_common.h
	
	# main
	ByteCode.h
	common.h
	config.h
	Instruction.h
	StackElement.h
	StackElement.tcc
	Symbol.h
	Type.h
	zetscript.h
	zetscript.tcc
	zs_exception.h
)
	

INSTALL_HEADERS_WITH_DIRECTORY(HS)
	
	
INSTALL(TARGETS ${ZETSCRIPT_LIB_NAME}${POST_NAME} zs${POST_NAME}
	ARCHIVE DESTINATION  	${CMAKE_INSTALL_PREFIX}/lib
	RUNTIME DESTINATION  	${CMAKE_INSTALL_PREFIX}/bin
	LIBRARY DESTINATION 	${CMAKE_INSTALL_PREFIX}/lib
	PUBLIC_HEADER DESTINATION	${CMAKE_INSTALL_PREFIX}/includes
)


