#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pwd.h>
#include<limits.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<signal.h>
#include"headers/definations.h"
#include <time.h>
#include <poll.h>

void cronjob(char *full_c)
{
	char *tok_env_var = strtok(full_c, " \n");
    int no_args = 0;
    char** argu = (char**)(malloc(sizeof(char*)*10));
    printf("in\n");
    while(tok_env_var!=NULL) 
    {   
        argu[no_args] = (char*)malloc(BUF_SIZE);
        strcpy(argu[no_args], tok_env_var);
        no_args++;
        printf("%s\n", argu[no_args-1]);
        tok_env_var = strtok(NULL, " \n");
    }
    int time_elapsed = 0;
    struct pollfd mypoll = {STDIN_FILENO, POLLIN | POLLPRI};

    while (1)
    {
        if (poll(&mypoll, 1, 1000 * atoi(argu[4])))
        {
            time_elapsed += atoi(argu[4]);
            if (time_elapsed == atoi(argu[6]))
                break;
        }
        execvp(argu[2],argu[2]); //If there is no builtin implementation.
    };
    return;
}