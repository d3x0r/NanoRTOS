#include <mod.h>
#include <video.h>


windowptr output;

char *commands[]={"help",
                  "dir",
                  "copy"};

enum
{
  HELP,
  DIRECTORY,
  COPY,
  NUM_COMMANDS
}

#define true !false
#define false 0

#define max_params 3
typedef struct command
{
  char *command;
  char *desc;
  void (*func)();
}command;

command command_list[]={
                        {0,0,0}
                       };

char hex[]="0123456789ABCDEF";

short get_line(char *line,short maxlen)
{
  unsigned short pos=0,ch=0;
  do
  {
    ch=getch();
    if (ch!=8)
    {
      if (pos<maxlen)
      {
        line[pos]=ch;
        pos++;
        printf("%c",ch);
      }
    }
    else
      if(pos)
      {
        pos--;
        printf("%c %c",ch,ch);
      }
  }
  while(ch!=13&&ch!=27);

  printf("\n");
  if (pos<maxlen)
    line[pos-1]=0;
  if (ch==13)
    return(true);
  else
    return(false);
}

void help(void)
{
  short i,len,cnt=0;
  for (i=0;command_list[i].command;i++)
  {
    printf(" %s",command_list[i].command);
    len=strlen(command_list[i].command);
    printf("              "+(len>13?13:len));
    printf("%s\n",command_list[i].desc);
    cnt++;
    if (cnt>22)
    {
      printf("... Press any key to continue\r");
      while (!kbhit());
      while (kbhit()) getch();
    }

  }
  printf(" %s","Exit");
  len=strlen("Exit");
  printf("              "+(len>13?13:len));
  printf("%s\n","Exit this diagnostics");
}

short get_value(short deflt)
{
  char entry[10];
  char *temp;
  short tval;
  char idx;
  if (get_line(entry,10))
  {
    idx=0;
    while (entry[idx]==' ') idx++;
    if (entry[0])
    {
      if (entry[idx]=='#')
      {
        idx++;
        tval=(short)strtol(entry+idx,&temp,16);
      }
      else
        tval=atoi(entry+idx);
    }
    else
      tval=deflt;
  }
  else
    return(-1);
  return(tval);
}

prompt()
{
  printf("\nCMD:");
}

short interp()
{
  char cmd[41];
  char i;
  short values[max_params];
  short cnt;
  char contin=true;
  cmd[40]=0;
  prompt();
  if (get_line(cmd,40))
  {
    for (i=0;command_list[i].command;i++)
    {
      if (strnicmp(cmd,command_list[i].command,
            strlen(command_list[i].command))==0&&
          strlen(command_list[i].command)==strlen(cmd))
        break;
    }
    if (command_list[i].command)
    {
      if (command_list[i].need_arm)
      {
        if (!armed)
        {
          printf("Card is not armed.  Operation impossible.\n");
          return(true);
        }
      }
      if (command_list[i].parameters)
      {
        for (cnt=0;cnt<command_list[i].parameters;cnt++)
        {
          values[cnt]=command_list[i].paramrequest[cnt]();
        }
      }
      if (command_list[i].repeatable)
        if (repeat_value==-1)
        {
          while (!kbhit())
            command_list[i].func(values[0],values[1],values[2]);
          if (!getch()) getch();
        }
        else
          for (cnt=0;cnt<repeat_value;cnt++)
          {
            printf("%d ",cnt);
            command_list[i].func(values[0],values[1],values[2]);
          }
      else
        command_list[i].func(values[0],values[1],values[2]);
    }
    else
      if (strnicmp(cmd,"exit",4)==0)
        contin=false;
      else
        printf("Unknown Command\n");
  }
  else
    printf("Canceled");
  return(contin);
}


void title(void)
{
   printf(" Welcome to the PCA command processor.\n"     \
          " Copyright 1992 Logical Data Corporation.\n"    \
          " Rev A.00\n");
}

short hexconv(char *line)
{
  char idx;
  short total=0;
  while (line[0])
  {
    for (idx=0;idx<16;idx++)
      if (line[0]==hex[idx])
      {
        total*=16;
        total+=idx;
        break;
      }
    if (idx<16)
      line++;
    else
      break;
  }
  return(total);
}

void main()
{
  short h;
main()
{
  output=opendisplay(2,1,50,10,BORDER|NEWLINE,0x1f,0x1f,0x2f,"Command");

}
  clrscr();
  title();
  load_defaults();
  while (interp());
}


