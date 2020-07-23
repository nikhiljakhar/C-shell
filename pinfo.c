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

void pinfo(char *fullcom, info d)
{
	char *token3 = strtok(fullcom, " ");
    token3 = strtok(NULL, " ");
    char* nopid = (char*)(malloc(sizeof(char)*10));
    if(token3 == NULL)
    {
    	char* temp = (char*)(malloc(sizeof(char)*10));
        sprintf(temp, "%d", getpid());
    	strcpy(nopid,temp);
    }
    else
    	strcpy(nopid,token3);	
    //printf("%s\n", nopid);
    char *pidfile = (char*)malloc(BUF_SIZE);
    sprintf(pidfile, "/proc/%s/stat", nopid);

    int fd = open(pidfile, O_RDONLY);
 
    if(fd >= 0)
    {
	    printf("PID -- %s\n", nopid);

	    struct stat attrib;
	    
	    fstat(fd, &attrib);
	    read(fd, pidfile, 1000);
	    
	    char *token4 = strtok(pidfile, delims);
	    int ind = 0;

	    while(token4!=NULL){
	        token4 = strtok(NULL, delims);
	        if(ind==1) 
	        	printf("Process Status -- %s\n", token4);
	        if(ind==21) 
	        	printf("Memory -- %s {Virtual Memory}\n", token4);
	        ind++;
	    }
	    
	    //exec path
	    sprintf(pidfile, "/proc/%s/exe", nopid);
	    char *pathe = (char*)malloc(PATH_SIZE);
	    readlink(pidfile, pathe, PATH_SIZE);
	    int hlen = 0;
	    while(hlen<strlen(d.HOME) && d.HOME[hlen]==pathe[hlen] && strlen(pathe)>=strlen(d.HOME))
	    {
	     	hlen++;
	    }
	    if(hlen!=strlen(d.HOME))
	    	printf("Executable path -- %s\n", pathe); 
	    else 
	        printf("Executable path -- ~%s\n", pathe+strlen(d.HOME));
	}
	else
		printf("Wrong PID\n");

	return;
}