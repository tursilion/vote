/* VOTE v1.2 by M. Brent - Nov 1995
   I/O redirection assumed
   Usage: VOTE mode user# [-vote file]
   Where mode is: N - new voter
                  R - results
                  S - sysop
                  A - add new question
 */
/* v1.1 - changed my name to M.Brent from M.Ward. Might as well go real.
        - moved 'Exitting VOTE' prompt to BEFORE data file saved
        - added new mode to add a single question (A)
        - now takes the name of the user entering vote questions
        - Removed Edit from the sysop mode. Waste of time, really
        - added buffer overflow protection (promised but not implemented!)
        - changed meaning of [data path] to allow multiple vote files
        - fixed bug displaying multi-line questions
        - removed some blank lines from result screen
        - allowed support for multi-name user IDs
 */
/* v1.2 - added indicator when vote is chosen as to which result was picked
        - improved sysop list and delete displays to show whole question
        - raised MAX to 1000
 */

#include "stdio.h"

#define MAX 1000

char mode, path[80], user[80], data[MAX][80];
int i, ptr, numvotes, line, ul, ierr;

main(argc, argv) int argc; char *argv[];
{ /* the main routine */
 int r,a;
 char x[80];

 puts("\nVOTE v1.2 by Mike Brent - Nov 1995\n\n");
 if (argc<3)
 { puts("\nUsage: VOTE mode user [-.DAT file]\nMode: N - new voter");
   puts("      R - results\n      S - sysop mode     A - add vote\n\n");
   exit(3);
 } 
 /* load globals with info from command line */
 mode=*argv[1];
 if (mode>'S') mode=mode-32;  /* might be lower-case, make uppercase */
 if ((mode!='N')&&(mode!='R')&&(mode!='S')&&(mode!='A'))
 { puts("\n* Invalid mode!\n");
   exit(2);
 }
 
 /* v1.1 User name might have spaces, so we use a - to indicate filename */

 strcpy(user,argv[2]);
 if (argc>3)
 { for (a=3; a<argc; a++)
   {  strcpy(x,argv[a]);
      if (x[0]!='-') { strcat(user," ");
                       strcat(user,x);   }
   }
 }
 
 strcpy(path,x[0]=='-' ? &x[1] : "VOTE.DAT");
 ptr=readdat();
 if (!ptr)
 { puts("\nUnable to locate data file!\n");
   puts(path);
   exit(1);
 }
 cntvotes();
 /* why are we here, anyway? */
 switch (mode)
 { case 'N' : r=newvote();
              break;
   case 'R' : r=results();
              break;
   case 'S' : r=sysop(0);
              break;
   case 'A' : r=newquest();
              break;
 }
puts("\nExitting vote...\n");
if (r==0) savedat();
printf("\n(c)1995 by Mike Brent\n\n");
printf("M.Brent\n");
printf("PO Box 42\n");
printf("Hedley, BC, Canada\n");
printf("V0X 1K0\n\n");
printf("email: mbrent@awinc.com\n");
printf("www: http:\\\\www.awinc.com\\users\\mbrent\n");
exit(r);
} 

readdat()
{ /* read the data file into memory */
 int x;
 
 ptr=fopen(path,"r");
 x=1;
 if (ptr)
 { while ((data[line-1][0]!='@')&&(x!=0))
   { x=fgets(&data[line][0],75,ptr);
     data[line][strlen(&data[line++][0])-1]=0; /* trim the CR off */
   }
 } 
 fclose(ptr);
 return(ptr);
}

savedat()
{ /* save the data file back */
 if (ierr==0) /* then there was no overflow */
 { ptr=fopen(path,"w");
   if (ptr)
   { for (line=0; data[line-1][0]!='@'; line++)
     { fputs(&data[line][0],ptr);
       fputs("\n",ptr);
     }
   } else
    puts("\n\n* Unable to save data file!\n");
   fclose(ptr);
  }
}

cntvotes()
{ /* count how many votes are in the file, load numvotes */
line=0; numvotes=0;
while (data[line][0]!='$')
 if (data[line++][0]=='#') numvotes++;
}

