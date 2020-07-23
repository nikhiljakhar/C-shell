# C-Shell

## To run the shell
```bash
make
./shell.out
```
Or just execute the uploaded executable file
```bash
./shell.out
```

## Features
1. Redirection
2. Piping
3. Background and foreground process management.
1. Bash commands working by passing the command to the underlying bash environment.

## Commands with Custom Implementations
1. **setenv var**: creates/changes the enviornment variable
2. **unsetenv ​var**: destroy the enviornment variable var
3. **jobs**: lists all the running jobs and their PIDs
4. **kjob**: kill a job
5. **fg**: bring a job to foreground
6. **bg**: send a job to background
7. **overkill**: kills all background process at once
8. **cd**: change directory
9. **pwd**: prints the directory path
10. **echo**: output whatever was the input
11. **ls**: list all the files and directories inside the current directory
12. ​**pinfo**: prints the process related info of shell program
13. **history <num>**: displays only latest <num> commands