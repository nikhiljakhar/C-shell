#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<dirent.h>
#include<sys/stat.h>
#include<sys/utsname.h>
#include<grp.h>
#include<pwd.h>
#include<sys/types.h>
#include<time.h>
#include"headers/definations.h"

int lflag = 0, aflag = 0, dirflag = 0;

void displayinfo(char *filofol){
    struct stat stats;
    stat(filofol, &stats);

    int gid = stats.st_gid;
    int uid =  stats.st_uid;
    
    if(S_ISREG(stats.st_mode)) printf("-");
    if(S_ISDIR(stats.st_mode)) printf("d");
    if(S_ISLNK(stats.st_mode)) printf("l");
    if(S_ISBLK(stats.st_mode)) printf("b");
    if(S_ISCHR(stats.st_mode)) printf("c");
    if(S_ISFIFO(stats.st_mode)) printf("|");

    printf( (stats.st_mode & S_IRUSR) ? "r" : "-");
    printf( (stats.st_mode & S_IWUSR) ? "w" : "-");
    printf( (stats.st_mode & S_IXUSR) ? "x" : "-");
    printf( (stats.st_mode & S_IRGRP) ? "r" : "-");
    printf( (stats.st_mode & S_IWGRP) ? "w" : "-");
    printf( (stats.st_mode & S_IXGRP) ? "x" : "-");
    printf( (stats.st_mode & S_IROTH) ? "r" : "-");
    printf( (stats.st_mode & S_IWOTH) ? "w" : "-");
    printf( (stats.st_mode & S_IXOTH) ? "x" : "-");
    printf(" %ld", stats.st_nlink);

    char* date = malloc(256);
    strftime(date, 256, "%b %d %H:%M", localtime(&stats.st_mtime));

    struct passwd *usr = getpwuid(uid);
    struct group *grp = getgrgid(gid); 
    printf(" %s\t%s\t%ld\t%s\t", usr->pw_name, grp->gr_name, stats.st_size, date);
}

void ls(char* flags[], info d)
{
	int k =0;
	char ** filofols = (char**)malloc(sizeof(char*)*100);
	while(flags[k]!=NULL)
	{
		//printf("%s\n", flags[k]);
		if(!strcmp(flags[k], "-la") || !strcmp(flags[k], "-al"))
		{
			lflag = 1;
			aflag = 1;
		}
		if(!strcmp(flags[k], "-a"))
			aflag = 1;
		if(!strcmp(flags[k], "-l"))
			lflag = 1;
		if(flags[k][0] != '-' && flags[k][0] != '&')
		{
			filofols[dirflag] = (char*)malloc(strlen(flags[k]));
            strcpy(filofols[dirflag], flags[k]);
            dirflag++;
		}
		if(flags[k][0] == '&')
		{
			backflag = 1;
			//printf("bf = 1\n");
			filofols[dirflag] = (char*)malloc(sizeof(char)*10);
			strcpy(filofols[dirflag], ".");
			dirflag++;
		}
		k++;
	}
	if(dirflag == 0)
	{
		filofols[dirflag] = (char*)malloc(sizeof(char)*10);
		strcpy(filofols[dirflag], ".");
		dirflag++;
	}
	for(int i=0; i<dirflag; i++)
	{
		if(dirflag > 1)
			printf("%s :\n",filofols[i] );
		if(filofols[i][0]=='~') {
	        char* cur = (char*)(malloc(PATH_SIZE));
	        strcpy(cur, d.HOME);
	        strcat(cur, filofols[i]+1);
	        filofols[i] = cur;
	    }
	    DIR* dir;
        struct dirent* dir_buf;
        dir = opendir(filofols[i]);
        if(dir==NULL) {
            perror("ls");
            lflag = 0;
			aflag = 0;
			dirflag = 0;
            return;
        }
	    if(lflag == 1 && aflag == 1)
	    {
	        char* distr = (char*)malloc(PATH_SIZE);

	        while((dir_buf = readdir(dir)) != NULL){
	            sprintf(distr, "%s/%s", filofols[i], dir_buf->d_name);
	            displayinfo(distr);
	            printf("%s\n", dir_buf->d_name);
	        }
	        closedir(dir);
	    }
	    else if(lflag == 0 && aflag == 1)
	    {
	        while((dir_buf = readdir(dir)) != NULL){
	            printf("%s\n", dir_buf->d_name);
	        }
	        closedir(dir);
	    }
	    else if(lflag == 1 && aflag == 0)
	    {
	        char* distr = (char*)malloc(PATH_SIZE);
	        while((dir_buf = readdir(dir)) != NULL){
		        if(dir_buf->d_name[0]!='.')
		        {            
		            sprintf(distr, "%s/%s", filofols[i], dir_buf->d_name);
		            displayinfo(distr);
		            printf("%s\n", dir_buf->d_name);
	        	}
	        }
	        closedir(dir);
	    }
	    else if(lflag == 0 && aflag == 0)
	    {
	        while((dir_buf = readdir(dir)) != NULL){
	            if(dir_buf->d_name[0]!='.')            
	            	printf("%s\n", dir_buf->d_name);
	        }
	        closedir(dir);
	    }
	}
	lflag = 0;
	aflag = 0;
	dirflag = 0;
	return;
}