newvote()
{ /* Ah ha! Someone wants to do some voting, eh? */
 int flag, i;
 char xs[80];

 /* is this person in the database? */
 line=0; ul=-1;
 while (data[line++][0]!='$');
 while (data[line][0]!='@')
 { if (strcmp(&data[line][0],user)==0) ul=line;
   line++;
 }
 if (ul==-1) /* then he's a newbie! */
 { if (line>MAX-3)
   { puts("\n* Not enough room to add new user!\n");
     return(4);
   }
   strcpy(&data[line++][0],user);
   strcpy(&data[line][0],"0");
   for (i=0; i<numvotes; i++)
    strcat(&data[line][0],"0");
   strcpy(&data[++line][0],"@");
   ul=line-2;
 }
 flag=0; ul++;
 for (i=0; i<numvotes; i++) 
 { if (i>=strlen(&data[ul][0])) strcat(&data[ul][0],"0");
   if (data[ul][i]=='0')
   { data[ul][i]=vote(i);
     show(i);
     flag=1;
     puts("\nHit <cr> to continue...\n");
     gets(xs);
   }
 }
 if (flag==0) puts("\nNo new votes\n");
 return(0);
}

results()
{ /* showing results of votes */
 char x[80];
 int y;

 x[0]=5;
 while (x[0]!=0)
 { printf("\nShow from which vote? (1-%d, <cr> to abort):",numvotes);
   gets(x);
   if (x[0])
   { y=atoi(x);
     if ((y<1)||(y>numvotes)) puts("\n* Invalid vote number!\n");
     else { x[0]=0; y--;
            while (x[0]!='Q')
            { if (show(y++)!=0)
              { puts("\n[N]ext or [Q]uit? (<cr> = Next)"); 
                gets(x);
                if (x[0]>96) x[0]=x[0]-32; /* uppercase */
              }
              else x[0]='Q';
            } /* while */
          } /* else */
   } /* if */
 } /* while */
} /* results */

sysop()
{ /* sysop control menu, basic functions. Use a text editor for anything
     more complex */
  char q[80];

  q[0]=0; ierr=0;
  while (q[0]!='Q')
  { puts("\nSysop control mode\n[A]dd question\n[D]elete question\n"); 
    puts("[L]ist questions\n[Q]uit\n");
    gets(q);
    q[1]=0; /* single character string only */
    if (q[0]>96) q[0]=q[0]-32; /* make uppercase */
    switch (q[0])
    { case 'A': newquest();
                break;
      case 'D': delquest();
                break;
      case 'L': listquest();
                break;
    }
  }
return(0);
}

vote(x) int x;
{ /* vote on question number x */
 int ml,rl,y,z,x1,x2;
 char xs[80],n1[80],n2[80];

 ml=setline(x);
 while (data[line][0]!='!')
  printf("%s\n",&data[line++][0]);
 line++; rl=line;
 y=1;
 while (data[line][0]!='#')
 { printf("%2d) %s\n",y,&data[line++][0]);
   while (data[line][0]!='!')
    printf("    %s\n",&data[line++][0]);
   y++; line=line+2;
 }
 y--; z=0;
 while ((z<1)||(z>y))
 { printf("\nYour choice? (1-%d):",y);
   gets(xs);
   puts("\n");
   z=atoi(xs);
 }
 line=rl; y=1;
 while (data[line][0]!='#')
 { while (data[line++][0]!='!');
   x1=0; x2=0;
   while (data[line][x2]!='/')
    n1[x1++]=data[line][x2++];
   n1[x1]=0; x2++; x1=0;
   while (data[line][x2])
    n2[x1++]=data[line][x2++];
   n2[x1]=0;
   if (z==y) itoa(atoi(n1)+1,n1);
   itoa(atoi(n2)+1,&n2);
   strcpy(&data[line][0],n1);
   strcat(&data[line][0],"/");
   strcat(&data[line][0],n2);
   y++; line++;
 }
 return(z+48);
}

itoa(x,s) int x; char s[];
{ /* integer to ASCII - upper limit 999 */
 int y;

 y=0;
 if (x>99)
 { s[y++]=x/100+48;
   x=x%100;
 }
 if (x>9)
 { s[y++]=x/10+48;
   x=x%10;
 }
 s[y++]=x+48;
 s[y]=0;
}

setline(x) int x;
{ /* set variable LINE to first line of question x. Also return line # */
 int y;

 line=0; y=0;
 while (y<x)
 { while (data[line++][0]!='#');
   y++;
 }
 return(line);
}

