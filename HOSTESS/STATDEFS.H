/*misplaced.  Needs to go in global.def */

#define back_queue(buf,outdata) (((buf)->data[(buf)->tail])=outdata)
#define queue(buf,outdata)      (((buf)->data[(buf)->head])=outdata)
#define inc_pointer(buf,what) ((buf)->what=((buf)->what+1)&(buf)->size)
#define dec_pointer(buf,what) ((buf)->what=((buf)->what-1)&(buf)->size)

/*MACROS for the setting,resetting, changing,and testing of status words*/

#define SET(word,bit)                           {statuses[word]|=bit;}
#define CHANGE(word,bit)                        {statuses[word]^=bit;}

#define BIT_SPREAD(width)           ((1<<width)-1)
#define MASK(position,width)        (((1<<width)-1)<<position)

#define STATMASK(name) (((1<<name##WD)-1)<<name##PO)

#define SET_VALUE(stat,pos,width,value)  (statuses[stat]&=                        \
                                       ~(((1<<width)-1)<<pos),            \
                                            statuses[stat]|=            \
                                                (value&((1<<width)-1))\
                                                       <<pos   )

#define CSET_VALUE(name,value)  (statuses[name##ST]&=                        \
                                       ~(((1<<name##WD)-1)<<name##PO),            \
                                            statuses[name##ST]|=            \
                                                (value&((1<<name##WD)-1))\
                                                       <<name##PO   )

#define GET_VALUE(stat,pos,width) ((statuses[stat]&(((1<<width)-1)<<pos))       \
                                                 >>pos)

#define CGET_VALUE(name) ((statuses[name##ST]&(((1<<name##WD)-1)<<name##PO))       \
                                                 >>name##PO)

/*#define CGET_VALUE(name) ((statuses[name##ST]&MASK(name##PO,name##WD))       \
                                                 >>name##PO)*/

/*Defines for naming the value fields in status words */

#define PROTDELAYF         PROTDELAYF_ST,PROTDELAYF_PO,PROTDELAYF_WD
#define PROTDELAYF_ST      15
#define PROTDELAYF_PO      0
#define PROTDELAYF_WD      16

