#include <mod.h>
#include <comm.h>
#include <video.h>

#define false 0
#define true (!false)
com_socket far *input;
windowptr output;
char done=false;
char idx,_idx;
char ch;
char line[256];

typedef struct session
{
  Device far *Device;
  Xltr   far *Translator;
  struct session far *next;
}session;

main()
{
  short comstat;
  char len,show;
  long stime;
  long curtime;
  comstat=comopen(0x3f8,192,'8','1','N',2000,500,4,0,&input);
  output=opendisplay(1,1,80,24,BORDER|NEWLINE,0x1f,0x1f,0x2e,"ComTest");
  displayln(output,"ComStatus: *d\n",comstat);
  gettime(&stime);
  while (!done)
  {
    if (keypressed(output))
    {
      if ((ch=readch(output))==0)
      {
        ch=readch(output);
        switch(ch)
        {
          case 'D':done=true;
                   break;
        }
      }
      else
      {
        comwrite(input,&ch,1);
      }
    }
    line[idx+=comread(input,line+_idx,255-_idx)]=0;
    while (_idx<idx)
    {
      curtime=0;
      if (line[_idx++]==0x0d)
        show=true;
    }
    if (show&&idx)
    {
      displayln(output,"*s",(char far *)line);
      _idx=idx=0;
      show=false;
    }
    Relinquish(0L);
    {
      gettime(&curtime);
      if (curtime-stime>50)
      {
        stime=curtime;
        show=true;
      }
    }
  }


}
