/*
MIT License
	
	Copyright (c) 2018 ishansanganeria
	
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/


#include <stdio.h> 
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define RESET   	"\x1B[0m"
#define BOLDRED 	"\033[1m\033[31m"
#define BOLDYELLOW  "\033[1m\033[33m"

char hist[1000000][80]; 
char list[2] = {'\0'};
int histcount=0;

void executehist(char* a);

char* strcpyR(char *in, int u, int l)			//UPPER LIMIT'S INDEX ISNT INCLUDED AND IT APPENDS A '\n' AT THE END
{
	char *op;
	op = malloc( sizeof(char) * (u-l+1) );
	for (int i = 0; i < (u-l); ++i)
		op[i] = in[l+i];
	op[strlen(op)]='\n';
	return op;
}

void printhistory()
{
	for (int i = 0; i < histcount; ++i)
		printf("%d\t\t%s", i+1, hist[i]);
}

void execute(char* in,int len)					//INPUT  EG: "cat hello.c","ps -l","ls"
{
	int a=0, i=0, flag=0;
	char *command, *arg;
	command=malloc(sizeof(char)*10);
	arg=malloc(sizeof(char)*50);

	while(in[i]!=' ' && i!=(len-1))				//SEPERATING THE MAIN COMMAND EG: cat
	{
		command[i] = in[i];
		i++;
		if (in[i] == ' ')						//CHECKS IF THERE IS ANY ARGUMENT
			flag=1;
	}
	command[i]='\0';
	i++;

	if (flag == 1)								//ARGUMENT EXISTS						
	{
		while (i!=len-1)
		{
			arg[a] = in[i];
			i++; a++;
		}
		arg[a]='\0';

		if (strcmp(command,"cd") == 0)
		{
			if( chdir(arg) == -1)
				printf("bash: cd: %s: No such file or directory",arg);
			return;
		}

		int id=fork();
		if (id == 0)
		{
			if( execlp(command,command,arg,0) == -1)
				printf("bash: %s: command not found\n",command);
		}
		else
			wait(NULL);
	}
	else										//ARGUMENT DOES NOT EXIST
	{
		char *in1;
		in1=malloc(sizeof(char)*len-1);
		strncpy(in1,in,len-1);

		if ( (strcmp(in1,"cd\0") == 0) || (strcmp(in1,"cd\n") == 0) )
		{
			chdir("/root");
			return;
		}

		int id=fork();
		if(id == 0)
		{
			if(strcmp(in1,"history\0") == 0)
				printhistory();
			else if(execlp(in1,in1,0) == -1)
				printf("bash: %s: command not found\n",in1);
		}
		else
			wait(NULL);
	}
}

void makearray(char *a)
{
	int top=0;
	list[2] = list[1] = list[0] = '\0';
	for (int i = 0; i < strlen(a); ++i)
		if (a[i] == '|' || a[i] == '>' || a[i] == '<')
			list[top++] = a[i];
}

void printshell()
{
	int fd,n;
	char buff[128];
	fd=open("printredirec.txt",O_WRONLY | O_CREAT,0777);
	printf(BOLDYELLOW  "\nshell_project@ishan_ashna:" BOLDRED "~");
	fflush(stdout);
	int id = fork();
	if(id==0)
	{
		dup2(fd,1);
		execlp("pwd","pwd",0);
	}
	else
	{
		wait(NULL);
		close(fd);
		fd=open("printredirec.txt",O_RDONLY);
		while( ( n = read(fd,buff,128) ) > 0 )
			write(1,buff,n-1);					//TO DELETE THE "\n" AT THE END 
		printf(RESET "# ");
		if(!fork())
			execlp("rm","rm","printredirec.txt");
		else
			wait(NULL);
	}
}

void append2hist(char* s)
{
	int i=0;
	while(i != strlen(s))
	{
		hist[histcount][i] = s[i];
		i++;
	}
	histcount++;
}

void execpr(char *a)
{
	char *c[3];
	int u=0, l=0, no=0;

	for (int i = 0; i < strlen(a); ++i) 			//THIS WILL SEPERATE THE COMMANDS AND ARGUMENTS INTO A CHAR * ARRAY
		if (a[i] == '|' || a[i] == '>')
		{
			if (l != 0)
				l = u + 3;
			u = i-1;
			c[no] = malloc( sizeof(char) * (u-l) );
			c[no] = strcpyR(a,u,l);
			no++;
			l++;
		}
	l = u + 3;
	u = strlen(a);
	c[no] = strcpyR(a,u-1,l);

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < strlen(c[i]); ++j)
			if(c[i][j]=='\n')
				c[i][j] = '\0';

	int id1 = fork();
	if (id1 == 0)
	{
		int pfd[2];
		pipe(pfd);
		int id=fork();

		if(id == 0)										//CHILD
		{
			close(pfd[0]);
			dup2(pfd[1],1);
			close(pfd[1]);
			if(	execlp(c[0],c[0],0) == -1)
				if(!fork())
					printf("bash: %s: This command could not be executed",c[0]);
			exit(0);
		}
		else											//PARENT
		{
			wait(NULL);
			close(pfd[1]);
			dup2(pfd[0],0);
			close(pfd[0]);
			int fd=open( c[2], O_WRONLY | O_CREAT, 0777);
			dup2(fd,1);
			close(fd);
			if(	execlp(c[1],c[1],0) == -1)
				if(!fork())
					printf("bash: %s: This command could not be executed",c[1]);
			close(fd);
			close(1);
		}
		no=0;
	}
	else
		wait(NULL);
	return;
}

void execlr(char *a)
{
	char *c[3];
	int u=0, l=0, no=0;

	for (int i = 0; i < strlen(a); ++i) 			//THIS WILL SEPERATE THE COMMANDS AND ARGUMENTS INTO A CHAR * ARRAY
		if (a[i] == '<' || a[i] == '>')
		{
			if (l != 0)
				l = u + 3;
			u = i - 1;
			c[no] = malloc( sizeof(char) * (u-l) );
			c[no] = strcpyR(a, u, l);
			no++;
			l++;
		}
	l = u + 3;
	u = strlen(a) - 1;
	c[no] = malloc( sizeof(char) * (u-l) );
	c[no] = strcpyR(a,u,l);

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < strlen(c[i]); ++j)
			if(c[i][j] == '\n')
				c[i][j] = '\0';

	int id2 = fork();
	if (id2 == 0)
	{
		int id1 = fork();
		if (id1 == 0)
		{
			int fd1,fd2;
			if ( (fd1 = open(c[1], O_RDONLY)) == -1)
			{
				printf("Error in opening %s\n",c[1]);
				exit(0);
			}
			if ( (fd2 = open(c[2], O_CREAT | O_WRONLY, 0777) ) == -1 )
				printf("Error in opening %s\n",c[2]);
			dup2(fd1,0);
			dup2(fd2,1);
			execlp(c[0],c[0],0);
			exit(0);
		}
		else
			wait(NULL);
	}
	else
		wait(NULL);
}

void execpp(char* a)
{
	char *c[3];
	int u=0, l=0, no=0;

	for (int i = 0; i < strlen(a); ++i) 			//THIS WILL SEPERATE THE COMMANDS AND ARGUMENTS INTO A CHAR * ARRAY
		if (a[i] == '|')
		{
			if (l != 0)
				l = u + 3;
			u = i-1;
			c[no] = malloc( sizeof(char) * (u-l) );
			c[no] = strcpyR(a,u,l);
			no++;
			l++;
		}

	l = u + 3;
	u = strlen(a);
	c[no] = strcpyR(a,u-1,l);

	int id=fork();
	if (id == 0)
	{
		int pfd[2];
		pipe(pfd);
		int id1=fork();
		if(id1>0)
		{
			close(pfd[1]);
			close(0);
			dup(pfd[0]);
			close(pfd[0]);
			int pfd1[2];
			pipe(pfd1);
			int id2=fork();
			if(id2>0)
			{
				close(pfd1[0]);
				close(1);
				dup(pfd1[1]);
				close(pfd1[1]);
				execute(c[1],strlen(c[1]));
			}
			else
			{
				close(pfd[1]);
				close(0);
				dup(pfd[0]);
				close(pfd[0]);
				execute(c[2],strlen(c[2]));
			}
		}
		else
		{
			close(pfd[0]);
			close(1);
			dup(pfd[1]);
			close(pfd[1]);
			execute(c[0],strlen(c[0]));
		}
	}
	else
		wait(NULL);
}

void execP(char* a)
{
	int i=0;
	while(a[i]!='|')
		i++;
	char *arg1,*arg2;
	arg1=malloc(sizeof(char) * (i-1));
	arg2=malloc(sizeof(char) * (strlen(a)-i+2));
	arg1=strcpyR(a,i-1,0);
	arg2=strcpyR(a,strlen(a)-1, i+2);

	int ik=fork();
	if (ik == 0)
	{
		int pfd[2];
		pipe(pfd);
		int id=fork();
		if(id==0)
		{
			close(pfd[0]);
			close(1);
			dup(pfd[1]);
			close(pfd[1]);
			execute(arg1,strlen(arg1));
		}
		else
		{
			close(pfd[1]);
			close(0);
			dup(pfd[0]);
			close(pfd[0]);
			execute(arg2,strlen(arg2));

		}
	}
	else
	{
		wait(NULL);
		wait(NULL);
	}
	return ;
}

void execL(char* a)
{
	int i=0;
	while(a[i]!='<')
		i++;
	char *arg1,*arg2;
	arg1=malloc(sizeof(char) * (i-1));
	arg2=malloc(sizeof(char) * (strlen(a)-i+2));
	arg1=strcpyR(a,i-1,0);
	arg2=strcpyR(a,strlen(a)-1, i+2);

	for (int i = 0; i < strlen(arg2); ++i)
		if(arg2[i] == '\n')
			arg2[i] = '\0';

	int id=fork();
	if (id == 0)
	{
		int fd1;
		if( (fd1 = open(arg2,O_RDONLY) ) == -1)
			printf("%s could not be opened/found\n",arg2);
		close(0);
		dup(fd1);
		execute(arg1,strlen(arg1));
	}
	else
		wait(NULL);
}

void execr(char* a)
{
	int i=0;
	while(a[i]!='>')
		i++;
	char *arg1,*arg2;
	arg1=malloc(sizeof(char) * (i-1));
	arg2=malloc(sizeof(char) * (strlen(a)-i+2));
	arg1=strcpyR(a,i-1,0);
	arg2=strcpyR(a,strlen(a)-1, i+2);

	for (int i = 0; i < strlen(arg2); ++i)
		if(arg2[i] == '\n')
			arg2[i] = '\0';

	int id=fork();
	if (id == 0)
	{
		int fd1;
		if( (fd1 = open(arg2,O_CREAT | O_WRONLY,0777) ) == -1)
		{
			printf("%s could not be opened/found\n",arg2);
			exit(0);
		}
		close(1);
		dup(fd1);
		execute(arg1,strlen(arg1));
	}
	else
		wait(NULL);
}

void analyseMulInput(char *a)
{
	int count=1,i,n;
	char* in;
	for (i = 0; i < strlen(a); ++i)
		if( (a[i] == '&') && (a[i+1] == '&') )		//COUNTING THE NUMBER OF OCCURENCE OF &&
		{
			count++;
			i++;
		}
	n = i = 0;
	while((count--)!=1)
	{
		while(a[i+1]!='&')
			i++;
		in=strcpyR(a,i,n);
		execute(in,strlen(in));
		puts("\n");
		free(in);
		i=i+4;
		n=i;
	}
	in=strcpyR(a,strlen(a)-1,n);
	execute(in,strlen(in));	
	free(in);
	return;
}

void analyseinput(char *a)
{
	int len=strlen(a);
	for (int i = 0; i < len; ++i)
	{
		if (a[i] == '|' || a[i] == '>' || a[i] == '<')
		{
			makearray(a);
			if 	(list[0] == '|' && list[1] == '>')
				execpr(a);
			else if	(list[0] == '<' && list[1] == '>')
				execlr(a);
			else if	(list[0] == '|' && list[1] == '|')
				execpp(a);
			else if	(list[0] == '|' && list[1] == '\0')
				execP(a);
			else if	(list[0] == '<' && list[1] == '\0')
				execL(a);
			else if	(list[0] == '>' && list[1] == '\0')
				execr(a);
			return;
		}
		else if (a[i] == '&')
		{
			analyseMulInput(a);
			return;
		}
		else if (a[i] == '!')
		{
			executehist(a);
			histcount--;
			return;
		}
	}
	execute(a,len);								//FOR A SINGLE WORD INPUT
}

void executehist(char* a)
{
	int n = atoi( strcpyR (a,strlen(a)-1,1) );
	analyseinput(hist[n-1]);
}

char* input()
{
	char *buff;
	buff = malloc( sizeof(char) * 80);
	fgets(buff,80,stdin);
	if(strlen(buff) == 1)
		return NULL;
	return buff;
}

int main(int argc, char const *argv[])
{
	if(!fork())
	{
		execlp("clear","clear",0);
		exit(0);
	}
	else
		wait(NULL);
	
	int i = getpid();
	while(i == getpid())
	{
		char *inputs;
		printshell();
		inputs=input();
		if (inputs == NULL)
			continue;
		else if(!strcmp(inputs,"exit\n"))
		{
			printf("Exiting the shell. Thankyou!! \n\n");
			return 0;
		}
		append2hist(inputs);
		analyseinput(inputs);
	}
}
