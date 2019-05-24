@echo off
echo /c/s/m/l/v/Ld:\tools\bcc\lib + >mllink.fil
echo %IPCBASE%\common.ipc\mmodhead %1 %2 %3 %4 %5>>mllink.fil
echo %1       >>mllink.fil
echo %1       >>mllink.fil
echo %IPCBASE%\common.ipc\jimos cl mathl >>mllink.fil
tlink @mllink.fil
del mllink.fil
