#include <mod.h>
#include <video.h>
#include "plisp.h"
windowptr output;
#define false 0
#define true (!false)

extern line_seg far *getkbdln(windowptr input);

void release_line(line_seg far *line)
{
  line_seg far *next;
  while (line)
  {
    next=line->next;
    Free(line);
    line=next;
  }
}

void release_list(atom far *list)
{
}

void show_line(line_seg far *line)
{
  char idx;
  while (line)
  {
    for (idx=0;idx<line->idx;idx++)
    {
      display(output,line->data[idx]);
    }
    line=line->next;
  }
  display(output,0);
}

void show_list(atom far *list)
{
  while (list)
  {
    displayln(output,"*s",list->name);
    if (list->child)
      show_list(list->child);
    list=list->next;
  }
  if (list->parent)
    displayln(output,")");
}

void process_line(void)
{
  line_seg far *line,far *current;
  atom far *list=Allocate(sizeof(atom));
  char ch;
  short idx,_idx;
  char parens=0;

  list->next=NULL;
  list->prior=NULL;
  list->parent=NULL;
  list->child=NULL;
  list->name=NULL;
  list->type=0;
  list->data=NULL;


  line=getkbdln(output);
  show_line(line);

  current=line;
  idx=0;
  _idx=0;
  while (line->data[idx])
  {
    ch=current->data[idx++];
    if (current->idx>=LINE_SEG)
    {
      current=current->next;
      idx-=LINE_SEG;
      _idx-=LINE_SEG;
    }
    switch(ch)
    {
      case ' ':
      case '\t':
        if (idx-_idx+1)
        {
          char far *temp;
          /*we have been going over data... and we need to make a name.*/
          temp=Allocate(idx-_idx);
          list->name=temp;
          strcpy(temp,"A");
        }
        _idx=idx;
        break;
      case '(':
        list->type=LIST;
        list->name="(";
        list->child=Allocate(sizeof(atom));
        list->child->parent=list;
        list->type=NIL;
        list->name=NULL;
        list->next=NULL;
        list->prior=NULL;
        list->child=NULL;
        _idx=idx;
        break;
      case ')':
        if (list->prior)
          if (list->prior->parent)
          {
            list->prior->parent->next=list;
            list->prior->next=NULL;
            list->parent=list->prior->parent;
            list->prior=list->prior->parent;
          }
          else
          {
            displayln(output,"Misplaced end Paren!");
            return;
          }
        else
          if (list->parent)
          {
            list->parent->next=list;
            list->parent->child=NULL;
            list->prior=list->parent;
            list->parent=list->prior->parent;
          }
          else
          {
            displayln(output,"Misplaced end Paren!");
            return;
          }
        _idx=idx;
        break;
      default:
        break;
    }
  }
  show_list(list);
  release_list(list);

  release_line(line);
}

void prompt()
{
  displayln(output,">");
}

void title()
{
  displayln(output,"Welcome to Panther LISP Version á.0\n");
}

void init()
{
  output=opendisplay(1,10,80,10,BORDER|NEWLINE,0x1f,0x1f,0x2f,"Lisp Interp");
  disowndisplay(output);
}

void main()
{
  init();
  title();
  prompt();
  process_line();
}
