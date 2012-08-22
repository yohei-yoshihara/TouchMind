rem @echo off
pushd .

REM DESTDIR=Debug/Release
SET DESTDIR=%1

REM LANG_LCID=0x0409/0x0411/...
SET LANG_LCID=%2

REM LANG_NAME=en-US/ja-JP/...
SET LANG_NAME=%3

REM DLL_POSTFIX=en_us/ja_jp/...
SET DLL_POSTFIX=%4

SET LIB_PREFIX=TouchMindMUI

SET LN_MODULE_NAME=.\%LIB_PREFIX%.dll
IF %5 EQU 0 goto DO_LOC
SET LN_MODULE_NAME=.\%LIB_PREFIX%_dummy.dll

:DO_LOC
cd %DESTDIR%
mkdir %LANG_NAME%
muirct.exe -q ..\..\TouchMind\mui_rcconfig.xml -v 2 -x %LANG_LCID% -g 0x0409 %LIB_PREFIX%_%DLL_POSTFIX%.dll %LN_MODULE_NAME% .\%LANG_NAME%\%LIB_PREFIX%.dll.mui
muirct.exe -c .\%LIB_PREFIX%.dll -v 2 -e .\%LANG_NAME%\%LIB_PREFIX%.dll.mui

IF EXIST .\%LIB_PREFIX%_dummy.dll del .\%LIB_PREFIX%_dummy.dll

popd
