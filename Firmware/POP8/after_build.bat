@echo off
set FILE=%0
for /F %%i in ("%FILE%") do cd %%~pi
cd Lpc407x\Debug\bin
..\..\..\..\..\bin\gcc\bin\arm-none-eabi-ar.exe -d libPOP8.a ld.o
copy libPOP8.a ..\..\..\..\..\bin\src\pop8
cd ..\..\..\