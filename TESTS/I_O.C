#include <mod.h>



main()
{
  if (fork(CHILD))
  {
    while (1)
    {
      time_struc cur_time;
      short i;
      gettime(&cur_time);
      for (i=0;i<cur_time&0xf;i++)
        Export("IO","12345");
      Relinquish(0L);
    }
  }
  else
  {
    while (1)
    {
      time_struc cur_time;
      short i;
      gettime(&cur_time);
      for (i=0;i<cur_time&0xf;i++)
        Import("IO");
      Relinquish(0L);
    }
  }
}
