#include <stdio.h>
#include <alloc.h>

#define true !false
#define false 0
FILE *temp,*output;


typedef struct prefix_data
{
  unsigned char prefix;
  char length;
  char *scancodes;
  short *table;
  struct prefix_data *next;
} prefix_data;

typedef struct kbd
{
  char *keydata;  /* The data that describes all the keys */
  short *table;   /* table of indexes into the data table for scancodes
                     below max*/
  unsigned char maxcode;   /* highest 'normal' scancode */

  prefix_data *prefix_table;
}kbd;

#define maxopts 7
char *option[maxopts]={"Toggle","Caps","Num","LED","Scroll","Skip","norepeat"};
kbd tempkbd;

dump_key(short idx)
{
  short cnt,len,key;
  char column=7;
  char op,first,done,tlen,tchar,topt,ofirst,otopt=-1;
  char lindex;
  char text;
  char line[80];
  char quote;
    for (cnt=0;cnt<8;cnt++)
    {
      quote='\'';
      first=true;
      done=false;
      ofirst=true;
      tchar=tempkbd.keydata[idx++];
      if (otopt!=tchar||cnt==0)
      {
        fprintf(output," (");
        column+=2;
        for (topt=0;topt<maxopts;topt++)
        {
          if (tchar&(1<<topt))
          {
            if (!ofirst) fprintf(output,",");
            if ((column+2+strlen(option[topt]))>=80)
            {
              fprintf(output,"\\\n       ");
              column=7;
            }
            column+=strlen(option[topt]);
            fprintf(output,"%s",option[topt]);
            ofirst=false;
          }
        }
        fprintf(output,")");
        column+=1;
      }
      otopt=tchar;
      do
      {
        switch(tempkbd.keydata[idx++])
        {
          case 0:fprintf(output,"]");
                 done=true;
                 column++;
                 break;
          case 1:if (!first) fprintf(output,","); else fprintf(output,"[");
                 if ((column+6)>=80)
                 {
                   fprintf(output,"\\\n      ");
                   column=8;
                 }
                 fprintf(output,"Shift");
                 column+=6;
                 break;
          case 2:if (!first) fprintf(output,","); else fprintf(output,"[");
                 if ((column+5)>=80)
                 {
                   fprintf(output,"\\\n      ");
                   column=7;
                 }
                 fprintf(output,"Ctrl");
                 column+=5;
                 break;
          case 3:if (!first) fprintf(output,","); else fprintf(output,"[");
                 if ((column+4)>=80)
                 {
                   fprintf(output,"\\\n      ");
                   column=7;
                 }
                 fprintf(output,"Alt");
                 column+=4;
                 break;
          case 4:if (!first) fprintf(output,","); else fprintf(output,"[");
                 if ((column+9)>=80)
                 {
                   fprintf(output,"\\\n      ");
                   column=7;
                 }
                 fprintf(output,"Capslck");
                 column+=9;
                 break;
          case 5:if (!first) fprintf(output,","); else fprintf(output,"[");
                 if ((column+7)>=80)
                 {
                   fprintf(output,"\\\n      ");
                   column=7;
                 }
                 fprintf(output,"Numlck");
                 column+=7;
                 break;
          case 6:if (!first) fprintf(output,","); else fprintf(output,"[");
                 if ((column+8)>=80)
                 {
                   fprintf(output,"\\\n      ");
                   column=7;
                 }
                 fprintf(output,"Scrllck");
                 column+=8;
                 break;
          case 7:if (!first) fprintf(output,","); else fprintf(output,"[");
                 text=false;
                 tlen=tempkbd.keydata[idx++];
                 for (lindex=tlen;lindex;lindex--)
                 {
                   if (tempkbd.keydata[idx+lindex-1]=='\'')
                     quote='\"';
                 }
                 for (;tlen;tlen--)
                 {
                   tchar=tempkbd.keydata[idx++];
                   if (tchar>=32)
                   {
                     if (!text)  fprintf(output,"%c",quote);
                     text=true;
                     fprintf(output,"%c",tchar);
                   }
                   else
                   {
                     fprintf(output,"#%02x",tchar);
                   }

                 }
                 if (text) fprintf(output,"%c",quote);
                 break;
          case 8:if (!first) fprintf(output,","); else fprintf(output,"[");
                 tlen=tempkbd.keydata[idx++];
                 for (;tlen;tlen--)
                   fprintf(output,"%c",tempkbd.keydata[idx++]);
                 break;
          case 9:fprintf(output,"[]");
                 column+=2;
                 done=true;
                 break;
        }
        first=false;
      }while (!done);
    }
    fprintf(output,"\n");
  }

