#define extern_def
#include <mod.h>
#include <video.h>
#include "plisp.h"



line_seg far *getkbdln(windowptr input)
{
  char ch;
  char done=false;
  line_seg far *first,far *current=Allocate(sizeof(line_seg));
  first=current;
  current->idx=0;
  current->next=NULL;
  current->prior=NULL;
  do
  {
    ch=readch(input);
    switch(ch)
    {
      case 0:
        ch=readch(input);
        break;
      case 8:
        display(input,ch);
        if (current->idx)
        {
          current->idx--;
        }
        else
          if (current->prior)
          {
            current=current->prior;
            Free(current->next);
            current->next=NULL;
          }
        break;
      case 13:
        display(input,'\n');
        current->data[current->idx]=0;
        done=true;
        break;
      default:
        display(input,ch);
        current->data[current->idx++]=ch;
        if (current->idx==LINE_SEG)
        {
          current->next=Allocate(sizeof(line_seg));
          current->next->prior=current;
          current=current->next;
          current->next=NULL;
          current->idx=0;
        }
        break;
    }
  }
  while (!done);
  return(first);
}
