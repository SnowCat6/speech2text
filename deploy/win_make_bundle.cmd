@echo OFF

REM Папка с расположением скомпилированного проекта
set BUILD=%1
REM Название бинарной программы
set APP=%2
REM Версия программы программы
set VER=%3

REM Путь к инструментарию разработчика
set DEPLOYT_TOOL="C:\devtools\msys64\mingw64\bin\windeployqt.exe"
REM Путь библиотекам из файла библиотек для Windows
set DEPLOY_LIBS="C:\devtools\msys64\mingw64\bin"

set "BUILD=%BUILD:/=\%"
set BASE=%~dp0

set day=%DATE:~0,2%
set month=%DATE:~3,2%
set year=%DATE:~6,2%
set DATE=%day%_%month%_%year%

set DEPLOY=%BUILD%\%APP%-%VER%-%DATE%

echo Make deploy %BUILD%\%APP%

del /S/Q %DEPLOY% >NUL 2>NUL
%DEPLOYT_TOOL% --dir %DEPLOY% %BUILD%/%APP%.exe >NUL
xcopy /y %BUILD%\%APP%.exe %DEPLOY% >NUL

for /f "tokens=*" %%i in (%BASE%\win_libs_bundle.txt) DO (
    xcopy /S/E "%DEPLOY_LIBS%\%%i" %DEPLOY% >NUL
)
