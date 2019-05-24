typedef struct video_line_type
{
	unsigned short count;				 /*number of words to move from window to screen*/
	short far *window_addr;			 /*first pos. of window data plane*/
	struct video_line_type far *next_line_seg;
	char far *video_addr;				 /*address of the beginning of the line on the
																 video screen.*/
} video_line_type;

typedef struct key_buffer
{
	short head,tail,size;
	char buffer[];
}key_buffer;

typedef struct task_link
{
	short links_used;
	module far *TCB[6];
	struct task_link far *next;
}task_link;

typedef void (far *mhandproc)(short mousex,short mousy,char buttons);

typedef struct window_type
{
	short ulx,uly;					 /*coordinate of the upper left corner of data */
	short width,height;			 /*height and width of the data plane*/
	short dwidth,dheight;		 /*height and width of display portion*/
	short owidth,oheight;		 /*save of dwidth and dheight if iconed */
	short xofs,yofs;				 /*offset of the upper left corner of the display
														 viewport on the data plane*/
	short cursx,cursy;			 /*Current Cursor x,y */
	short _cursx,_cursy;		 /*Last Cursor x,y*/
	short status;						 /*status-
																0x0080 Window is in use already
																0x0004 Window is offset on dataplane - x
																0x0008 Window is offset on dataplane - y
																0x0100 Filter of at end of line linefeeds
													 */

	short opts;							 /*opts- Defines follow*/
	key_buffer far *keys;		 /*keyboard buffer for this window*/
	char cursattr,attr,borattr,backattr;
	short far *data;
	char far *border;
	short far *shadow;
	char title[16];
	struct window_type far *next_window,far *previous_window;
	mhandproc mouse_handle;
	video_line_type far **lines;
	task_link far *links;
} window_type;

typedef window_type far *windowptr;

/*These are the defines for the opts field of the window_structure*/

#define NO_KEYS			0x0001
#define NODO_ESC		0x0002
#define NO_CURSOR		0x0004
#define CHANGE_DATA 0x0008
#define NO_SCROLL		0x0010
#define BORDER			0x0020
#define SHADOW1			0x0100
#define SHADOW2			0x0200
#define SHADOW3			0x0300
#define NEWLINE			0x0400
#define NO_DATA			0x1000
#define NO_TITLE		0x2000
#define ICON				0x4000
#define CLOSED			0x8000

/*These are the defines for the attribute fields of the window_structure*/

#define INTENSITY		0x08
#define BLINK				0x80

#define BLACK				0x00
#define BLUE				0x01
#define GREEN				0x02
#define CYAN				0x03
#define RED					0x04
#define MAGENTA			0x05
#define BROWN				0x06
#define GREY				0x07
#define DK_GREY		 INTENSITY|BLACK
#define LT_BLUE		 INTENSITY|BLUE
#define LT_GREEN	 INTENSITY|GREEN
#define LT_CYAN		 INTENSITY|CYAN
#define LT_RED		 INTENSITY|RED
#define LT_MAGENTA INTENSITY|MAGENTA
#define YELLOW		 INTENSITY|BROWN
#define WHITE			 INTENSITY|GREY

#define ON_BLACK		0x00
#define ON_BLUE			0x10
#define ON_GREEN		0x20
#define ON_CYAN			0x30
#define ON_RED			0x40
#define ON_MAGENTA	0x50
#define ON_BROWN		0x60
#define ON_GREY			0x70


#define KEY_BUF_SIZE 50


/*operations for moddisplay */

enum windowmods
{
				WEND_MOD,
				WMOVE_WINDOW,
				WMOVE_MOUSE,
				WSELECT_WINDOW,
				WREMOVE_DATA,
				WRESTORE_DATA,
				WICON_WINDOW,
				WDELTA_MOVE_W,
				WDELTA_MOVE_M,
				WCLOSE_WINDOW,
				WRESIZEX,
				WSCROLL_UD,
				WRESIZEY,
				WSCROLL_LR,
				WSCROLL_PUD,
				WSCROLL_PLR,
				WRESTORE_WINDOW,
				WHIDE_WINDOW,
				WCHANGE_MHAND
};

