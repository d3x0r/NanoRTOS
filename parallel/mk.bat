call asm lptirq
call mc lptdrv
call mc rawprint
call ml rawprint lptdrv lptirq \video\text\vidlib
copy rawprint.exe \
