#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/utsname.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <fcntl.h>
#include <sys/sysmacros.h>
#include"headers/definations.h"
#include"headers/cmdls.h"
#include"headers/pinfo.h"
#include"headers/cronjob.h"
#include"headers/morebuiltins.h"

int out_fd, in_fd;
volatile sig_atomic_t cint, zint;

void check_dir(char* dir_comm)
{
    for(int i=0;i<strlen(dir_comm)-1;i++)
    {
        if(i>0)
        {
            if(dir_comm[i] == '>' && dir_comm[i+1] != '>' && dir_comm[i-1] != '>')
            {
                outdirect = 1;
                if(wcf == 0)
                    wcf = 1;
                else if(wcf == 2)
                    wcs = 1;
            }
        }
        if(dir_comm[i] == '>' && dir_comm[i+1] == '>')
        {
            doutdirect = 1;
            if(wcf == 0)
                wcf = 1;
            else if(wcf == 2)
                wcs = 1;
        }
        if(dir_comm[i] == '<')
        {
            indirect = 1;
            if(wcf == 0)
                wcf = 2;
            else if(wcf == 1)
                wcs = 2;
        }
    }
}
void sig_handle(int x)
{
    pid_t pid;
    int status3;
    //printf("inchild pid = %d\n", child_procs[no_child_processes-1].id);
    for(int l = 0;l<no_child_processes;l++)
    {
        pid = waitpid(child_procs[l].id,&status3,WNOHANG);
    //printf("in pid = %d\n", pid);
        if(pid>0)
        {
            printf("\nProcess with pid = %d exited normally\n", pid);
            for(int m = 0;m<no_child_processes;m++)
            {
                if(child_procs[m].id == pid)
                    child_procs[m].state = -1;
            }
        }
    }
}

void signalHandler()
{
    int stat;
    if(fgchild)
        stat=kill(fgchild, SIGINT);
    fgchild = 0;
    return;
}

void signalHandlerctrl_z()
{
    int stat;
    if(fgchild)
        stat = kill(fgchild, SIGSTOP);
    //change_process_status(fgchild, STATUS_SUSPENDED);
    fgchild = 0;
    return;
}

