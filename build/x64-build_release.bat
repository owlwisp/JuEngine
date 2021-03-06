
:: Copyright (c) 2016 Juan Delgado (JuDelCo)
:: License: GPLv3 License
:: GPLv3 License web page: http://www.gnu.org/licenses/gpl.txt

@echo off

set PATH=%PATH%;C:\Programacion\CppLibs\TDM-GCC_4.9.2-x64\bin
set PATH=%PATH%;C:\Programacion\CppLibs\CMake_3.1.0-x64\bin
set DIR=%CD%

:build

cd %DIR%
cd ../

cls
cmake -D BUILD_CPU_ARCH:STRING=x64 -D CMAKE_BUILD_TYPE:STRING=Release -D CMAKE_INSTALL_PREFIX:PATH="" build/x64_Release/
cls
cmake -L -N build/x64_Release/
echo.

cd build/x64_Release
mingw32-make all
mingw32-make install

echo.
echo Press any key to build again (or press CTRL+C to exit) ...
pause > nul

goto :build
