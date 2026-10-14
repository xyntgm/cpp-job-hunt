@echo off
chcp 65001 >nul
cd /d D:\code\cpp-job-hunt\project2-devicehmi
set "PATH=E:\Qt\6.5.3\mingw_64\bin;E:\Qt\Tools\mingw1310_64\bin;%PATH%"
start "" "%~dp0build\devicehmi.exe"
