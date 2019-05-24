#include <mod.h>
#include <video.h>

#define false 0
#define true !false
#define secconv(a) (((long)(a-2)*secsperunit)+data_base)
#define sector(a) (a%maxsecs)
#define head(a)   ((a/maxsecs)%maxheads)
#define track(a)  (a/(maxsecs*maxheads))

extern short Hard_Test_Rdy(char drive);

extern short Read(unsigned char drive,char head,short track,char sector,
                  char count,void far *buffer);

extern short Write(unsigned char drive,char head,short track,char sector,
                  char count,void far *buffer);

unsigned char buffer[1000];
unsigned char far *fat;
short maxheads,maxtracks,maxsecs;
short data_base;
short root_dir,fat1,fat2,current_dir,fat_size=0;
char secsperunit,fat16;
short  drive_n=0x80;
short done;
char drive=2;

typedef struct file_holder
{
  short start_cluster;
  short current_cluster;
  char current_sector;
  char drive_n;
  long FPI;
  short BPI;
  char attr;
  long size;
  unsigned char sector_buffer[512];
} file_holder;

file_holder file[10];

typedef struct directory_entry
{
  unsigned char filename[8];
  unsigned char ext[3];
  unsigned char attr;
  char res[0xa];
  unsigned short time;
  unsigned short date;
  unsigned short start;
  long size;
} directory_entry;

unsigned short return_fat(short start)
{
  unsigned short idx,value;
  if (fat16)
  {
      idx=(start<<1);
      value=*(short*)&fat[idx];
      start=value;
  }
  else
  {
      idx=start+(start>>1);
      if (start&1)
      {
        value=*(short*)&fat[idx]>>4;
        start=value;
      }
      else
      {
        value=*(short*)&fat[idx]&0xfff;
        start=value;
      }
  }
  return(value);
}

void trace_fat(unsigned short start)
{
  do
  {
    printf("%d ",start);
    if (fat16)
    {
      if (start>=0xfff8)
        break;
    }
    else
      if (start>=0xff8)
        break;
    start=return_fat(start);
  }while (1);
}

void dis_dir(unsigned short clus)
{
  unsigned short sec=0,status,i,line,dline=0;
  unsigned long pos;
  directory_entry entry[16];
  if (clus==0)
    pos=root_dir;
  do
  {
    if (clus)
      pos=secconv(clus)+sec;
    status=Read(drive_n,head(pos),
           track(pos),sector(pos)+1,1,entry);
    for (line=0;line<16;line++)
    {
      if (entry[line].filename[0]==0) return;
      if (entry[line].filename[0]==0xe5) continue;
      dline++;
      if (entry[line].attr&8)
      {
        printf("V:");
        for (i=0;i<8;i++)
          printf("%c",entry[line].filename[i]);
        for (i=0;i<3;i++)
          printf("%c",entry[line].ext[i]);
        printf("           ");
      }
      else
      {
        if (entry[line].attr&2)
          printf("________.___");
        else
        {
          for (i=0;i<8;i++)
            printf("%c",entry[line].filename[i]);
          if (entry[line].ext[0]!=' ')
            printf(".");
          else
            printf(" ");
          for (i=0;i<3;i++)
            printf("%c",entry[line].ext[i]);
        }
        if (entry[line].attr&0x10)
          printf(" Directory  ");
        else
          printf("  %8ld  ",entry[line].size);
      }
      if ((dline%3)==0)
        printf("\n");
    }
    if (clus)
    {
      sec++;
      if (sec>=secsperunit)
      {
        sec=0;
        clus=return_fat(clus);
        if (clus>=0xfff8)
          return;
      }
    }
    else
      pos++;
  }while (1);
}

