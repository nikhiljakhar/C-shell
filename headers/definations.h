#ifndef DEFINATIONS_H
#define DEFINATIONS_H

long long int PATH_SIZE; 
long long int NUM_CMDS;
int backflag;
int outdirect;
int doutdirect;
int indirect;
int backflag;
int wcf;
int wcs;
int fgchild;
int no_child_processes;
char* child_processes;
size_t BUF_SIZE;
char* delims;
char** names_child;

struct info{
    char* HOME;
    char* user;
    char* host;
};
typedef struct info info;
struct back_job{
        int id;
        char child_name[100];
        int state;
};
typedef struct back_job back_job;
back_job child_procs[100];
#endif