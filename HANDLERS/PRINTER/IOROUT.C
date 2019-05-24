#define IOROUT
#include <mod.h>
#include <npca.h>
#include "iorout.h"


public (char,Get_Rewind,(Node far *Node,short far *Bytes_left))
{
  *Bytes_left=0;
  return('a');
}

public (char,Get_Back_File,(Node far *Node,short far *Bytes_left))
{
  *Bytes_left=0;
  return('b');
}

public (char,Get_Back_Record,(Node far *Node,short far *Bytes_left))
{
  *Bytes_left=0;
  return('c');
}

public (char,Get_Adv_Record,(Node far *Node,short far *Bytes_left))
{
  *Bytes_left=0;
  return('d');
}

public (char,Get_Adv_File,(Node far *Node,short far *Bytes_left))
{
  *Bytes_left=0;
  return('e');
}

public (char,Get_EOF,(Node far *Node,short far *Bytes_left))
{
  *Bytes_left=0;
  return('f');
}

public (char,Get_Home,(Node far *Node,short far *Bytes_left))
{
  *Bytes_left=0;
  return('g');
}

public (char,Get_Data_SA,(Node far *Node,short far *Bytes_left))
{
  *Bytes_left=0;
  return('h');
}

public (char,Get_Data_SB,(Node far *Node,short far *Bytes_left))
{
  *Bytes_left=0;
  return('i');
}

public (char,Get_Data_NSA,(Node far *Node,short far *Bytes_left))
{
  *Bytes_left=0;
  return('j');
}

public (char,Get_Data_NSB,(Node far *Node,short far *Bytes_left))
{
  *Bytes_left=0;
  return('k');
}

void main()
{
  _Get_Rewind();
  _Get_Back_File();
  _Get_Back_Record();
  _Get_Adv_Record();
  _Get_Adv_File();
  _Get_EOF();
  _Get_Home();
  _Get_Data_SA();
  _Get_Data_SB();
  _Get_Data_NSA();
  _Get_Data_NSB();
  Relinquish(1L);
}