show(x) int x;
{ /* show question x and results. Return 0 if the question asked for
     is invalid */
 int ml,y,z,x1,x2,rc;
 char xs[80];

 if (x<numvotes)
 { ml=setline(x);
   printf("\nVote number %d - ",x+1);
   while (data[line][0]!='!')
    printf("%s\n",&data[line++][0]);
   line++; puts("\n");
   y=1;
   while (data[line][0]!='#')  
   { printf("%2d) %s\n",y,&data[line++][0]);
     while (data[line][0]!='!')
      printf("    %s\n",&data[line++][0]); 
     strcpy(xs,&data[++line][0]);
     z=0; i=0;
     while(xs[++i]!='/');
     z=i+1; xs[i]=0;
     x1=atoi(xs); x2=atoi(&xs[z]);
     printf("-----> %d / %d = %d%%\n",x1,x2,x1*100/x2);
     y++; line++;
   }
   rc=1;
   /* is this person in the database? */
   line=0; ul=-1;
   while (data[line++][0]!='$');
   while (data[line][0]!='@')
   { if (strcmp(&data[line][0],user)==0) ul=line;
     line++;
   }
   if (ul!=-1) /* -1 means not found */
   { ul++;
     printf("Your vote: %d\n",data[ul][x]-48);
   }
 }
 else rc=0;
 return(rc);
}

newquest()
{ /* insert a new question into the data file */
 char x[80];
 int x,y,z,rc;

 puts("\nReally insert a new question? y/[N]\n");
 gets(x);
 rc=1; /* return code 1 means don't save data file */
 if ((x[0]=='y')||(x[0]=='Y'))
 { x=0;
   while (data[x++][0]!='@') 
    if (data[x][0]=='$') y=x;
   printf("There are %d free lines...\n",MAX-x);
   if ((MAX-x)<11) puts("Not enough room... delete something!\n\n");
   else { insert(y);
          puts("Enter the question. Enter a blank line to end.\n");
          strcpy(&data[y][0],"Contributed by: ");
          strcat(&data[y++][0],user);
          insert(y);
          while (x[0])
          { gets(x);
            if (x[0]) 
            { strcpy(&data[y++][0],x);
              insert(y);
            }
          }
          strcpy(&data[y++][0],"!");
          insert(y);
          z=1; x[0]='Y'; 
          while (x[0]=='Y')
          { printf("\nEnter response item %d. Enter a blank line to end.\n",z++);
            x[0]=1;
            while (x[0])
            { gets(x);
              if (x[0])
              { strcpy(&data[y++][0],x);
                insert(y);
              }
            }
            strcpy(&data[y++][0],"!");
            insert(y);
            strcpy(&data[y++][0],"0/0");
            insert(y);
            puts("\nAnother response? (Y/[N])\n");
            gets(x);
            if (x[0]>96) x[0]=x[0]-32;
          }
          strcpy(&data[y++][0],"#");
          strcpy(&data[y][0],"$");
          rc=0; /* DO save data file (if IERR wasn't set) */
        }
 }
 return(rc);
}

delquest()
{ /* delete a question from the data file */
 int l,x,y,z;
 char xs[80];

 printf("\nDelete which question? (1-%d, <cr> = abort):",numvotes);
 gets(xs);
 x=atoi(xs);
 if ((xs[0]==0)||(x<1)||(x>numvotes))
  puts("\nNot done.\n");
 else
 { setline(--x);
   printf("%s\n%s\n",&data[line][0],&data[line+1][0]);
   puts("\nDelete this question? (y/[N])\n");
   gets(xs);
   if ((xs[0]=='y')||(xs[0]=='Y'))
   { while (data[line][0]!='#')
      delline(line);
     delline(line);
     puts("\nQuestion deleted.\n");
     /* now update the users */
     l=0;
     while (data[l++][0]!='$');
     while (data[l++][0]!='@')
     { y=0; z=0;
       while (data[l][z])
       { if (z!=x) xs[y++]=data[l][z];
         z++;
       } xs[y]=0;
       strcpy(&data[l][0],xs);
       l++;
     }
   }
   else puts("\nNot done.\n");
 }
}

listquest()
{ /* list the questions with numbers, for reference purposes.
     Just list the first line of each question. */
 int y;

 puts("\n");
 line=0; y=1;
 while (data[line][0]!='$')
 { printf("%2d) %s\n     %s\n",y++,&data[line][0],&data[line+1][0]);
   while (data[line++][0]!='#');
 }
}

insert(x) int x;
{ /* insert a blank line at line x. Sets IERR to 1 if overflow. */
 int i,l;

 l=0;
 while (data[++l][0]!='@');
 if (l>=MAX-1) 
 { ierr=1;
   puts("\n**WARNING** Data buffer overflow. Data file will NOT be saved.\n");
   l=MAX-2;
 }
 for (i=l+1; i>x+1; i--)
  strcpy(&data[i][0],&data[i-1][0]);
}

delline(x) int x;
{ /* delete line x */
 int i,l,u;

 l=0;
 while (data[++l][0]!='@');
 for (i=x; i<l; i++)
  strcpy(&data[i][0],&data[i+1][0]);
}

