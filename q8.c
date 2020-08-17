#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{	
	if(argc != 2){
		printf("usage: q8 <string>\n");
	}
	char str[80];
	int fd[2];
	pipe(fd);
	int rc = fork();
	int rc2 = fork();
	if(rc >0 && rc2>0){
		printf("Parent of two children connected via pipe. (pid: %d )\n", getpid());
		printf("parentpid: %d of childpid: %d and child2pid: %d \n",getpid(),rc,rc2);
	}
	else if(rc == 0 && rc2> 0){
		strcpy(str, argv[1]);
		strcat(str,"kincaid");
		write(fd[1],str,sizeof(str));
		close(fd[1]);
		printf("child: %d finished \n",getpid());
		//first child
	}
	else if(rc>0 && rc2 == 0){
			char str2[80];
			read(fd[0],str2,100);
			close(fd[0]);
			char backwards[80];
			char str3[80];
			strcpy(str3,argv[1]);
	  		int i, j, len;
	  		j = 0;
	  		len = strlen(str3);

	  		for (i = len - 1; i >= 0; i--){
	  			backwards[j++] = str3[i];
	  		}
	  		backwards[i] = '\0';
	  		strcat(str2,backwards);
	  		str2[15]= '\0';
	  		printf("%s \n",str2);
	  		printf("child: %d finished \n",getpid());

  	}
	return 0;
}