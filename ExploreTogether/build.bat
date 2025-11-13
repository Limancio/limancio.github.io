@echo off
setlocal

set METABASIC_EXE_PATH=C:\dev\meta_basic\build\win32_release.exe

set PROJECT_NAME=exp
set SOURCE_PATH=C:\dev\%PROJECT_NAME%\code
set CFLAGS=/nologo /W3 /Z7 /GS- /Gs999999
set LDFLAGS=/NODEFAULTLIB:MSVCRT /NODEFAULTLIB:LIBCMT /ENTRY:mainCRTStartup /incremental:no /opt:ref /machine:x64 /STACK:0x100000,0x100000 /manifest:no /subsystem:console opengl32.lib user32.lib gdi32.lib kernel32.lib shell32.lib

set CINCLUDES=/IC:\dev\code_lib\external\stb /IC:\dev\code_lib\external\glew\include /IC:\dev\code_lib\external\glfw\include /IC:\dev\code_lib
set LINCLUDES=/LIBPATH:C:\dev\code_lib\external\glfw\lib-vs2019 glfw3.lib /LIBPATH:C:\dev\code_lib\external\glew\lib glew32s.lib

call %METABASIC_EXE_PATH% %SOURCE_PATH%
cd %SOURCE_PATH%

if not exist ..\build mkdir ..\build
pushd ..\build

set BASE_FILES=C:\dev\%PROJECT_NAME%\meta_code\win32_main.cpp
:: set BASE_FILES=C:\dev\%PROJECT_NAME%\code\win32_main.cpp

call cl /MDd /Od %CINCLUDES% %CFLAGS% /Fewin32_debug.exe %BASE_FILES% /link /DEBUG %LDFLAGS% %LINCLUDES%
:: call cl /MDd /O2 %CINCLUDES% %CFLAGS% /Fewin32_release.exe %BASE_FILES% /link %LDFLAGS% %LINCLUDES%

popd
