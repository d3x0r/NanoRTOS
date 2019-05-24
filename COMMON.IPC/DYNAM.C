
/*
need a list of the function names that I need to locate.  They have
  to have a unique first 8 characters for now.  When I introduce librarys
  then, they can be of practically unlimited length.  Right now, you have
  to define the routines as dynamic.  As a function of that, it creates a
  function definition for that function and it's parameters accordingly.
  It also needs to add the name to a list of names.  */

#ifdef relocateable
#define Link_Code(routine)  {\
  asm mov dx,offset routine##name;\
  asm mov ds,seg routine##name    \
  asm int 60h;\
}
#else
#define Link_Code(routine)  {\
  asm mov dx,offset routine##name;\
  asm mov ax,cs; asm mov ds,ax;\
  asm int 60h;\
}
#endif

#define dynamic(return,routine,paramlist) char *routine##name=#routine; \
                                          return far routine##paramlist \
                                          Link_Code(routine)



dynamic ((windowptr),open_display,
           (short ulx,short uly,short width,short height));

