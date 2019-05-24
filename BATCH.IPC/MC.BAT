@echo off
echo ;This file is auto built by mc.bat >%IPCBASE%\batch.ipc\medres.fil
echo -I%IPCBASE%\common.ipc -w-pro -2 -r- -ml! -c -Drelocatable >>%IPCBASE%\batch.ipc\medres.fil
if .%1==. goto error2
if .%2==. goto do1
goto check3
:do1
bcc @%IPCBASE%\batch.ipc\medres.fil %1.c
goto end1
:check3

if .%3==. goto do2
goto check4
:do2
bcc @%IPCBASE%\batch.ipc\medres.fil %1.c %2.c
goto end1
:check4

if .%4==. goto do3
goto check5
:do3
bcc @%IPCBASE%\batch.ipc\medres.fil %1.c %2.c %3.c
goto end1
:check5

if .%5==. goto do4
goto error1
:do4
bcc @%IPCBASE%\batch.ipc\medres.fil %1.c %2.c %3.c %4.c
goto end1

:error2
echo No Files Specified
goto end1
:error1
echo Too many files to compile for batch procedure
:end1
