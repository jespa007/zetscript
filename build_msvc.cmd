@echo off
SET ROOT_DIR=%~dp0
 
:: Detect which Visual Studio has installed
IF NOT "%VS160COMNTOOLS%"=="" (
	SET CMAKE_GENERATOR="Visual Studio 16 2019"
)ELSE IF NOT "%VS150COMNTOOLS%"=="" (
	SET CMAKE_GENERATOR="Visual Studio 15 2017"
)ELSE IF NOT "%VS140COMNTOOLS%"=="" (
	SET CMAKE_GENERATOR="Visual Studio 14 2015"
) ELSE (
	echo.
	echo Cannot detect Visual Studio environment vars through "Visual Studio Developer Command Prompt". The environment should set one of the followings environment variables,
	echo.
	echo  VS140COMNTOOLS: Environment variable for Visual Studio 2015
	echo  VS150COMNTOOLS: Environment variable for Visual Studio 2017
	echo  VS160COMNTOOLS: Environment variable for Visual Studio 2019
	echo. 
	echo Other Visual Studio versions are not supported
	echo. 
	PAUSE
	EXIT
)


echo -- CMake generator: %CMAKE_GENERATOR%

cmake  -H. -Bbuild\msvc -G %CMAKE_GENERATOR%
msbuild build\msvc\zetscript.sln /t:Rebuild /p:Configuration=Release
