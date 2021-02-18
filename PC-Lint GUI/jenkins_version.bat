:: Run Jenkins version script

@echo off

for /F "delims=" %%i in ('git rev-parse HEAD') do set BUILD_REVISION=%%i
for /f "tokens=2 delims==" %%a in ('wmic OS Get localdatetime /value') do set "dt=%%a"
set "YY=%dt:~2,2%" & set "YYYY=%dt:~0,4%" & set "MM=%dt:~4,2%" & set "DD=%dt:~6,2%"
set "HH=%dt:~8,2%" & set "Min=%dt:~10,2%" & set "Sec=%dt:~12,2%"
set "FULL_STAMP=%YYYY%-%MM%-%DD%_%HH%-%Min%-%Sec%"

echo #ifndef JENKINS_H > D:\Users\Ayman\Desktop\PC-Lint GUI\PC-Lint GUI\Jenkins.h
echo #define JENKINS_H >> D:\Users\Ayman\Desktop\PC-Lint GUI\PC-Lint GUI\Jenkins.h
echo #define APPLICATION_NAME "PC-Lint GUI" >> D:\Users\Ayman\Desktop\PC-Lint GUI\PC-Lint GUI\Jenkins.h
echo #define BUILD_VERSION "%LINTY_MAJOR_BUILD%.%LINTY_MINOR_BUILD%" >> D:\Users\Ayman\Desktop\PC-Lint GUI\PC-Lint GUI\Jenkins.h
echo #define BUILD_DATE "%FULL_STAMP%" >> D:\Users\Ayman\Desktop\PC-Lint GUI\PC-Lint GUI\Jenkins.h
echo #define BUILD_COMMIT "%BUILD_REVISION%" >> D:\Users\Ayman\Desktop\PC-Lint GUI\PC-Lint GUI\Jenkins.h
echo #endif // JENKINS_H >> D:\Users\Ayman\Desktop\PC-Lint GUI\PC-Lint GUI\Jenkins.h

type D:\Users\Ayman\Desktop\PC-Lint GUI\PC-Lint GUI\Jenkins.h