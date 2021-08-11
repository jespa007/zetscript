cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)
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

# Copy files from source directory to destination directory, substituting any
# variables.  Create destination directory if it does not exist.

macro(configure_files srcDir destDir)
    message(STATUS "Configuring directory ${destDir}")
    make_directory(${destDir})

    file(GLOB templateFiles RELATIVE ${srcDir} ${srcDir}/*)
    foreach(templateFile ${templateFiles})
        set(srcTemplatePath ${srcDir}/${templateFile})
        if(NOT IS_DIRECTORY ${srcTemplatePath})
            message(STATUS "Configuring file ${srcTemplatePath} to ${destDir}/${templateFile}")
            configure_file(
                    ${srcTemplatePath}
                    ${destDir}/${templateFile}
                    COPYONLY)
        endif(NOT IS_DIRECTORY ${srcTemplatePath})
    endforeach(templateFile)
endmacro(configure_files)

#-------------------------------------------------------------------------------------------------
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

	SET( COMMON_CXX_PARAMS " ${COMMON_CXX_PARAMS} -D__STDC_CONSTANT_MACROS -std=gnu++0x -I -Wall -Wextra")
   
	if (MINGW)
		MESSAGE("-- Plataform: MINGW" )
	elseif(UNIX)
    	MESSAGE ("-- Plataform: UNIX" )
    else()
    	MESSAGE ("-- Plataform: Unknown" )
	endif()
	
	IF ( "${TARGET}" STREQUAL "debug")
	
		set(COMPILE_TARGET_FLAGS " -O0 -g ")
	
		IF( EXISTS ${PROJECT_SOURCE_DIR}/../../memmgr )
			message("-- Memmanager: ON")
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
		set(COMPILE_TARGET_FLAGS " -O2")
		message("-- Memmanager: OFF")
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

SET(CMAKE_CXX_FLAGS   " ${COMPILE_TARGET_FLAGS} ${COMMON_CXX_PARAMS}  " )


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
add_library( zetscript${POST_NAME} ${LIBRARY_TYPE_CREATION} ${ZETSCRIPT_LIB_SRCS} )
if(MSVC)
	#only for msvc...
	#set_target_properties(zetscript${POST_NAME} PROPERTIES COMPILE_DEFINITIONS ZETSCRIPT_EXPORTS)
endif()

# zs script
add_executable(zs${POST_NAME}
	${PROJECT_SOURCE_DIR}/zs.cpp
)
target_link_libraries(zs${POST_NAME} zetscript${POST_NAME} )


# zs_test
add_executable(zs_test${POST_NAME}
	${PROJECT_SOURCE_DIR}/../test/zs_test.cpp
)

target_link_libraries(zs_test${POST_NAME} zetscript${POST_NAME})

# helloworld
add_executable(helloworld${POST_NAME}
	${PROJECT_SOURCE_DIR}/../test/helloworld.cpp
)

target_link_libraries(helloworld${POST_NAME} zetscript${POST_NAME})

# test_register_variables
add_executable(test_register_variable${POST_NAME}
	${PROJECT_SOURCE_DIR}/../test/test_register_variable.cpp
)	

target_link_libraries(test_register_variable${POST_NAME} zetscript${POST_NAME})

# test_register_functions
add_executable(test_register_function${POST_NAME}
	${PROJECT_SOURCE_DIR}/../test/test_register_function.cpp
)	

target_link_libraries(test_register_function${POST_NAME} zetscript${POST_NAME})

# test_register_class
add_executable(test_register_class${POST_NAME}
	${PROJECT_SOURCE_DIR}/../test/test_register_class.cpp
)	

target_link_libraries(test_register_class${POST_NAME} zetscript${POST_NAME})

# function_script_call
add_executable(test_script_call${POST_NAME}
	${PROJECT_SOURCE_DIR}/../test/test_script_call.cpp
)

target_link_libraries(test_script_call${POST_NAME} zetscript${POST_NAME})


# register_constants
add_executable(test_register_constant${POST_NAME}
	${PROJECT_SOURCE_DIR}/../test/test_register_constant.cpp
)

target_link_libraries(test_register_constant${POST_NAME} zetscript${POST_NAME})


SET(
	HS
	# Utils
	util/zs_file.h
	util/zs_dir.h
	util/zs_log.h
	util/zs_path.h
	util/zs_rtti.h
	util/zs_strutils.h
	util/zs_vector.h
	util/zs_map.h
	util/zs_map_iterator.h
	util/zs_timespan.h
	util/zs_datetime.h
	util/zs_util.h
	
	#eval
	eval/eval_keyword.h
	eval/eval_operator.h
	eval/eval.h
	
	
	# extra
	module/MathModule.h
	module/SystemModule.h
	module/ConsoleModule.h
	module/JsonModule.h
	module/DateTimeModule.h
	module/zs_module.h
	
	
	# script
	script/MemberAttribute.h
	script/ScriptClass.h
	script/ScriptClass.tcc
	script/ScriptClassFactory.h
	script/ScriptClassFactory.tcc
	script/ScriptFunction.h
	script/ScriptFunctionArg.h
	script/ScriptFunctionFactory.h
	script/ScriptFunctionTraits.h
	script/ScriptObject.h
	script/ScriptObjectClass.h
	script/ScriptObjectObject.h
	script/ScriptObjectObjectIterator.h
	script/ScriptObjectObjectIteratorWrap.h
	script/ScriptObjectObjectWrap.h
	script/ScriptObjectString.h
	script/ScriptObjectStringIterator.h
	script/ScriptObjectStringIteratorWrap.h
	script/ScriptObjectStringWrap.h
	script/ScriptObjectVarRef.h
	script/ScriptObjectVector.h
	script/ScriptObjectVector.tcc
	script/ScriptObjectVectorIterator.h
	script/ScriptObjectVectorIteratorWrap.h
	script/ScriptObjectVectorWrap.h
	script/zs_script.h
			
	# scope
	scope/Scope.h
	scope/ScopeFactory.h
	scope/zs_scope.h
	
	# vm
	vm/vm.h
	vm/vm.tcc
	
	
	# main
	BuiltinType.h
	ByteCode.h
	common.h
	config.h
	zs_exception.h
	Instruction.h
	StackElement.h
	Symbol.h
	zetscript.h
	zetscript.tcc
)
	

INSTALL_HEADERS_WITH_DIRECTORY(HS)
	
	
INSTALL(TARGETS zetscript${POST_NAME} zs${POST_NAME}
	ARCHIVE DESTINATION  	${CMAKE_INSTALL_PREFIX}/lib
	RUNTIME DESTINATION  	${CMAKE_INSTALL_PREFIX}/bin
	LIBRARY DESTINATION 	${CMAKE_INSTALL_PREFIX}/lib
	PUBLIC_HEADER DESTINATION	${CMAKE_INSTALL_PREFIX}/includes
)

# copy tests to
configure_files(${PROJECT_SOURCE_DIR}/../test ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/test )
configure_files(${PROJECT_SOURCE_DIR}/../test/import ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/test/import )

