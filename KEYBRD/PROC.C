#include <stdio.h>
#include <alloc.h>

typedef struct key_def
{
  unsigned char scancode;  /*actual value of the key.  Unused save when they
                             key has a prefix*/
  unsigned char prefix;    /*value received to signify that the next value
                             is actually the key */
  unsigned char index;     /*Unused save when the key has a prefix.  This is
                             what position it is*/
  char label[10];
}key_def;

#define false 0
#define true !false
short list_keys=false;

#define maxopts 7
char *options[maxopts]={"Toggle","Caps","Num","LED","Scroll","skip","norepeat"};
char *known_funcs[6]={"Shift","Ctrl","Alt",
                      "Capslck","Numlck","Scrllck"};


process(FILE *source,FILE *dest)
{
  char line[80];
  short idx;
  char valid;
  char quote=0;
  short linenumber=0;
  while (fgets(line,80,source))
  {
    idx=0;
    valid=false;
    linenumber++;
    while (line[idx]==' ') idx++;
    if (line[idx]=='!') continue;
    while (line[idx])
    {
      if (line[idx]>' '||quote)
      {
        fputc(line[idx],dest);
        valid=true;
      }
      if (!quote&&idx)
      {
        if (line[idx]=='\''||line[idx]=='\"')
          quote=line[idx];
      }
      else
      {
        if (line[idx]==quote) quote=0;
      }
      idx++;
    }
    if (quote)
    {
      printf("Expected Ending Quote in line %d",linenumber);
      exit(0);
    }

    if (valid)
    {
      fprintf(dest,"\n%d\n",linenumber);
    }
  }
  rewind(source);
  rewind(dest);
}

char hexval(char *line)
{
  char temp;
        if (line[0]>='A'&&line[0]<='F')
          temp=((line[0]-'A')+10)<<4;
        else
          if (line[0]>='a'&&line[0]<='f')
            temp=((line[0]-'a')+10)<<4;
          else
          if (line[0]>='0'&&line[0]<='9')
            temp=(line[0]-'0')<<4;
          else
          {
            printf("Error converting Value:%s ",line);
            exit(0);
          }
        if (line[1]>='A'&&line[1]<='F')
          temp+=((line[1]-'A')+10);
        else
          if (line[1]>='a'&&line[1]<='f')
            temp+=((line[1]-'a')+10);
          else
          if (line[1]>='0'&&line[1]<='9')
            temp+=(line[1]-'0');
          else
          {
            printf("Error converting Value:%s ",line);
            exit(0);
          }
  return(temp);
}

char decval(char *line)
{
  char temp=0;
  if (line[0]>='0'&&line[0]<='9')
    temp=temp*10+(line[0]-'0');
  else
    return(temp);

  if (line[1]>='0'&&line[1]<='9')
    temp=temp*10+(line[1]-'0');
  else
    return(temp);
  if (line[2]>='0'&&line[2]<='9')
    temp=temp*10+(line[2]-'0');
  return(temp);
}



short findkey(char *key,key_def *table,char length,short entries)
{
  short i;
  for (i=0;i<entries;i++)
  {
    if (strncmp(key,table[i].label,length>10?10:length)==0)
    {
      return(i);
    }
  }
  return(-1);
}

short get_num_scancodes(FILE *source)
{
  char line[80];
  char deftab=false;
  short keys=0;
  while (fgets(line,80,source))
  {
    if (strncmp(line,"KeyDefs",7)==0)
      deftab=false;
    if (deftab&&strncmp(line,"Prefix",6))
      keys++;
    if (strncmp(line,"ScanDefs",8)==0)
      deftab=true;
    fgets(line,80,source);  /*skip line number */
  }
  rewind(source);
  return(keys);
}

