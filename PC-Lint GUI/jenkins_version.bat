:: Run Jenkins version script

@echo off

for /F "delims=" %%i in ('git rev-parse HEAD') do set BUILD_REVISION=%%i
for /f "tokens=2 delims==" %%a in ('wmic OS Get localdatetime /value') do set "dt=%%a"
set "YY=%dt:~2,2%" & set "YYYY=%dt:~0,4%" & set "MM=%dt:~4,2%" & set "DD=%dt:~6,2%"
set "HH=%dt:~8,2%" & set "Min=%dt:~10,2%" & set "Sec=%dt:~12,2%"
set "FULL_STAMP=%YYYY%-%MM%-%DD%_%HH%-%Min%-%Sec%"

if %errorlevel% neq 0 exit /b 1

set APP_PATH=D:\Users\Ayman\Desktop\PC-Lint GUI\PC-Lint GUI

echo #pragma once > "%APP_PATH%\Jenkins.h"
echo #define APPLICATION_NAME "PC-Lint GUI" >> "%APP_PATH%\Jenkins.h"
echo #define BUILD_VERSION "%LINTY_MAJOR_BUILD%.%LINTY_MINOR_BUILD%" >> "%APP_PATH%\Jenkins.h"
echo #define BUILD_DATE "%FULL_STAMP%" >> "%APP_PATH%\Jenkins.h"
echo #define BUILD_COMMIT "%BUILD_REVISION%" >> "%APP_PATH%\Jenkins.h"

type "%APP_PATH%\Jenkins.h"