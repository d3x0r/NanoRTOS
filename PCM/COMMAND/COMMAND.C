#include <mod.h>
#include <video.h>

#define false 0
#define true (!false)

/* This is the main heart of the PC Mynah product.  It takes user/macro/
   whatever input, and 1> defines sessions, and manages where the resources
   are going, activates pumps and coordinates session structures.
*/

windowptr output;

short getline(windowptr input,char far *buffer,short maxlen,char echo)
{
  char ch,done=false,idx=0;
  do
  {
    ch=readch(input);
    if (ch<32)
    {
      if (ch==27)
      {
        while(idx)
        {
          if (echo)
            displayln(output,"\b \b");
          idx--;
        }
      }
      if (ch==13)
      {
        if (echo)
          display(output,'\n');
        done=true;
      }
      if (ch==8&&idx)
      {
        if (echo)
          displayln(output,"\b \b");
        idx--;
      }
    }
    else
    {
      buffer[idx++]=ch;
      if (echo)
      {
        display(output,ch);
        display(output,0);
      }
      if (idx==maxlen)
      {
        if (echo)
          displayln(output,"\b \b");
        idx--;
      }
    }
  }
  while (!done);
  buffer[idx]=0;
  return(idx);
}

void process(char far *command)
{

}

void main()
{
  char thread;
  char far *line=Allocate(50);
  output=opendisplay(1,1,40,10,BORDER|NEWLINE,0x71,0x79,0x2f,"Command");
  if (fork(CHILD))
  {
    while(1)
    {
      getline(output,line,20,true);
      process(line);
    }
  else
    while(1)
    {
      line=Import(devicename);
      process(line);
    }

}
