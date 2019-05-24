#include "hostess.h"
#include "ipchost.h"

#define TRMGETC    0
#define TRMWRITE   2


line_table lines[16];
card_entry far *card_data[16];
break_info breaks[16];
char curline=0;


