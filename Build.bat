cd /d %~dp0
@ECHO off
SETLOCAL enabledelayedexpansion
cls
COLOR 1f

ECHO.
ECHO.
ECHO   ##############################################################
ECHO   #               ��ӭʹ�� SOUI ����������                   #
ECHO   #                                ������� 2017.04.01         #
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

rem ѡ�����汾
SET /p selected=1.ѡ�����汾[1=x86;2=x64;3=x86+x64]:
if %selected%==1 (
	SET target=x86
) else if %selected%==2 (
	SET target=x64
	SET cfg=!cfg! x64
) else if %selected%==3 (
	SET target=x86
	SET targetx86andx64=1
) else (
	goto error
)

for /f "skip=2 delims=: tokens=1,*" %%i in ('%windir%\system32\reg query "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion" /v "ProgramFilesDir (x86)"') do ( 
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
	for /f "delims=" %%i in ('"%strCMD%" -nologo -version [17.0^,18.0] -prerelease -property installationPath -format value') do (
		set vs2022path=%%i
		rem echo vs2022 is:!vs2022path!
	)
)

rem ѡ�񿪷�����
SET /p selected=2.ѡ�񿪷�����[1=2005;2=2008;3=2010;4=2012;5=2013;6=2015;7=2017;8=2019;9=2022]:

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
	goto toolsetxp
) else if %selected%==7 (
	SET specs=win32-msvc2017
	for /f "skip=2 delims=: tokens=1,*" %%i in ('%windir%\system32\reg query "HKLM\SOFTWARE\Microsoft\VisualStudio\SxS\VS7" /v "15.0" /reg:32') do ( 
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
	goto toolsetxp
) else if %selected%==8 (		 
	SET specs=win32-msvc2017
	SET vs2019path=!vs2019path!\VC\Auxiliary\Build\vcvarsall.bat
	ECHO Vs2019 path is:!vs2019path! 
	SET vsvarbat="!vs2019path!"
	call !vsvarbat! %target%
	rem call "!value!" %target%
	goto toolsetxp
) else if %selected%==9 (		 
	SET specs=win32-msvc2017
	SET vs2022path=!vs2022path!\VC\Auxiliary\Build\vcvarsall.bat
	ECHO Vs2022 path is:!vs2022path! 
	SET vsvarbat="!vs2022path!"
	call !vsvarbat! %target%
	rem call "!value!" %target%
	goto toolsetxp
)else (
	goto error
)
:toolsetxp
rem XP֧��
SET /p selected=2.�Ƿ�֧��xp[1=֧��;2=��֧��]:
		if %selected%==1 (
		SET cfg=!cfg! TOOLSET_XP 
		SET supportxp=1)
:built
rem @echo !vsvarbat! %target%
rem ѡ���������
SET /p selected=3.ѡ��SOUI����ģʽ[1=ȫģ��DLL;2=ȫģ��LIB;3=�ں�LIB,���DLL(����ʹ��LUA�ű�ģ��)]:
if %selected%==1 (
	rem do nothing
) else if %selected%==2 (
	SET cfg=!cfg! LIB_ALL
) else if %selected%==3 (
	SET cfg=!cfg! LIB_CORE
) else (
	goto error
)

rem ѡ���ַ���
SET /p selected=4.ѡ���ַ���[1=UNICODE;2=MBCS]:

if %selected%==1 (
	rem do nothing
	set unicode=1
) else if %selected%==2 (
	SET unicode=0
	SET cfg=!cfg! MBCS
) else (
	goto error
)

rem ѡ��WCHAR֧��
SET /p selected=5.��WCHAR��Ϊ�ڽ�����[1=��;2=��]:
if %selected%==1 (
	rem do nothing
	SET wchar=1
) else if %selected%==2 (
	SET cfg=!cfg! DISABLE_WCHAR
	SET wchar=0
) else (
	goto error
)

rem CRT
SET /p selected=6.ѡ��CRT����ģʽ[1=��̬����(MT);2=��̬����(MD)]:
if %selected%==1 (
	SET mt=1
	SET cfg=!cfg! USING_MT
) else if %selected%==2 (
	SET mt=0
	rem do nothing
) else (
	goto error
)

rem Ϊrelease�汾���ɵ�����Ϣ
SET /p selected=7.�Ƿ�Ϊrelease�汾���ɵ�����Ϣ[1=����;2=������]:
if %selected%==1 (
	SET cfg=!cfg! CAN_DEBUG
) else if %selected%==2 (
	rem do nothing
) else (
	goto error
)
cd /d %~dp0
rem @echo %cfg%
rem ������ĿĬ������
if exist .\config\build.cfg del .\config\build.cfg
set configStr=[BuiltConfig]
echo !configStr!>>.\config\build.cfg
set configStr=UNICODE=%unicode%
echo !configStr!>>.\config\build.cfg
set configStr=WCHAR=%wchar%
echo !configStr!>>.\config\build.cfg
set configStr=MT=%mt%
echo !configStr!>>.\config\build.cfg
set configStr=SUPPORT_XP=%supportxp%
echo !configStr!>>.\config\build.cfg
rem �����������

if %specs%==win32-msvc2017 (	
	tools\qmake2017 -tp vc -r -spec .\tools\mkspecs\%specs% "CONFIG += %cfg%"
	if %targetx86andx64%==1 (
		call !vsvarbat! x64
		SET cfg=!cfg! x64
		cd /d %~dp0
		tools\qmake2017 -tp vc -r -spec .\tools\mkspecs\%specs% "CONFIG += !cfg!"
	)
	if %supportxp%==1 (
		tools\ConvertPlatformToXp -f souiprosubdir.xml
		)
) else (
	tools\qmake -tp vc -r -spec .\tools\mkspecs\%specs% "CONFIG += %cfg%"
	if %targetx86andx64%==1 (
		call !vsvarbat! x64
		SET cfg=!cfg! x64
		cd /d %~dp0
		tools\qmake2017 -tp vc -r -spec .\tools\mkspecs\%specs% "CONFIG += !cfg!"
	)
)

SET /p selected=open[o], compile[c] "soui3.sln" or quit(q) [o,c or q]?
if "%selected%" == "o" (
	if %targetx86andx64%==1 (
		soui3.sln
		soui364.sln
	) else if "%target%"=="x86" (
		soui3.sln
	) else (
		soui364.sln
	)
) else if "%selected%" == "c" (
		if %targetx86andx64%==1 (
			call !vsvarbat! x86
			call devenv soui3.sln /Clean Debug
			call devenv soui3.sln /build Debug
			call devenv soui3.sln /Clean Release
			call devenv soui3.sln /build Release
			call !vsvarbat! x64
			call devenv soui364.sln /Clean Debug
			call devenv soui364.sln /build Debug
			call devenv soui364.sln /Clean Release
			call devenv soui364.sln /build Release
		) else if "%target%"=="x86" (			
			call devenv soui3.sln /Clean Debug
			call devenv soui3.sln /build Debug
			call devenv soui3.sln /Clean Release
			call devenv soui3.sln /build Release
		) else if "%target%"=="x64" (
		call devenv soui364.sln /Clean Debug
		call devenv soui364.sln /build Debug
		call devenv soui364.sln /Clean Release
		call devenv soui364.sln /build Release
		)
) else (
	goto final
)

goto final

:error
	ECHO ѡ�����������ѡ��	
:final

rem pause