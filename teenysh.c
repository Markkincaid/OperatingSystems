#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>

#define BUFSZ 100
#define MAXWDS 10
char line[BUFSZ]; // buffer for line
char *words_on_line[MAXWDS]; // MAXWDS words on a line
char whitespace[] = " \t\r\n\v";
//signal handler function
void signal_handler(int signo) {
    if (signo == SIGINT)
        fprintf(stdout, "\n");
}
//creating a command struct
typedef struct {
    int index;
    char cmd; // | < > ; &
} cmd_type;
//funciton for finding what type of command is entered.
cmd_type what_cmds(char *argv[]) {
    int i = 0;
    cmd_type cmd = {0, 0};
    while(argv[i] != 0) {
        char x = *argv[i];
        if (x == '|' || x == '<' || x == '>' || x == ';' || x == '&') {
            argv[i] = 0;
            if (argv[i+1] != 0)
                cmd.index = i+1;
            else if (x != '&')
                x = 0;
            cmd.cmd = x;
            return cmd;
        }
        i++;
    }
    return cmd;
}
//function to handle if a command isnt found.
void cmd_not_found(char *cmd) {
    fprintf(stderr, "teenysh: cmd not found: %s\n", cmd);
    exit(1);
}
//function to wait if fork is called.
void fork_run_wait(char *cmd, char **args) {
    if (fork() == 0) {
        execvp(cmd, args);
        cmd_not_found(cmd);
    }
    wait(NULL);
}
//main function
int main(int argc, char **argv) {
    int shn = 0;
    signal(SIGINT, signal_handler);
	//while true 
    while (1) {
        memset(line, 0, BUFSZ);
        for (int i = 0; i < MAXWDS; i++) words_on_line[i] = 0;
        fprintf(stdout, "teenysh%d $ ", shn++);
        fflush(stdout);
        if (fgets(line, BUFSZ, stdin) == 0) return 0; // ctl-d
        line[strcspn(line, "\n")] = '\0';
        if(line[0] == 'c' && line[1] == 'd' && line[2] == ' '){ // Clumsy 
            if(chdir(line+3) < 0) // Chdir has no effect on the parent if run in the child.
                fprintf(stderr, "cannot cd %s\n", line+3);
            continue;
        }
        char *s = line;
        char *end_buf = line + strlen(line);
        int eol = 0, i = 0;
		//while true 
        while (1) {
            while (s < end_buf && strchr(whitespace, *s)) s++;
            if (*s == 0) break; // eol - done
            words_on_line[i++] = s;
            while (s < end_buf && !strchr(whitespace, *s)) s++;
            *s = 0;
        }
        if (words_on_line[0] == 0) continue;
        if (strcmp(words_on_line[0],"exit") == 0) break;
        cmd_type c = what_cmds(words_on_line);
        int secondcmd = c.index;
		// if the command is a pipe
        if (c.cmd == '|') {
            int p[2];
            pipe(p);
            if(fork() == 0){
              close(1); dup(p[1]); close(p[0]); close(p[1]);
              execvp(words_on_line[0], words_on_line);
              cmd_not_found(words_on_line[0]);
            }
            if(fork() == 0) {
              close(0); dup(p[0]); close(p[0]); close(p[1]);
              execvp(words_on_line[secondcmd], &words_on_line[secondcmd]);
              cmd_not_found(words_on_line[0]);
            }
            close(p[0]); close(p[1]);
            wait(NULL); wait(NULL);
        } else if (c.cmd == ';') {
            fork_run_wait(words_on_line[0], &words_on_line[0]);
            fork_run_wait(words_on_line[secondcmd], &words_on_line[secondcmd]);
		//if the command is greater than or less than
        } else if (c.cmd == '>' || c.cmd == '<') {
            int mode;
            if (fork() == 0) {
                if (c.cmd == '>') {
                  close(1); // close stdout
                  mode = O_WRONLY|O_CREAT|O_TRUNC;
                } 
                else {
                  close(0); // close stdinput
                  mode = O_RDONLY;
                }
                if (open(words_on_line[secondcmd], mode, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0) {
                    fprintf(stderr, "open %s failed\n", words_on_line[secondcmd]);
                    exit(-1);
                }
                execvp(words_on_line[0], words_on_line);
                cmd_not_found(words_on_line[0]);
            }
            wait(NULL);
			//if the command is and
        } else if (c.cmd == '&') { // this is not quite right
            if (fork() == 0)
                execvp(words_on_line[0], words_on_line);
        } else {
            fork_run_wait(words_on_line[0], &words_on_line[0]);
        }
    }
}
