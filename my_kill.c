#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	 if (argc != 2) {       
	 	printf("Error - command format is $ my_kill pid\n");       
	 	exit(-1); 
	 }
	int i = atoi(argv[1]);
	if(i == 0){
		printf("Error - invalid pid\n");
		exit(-2);
	}
	int status = kill(i, SIGINT);  
	int errnum = errno;  
	if (status == -1) {      
		fprintf(stderr, "Value of errno: %d\n", errno);      
		perror("Error printed by perror");      
		fprintf(stderr, "Error killing process: %s\n", strerror( errnum));  
	}
	else{
		printf("process %d was killed\n", i);
	}
}