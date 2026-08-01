@echo off
call "F:\VS\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
set PATH=F:\Clion\CLion 2025.2\bin\cmake\win\x64\bin;F:\VS\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja;%PATH%
cd /d "C:\Users\26289\Desktop\Farewell_Trip-main"
cmake --build build\Desktop_Qt_6_5_3_MSVC2019_64bit-Release --config Release
