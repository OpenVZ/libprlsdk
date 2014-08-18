@echo off

rem Re-executing the script via cmd /c to prevent from accidental exiting
rem user's interactive shell when using "exit" command in the script
if not "%1" == "build" (
	cmd /c %0 build
	pause
	goto :eof
)

setlocal

set orig=%~dp0
set crt_x86=%orig%..\..\z-Build\Release
set crt_amd64=%orig%..\..\z-Build\Release64
set crt_src=%orig%..\..\Sources\Libraries\msvcrt_compat
set vsenv="%VS120COMNTOOLS%..\..\VC\vcvarsall.bat"

rem Check for required tools
call :checktool tar
call :checktool perl
call :checktool patch


pushd %orig%

rem If executed from a VS environment, then build only the appropriate architecture
rem Otherwise build both architectures
if not "%VCINSTALLDIR%" == "" (
	if /i "%platform%" == "x64" (
		call :build amd64
	) else (
		call :build x86
	)
) else (
	if not exist %vsenv% (
		echo Visual Studio 2013 not found, cannot continue
		goto :fatal
	)
	call :build_in_env x86
	call :build_in_env amd64
)


echo All done
goto :eof


:build_in_env
	setlocal
	call %vsenv% %1
	call :build %1
	endlocal
	exit /b 0


:patch
	call :cmd patch -p0 < %orig%\%1
	exit /b 0

:applypatches
	call :patch openssl-1.0.1c-configure.patch
	call :patch openssl-1.0.1c-export-func.patch
	call :patch openssl-1.0.1e-cve-2014-0160.patch
	call :patch openssl-1.0.1c-wdkruntime.patch
	exit /b 0


:chkruntime
	setlocal
	if /i "%arch%" == "amd64" (
		set arch_suff=64
		set qmakecfg= -config x86_64
	)
	if exist %orig%..\..\z-Build\Release%arch_suff%\msvcrt_over.obj exit /b 0
	rem If qmake is available, try to build runtime, otherwise ask the user to do
	where qmake 2>nul
	if not "%errorlevel%"=="0" goto :query
	call :cmd pushd %crt_src%
	call :cmd qmake %qmakecfg%
	call :cmd nmake /nologo -f Makefile%arch_suff% clean release
	if exist %orig%..\..\z-Build\Release%arch_suff%\msvcrt_over.obj exit /b 0

	:query
	echo Runtime library files don't exist. Please, build runtime library first,
	echo or execute the script from a qmake environment to make it built automatically.
	echo To build the library yourself, go to Sources\Libraries\msvcrt_compat
	echo and run 'qmake' then 'nmake release'. Don't forget about x64 version.
	goto :fatal


:build
	setlocal
	set arch=%1
	set opensslver=1.0.1c

	call :chkruntime %arch%

	set bldir=win32_build_%arch%
	call :cmd mkdir %bldir%
	call :cmd pushd %bldir%
	call :cmd tar xf ../openssl-%opensslver%.tar.gz
	call :applypatches

	call :cmd pushd openssl-%opensslver%
	if /i "%arch%" == "x86" (
		call :cmd perl Configure enable-static-engine VC-WIN32 -L%crt_x86%
		call :cmd cmd.exe /c ms\do_ms.bat
	) else (
		call :cmd perl Configure no-asm enable-static-engine VC-WIN64A -L%crt_amd64%
		call :cmd cmd.exe /c ms\do_win64a.bat
	)

	call :cmd nmake /nologo -f ms\ntdll.mak
	exit /b 0


:checktool
	setlocal
	set tool=%1
	where %tool% > nul 2>nul
	if not "%errorlevel%"=="0" (
		echo Command "%tool%" is not available, cannot continue
		goto :fatal
	)
	exit /b 0

:cmd
	%*
	if not "%errorlevel%"=="0" (
		echo Error: %*
		goto :fatal
	)
	exit /b 0


:fatal
	exit 1
