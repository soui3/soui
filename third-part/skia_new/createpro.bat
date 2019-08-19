cd /d %~dp0
@ECHO off
SETLOCAL enabledelayedexpansion
cls
COLOR 1f

ECHO.
ECHO.
ECHO   ##############################################################
ECHO   #               欢迎使用 SOUI 工程配置向导                   #
ECHO   #                                启程软件 2017.04.01         #
ECHO   ##############################################################
ECHO.
ECHO.

SET cfg=
SET specs=
SET target=x86
SET targetx86andx64=0
SET selected=
SET mt=1
SET unicode=1
SET wchar=1
SET supportxp=0
SET vsvarbat=

rem 选择编译版本
SET /p selected=1.选择编译版本[1=x86;2=x64]:
if %selected%==1 (
	SET target=x86
) else if %selected%==2 (
	SET target=x64
	SET cfg=!cfg! x64
) else (
	goto error
)

for /f "skip=2 delims=: tokens=1,*" %%i in ('reg query "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion" /v "ProgramFilesDir (x86)"') do ( 
	    set str=%%i
		set var=%%j
		set "var=!var:"=!"
		if not "!var:~-1!"=="=" set strCMD=!str:~-1!:!var!
	 )
	 SET strCMD=%strCMD%\Microsoft Visual Studio\Installer\vswhere.exe
	
	 if exist "%strCMD%" (
	 for /f "delims=" %%i in ('"%strCMD%" -nologo -version [16.0^,17.0] -prerelease -property installationPath -format value') do (
	    set vs2019path=%%i
		)
	 )

rem 选择开发环境
SET /p selected=2.选择开发环境[1=2005;2=2008;3=2010;4=2012;5=2013;6=2015;7=2017;8=2019]:

if %selected%==1 (
	SET specs=win32-msvc2005
	SET vsvarbat="%VS80COMNTOOLS%..\..\VC\vcvarsall.bat"
	call !vsvarbat! %target%
	rem call "%VS80COMNTOOLS%..\..\VC\vcvarsall.bat" %target%
	goto built
) else if %selected%==2 (
	SET specs=win32-msvc2008
	SET vsvarbat="!VS90COMNTOOLS!..\..\VC\vcvarsall.bat"
	call !vsvarbat! %target%
	rem call "%VS90COMNTOOLS%..\..\VC\vcvarsall.bat" %target%
	goto built
) else if %selected%==3 (
	SET specs=win32-msvc2010
	SET vsvarbat="%VS100COMNTOOLS%..\..\VC\vcvarsall.bat"
	call !vsvarbat! %target%
	rem call "%VS100COMNTOOLS%..\..\VC\vcvarsall.bat" %target%
	goto built
) else if %selected%==4 (
	SET specs=win32-msvc2012	
	SET vsvarbat="%VS110COMNTOOLS%..\..\VC\vcvarsall.bat"
	call !vsvarbat! %target%
	rem call "%VS110COMNTOOLS%..\..\VC\vcvarsall.bat" %target%
	goto built
) else if %selected%==5 (
	SET specs=win32-msvc2013
	SET vsvarbat="%VS120COMNTOOLS%..\..\VC\vcvarsall.bat"
	call !vsvarbat! %target%
	rem call "%VS120COMNTOOLS%..\..\VC\vcvarsall.bat" %target%
	goto toolsetxp
) else if %selected%==6 (
	SET specs=win32-msvc2015
	SET vsvarbat="%VS140COMNTOOLS%..\..\VC\vcvarsall.bat"
	call !vsvarbat! %target%
	rem call "%VS140COMNTOOLS%..\..\VC\vcvarsall.bat" %target%
	goto built
) else if %selected%==7 (
	SET specs=win32-msvc2017
	for /f "skip=2 delims=: tokens=1,*" %%i in ('reg query "HKLM\SOFTWARE\Microsoft\VisualStudio\SxS\VS7" /v "15.0" /reg:32') do ( 
	    set str=%%i
		set var=%%j
		set "var=!var:"=!"
		if not "!var:~-1!"=="=" set value=!str:~-1!:!var!
	 )
	 SET value=!value!\VC\Auxiliary\Build\vcvarsall.bat
	 rem ECHO Vs2017 path is:!value! 
		SET vsvarbat="!value!"
		call !vsvarbat! %target%
		rem call "!value!" %target%
		goto built
) else if %selected%==8 (		 
	  SET specs=win32-msvc2017
	  SET vs2019path=!vs2019path!\VC\Auxiliary\Build\vcvarsall.bat
	 rem ECHO Vs2019 path is:!vs2019path! 
		SET vsvarbat="!vs2019path!"
		call !vsvarbat! %target%
		rem call "!value!" %target%
		goto built
)else (
	goto error
)

:built
rem @echo !vsvarbat! %target%
cd /d %~dp0

..\..\tools\qmake2017 -project -spec ..\..\tools\mkspecs\%specs%

rem pause