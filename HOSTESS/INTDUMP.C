

main()
{
  short i;
  for (i=0;i<100;i++)
    printf("%08lx  ",*(long far *)((long)i*4));
}