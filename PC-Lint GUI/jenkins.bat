:: Run Jenkins deployment script

@echo off

SET APP_NAME=PC-Lint GUI
set APP_EXE=%APP_NAME%.exe
set DEBUG_APP="D:\Users\Ayman\Desktop\PC-Lint GUI\PC-Lint GUI Build\%APP_EXE%"
set RELEASE_APP="D:\Users\Ayman\Desktop\PC-Lint GUI\PC-Lint GUI Build\%APP_EXE%"
set BUILD_FOLDER=D:\Users\Ayman\Desktop\PC-Lint GUI\build
set WINDEPLOYQT_APP="D:\Qt\5.14.1\mingw73_32\bin\windeployqt.exe"
set WINDEPLOYQT_ARGUMENTS=--force --no-quick-import --no-translations --no-webkit2 --no-angle --no-opengl-sw --no-system-d3d-compiler

set ZIP_APP=7z
set ZIP_ARGUMENTS=a -t7z "%APP_NAME%_%LINTY_MAJOR_BUILD%.%LINTY_MINOR_BUILD%.7z" -m0=lzma2 -mx=9 -aoa "%BUILD_FOLDER%"

echo [APP NAME]: %LINTY_APPNAME%
echo [MAJOR BUILD]: %LINTY_MAJOR_BUILD%
echo [MINOR BUILD]: %LINTY_MINOR_BUILD%

if exist "%BUILD_FOLDER%" rd /s /q "%BUILD_FOLDER%"
mkdir "%BUILD_FOLDER%"

:: Copy release or debug
if "%1" == "DEBUG" (
	copy /B /Y %DEBUG_APP% "%BUILD_FOLDER%"
) else if "%1" == "RELEASE" (
	copy /B /Y %RELEASE_APP% "%BUILD_FOLDER%"
) else (
	echo Unknown configuration provided: %1
	exit /b 1
)

echo [BUILDING USING %1 CONFIGURATION]
%WINDEPLOYQT_APP% "%BUILD_FOLDER%\%APP_EXE%" %WINDEPLOYQT_ARGUMENTS%

:: Rename the file to show version information
echo [FILE RENAME]
pushd "%BUILD_FOLDER%"
ren "%APP_EXE%" "%APP_NAME%_%LINTY_MAJOR_BUILD%.%LINTY_MINOR_BUILD%.exe"
popd

:: Zip file up
echo [ZIP UP]
%ZIP_APP% %ZIP_ARGUMENTS%

:: Move to folder
echo [FOLDER MOVE]
move /Y "%APP_NAME%_%LINTY_MAJOR_BUILD%.%LINTY_MINOR_BUILD%.7z" "%BUILD_FOLDER%"

:: Increment minor build number
echo [NEW BUILD NUMBER]
set NEW_MINOR=%LINTY_MINOR_BUILD%
set /A NEW_MINOR=NEW_MINOR+1
setx LINTY_MINOR_BUILD %NEW_MINOR%