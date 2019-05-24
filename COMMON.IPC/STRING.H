/*  string.h */

#ifndef NULL
#include "m:\common.ipc\_null.h"
#endif

//#ifndef strnicmp
//#include "m:\common.ipc\mod.h"
//#endif

//#ifndef strcat
//#include "m:\common.ipc\mod.h"
//#endif

//#ifndef strncpy
//#include "m:\common.ipc\mod.h"
//#endif

//#ifndef strlen
//#include "m:\common.ipc\mod.h"
//#endif

extern char far * (far _fstrtok)(char far *__s1, const char far *__s2);
#define strtok _fstrtok

#define strncmp strnicmp
#define strcmp(a,b) strnicmp(a,b,256)