short name_search(char far *name,char far *attr,long far *size,
                  unsigned long far *start)
{
  unsigned short status,i,line,sec=0;
  unsigned long pos;
  unsigned short clus=current_dir;
  char far *_name=name;
  directory_entry entry[16];
  if (clus==0)
    pos=root_dir;
  do
  {
    if (clus)
      pos=secconv(clus)+sec;
    status=Read(drive_n,head(pos),
           track(pos),sector(pos)+1,1,entry);
    for (line=0;line<16;line++)
    {
      name=_name;
      if (entry[line].filename[0]==0) return(false);
      if (entry[line].filename[0]==0xe5) continue;
/*      if (entry[line].attr&8)
        continue;
      else*/
      {
        for (i=0;i<8;i++)
        {
          if (name[0]=='.')
          {
            i=8;
            break;
          }
          if (name[0]!=entry[line].filename[i])
            if ((name[0]==0)&&(entry[line].filename[i]==' '))
            {
              i=9;
              break;
            }
            else
              break;
          name++;
        }
        if (i==8)
          name++;
        if (i>=8)
        {
          for (i=0;i<3;i++)
          {
            if (name[0]!=entry[line].ext[i])
              if ((name[0]==0)&&(entry[line].ext[i]==' '))
              {
                i=3;
                break;
              }
              else
                break;
            name++;
          }
          if (i==3)
          {
            *start=entry[line].start;
            *size=entry[line].size;
            *attr=entry[line].attr;
            return(true);
          }
        }
      }
    }
    if (clus)
    {
      sec++;
      if (sec>secsperunit)
      {
        sec=0;
        clus=return_fat(clus);
        if (clus>=0xfff8)
          return(false);
      }
    }
    else
      pos++;
  }while (1);
}

void change_dir(char far *name)
{
  unsigned long start;
  char attr;
  long size;
  if (name_search(name,&attr,&size,&start))
    if (attr&0x10)
    {
      current_dir=start;
      dis_dir(current_dir);
    }
}

void test_read(unsigned short pos)
{
  short i,status;
  while (1)
  {
    status=Read(drive_n,
                head(pos),
                track(pos),
                sector(pos)+1,
                1,buffer);
    for (i=0;i<512;i++)
    {
      if ((i%64)==0)
        printf("\n");
      if (buffer[i]>31)
        printf("%c",buffer[i]);
      else
        printf(".");
    }
    printf("\n%x Pos:%x  s:%x  h:%x  t:%x",status,pos,sector(pos),head(pos),track(pos));
    pos++;
    while (~kbhit());
    getch();
  }
}

short init()
{
  unsigned short i,status,dist,pos;
  if (drive_n>4)
    status=Read(drive_n,1,0,1,1,buffer);
  else
    status=Read(drive_n,0,0,1,1,buffer);
  if (status&0xff00)
  {
    return(status);
  }
  maxheads=*(short*)&buffer[0x1a];
  maxtracks=160;
  maxsecs=*(short*)&buffer[0x18];
  secsperunit=buffer[0x0d];
  if (fat_size==0)
  {
    fat_size=*(short*)&buffer[0x16];
    fat=(unsigned char far *)Allocate(fat_size*512);
  }
  else
  if (fat_size<*(short far*)&buffer[0x16])
  {
    free(fat);
    fat_size=*(short*)&buffer[0x16];
    fat=(unsigned char far *)Allocate(fat_size*512);
  }
  dist=(buffer[0x10]**(short*)&buffer[0x16])+*(short*)&buffer[0xe];
  if (drive_n>4)
  {
    pos=maxsecs;
    fat16=true;
  }
  else
  {
    pos=0;
    fat16=false;
  }
  fat1=pos+buffer[0xe];
  pos+=dist;
  root_dir=pos;
  data_base=pos+((*(short*)&buffer[0x11]*32)/512);
  current_dir=0;
  pos=fat1;
  for (i=0;i<*(short*)&buffer[0x16];i++)
  {
    status=Read(drive_n,head(pos),track(pos),sector(pos)+1,
       1,fat+(i*512));
    pos++;
  }
  pos=data_base;
  return(status);
}

short getdisk()
{
  return(drive);
}

short setdisk(short new_drive)
{
  drive=new_drive;
  if (new_drive<2)
    drive_n=new_drive;
  else
    drive_n=0x7e+new_drive;
  return(init());
}

/*typedef struct file_holder
{
  short start_cluster;
  short current_cluster;
  char current_sector;
  char drive_n;
  long FPI;
  short BPI;
  char attr;
  long size;
  unsigned char sector_buffer[512];
} file_holder;*/