void gen_symbol_table(key_def *table,FILE *source)
{
  short key=0;
  char deftab=false;
  short idx;
  char line[80];
  char prefix=0;
  short max_code=0;
  short cnt;

  while (fgets(line,80,source))
  {
    idx=1;

    if (strncmp(line,"KeyDefs",7)==0)
      deftab=false;
    if (deftab)
    {
      if (strncmp(line,"Prefix",6)==0)
      {
        if (line[7]=='#')
          prefix=hexval(line+8);
        if (list_keys)
          printf("New Prefix %x\n",prefix);

      }
      else
      {
        while (line[idx]&&line[idx]!='[') idx++;
        if (line[idx])
        {
          table[key].prefix=prefix;
          if (line[idx+1]=='#')
            table[key].scancode=hexval(line+idx+2);
          else
            table[key].scancode=decval(line+idx+1);
          if (max_code<table[key].scancode)
            max_code=table[key].scancode;
          strncpy(table[key].label,line,idx>10?10:idx);
          if (idx<10)
            table[key].label[idx]=0;
/*          printf("New Label: num:%d Scan:%d \"%s\"\n",key,
                       table[key].scancode,
                       table[key].label);*/
          key++;
        }
        else
        {
          printf(" Expected opening bracket in line: %s\n",
              fgets(line,80,source));
          exit(0);
        }
      }
    }
    if (strncmp(line,"ScanDefs",8)==0)
      deftab=true;
    fgets(line,80,source);  /*skip line number */
  }

/*  fputc(sort_table(table,key,dest));     /*put out number of prefixes in file
  for (cnt=0;cnt<level;cnt++)
  {
    fputc(0,dest);  /*initialize prefix spots
  }
  fputc(max_code+1,dest);
  for (cnt=0;cnt<=max_code;cnt++)
  {
    fputc(0xff,dest);   /*put out short values to init indexes into
    fputc(0xff,dest);   /*the scancode table
  }*/
  rewind(source);


}

void sort_table(key_def *table,short length)
{
  key_def *skeys=calloc(length,sizeof(key_def));
  short idx,idx2;
  short index;
  short keys;
  short i;
  short prefixes=0;
  short current_prefix;
  keys=0;
  for (idx=0;idx<length;idx++)  /*put all normal codes together*/
  {
    if (!table[idx].prefix)
    {
      skeys[keys]=table[idx];
      keys++;
    }
  }
  for (idx=0;idx<length;idx++)  /*put all sets of prefixes together*/
  {
    if ((current_prefix=table[idx].prefix)!=0)
    {
      prefixes++;
      index=0;
      for (idx2=idx;idx2<length;idx2++)
      {
        if (table[idx2].prefix==current_prefix)
        {
          skeys[keys]=table[idx2];
          table[idx2].prefix=0;
          skeys[keys].index=index;
          index++;
          keys++;
        }
      }
    }
  }
  movmem(skeys,table,length*sizeof(key_def));  /*put it back in other table*/
  free(skeys);
}

Create_compilation(key_def *table,short table_len,
                   char *keydata,short data_len,
                   FILE *output)
{
  short cnt,initcnt;
  char max_scancode=0;
  char pre_entries;
  char key;
  short keylen;
  short pre_start;
  short data_idx;
  short new_data_idx;
  char cur_pre;
  long FPI;
  for (cnt=0;cnt<table_len;cnt++)
  {
    if (table[cnt].scancode>max_scancode)
      max_scancode=table[cnt].scancode;
    if ((cur_pre=table[cnt].prefix)!=0) break;
  }
  max_scancode++; /*go one more to include max in table */
  fputc(0,output);  /*total size of the datatable*/
  fputc(0,output);
  fputc(max_scancode,output);
  for (initcnt=0;initcnt<max_scancode;initcnt++)
  {
    fputc(0xff,output);
    fputc(0xff,output);
  }
  while (cnt<table_len)
  {
    pre_entries=0;
    pre_start=cnt;
    while (table[cnt].prefix==cur_pre&&cnt<table_len)
    {
      pre_entries++;
      cnt++;
    }
    fputc(pre_entries,output);
    fputc(cur_pre,output);
    for (initcnt=pre_start;initcnt<cnt;initcnt++)
      fputc(table[initcnt].scancode,output);
    for (initcnt=pre_start;initcnt<cnt;initcnt++)
    {
      fputc(0xff,output);
      fputc(0xff,output);
    }
    cur_pre=table[cnt].prefix;
  }
  fputc(0,output);
  FPI=ftell(output);
  rewind(output);
  data_idx=0;
  new_data_idx=0;
  for (cnt=0;cnt<table_len;cnt++)
  {
    key=keydata[data_idx];
    keylen=*(short*)(keydata+data_idx+1);
    if (table[key].prefix)
    {
      fseek(output,3+(max_scancode<<1),0);
      do
      {
        pre_entries=fgetc(output);
        cur_pre=fgetc(output);
        if (cur_pre==table[key].prefix)
        {
          fseek(output,pre_entries,1);
          fseek(output,table[key].index<<1,1);
          fwrite(&new_data_idx,1,2,output);
        }
        else
        {
          if (!pre_entries)
          {
            printf("Prefix not Found.  ??? Error Somplace.\n");
            exit(0);
          }
          fseek(output,3*pre_entries,1);
        }
      }while (cur_pre!=table[key].prefix);
    }
    else
    {
      fseek(output,3+(table[key].scancode<<1),0);
      fwrite(&new_data_idx,1,2,output);
    }
    fseek(output,FPI+new_data_idx,0);
    fwrite(keydata+data_idx+3,keylen-3,1,output);
    new_data_idx+=keylen-3;
    data_idx+=keylen;
    if (data_idx>data_len)
    {
      printf("Tried to access more data than was compiled.\n");
    }
  }
  if (data_idx<data_len)
  {
    printf("Missed the Mark by %d bytes.\n",data_len-data_idx);
    exit(0);
  }
  fseek(output,0,0);
  fwrite(&data_idx,1,2,output);
}