/* moddisplay (window,ICON_WINDOW,MOVE_WINDOW(1,1),END_MOD);
	 moddipslay (window,6					 ,1,1,1						,0);
	 Cursor Positions are 0 based.
	 Window Positions are 1 based.				*/

#define END_MOD						WEND_MOD					/*terminates modification list*/
#define MOVE_WINDOW(x,y)	WMOVE_WINDOW,x,y	/*Moves window*/
#define MOVE_MOUSE(x,y)		WMOVE_MOUSE,x,y		/*Moves the mouse position*/
#define SELECT_WINDOW			WSELECT_WINDOW		/*brings window the the foreground*/
#define REMOVE_DATA				WREMOVE_DATA			/*makes the data area of window invisible*/
#define RESTORE_DATA			WRESTORE_DATA			/*restores the data area visibility*/
#define ICON_WINDOW				WICON_WINDOW			/*turns the window into an iconified form*/
#define RESTORE_WINDOW		WRESTORE_WINDOW		/*turns a window icon into a full window*/
#define DELTA_MOVE_W(x,y) WDELTA_MOVE_W,x,y /*move window a delta amount*/
#define DELTA_MOVE_M(x,y) WDELTA_MOVE_M,x,y /*move the mouse a delta amount*/
#define CLOSE_WINDOW			WCLOSE_WINDOW			/*removes the window from display altogether*/
#define RESIZEX(x)				WRESIZEX,x				/*change window width a delta amount*/
#define RESIZEY(y)				WRESIZEY,y				/*change window height a delta amount*/
#define SCROLL_UP					WSCROLL_UD,0			/*scroll the window up one line*/
#define SCROLL_DOWN				WSCROLL_UD,1			/*scroll the window down one line*/
#define SCROLL_LEFT				WSCROLL_LR,0			/*scroll the window left one character*/
#define SCROLL_RIGHT			WSCROLL_LR,1			/*scroll the window right one character*/
#define PAGE_UP						WSCROLL_PUD,0			/*scroll the window up a page*/
#define PAGE_DOWN					WSCROLL_PUD,1			/*scroll the window down a page*/
#define PAGE_LEFT					WSCROLL_PLR,0			/*scroll the window left a page*/
#define PAGE_RIGHT				WSCROLL_PLR,1			/*scroll the window right a page*/
#define HIDE_WINDOW				WHIDE_WINDOW			/*moves the window to the backround*/
#define CHANGE_MHAND(mh)	WCHANGE_MHAND,(long)mh /*changes the mouse handle routine*/



#pragma warn -rvl
#ifndef VIDEO
dynamic( short,keypressed,(windowptr window));
dynamic( char,readch,(windowptr window));
dynamic( void,Clr_window,(windowptr window));
dynamic( windowptr,
					 opendisplay,
					 (short ulx,short uly,short width,short height,short opts,
					 char data_attr,char border_attr,char cursor_attr,char far *title));
dynamic( void,get_xy,
					 (windowptr window,short far *x,short far *y));
dynamic( void,display,(windowptr window,char charac));
dynamic ( void,displayln,(windowptr window,char far *line,...));
dynamic( void,position,(windowptr window,int x,int y));
dynamic( void,setattr,(windowptr window,char attr));
dynamic( char,getattr,(windowptr window));
dynamic( void,clr_display,(windowptr window,char notes));
dynamic(short,getdisplay,(short far *x,short far *y,windowptr far *window));
dynamic(short,moddisplay,(windowptr window,...));
dynamic(short,closedisplay,(windowptr window));
dynamic(windowptr,dupdisplay,(windowptr window,short opts));
dynamic(short,disowndisplay,(windowptr window));
#else
dynamic(short,getdisplay,(short far *x,short far *y,windowptr far *window));
dynamic(short,moddisplay,(windowptr window,...));
dynamic(char,rawkeypressed,(void));
dynamic(char,rawreadch,(void));
#endif
#pragma warn .rvl
