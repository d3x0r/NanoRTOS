#include <alloc.h>
#define Allocate(a) malloc(a)
#define Free(a) free(a)

typedef struct video_line_seg
{
  short count;
  char far *data;
  short vid_start;
  struct video_line_seg far *next;
} video_line_seg;

typedef struct window_type
{
  short ulx,uly;           /*coordinate of the upper left corner of data */
  short width,height;      /*height and width of the data plane*/
  short dwidth,dheight;    /*height and width of display portion*/
  short xofs,yofs;         /*offset of the upper left corner of the display
                             viewport on the data plane*/
  short cursx,cursy;       /*Current Cursor x,y */
  short _cursx,_cursy;     /*Last Cursor x,y*/
  short status;            /*status-
                                0x0080 Window is in use already
                                0x0004 Window is offset on dataplane - x
                                0x0008 Window is offset on dataplane - y
                                0x0100 Filter of at end of line linefeeds
                           */

  short opts;              /*opts-
                               0x0001 display Escape code TXT
                               0x0002      do not process escape codes
                               0x0004      no cursor
                               0x0010      no scroll
                               0x0020      Add Border
                               0x0(00xx)00 Add Shadow depth xx (1-3)
                               0x1000      no dataplane- moving windows
                           */
  char cursattr,attr,borattr,backattr;
  short page_size;
  char far *data;
  char far *border;
  char far *shadow;
  struct window_type far *next_window,far *previous_window;
  video_line_seg far *line_segments[];
} window_type;

window_type far *Current_Window;
window_type far *First_Window;
short vwidth=80,vheigth=480;

char covered[640];
char mark=0;

void build_lines(short begin,short end)
{
  /*this routine goes though the lines from begin to end(inclusive) and
    computes the
    video line segments for each window that has a portion of data on that
    line.  */
  window_type far *temp;
  window_type far *cover;
  video_line_seg far *tseg,far *nseg;
  short start,wend;
  short base;
  short cnt;
  short wline;
  for (;begin<end;begin++)
  {
    temp=Current_Window;
    mark++;
    base=begin*vwidth;
    while (temp)
    {
      if (temp->uly<=begin&&(temp->uly+temp->dheight)>=begin)
      {
        /*the window is on the line*/
        wline=begin-temp->uly;
        tseg=temp->line_segments[wline];
        while (tseg)
        {
          nseg=tseg->next;
          Free(tseg);
          tseg=nseg;
        }
        temp->line_segments[wline]=0L;
        start=temp->ulx;
        wend=temp->dwidth;
        do
        {
          cnt=0;
          while ((covered[start+cnt]==mark)&&cnt<wend)
            cnt++;
          if (cnt<wend)
          {
            start+=cnt;
            wend-=cnt;
            cnt=0;
            while (!(covered[start+cnt]==mark)&&cnt<wend)
            {
              covered[start+cnt]=mark;
              cnt++;
            }
            /* we now have our first segment, which is marked by start,and cnt is
               the width of the segment showing */
            wend-=cnt;
            {
              if (temp->line_segments[wline])
              {
                tseg=temp->line_segments[wline];
                while (tseg->next) tseg=tseg->next;
                tseg->next=Allocate(sizeof(video_line_seg));
                tseg=tseg->next;
              }
              else
                tseg=temp->line_segments[wline]=Allocate(sizeof(video_line_seg));

              tseg->next=0L;
              tseg->vid_start=base+(start);
              tseg->data=temp->data+
                         (wline*temp->dwidth)+
                         (start-temp->ulx);
              tseg->count=cnt;
            }
          }
        }
        while(wend);
      }
      temp=temp->previous_window;  /*go from front to back*/
    }
  }
}

plot(window_type far *window,short x,short y,char color)
{

}


dump_lines()
{
  short cnt,value,c;
  unsigned short bit;
  window_type far *t=First_Window;
  for (cnt=0;cnt<50;cnt++)
  {
    if (t->line_segments[cnt])
    {
      printf("Made Line! %d %d %p-",cnt,
                   t->line_segments[cnt]->vid_start,
                   t->line_segments[cnt]->data);

      for (c=0;c<t->line_segments[cnt]->count ;c++)
        printf("1111 1111 ");
      printf("\n");
    }
  }
}



main()
{
  window_type far *t;
  short cnt;
  t=Current_Window=First_Window=malloc(sizeof(window_type)+
                                 50*sizeof(video_line_seg));
  t->next_window=NULL;
  t->previous_window=NULL;
  t->uly=50;
  t->ulx=43/8;
  t->dwidth=23/8;
  t->dheight=50;
  t->width=23/8;
  t->height=50;
  t->page_size=t->width*t->height;
  t->data=malloc(t->width*t->height*4);
  for (cnt=0;cnt<50;cnt++)
  {
    t->line_segments[cnt]=NULL;
  }
  printf("\n\n");
  for (cnt=0;cnt<20;cnt++)
  {
    t->ulx=cnt/8;
    build_lines(50,53);
    dump_lines();
  }
}