compile(FILE *source,FILE *dest)
{
  char line[80];            /*scratch line from the source file to process*/
  char line_number[20];     /*temp for holding error line numbers */
  short idx;                /*index into the line for parsing*/
  short keys;               /*counter of key scancodes */
  char deftab;              /*boolean variable whether the lines are scan defs or not*/
  short key;                /*used as index into key def table */
  key_def *table;           /*table of scan code label definitions */
  char cnt;                 /*miscellaneous counter*/
  short *indexes;           /*table of indexes into data */
  char *data;               /*data array for keys*/
  short data_idx;           /*index into the data array*/
  short data_start;         /*beginning of current key def */
  short opts;               /*options for the key*/
  short _mod_level=-1;      /*last_mod, to check for delta for
                              putting options*/
  short mod_level=8;        /*this is to keep track what what level of the
                              key we are working on, whether it be
                                Normal
                                Shift
                                Ctrl
                                Shift-Ctrl
                                etc.
                            */
  char state=0;             /*this is what we are working on in the
                              case of continuations.
                                0=newkey;
                                1=option list;
                                2=definition list;
                            */
  char voidlist;           /*marks whether there was defin. data */
  char newkey=true;
  #define buffer_size 5000
#define inc_data_idx  data_idx++;                           \
                      if (data_idx>buffer_size)             \
                      {                                      \
                        printf("Not enough room to compile Keyboard.");\
                        exit(0);                                       \
                      }

  keys=get_num_scancodes(source);
  if (list_keys)
    printf("Number of Scancodes- %d\n",keys);

  table=calloc(keys,sizeof(key_def));
  gen_symbol_table(table,source);
  sort_table(table,keys);
  data_idx=0;
  data_start=0;
  data=calloc(buffer_size,1);  /*allocate a table for the key data*/
  while (fgets(line,80,source))
  {
    {
      idx=0;
      if (line[idx]=='(')
      {
        idx++;
        state=1;
        opts=0;
      }
      else
      if (line[idx]=='[')
      {
        idx++;
        if (mod_level>7)
        {
          printf(" Too many states defined for key in line %s",
               fgets(line_number,20,source));
          exit(0);
        }
        state=2;
      }
      else
      if (newkey)
      {
        *(short*)(data+data_start+1)=data_idx-data_start;
        data_start=data_idx;
        state=0;
      }
    }
    if (strncmp(line,"ScanDefs",8)==0)
      deftab=true;

    if (!deftab)
    {
      do
      {
        switch(state)
        {
          case 0:
            if (mod_level<8)
            {
              printf("Too few states defined for key prior to line %s",
                   fgets(line_number,20,source));
              exit(0);
            }
            if (mod_level>8)
            {
              printf("Too many states defined for key prior to line %s",
                   fgets(line_number,20,source));
              exit(0);
            }

            while (line[idx]&&!(line[idx]=='['||line[idx]=='(')) idx++;
            if (line[idx])
            {
              key=findkey(line,table,idx,keys);
              if (key==-1)
              {
                line[idx]=0;
                printf(" Undefined ScanLabel %s in line %s\n",
                    line,
                    fgets(line_number,20,source));
                exit(0);
              }
              data[data_start]=key;
              data_idx=data_start+3;  /*skip 1 for keyindex,
                                             2 for length */
              if (line[idx]=='(')
                state=1;
              else
                state=2;
              mod_level=0;
              opts=0;
            }
            else
            {
              printf(" Expected opening bracket or Paren in line: %s\n",
                  fgets(line_number,20,source));
              exit(0);
            }
            newkey=false;
            idx++;
            break;

          case 1:
            /*This is a list of options.*/
            {
              short start=idx,end;
              do
              {
                end=start;
                while (line[end]&&
                       line[end]!='\\'&&
                       line[end]!=','&&
                       line[end]!=')') end++;
                if (end-start)
                {
                  for (cnt=0;cnt<maxopts;cnt++)
                    if (strncmp(line+start,options[cnt],end-start)==0)
                    {
                      opts|=1<<cnt;
                      break;
                    }
                  if (cnt==4)
                  {
                    line[end]=0;
                    printf("Unknown option %s in line ",line+start,
                       fgets(line_number,20,source));
                    exit(0);
                  }
                }
                start=end+1;
              }
              while (line[end]!=')'&&line[end]&&line[end]!='\\');
              if (!line[end])
              {
                printf("Expected a closing Parentheses or a Continuation in line %s",
                         fgets(line_number,20,source));
                exit(0);
              }
              idx=start;
              if (line[end]=='\\')
              {
                idx++;
                if (line[idx])
                {
                  printf("Extra Characters on line after Continuation in line %s",
                        fgets(line_number,20,source));
                  exit(0);
                }
                break;
              }
              else
              {
                newkey=true;
                if (line[idx]=='(')
                {
                  state=1;
                  opts=0;
                }
                else
                  if (line[idx]=='[')
                  {
                    if (mod_level>7)
                    {
                      printf(" Too many states defined for key in line %s",
                            fgets(line_number,20,source));
                      exit(0);
                    }
                    state=2;
                  }
              }
            }
            idx++;
            break;
          case 2:
            {
              short start=idx,end;
              char quote=0;
              voidlist=false;
              newkey=true;
              do
              {
                end=start;
                while (line[end]&&
                       (!(line[end]=='\\'||
                         line[end]==','||
                         line[end]==']')||
                       (quote)))
                {
                  if (!quote)
                  {
                    if (line[end]=='\''||line[end]=='\"')
                      quote=line[end];
                  }
                  else
                    if (line[end]==quote) quote=0;
                  end++;
                }
                if (quote)
                {
                  printf("Missing end Quote in line %s",
                      fgets(line_number,20,source));
                  exit(0);
                }
                if (line[end]=='\\')
                {
                  newkey=false;
                }
                /*This is where we do some compiling*/
                {
                  char temp[50];
                  if (_mod_level!=mod_level)
                  {
                    data[data_idx]=opts;
                    inc_data_idx;
                    _mod_level=mod_level;
                  }
                  if (end-start)
                  {
                    if (line[start]=='\''||line[start]=='\"')
                    {
                      start++;
                      data[data_idx]=7;
                      inc_data_idx;
                      data[data_idx]=end-start-1;
                      inc_data_idx;
                      if (list_keys)
                        printf("Text: ");
                      for (;start<(end-1);start++)
                      {
                      if (list_keys)
                        printf("%c",line[start]);
                        data[data_idx]=line[start];
                        inc_data_idx;
                      }
                      if (list_keys)
                      printf("\n");
                    }
                    else
                    if (line[start]=='#')
                    {
                      data[data_idx]=7;
                      inc_data_idx;
                      data[data_idx]=1;
                      inc_data_idx;
                      if (data_idx>buffer_size)
                      {
                        printf("Not enough room to compile Keyboard.");
                        exit(0);
                      }
                      data[data_idx]=hexval(line+start+1);
                      inc_data_idx;
                      if (list_keys)
                      printf("Text: #%x %d\n",
                            data[data_idx-1],data[data_idx-1]);
                    }
                    else
                    if (line[start]>='0'&&line[start]<='9')
                    {
                      data[data_idx]=7;
                      inc_data_idx;
                      data[data_idx]=1;
                      inc_data_idx;
                      data[data_idx]=decval(line+start);
                      inc_data_idx;
                      if (list_keys)
                      printf("Text: %d #%x\n",
                            data[data_idx-1],data[data_idx-1]);
                    }
                    else
                    {
                      for (cnt=0;cnt<6;cnt++)
                        if (strncmp(line+start,known_funcs[cnt],end-start)==0)
                        {
                      if (list_keys)
                          printf("Function: %s\n",known_funcs[cnt]);
                          data[data_idx]=1+cnt;
                          inc_data_idx;
                          break;
                        }
                      if (cnt==6)
                      {
                        data[data_idx]=8;
                        inc_data_idx;
                        data[data_idx]=end-start;
                        inc_data_idx;
                      if (list_keys)
                        printf("Extrn: ");
                        for (;start<end;start++)
                        {
                      if (list_keys)
                          printf("%c",line[start]);
                          data[data_idx]=line[start];
                          inc_data_idx;
                        }
                      if (list_keys)
                        printf("\n");
                      }
                    }
                  }
                  else
                  {
                    if (newkey)
                    {
                      data[data_idx]=9;
                      inc_data_idx;
                      voidlist=true;
                      if (list_keys)
                        printf("No Function\n");
                    }
                  }
                }
                /*then go to end of this entry*/
                start=end+1;
              }
              while (line[end]!=']'&&line[end]&&line[end]!='\\');
              if (line[end]==']')
              {
                if (!voidlist)
                {
                  data[data_idx]=0;
                  inc_data_idx;
                }
                mod_level++;
              }
              if (!line[end])
              {
                printf("Expected a closing Bracket or a Continuation in line %s",
                         fgets(line_number,20,source));
                exit(0);
              }
              idx=start;

              if (line[end]=='\\')
              {
                idx++;
                if (line[idx])
                {
                  printf("Extra Characters on line after Continuation in line %s",
                        fgets(line_number,20,source));
                  exit(0);
                }
                break;
              }
              else
              {
                if (line[idx]=='(')
                {
                  state=1;
                  opts=0;
                }
                else
                  if (line[idx]=='[')
                  {
                    if (mod_level>8)
                    {
                      printf(" Too many states defined for key in line %s",
                            fgets(line_number,20,source));
                      exit(0);
                    }
                    state=2;
                  }
              }
            }
            idx++;
            break;
        }
      }
      while (line[idx]);
    }
    if (strncmp(line,"KeyDefs",7)==0)
      deftab=false;
    fgets(line,80,source);  /*skip line number */
  }
  Create_compilation(table,keys,data,data_idx,dest);
}


main(char argc,char *argv[])
{
  FILE *in,*out,*tmp;
  if (argc<3)
  {
    printf("Usage: PreProc <source> <dest> <list (y/n)>");
    exit(0);
  }
  if (argc==4)
  {
    if (argv[3][0]=='Y'||argv[3][0]=='y')
      list_keys=true;
  }
  in=fopen(argv[1],"rt");
  if (!in)
  {
    printf("Error opening %s",argv[1]);
    exit(0);
  }
  out=fopen(argv[2],"wb+");
  if (!out)
  {
    printf("Error Creating %s",argv[2]);
    exit(0);
  }
  tmp=fopen("Preproc.$$$","wt+");
  if (!tmp)
  {
    printf("Could not open Scratch File!");
    exit(0);
  }
  process(in,tmp);
  compile(tmp,out);
  fclose(in);
  fclose(out);
  fclose(tmp);
  remove("Preproc.$$$");
}
