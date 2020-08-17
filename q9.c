#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
	int fd[2];
	int fd2[2];
	char str[80];
	pipe(fd);
	pipe(fd2);
	strcpy(str,argv[1]);
	int rc = fork();
	if (rc < 0) { // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    } 
    else if (rc>0) {
    	printf("original string:%s\n",str);
    	char str2[80];
    	write(fd[1],str,sizeof(str));
    	read(fd2[0],str2,80);
    	printf("string read by parent: %s \n", str2);
    }
    else if (rc == 0) { // child (new process)
    	char str3[80];
    	read(fd[0],str3,80);
    	strcat(str3,"fixed_str");
    	write(fd2[1],str3,80);
    	printf("string put into pipe 2 by child: %s\n", str3);
    	close(fd[0]);
    	close(fd[1]);
    	close(fd2[0]);
    	close(fd2[1]);
  }

	return 0;
}