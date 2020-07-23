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

void set_env_var(char * full_c)
{

	char *tok_env_var = strtok(full_c, " \n");
    int no_args = 0;
    char** argu = (char**)(malloc(sizeof(char*)*5));

    while(tok_env_var!=NULL) 
    {   
        argu[no_args] = (char*)malloc(BUF_SIZE);
        no_args++;
        strcpy(argu[no_args-1], tok_env_var);
        //printf("%s\n", argu[no_args-1]);
        tok_env_var = strtok(NULL, " \n");
    }
    //printf("%d\n", no_args);

    if(no_args!=3 && no_args!=2)
    	printf("Usage: setenv [var name] [value]\n");
    if(no_args==3)
    	setenv(argu[1], argu[2], 1);
    if(no_args==2)
    	setenv(argu[1], "", 1);
	return;
}

void unset_env_var(char* full_c){

    char *tok_unset = strtok(full_c, " \n");
    int no_args = 0;
    char** argu = (char**)(malloc(sizeof(char*)*5));

    while(tok_unset!=NULL) 
    {   
        argu[no_args] = (char*)malloc(BUF_SIZE);
        strcpy(argu[no_args], tok_unset);
        tok_unset = strtok(NULL, " \n");
        no_args++;
    }

    if(no_args!=2)
    	printf("Usage: unsetenv [var name]\n"); 
    else if(no_args == 2)
    	unsetenv(argu[1]);

}

void jobs(){
	//printf("child prc = %d\n", no_child_processes);
	int k = 0;
	int no_r = 1;
    while(k<no_child_processes){
    	if(child_procs[k].state!=-1)
    	{
    		//printf("%s\n", child_procs[k].c);
    		char inff[500];
	        //printf("%d\n", pid);
	        char *infpfile = (char*)malloc(BUF_SIZE);
	        sprintf(infpfile, "/proc/%d/stat", child_procs[k].id);
	        int fd = open(infpfile, O_RDONLY);
	        if(fd>=0)
	        {
	        	int h = 1;
		        read(fd, inff, 500);
		        char *job_tok = strtok(inff, " \n"); 
		        while(job_tok!=NULL && h<3){
		            job_tok = strtok(NULL, " \n");
		            h++;
		        }
		        printf("[%d] ", no_r);
		        if(strcmp(job_tok,"S") && strcmp(job_tok,"R")) printf("Stopped ");
		        else printf("Running ");
		        printf("%s [%d]\n", child_procs[k].child_name, child_procs[k].id);
		        no_r++;
		    }
		}
		k++;
    }
    return;
}

void kjob(char* full_c)
{
	int i = 0;
	//printf("%s\n", full_c);
	char *kjob_tok = strtok(full_c, " \n");
    char** part_arg = (char**)(malloc(sizeof(char*)*10));
    while(kjob_tok!=NULL) 
    {
    	i++;
        part_arg[i-1] = (char*)malloc(BUF_SIZE);
        strcpy(part_arg[i-1], kjob_tok);
        //printf("%s\n", part_arg[i-1]);
        kjob_tok = strtok(NULL, " \n");
    }
    if(i!=3)
    	printf("Usage: kjob [job num] [signal num]\n");
	else
	{
    	int j = 0;
    	int req_pid;
    	int req_ind = 0;
    	for(j=0;j<no_child_processes;j++)
    	{
    		if(child_procs[j].state !=-1)
    		{
    			req_ind++;
    			if(atoi(part_arg[1]) == req_ind)
    			{
    				req_pid = child_procs[j].id;
    				break;
    			}		
    		}
    	}
    	if(j < no_child_processes)
    		kill(req_pid,atoi(part_arg[2]));
		if(j==no_child_processes)
			printf("Process with this PID does not exist\n");
	}
		
	return;
}

void overkill()
{
	int j = 0;
	while(j<no_child_processes)
	{
		if(child_procs[j].state !=-1)
			kill(child_procs[j].id,9);
		j++;
	}
	return;
}

void fg(char* full_c)
{
	int i = 0;
	char *kjob_tok = strtok(full_c, " \n");
    char** part_arg = (char**)(malloc(sizeof(char*)*10));
    while(kjob_tok!=NULL) 
    {
    	i++;
        part_arg[i-1] = (char*)malloc(BUF_SIZE);
        strcpy(part_arg[i-1], kjob_tok);
        kjob_tok = strtok(NULL, " \n");
    }
	if(i!=2)
		printf("Usage: fg [job num]\n");
	else
	{
    	int j = 0;
    	int req_pid;
    	int req_ind = 0;
    	for(j=0;j<no_child_processes;j++)
    	{
    		if(child_procs[j].state !=-1)
    		{
    			req_ind++;
    			if(atoi(part_arg[1]) == req_ind)
    			{
    				req_pid = child_procs[j].id;
    				child_procs[j].state = -1;
    				break;
    			}		
    		}
    	}
    	if(j < no_child_processes)
    	{
    		int status;
    		kill(req_pid, SIGCONT);
			waitpid(req_pid,&status,WCONTINUED);
    	}
		if(j==no_child_processes)
			printf("Process with this PID does not exist\n");
	}
	return;
}

void bg(char* full_c)
{
	int i = 0;
	char *kjob_tok = strtok(full_c, " \n");
    char** part_arg = (char**)(malloc(sizeof(char*)*10));
    while(kjob_tok!=NULL) 
    {
    	i++;
        part_arg[i-1] = (char*)malloc(BUF_SIZE);
        strcpy(part_arg[i-1], kjob_tok);
        kjob_tok = strtok(NULL, " \n");
    }
	if(i!=2)
		printf("Usage: bg [job num]\n");
	else
	{
		
    	int j = 0;
    	int req_pid;
    	int req_ind = 0;
    	for(j=0;j<no_child_processes;j++)
    	{
    		if(child_procs[j].state !=-1)
    		{
    			req_ind++;
    			if(atoi(part_arg[1]) == req_ind)
    			{
    				req_pid = child_procs[j].id;
    				child_procs[j].state = 1;
    				break;
    			}		
    		}
    	}
    	if(j < no_child_processes)
    	{
    		kill(req_pid, SIGCONT);
    		child_procs[j].state = 1;
    	}
		if(j==no_child_processes)
			printf("Process with this PID does not exist\n");
	
	}
	return;
}