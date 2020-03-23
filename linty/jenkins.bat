:: Run Jenkins deployment script

@echo off

SET APP_NAME=linty
set APP_EXE=%APP_NAME%.exe
set DEBUG_APP="D:\Users\Ayman\Desktop\Linty\linty\Debug\%APP_EXE%"
set RELEASE_APP="D:\Users\Ayman\Desktop\Linty\linty\Release\%APP_EXE%"
set BUILD_FOLDER=D:\Users\Ayman\Desktop\Linty\build
set WINDEPLOYQT_APP="D:\Qt\5.14.1\mingw73_32\bin\windeployqt.exe"
set WINDEPLOYQT_ARGUMENTS=--force --no-quick-import --no-translations --no-webkit2

set ZIP_APP=7z
set ZIP_ARGUMENTS=a -t7z %APP_NAME%_%LINTY_MAJOR_BUILD%.%LINTY_MINOR_BUILD%.7z -m0=lzma2 -mx=9 -aoa %BUILD_FOLDER%

echo Application name: %LINTY_APPNAME%
echo Major build: %LINTY_MAJOR_BUILD%
echo Minor build: %LINTY_MINOR_BUILD%

if exist %BUILD_FOLDER% rd /s /q %BUILD_FOLDER%
mkdir %BUILD_FOLDER%

:: Copy release or debug
if "%1" == "DEBUG" (
	copy /B /Y %DEBUG_APP% %BUILD_FOLDER%
) else if "%1" == "RELEASE" (
	copy /B /Y %RELEASE_APP% %BUILD_FOLDER%
) else (
	echo Unknown configuration provided: %1
	exit /b 1
)

echo Building using %1 configuration
%WINDEPLOYQT_APP% %BUILD_FOLDER%\%APP_EXE% %WINDEPLOYQT_ARGUMENTS%

:: Rename the file to show version information
pushd %BUILD_FOLDER%
ren %APP_EXE% %APP_NAME%_%LINTY_MAJOR_BUILD%.%LINTY_MINOR_BUILD%.exe
popd

:: Zip file up
%ZIP_APP% %ZIP_ARGUMENTS%

:: Move to folder
move /Y %APP_NAME%_%LINTY_MAJOR_BUILD%.%LINTY_MINOR_BUILD%.7z %BUILD_FOLDER%

:: Increment minor build number
set /A LINTY_MINOR_BUILD=LINTY_MINOR_BUILD+1