void lsh_launch(char **args)
{
    pid_t pid, wpid;
    int status;

    pid = fork();
    if(pid<0)
        perror("sh");
    else if(pid>0) {
       // Parent process
        if(backflag==0)
        {
            do {
            wpid = waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
        else
        {
            printf("Process with pid = %d started\n", pid );
            child_procs[no_child_processes].id = pid;
            //printf("startchild pid = %d\n", child_procs[no_child_processes].id);
            strcpy(child_procs[no_child_processes].child_name,args[0]);
            child_procs[no_child_processes].state = 1;
            no_child_processes++;
            //printf("%s\n", child_procs[no_child_processes].child_name);
        }
    }
    else {
        // Child process
        if(indirect == 1){
            dup2(in_fd, STDIN_FILENO);
            //close(in_fd);
        }
        if(outdirect == 1 || doutdirect == 1){
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }
        if (execvp(args[0], args) == -1) 
        {
            perror("shell");
        }
        exit(EXIT_FAILURE);
    }

    return;
}

int main(){
	info person;
    person.HOME=(char*)(malloc(PATH_SIZE));
    getcwd(person.HOME,PATH_SIZE);
    person.host = (char*)(malloc(HOST_NAME_MAX));
    gethostname(person.host, HOST_NAME_MAX);
    person.user = (char*)(malloc(LOGIN_NAME_MAX));
    getlogin_r(person.user, LOGIN_NAME_MAX);

	char* s = (char*)malloc(BUF_SIZE);
    char* inputstring = (char*)malloc(BUF_SIZE);
    char** history = (char**) malloc(sizeof(char*)*21);
    int hpoint = 0, fhf = 0, prevl =0, newl =0;

    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

	while(1)
	{
        //pid_t checkpid;
        //checkpid = wait(NULL);
        //if(checkpid<0)
        //signal(SIGTSTP, SIG_IGN);
        //signal(SIGINT, signalHandler);
        //signal(SIGINT, SIG_IGN);
        signal(SIGCHLD, sig_handle);
        //printf("no = %d\n", no_child_processes);
		char* dirhome = (char*)malloc(PATH_SIZE);
	    getcwd(dirhome, PATH_SIZE);
        char** allcomm = (char**) malloc(sizeof(char*)*NUM_CMDS);
        int fl=0, dirin = 0, coml = 0;
	    if(strlen(person.HOME)<=strlen(dirhome)){
	        while(dirin<strlen(person.HOME))
            {
	            if(person.HOME[dirin]!=dirhome[dirin]){
	                fl = 1;
	                break;
	            }
                dirin++;
	        }
	        if(fl == 0)
                printf("<%s@%s:~%s> ", person.user, person.host, &dirhome[strlen(person.HOME)]);
	        else
	            printf("<%s@%s:%s> ", person.user, person.host, &dirhome[0]);
	    }
        else
            printf("<%s@%s:%s> ", person.user, person.host, dirhome);

	    fgets(inputstring, BUF_SIZE, stdin);

        //signal(SIGCHLD, sig_handle);

        history[hpoint] = (char*)malloc(strlen(inputstring));
        strcpy(history[hpoint], inputstring);
        hpoint = (hpoint+1)%21;
        prevl = hpoint;
        newl = 0;
        if(!hpoint)
            fhf = 1;
        int hreq = 0;
        //signal(SIGCHLD, sig_handle);

        char* token = strtok(inputstring, ";\n");
        while(token!=NULL){
            allcomm[coml] = (char*)malloc(strlen(token));
            strcpy(allcomm[coml], token);
            //printf("%s\n", allcomm[coml]);
            token = strtok(NULL, ";\n");
            coml++;
        }

        int cmdno = 0;

        while(cmdno < coml){
            
            //printf("%s\n",allcomm[cmdno] );
            char* pipe_tok = (char*) malloc(BUF_SIZE);
            strcpy(pipe_tok, allcomm[cmdno]);
            int pipe_tokind=0;
            char* pipe_token = strtok(pipe_tok, "|");
            char** pipe_tokens = (char**) malloc(1000*sizeof(char*));

            while(pipe_token!=NULL){
                pipe_tokens[pipe_tokind] = (char*) malloc(BUF_SIZE);
                strcpy(pipe_tokens[pipe_tokind], pipe_token);
                pipe_token = strtok(NULL, "|\n");
                pipe_tokind++;
            }

            int pipe_ind = 0;
            int pipe_fd[2];
            int inpfd = 0;
            pid_t pipe_id;
            int me_first = 0;
            while(pipe_ind<pipe_tokind)
            {
                char* temp_fullcom = (char*)malloc(BUF_SIZE);
                char* temp_fullcom2 = (char*)malloc(BUF_SIZE);
                strcpy(temp_fullcom, pipe_tokens[pipe_ind]);
                strcpy(temp_fullcom2, pipe_tokens[pipe_ind]);
                char* temp_bf_tok = strtok(temp_fullcom, delims);
                if(!strcmp(temp_bf_tok,"quit"))
                    exit(0);
                else if(!strcmp(temp_bf_tok, "setenv"))
                {
                    set_env_var(temp_fullcom2);
                }
                else if(!strcmp(temp_bf_tok, "cronjob"))
                {
                    cronjob(temp_fullcom2);
                }
                else if(!strcmp(temp_bf_tok, "unsetenv"))
                {
                    unset_env_var(temp_fullcom2);
                }
                else if(!strcmp(temp_bf_tok, "cd"))
                {
                    temp_bf_tok = strtok(NULL, delims);
                    int retcd;
                    char* prevdir = (char*)malloc(PATH_SIZE);
                    getcwd(prevdir, PATH_SIZE);

                    if(temp_bf_tok == NULL) 
                        temp_bf_tok= "~";
                    if(temp_bf_tok[0]=='~') {
                        char* newdir = (char*)(malloc(BUF_SIZE));
                        strcpy(newdir, person.HOME);
                        strcat(newdir, temp_bf_tok+1);
                        temp_bf_tok = newdir;
                    }

                    retcd = chdir(temp_bf_tok);
                    if(retcd<0) {
                        chdir(prevdir);
                        printf("Wrong folder input\n");
                    }
                }
                else if(temp_fullcom2[strlen(temp_fullcom2)-1] == '&')
                {
                    me_first = 1;
                    backflag = 1;
                    temp_fullcom2[strlen(temp_fullcom2)-1] = '\0';
                    char **excvt = malloc(20 * sizeof(char*));
                    char *sing = strtok(temp_fullcom2, " \n");
                    int inde = 0;
                    for(int f=0;f<20;f++)
                    {
                        excvt[f] = NULL;
                    }
                    while(sing!=NULL){
                        excvt[inde] = (char*)malloc(strlen(sing));
                        strcpy(excvt[inde], sing);
                        sing = strtok(NULL, " \n");
                        inde++;
                    }
                    // if(backflag == 0)
                    // {
                    signal(SIGINT, SIG_IGN);
                    signal(SIGTSTP, SIG_IGN);
                    // }
                    lsh_launch(excvt);
                }

                
                if (pipe(pipe_fd) < 0)
                {
                    exit(2);
                }
                pipe_id = fork();
                if(pipe_id == 0) 
                {
                    dup2(inpfd, STDIN_FILENO);
                    close(inpfd);
                    //printf("%s\n", pipe_tokens[pipe_ind]);
                    if(pipe_tokens[pipe_ind+1] != NULL){
                        dup2(pipe_fd[1], STDOUT_FILENO);
                        close(pipe_fd[0]);
                        close(pipe_fd[1]);
                    }
                    //printf("uygk\n" );
                    int no_files = 1;
                    char *fullcom = (char*)malloc(BUF_SIZE);
                    char *fullcom2 = (char*)malloc(BUF_SIZE);
                    char *fullcom3 = (char*)malloc(BUF_SIZE);
                    char *outfile = (char*)malloc(BUF_SIZE);
                    char *infile = (char*)malloc(BUF_SIZE);
                    char *dircheck1 = (char*)malloc(BUF_SIZE);
                    check_dir(pipe_tokens[pipe_ind]);

                    strcpy(dircheck1, pipe_tokens[pipe_ind]);
                    char* tok_dir = strtok(dircheck1, "><\n");
                    strcpy(fullcom, tok_dir);
                    strcpy(fullcom2, tok_dir);
                    strcpy(fullcom3, tok_dir);
                    //printf("%s\n", pipe_tokens[pipe_ind]);
                    while(tok_dir!=NULL)
                    {
                        tok_dir = strtok(NULL, " \t><");
                        if (tok_dir!=NULL){
                            if(wcs == 1 && no_files == 2)
                                strcpy(outfile,tok_dir);
                            if(wcs == 2 && no_files == 2)
                                strcpy(infile,tok_dir);
                            if(wcf == 1 && no_files == 1)
                            {
                                strcpy(outfile,tok_dir);
                                no_files++;
                            }
                            if(wcf == 2 && no_files == 1)
                            {
                                strcpy(infile,tok_dir);
                                no_files++;
                            }

                        }
                    }
                    if(outdirect == 1){
                        out_fd = open(outfile , O_WRONLY | O_TRUNC | O_CREAT, 0644 );
                    }
                    if(doutdirect == 1) {
                        out_fd = open(outfile , O_WRONLY | O_APPEND | O_CREAT, 0644 );   
                    }
                    if(indirect == 1) {
                        in_fd = open(infile , O_RDONLY );   
                    }

                    
                    if(indirect == 1){
                        dup2(in_fd, STDIN_FILENO);
                        //close(in_fd);
                    }
                    if(outdirect == 1 || doutdirect == 1){
                        dup2(out_fd, STDOUT_FILENO);
                        close(out_fd);
                    }
                    //printf("%s\n", fullcom);
                    char* token2 = strtok(fullcom3, delims);
                    //printf("%s\n", fullcom);
                    if (!strcmp(token2, "quit"))
                    {
                        printf("Shell exited successfully\n");
                        exit(0);
                    }
                    if(!strcmp(token2, "pwd"))
                    {
                    	char* dirhome = (char*)malloc(PATH_SIZE);
        			    getcwd(dirhome, PATH_SIZE);
        			    printf("%s\n", dirhome);
                    }
                    else if(!strcmp(token2, "echo"))
                    {
        			    token2 = strtok(NULL, delims);
        			    while(token2!=NULL){

        			        if(token2[0]=='$')
                                printf("%s ", getenv(token2+1)); 
        			        else 
        			             printf("%s ", token2); 
        			        token2 = strtok(NULL, " \n\t|");
        			    }
        			    printf("\n");
        			}
                    else if(!strcmp(token2, "ls") && me_first == 0)
                    {
                        char ** flags = (char**)malloc(sizeof(char*)*100);
                        token2 = strtok(NULL, delims);
                        int k = 0;
                        for(int j=0; j<100; j++)
                        {
                            flags[j] = NULL;
                            if(token2!=NULL){
                                flags[k] = (char*)malloc(strlen(token2));
                                strcpy(flags[k], token2);
                                token2 = strtok(NULL, delims);
                                k++;
                            }
                        }
                        ls(flags,person);
                        //signal(SIGCHLD, sig_handle);
                    }
                    else if(!strcmp(token2, "pinfo"))
                    {
                        pinfo(fullcom,person);
                    }
                    else if(!strcmp(token2, "kjob"))
                    {
                        kjob(fullcom);
                    }
                    else if(!strcmp(token2, "bg"))
                    {
                        bg(fullcom);
                    }
                    else if(!strcmp(token2, "fg"))
                    {
                        fg(fullcom);
                    }
                    else if(!strcmp(token2, "jobs"))
                    {
                        jobs();
                    }
                    else if(!strcmp(token2, "overkill"))
                    {
                        overkill();
                    }
                    else if(!strcmp(token2, "history"))
                    {
                        int num = 20;
                        token2 = strtok(NULL, " ");
                        if(token2 != NULL)
                        {
                            num = atoi(token2);
                        }
                        if(num>=hpoint)
                        {
                            num = num - hpoint;
                            prevl = 21 - num;
                        }
                        else
                        {
                            prevl = 21;
                            newl = hpoint - num - 1;
                        }
                        
                        if(fhf)
                        {
                            for (int h = prevl; h < 21; h++)
                            {
                                printf("%s", history[h]);
                            }
                        }
                        for(int g = newl;g<hpoint-1;g++)
                        {
                            printf("%s", history[g]);
                        }
                    }
                    else if(strcmp(token2, "setenv") && strcmp(token2, "unsetenv") && me_first==0 && strcmp(token2, "cd"))
                    {
                        //printf("%s\n", fullcom2);
                        char **excvt = malloc(20 * sizeof(char*));
                        char *sing = strtok(fullcom2, " \n");
                        int inde = 0;
                        for(int f=0;f<20;f++)
                        {
                            excvt[f] = NULL;
                        }  
                        while(sing!=NULL){
                            excvt[inde] = (char*)malloc(strlen(sing));
                            strcpy(excvt[inde], sing);
                            sing = strtok(NULL, " \n");
                            inde++;
                        }
                        lsh_launch(excvt);
                        // if(excvt[inde - 1][strlen(excvt[inde -1]) - 1] == '&')
                        // {
                        //     backflag = 1;
                        //     excvt[inde - 1] = NULL;
                        // }
                        //printf("%s\n", excvt[0]);
                        //printf("%s\n", excvt[1]);
                        // excvt[inde-1][strlen(excvt[inde -1])] = '\0';
                        //if(backflag == 0)
                        //{
                        signal(SIGINT, signalHandler);
                        signal(SIGTSTP, signalHandlerctrl_z);
                         //}
                        
                        
                        //printf("no = %d\n", no_child_processes);
                    }

                    outdirect = 0;
                    doutdirect = 0;
                    indirect = 0;
                    wcf = 0;
                    wcs = 0;

                    //signal(SIGCHLD, sig_handle);
                    
                    // printf("inloop pid = %d\n", child_procs[no_child_processes-1].id);
                    exit(0);
                }
                else
                {
                    //printf("pipe = %d\n", pipe_id);
                    fgchild = pipe_id;
                    //signal(SIGINT, signalHandler);
                    wait(NULL);
                    fgchild = 0;
                    close(pipe_fd[1]);
                    inpfd = pipe_fd[0];
                }
                pipe_ind++;
                me_first = 0;
            }
            cmdno++;
            backflag = 0;
        }	    

	}
	return 0;
}