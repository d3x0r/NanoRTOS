#include "mod.h"
#define false 0
#define true !false
/*#define DEBUG
#define SHOW*/

#ifdef DEBUG
print(char *string)
{
#ifdef SHOW
  asm push ds
  asm push cs
  asm pop ds
  asm push si
  asm mov si,string
  asm mov ah,14
  asm mov bx,1fh
top:
  asm lodsb
  asm cmp al,0
  asm je exitprint
  asm int 10h
  asm jmp top
exitprint:
  asm pop si
  asm pop ds
done_now:
#endif
}
#endif

public (void,display,(void))
{
#ifdef DEBUG
  print("dCh ");
#endif
}

public (void,displayln,(void))
{
#ifdef DEBUG
  print("dLn ");
#endif
}

public(void,setattr,(void))
{
#ifdef DEBUG
  print("Set ");
#endif
  }

public(void,position,(void))
{
#ifdef DEBUG
  print("Pos ");
#endif
  }

public(void,clr_display,(void))
{
#ifdef DEBUG
  print("Clr ");
#endif
  }

public(void,get_xy,(void))
{
#ifdef DEBUG
  print("GXY ");
#endif
  }

public(void,bury_window,(void))
{
#ifdef DEBUG
  print("Bur ");
#endif
  }

public(char,getattr,(void))
{
#ifdef DEBUG
  print("Get ");
#endif
  return(0);
}

public(long,dupdisplay,(void))
{
#ifdef DEBUG
  print("Dup ");
#endif
  return(0L);
}

public(short,moddisplay,(void))
{
#ifdef DEBUG
  print("Mod ");
#endif
  return(true);
}


public(short,getdisplay,(void))
{
#ifdef DEBUG
  print("GDi ");
#endif
  return(0);
}

public(long,opendisplay,(void))
{
#ifdef DEBUG
  print("Opn ");
#endif
  return(0);
}

public(short,closedisplay,(void))
{
#ifdef DEBUG
  print("Clo ");
#endif
  return(false);
}

public(short,keypressed,(void))
{
#ifdef DEBUG
  print("Key ");
  asm int 3;
#endif
  return(false);
}

public(char,readch,(void))
{
#ifdef DEBUG
  print("RdC ");
#endif
  return(0);
}

void main()
{
  _keypressed();
  _readch();
  _displayln();
  _display();
  _position();
  _clr_display();
  _setattr();
  _getattr();
  _opendisplay();
  _closedisplay();
  _getdisplay();
  _moddisplay();
  _dupdisplay();
  _get_xy();
  _bury_window();
  Relinquish(1L);
}