dump_defs()
{
  short cnt,len,datalen,key,pkey;
  short idx;
  prefix_data *tpretable;

  fprintf(output,"KeyDefs\n\n");
  fprintf(output,"!     These are the \'Normal\' Keys.\n");
  pkey=0;
  for (key=0;key<tempkbd.maxcode;key++)
  {
    idx=tempkbd.table[key];
    if (idx==-1)
    {
      fprintf(output,"!   NO KEY\n");
      continue;
    }
    fprintf(output,"Key%02d",pkey++);
    dump_key(idx);
  }
  tpretable=tempkbd.prefix_table;
  while (tpretable->next)
  {

    fprintf(output,"\n!  These are for Prefix #%x \n",tpretable->prefix);
    for (key=0;key<tpretable->length;key++)
    {
      idx=tpretable->table[key];
      if (idx==-1)
      {
        fprintf(output,"!   NO KEY\n");
        continue;
      }
      fprintf(output,"Key%02d",pkey++);
      dump_key(idx);
    }
    tpretable=tpretable->next;
  }

}

dump_scans()
{
  short cnt,len,datalen,key,pkey;
  short idx;
  prefix_data *tpretable;
  fprintf(output,"ScanDefs\n\n");
  fprintf(output,"!     These are the \'Normal\' Keys.\n");
  pkey=0;
  for (key=0;key<tempkbd.maxcode;key++)
  {
    idx=tempkbd.table[key];
    if (idx==-1)
    {
      fprintf(output,"!   NO KEY\n");
      continue;
    }
    fprintf(output,"Key%02d      [%d]\n",pkey++,key);
  }
  tpretable=tempkbd.prefix_table;
  while (tpretable->next)
  {

    fprintf(output,"\nPrefix [#%02x] \n",tpretable->prefix);
    for (key=0;key<tpretable->length;key++)
    {
      idx=tpretable->table[key];
      if (idx==-1)
      {
        fprintf(output,"!   NO KEY\n");
        continue;
      }
      fprintf(output,"Key%02d   [#%02x]\n",pkey++,tpretable->scancodes[key]);
    }
    tpretable=tpretable->next;
  }

}

main(char argc,char *argv[])
{
  short cnt,len,datalen,key,pkey;
  short idx;
  prefix_data *tpretable;
  if (argc<2)
  {
    printf("Usage: UNPROC <key config file> [<output file>]\n");
    exit(0);
  }
  temp=fopen(argv[1],"rb");
  if (!temp)
  {
    printf("Error opening Configuration file %s.",argv[1]);
    exit(0);
  }
  if (argc>2)
  {
    output=fopen(argv[2],"wt");
    if (!output)
    {
      printf("Error opening output file %s.",argv[2]);
      exit(0);
    }
  }
  else
    output=stdout;
  fread(&datalen,1,2,temp);
  tempkbd.keydata=calloc(datalen,1);

  tempkbd.maxcode=fgetc(temp);

  tempkbd.table=calloc(tempkbd.maxcode,2);

  fread(tempkbd.table,tempkbd.maxcode,2,temp);

  tpretable=tempkbd.prefix_table=calloc(sizeof(prefix_data),1);
  while ((len=fgetc(temp))!=0)
  {
    tpretable->prefix=fgetc(temp);
    tpretable->length=len;
    tpretable->scancodes=calloc(len,1);
    fread(tpretable->scancodes,len,1,temp);
    tpretable->table=calloc(len,2);
    fread(tpretable->table,len,2,temp);
    tpretable->next=calloc(sizeof(prefix_data),1);
    tpretable=tpretable->next;
  }
  tpretable->length=len;
  tpretable->next=NULL;
  fread(tempkbd.keydata,datalen,1,temp);
  dump_defs();
  dump_scans();

}
