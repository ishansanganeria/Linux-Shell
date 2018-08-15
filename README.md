# Linux-Shell

AIM:

1. UNIX Shell and History Feature
This project consists of designing a C program to serve as a shell interface that accepts
user commands and then executes each command in a separate process. This project
consists of two parts: Creating a Child Process and Creating a History Feature.

2. UNIX Shell with I/O redirection and pipe
This project consists of designing a C program to serve as a shell interface that accepts
user commands and then executes each command in a separate process. The program
should also support for I/O redirection and pipe, like ls | wc> file1.

3. Shell and System Calls
Design and implement an extremely simple shell that knows how to launch new
programs, and also recognizes two internal commands (exit and cd).


List of functions used:

1.strcpyR (char *in, int u, int l) :
It is used to create a new string. this function returns the address of new string
which is a substring of original string. the new string is created by taking only the
upper(excluded) and lower bound(included) of the original string.

2.printhistory() :
It is used to print history of commands executed uptil now.

3.execute(char* in, int len):
This function is used to execute the following types of commands
eg: cathello.c, ps -l, ls, history, cd, cd <directory>
This function first checks if any particular command has any arguments and deals
with them accordingly.
Since the execlp function cannot be called in the main process therefore this
function creates a child process of the main process and performs the execlp
function in the child process.

4.makearray(char *a) :
This function takes the input command as an argument and fills an array of
characters depending upon the type of pipe/redirection that it encounters.

5.printshell() :
It is used to print the dollar prompt each time when the user needs to enter a new
command.

6.append2hist(char* s) :
This function appends the command which the user enter to an array named hist.
This array can store upto a million commands.

7. analyseMulInput(char *a) :
It is used to deal with compound statements
eg: ps && ls && ps-l

8.analyseinput(char *a) :
This function decides about the type of command that the user has entered and
calls the relevant function needed.

9.input() :
This function is used to take input from the user.

10.executehist(char *a) :
It is used to execute any previously entered command.

11.execpr(char *a) :
This fn is used to execute the pipe with right redirection
ex: ps |wc>pr.txt ls | grep .c > pr1.txt

12.execlr(char *a):
This function is used to execute left and right redirection
ex: wc<f1>f2

13.execpp(char *a):
This function is used to execute two pipe functions
ex: ls|wc|sort

14.execP(char *a):
This function deals with single pipe
ex: ls|wc

15.execL (char *a):
This function is used to execute left redirection
ex: wc<f1

16.execr(char *a):
This function is used to execute right redirection
ex: ls>f1