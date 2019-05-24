#include "mod.h"


static cnt=0;

main()
{
  char i;
  Relinquish(-100);
  for(i=0;i<5;i++)
  {
    if (fork(PARENT)==0)
        Relinquish(i+1);
  }
  Relinquish(100);
}
