a.out: *.c
	gcc -o shell *.c
shell: definations.c main.c cmdls.c pinfo.c morebuiltins.c cronjob.c
	gcc -o shell definations.c main.c cmdls.c morebuiltins.c cronjob.c

clean: 
	rm -rf .*