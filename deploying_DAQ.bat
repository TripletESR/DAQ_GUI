@echo off
title Deploying DAQ_GUI
echo =====================================================
echo This is batch script for deploying DAQ_GUI in winodws
echo =====================================================
pause

Set name=DAQ_GUI.exe
Set origin=C:\Users\Triplet-ESR\Desktop\build-DAQ_GUI-Desktop_Qt_5_7_0_MinGW_32bit-Release\release
Set destination=C:\Users\Triplet-ESR\Desktop\DAQ_UI_release\
Set compiler=C:\Qt\5.7\mingw53_32\bin\

rem -----------------------------------------
echo Copy *.exe to %destination%
xcopy %origin%\%name% %destination%
pause

rem -----------------------------------------
set /p deployFlag="Deploy: Y/N?"

IF "%deployFlag%"=="Y" (
	rem -----------------------------------------
	echo Use windepoly to create nessacary *.dll
	cd %compiler%
	echo %cd%
	windeployqt.exe %destination%\%name%
)
pause
