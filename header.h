#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <ctype.h>

#define BUILTIN		1
#define EXTERNAL	2
#define NO_COMMAND  3
#define SUCCESS 1
#define FAILURE 0

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"


//linked list to store jobs
typedef struct jobs
{
    int count, pid;
    char cmd[50];
    struct jobs *link;
}jobs_t;

extern char *builtins[];
extern char *external_commands[154];
extern char prompt[30];
extern char input_str[50];
extern int main_pid;
extern jobs_t *head;
extern int count;
extern int status;

void scan_input(char *prompt, char *input_string);
char *get_command(char *input_string);

void copy_change(char *prompt, char *input_string);

int check_command_type(char *command);
void echo(char *input_string, int status);
void execute_internal_commands(char *input_string);
void signal_handler(int sig_num);
void extract_external_commands(char **external_commands);
void execute_external_commands(char *input_string);

int print_job_list(jobs_t *head);
void n_pipe_exec(char **argv, int );
int check_spaces(char *);
int insert_last(jobs_t **head, char *);
int delete_last(jobs_t **head);
#endif
