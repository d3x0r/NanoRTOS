/*  stdlib.h */

#ifndef NULL
#include "m:\common.ipc\_null.h"
#endif

//#define atoi
//#define atol
//#define itoa

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

/*----- will need these some day --------------*
                                               |
#ifndef _DIV_T                                 |
#define _DIV_T                                 |
typedef struct {                               |
        int     quot;                          |
        int     rem;                           |
} div_t;                                       |
#endif                                         |
                                               |
#ifndef _LDIV_T                                |
#define _LDIV_T                                |
typedef struct {                               |
        long    quot;                          |
        long    rem;                           |
} ldiv_t;                                      |
#endif                                         |
                                               |
ldiv_t  ldiv(long __numer, long __denom);      |
div_t   div(int __numer, int __denom);         |
                                               */