short _open(char far *name,char attr)
{
  unsigned long start;
  long size;
  char t;
  if (name_search(name,&t,&size,&start))
  {
    if (t&0x1e)
      return(false);
    for (t=0;t<10;t++)
    {
      if (file[t].start_cluster==0)
        break;
    }
    if (t==10)
      return(false);
    file[t].start_cluster=start;
    file[t].current_cluster=start;
    file[t].current_sector=0;
    file[t].drive_n=drive_n;
    file[t].FPI=0;
    file[t].BPI=0;
    file[t].attr=attr;
    file[t].size=size;
    start=secconv(start);
    Read(drive_n,head(start),track(start),sector(start)+1,1,
              file[t].sector_buffer);
    return(t+1);
  }
  else
    return(false);
}

void close(short handle)
{
  file[handle-1].start_cluster=0;
}

long sizefind(short handle)
{
  return(file[handle-1].size);
}

void rewind(short handle)
{
  unsigned long pos;
  handle--;
  file[handle].FPI=0;
  file[handle].BPI=0;
  pos=secconv(file[handle].start_cluster);
  file[handle].current_cluster=pos;
  file[handle].current_sector=0;
  Read(file[handle].drive_n,head(pos),
       track(pos),sector(pos)+1,1,file[handle].sector_buffer);
}

short _read(char far *buffer,short length,short handle)
{
  short cnt,BPI,FPI,size;
  long temp;
  handle--;
  BPI=file[handle].BPI;
  FPI=file[handle].FPI;
  if (FPI>=file[handle].size)
    return(0);
  for (cnt=0;cnt<length;cnt++)
  {
    buffer[cnt]=file[handle].sector_buffer[BPI];
    BPI++;
    FPI++;
    if (FPI>file[handle].size) break;
    if (BPI>=512)
    {
      BPI=0;
      file[handle].current_sector++;
      if (file[handle].current_sector>=secsperunit)
      {
        file[handle].current_cluster=
               return_fat(file[handle].current_cluster);
        file[handle].current_sector=0;
      }
      temp=secconv(file[handle].current_cluster)+
                file[handle].current_sector;
/*      printf("%d ",temp);*/
      Read(file[handle].drive_n,head(temp),track(temp),
             sector(temp)+1,1,file[handle].sector_buffer);
    }
  }
  file[handle].BPI=BPI;
  file[handle].FPI=FPI;
  return(cnt);
}

short gets(char far *buffer,short maxlen,short handle)
{
  short cnt;
  if (read(buffer,1,handle)==0)
    return(0);
  for (cnt=1;cnt<maxlen;cnt++)
  {
    if (read(buffer+cnt,1,handle))
    {
      if (buffer[cnt]==10&&buffer[cnt-1]==13)
      {
        buffer[cnt-1]=10;
        buffer[cnt]=0;
        break;
      }
    }
    else
      break;
  }
  return(cnt);
}

void prompt()
{
  printf("\nCMD:");
}

short get_line(char far *line,short maxlen)
{
  unsigned short pos=0,ch=0;
  while(ch!=13&&ch!=27)
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
  printf("\n");
  if (pos<maxlen)
    line[pos-1]=0;
  if (ch==13)
    return(true);
  else
    return(false);
}

void interp()
{
  char cmd[41];
  char i;
  short h;
  long size;
  long cnt;
  unsigned char temp[80];
  cmd[40]=0;
  prompt();
  if (get_line(cmd,40))
  {
    if (strncmp(cmd,"dir",3)==0)
      dis_dir(current_dir);
    if (strncmp(cmd,"cd ",3)==0)
      change_dir(cmd+3);
    if (strncmp(cmd,"exit",4)==0)
      done=true;
    if (strncmp(cmd,"type ",5)==0)
      if ((h=open(cmd+5,0))!=0)
      {
        printf("Success");
        size=sizefind(h);
        printf(" %ld ",size);
        while (gets(temp,80,h))
        {
          printf("%s",temp);
        }
        close(h);
      }
    if (strncmp(cmd,"set ",4)==0)
    {
      setdisk(cmd[4]-0x30);
    }
  }
  else
    printf("Canceled");
}

void main()
{
  short h;
  char temp[600];
  setdisk(2);
  done=false;
  while (!done)
    interp();
}
