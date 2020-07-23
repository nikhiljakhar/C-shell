#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pwd.h>
#include<limits.h>

long long int PATH_SIZE = 1000; 
size_t BUF_SIZE = 131072;
char* delims=" ;\t\n\r";

typedef struct info info;
typedef struct back_job back_job;


long long int NUM_CMDS = 10;
int backflag = 0;
int outdirect = 0;
int doutdirect = 0;
int indirect = 0;
int wcf = 0;
int wcs = 0;
int no_child_processes = 0;
int fgchild = 0;
