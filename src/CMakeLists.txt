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

if("${FLOAT_PRECISION}" STREQUAL "DOUBLE")
	set( ZS_FLOAT_TYPE 			"double" )
	MESSAGE("-- Float precision: double"  )
else()
	set( ZS_FLOAT_TYPE		"float" )
	MESSAGE("-- Float precision: single"  )
endif()

configure_file(${PROJECT_SOURCE_DIR}/config.h.in ${PROJECT_SOURCE_DIR}/config.h)

set( ZS_TEST_ALL_SCRIPT_TEST_PATH		"${PROJECT_SOURCE_DIR}/../test/script" )
MESSAGE("-- Script test path: " ${ZS_TEST_ALL_SCRIPT_TEST_PATH}  )

configure_file(${PROJECT_SOURCE_DIR}/../test/test_all_config.h.in ${PROJECT_SOURCE_DIR}/../test/test_all_config.h)

#-------------------------------------------------------------------------------------------------
#
# COMMON PARAMS
#

include_directories (
	${PROJECT_SOURCE_DIR}
)

file(GLOB_RECURSE INCS "${PROJECT_SOURCE_DIR}/*.h,${PROJECT_SOURCE_DIR}/*.tcc")


message("-- Install directory: " ${CMAKE_INSTALL_PREFIX})

set(ZETSCRIPT_LIB_SRCS "")

if(MSVC)
	SET(COMPILER "msvc")
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

	SET( COMMON_CXX_PARAMS " ${COMMON_CXX_PARAMS} -D__STDC_CONSTANT_MACROS -std=gnu++0x -I -Wall -Wextra -pedantic")
   
	if (MINGW)
		MESSAGE("-- Plataform: MINGW" )
	else()
		SET( COMMON_CXX_PARAMS " ${COMMON_CXX_PARAMS} -fPIC ")
		
		if(UNIX)
    		MESSAGE ("-- Plataform: UNIX" )
    	else()
    		MESSAGE ("-- Plataform: Unknown" )
    	endif()
	endif()
	
	IF ( "${TARGET}" STREQUAL "debug")
	
		set(COMMON_CXX_PARAMS " ${COMMON_CXX_PARAMS} -O0 -g ")
	
		IF( EXISTS ${PROJECT_SOURCE_DIR}/../../memmgr )
			message("-- Memmanager: Yes")
			SET( COMMON_CXX_PARAMS " ${COMMON_CXX_PARAMS} -D__MEMMANAGER__")
			include_directories (
				${PROJECT_SOURCE_DIR}/../../memmgr
			)
		
	   		set(ZETSCRIPT_LIB_SRCS ${ZETSCRIPT_LIB_SRCS} ${PROJECT_SOURCE_DIR}/../../memmgr/memmgr.cpp)
	   	else()
	   		message("-- Memmanager: OFF (../../memmgr not exist)")
	   	endif()
		
	ELSE()
		set(TARGET "release")
		set(COMMON_CXX_PARAMS " ${COMMON_CXX_PARAMS} -O2 ")
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
	SET( COMMON_CXX_PARAMS " ${COMMON_CXX_PARAMS} -D__DEBUG__ ")
ENDIF()

SET(CMAKE_CXX_FLAGS   " ${COMMON_CXX_PARAMS}  " )


#
# COMMON PARAMS
#
#-------------------------------------------------------------------------------------------------
#
# COMPILE PARAMS
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
	
	
	# register_variables
	add_executable(register_variable${POST_NAME}
		${PROJECT_SOURCE_DIR}/../samples/register_variable.cpp
	)	
	
	target_link_libraries(register_variable${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})
	
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
	# test_consecutive_evals
	add_executable(test_consecutive_evals${POST_NAME}
		${PROJECT_SOURCE_DIR}/../test/test_consecutive_evals.cpp
	)
	target_compile_definitions(test_consecutive_evals${POST_NAME} PUBLIC -D__MAIN__ ) 
	target_link_libraries(test_consecutive_evals${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})

	#-----------------------------------------------
	# test arithmetic constants
	add_executable(test_arithmetic_constants${POST_NAME}
		${PROJECT_SOURCE_DIR}/../test/test_arithmetic_common.cpp
		${PROJECT_SOURCE_DIR}/../test/test_arithmetic_constants.cpp
	)
	target_compile_definitions(test_arithmetic_constants${POST_NAME} PUBLIC -D__MAIN__ )
	target_link_libraries(test_arithmetic_constants${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})
	
	
		#-----------------------------------------------
	# test_map
	add_executable(test_benchmark_std${POST_NAME}
		${PROJECT_SOURCE_DIR}/../test/test_benchmark_std.cpp
	)
	target_link_libraries(test_benchmark_std${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})
	
	#-----------------------------------------------
	# test arithmetic vars
	add_executable(test_arithmetic_vars${POST_NAME}
		${PROJECT_SOURCE_DIR}/../test/test_arithmetic_common.cpp
		${PROJECT_SOURCE_DIR}/../test/test_arithmetic_vars.cpp
	)
	
	target_compile_definitions(test_arithmetic_vars${POST_NAME} PUBLIC -D__MAIN__ )
	target_link_libraries(test_arithmetic_vars${POST_NAME} ${ZETSCRIPT_LIB_NAME}${POST_NAME})
	
	endif()
	
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
	script/ContainerSlot.h
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
	script/object/ScriptObject.h
	script/object/ScriptObjectClass.h
	script/object/ScriptObjectMemberFunction.h
	script/object/ScriptObjectContainerSlot.h
	script/object/ScriptObjectObject.h
	script/object/ScriptObjectObjectWrap.h
	script/object/ScriptObjectString.h
	script/object/ScriptObjectStringWrap.h
	script/object/ScriptObjectVarRef.h
	script/object/ScriptObjectVector.h
	script/object/ScriptObjectVectorWrap.h
	script/object/ScriptObjectClass.tcc
	
	# script/object/iterator
	script/object/iterator/ScriptObjectIteratorObject.h
	script/object/iterator/ScriptObjectIteratorObjectWrap.h
	script/object/iterator/ScriptObjectIteratorString.h
	script/object/iterator/ScriptObjectIteratorStringWrap.h
	script/object/iterator/ScriptObjectIteratorVector.h
	script/object/iterator/ScriptObjectIteratorVectorWrap.h
			
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
	vm/vm_internals.h
	
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


