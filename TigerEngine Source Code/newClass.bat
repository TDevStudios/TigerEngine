@echo off
mode con cols=25 lines=4
echo Enter the class name:
set /p classname=
copy vorlage.cpp %classname%.cpp
copy vorlage.h %classname%.h

REM Refractor .h
echo #pragma once >%classname%.h
echo namespace Tiger {>>%classname%.h
echo. >>%classname%.h
echo     class %classname% {>>%classname%.h
echo. >>%classname%.h
echo.     public:>>%classname%.h
echo. >>%classname%.h
echo     };>>%classname%.h
echo }>>%classname%.h

REM Refractor .cpp
echo #include "%classname%.h" >%classname%.cpp
echo.>>%classname%.cpp
echo using namespace std;>>%classname%.cpp
echo using namespace Tiger;>>%classname%.cpp
echo.>>%classname%.cpp
echo %classname%::%classname%() { >>%classname%.cpp
echo. >>%classname%.cpp
echo } >>%classname%.cpp