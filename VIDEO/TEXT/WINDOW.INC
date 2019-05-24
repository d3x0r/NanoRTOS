
;typedef struct window_type
;{
;  short ulx,uly;           /*coordinate of the upper left corner of data */
;  short width,height;      /*height and width of the data plane*/
;  short dwidth,dheight;    /*height and width of display portion*/
;  short owidth,oheight;    /*save of dwidth and dheight if iconed */
;  short xofs,yofs;         /*offset of the upper left corner of the display
;                             viewport on the data plane*/
;  short cursx,cursy;       /*Current Cursor x,y */
;  short _cursx,_cursy;     /*Last Cursor x,y*/
;  short status;            /*status-
;                                0x0080 Window is in use already
;                                0x0004 Window is offset on dataplane - x
;                                0x0008 Window is offset on dataplane - y
;                                0x0100 Filter of at end of line linefeeds
;                           */
;
;  short opts;              /*opts-
;                               0x0001 display Escape code TXT
;                               0x0002      do not process escape codes
;                               0x0004      no cursor
;                               0x0010      no scroll
;                               0x0020      Add Border
;                               0x0(00xx)00 Add Shadow depth xx (1-3)
;                               0x1000      no dataplane- moving windows
;                           */
;  key_buffer far *keys;
;  char cursattr,attr,borattr,backattr;
;  short far *data;
;  short far *border;
;  short far *shadow;
;  struct window_type far *next_window,far *previous_window;
;} window_type;


WINDOW STRUC
        ulx             dw      ?
        uly             dw      ?
        width           dw      ?
        height          dw      ?
        dwidth          dw      ?
        dheight         dw      ?
        owidth          dw      ?
        oheight         dw      ?
        xofs            dw      ?
        yofs            dw      ?
        cursx           dw      ?
        cursy           dw      ?
        _cursx          dw      ?
        _cursy          dw      ?
        status          dw      ?
        opts            dw      ?
        keys            dd      ?
        cursattr        db      ?
        attr            db      ?
        borattr         db      ?
        backattr        db      ?
        data            dd      ?
        border          dd      ?
        shadow          dd      ?
        next            dd      ?
        previous        dd      ?
ENDS

NO_SCROLL equ 10h
