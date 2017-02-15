@echo off 
if exist bin\ goto compile
mkdir bin 
:compile
cls
set WarningOptions=-W4 -WX -wd4100 -wd4189
set CompilerOptions=-EHsc -nologo -Zi %WarningOptions%
set LinkerOptions=User32.lib Gdi32.lib
pushd bin  
cl %CompilerOptions% ..\src\main.cpp -link %LinkerOptions%
popd 
