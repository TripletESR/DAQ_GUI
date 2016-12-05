@echo off
title Deploying DAQ_GUI
echo =====================================================
echo This is batch script for deploying DAQ_GUI in winodws
Set desktop=%USERPROFILE%\Desktop
echo Desktop PATH: %Desktop%
echo =====================================================
pause

Set name=DAQ_GUI.exe
Set origin=%desktop%\build-DAQ_GUI-Desktop_Qt_5_7_0_MinGW_32bit-Release\release
Set destination=%desktop%\DAQ_UI_release\
Set compiler=C:\Qt\5.7\mingw53_32\bin\

rem -----------------------------------------
echo =====================================================
echo Copy *.exe to %destination%
echo =====================================================
xcopy /I/Y %origin%\%name% %destination%

rem -----------------------------------------
echo =====================================================
set /p deployFlag="  Deploy Application? (Y/N)"
echo =====================================================

IF "%deployFlag%"=="Y" (
	rem -----------------------------------------
	echo Use windepoly to create nessacary *.dll
	cd %compiler%
	echo %cd%
	windeployqt.exe %destination%\%name%
)

echo --------------- bat script finished. ----------------
pause
