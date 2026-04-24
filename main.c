/*  
 * Name : Prasad Kumbhar
 * Project Name : Mini-Shell
 * Date : 31-03-2026

 * Description:
    This project implements a simple Linux-like shell where users can enter
    commands through a prompt and execute them.

    The shell identifies whether the command is built-in (cd, pwd, echo, jobs,
    fg, bg) or external (like ls, date). Built-in commands are handled directly,
    while external commands are executed using fork() and execvp().

    It supports job control using signals:
      - Ctrl+C (SIGINT) to interrupt processes
      - Ctrl+Z (SIGTSTP) to stop processes and store them as jobs

    Stopped jobs are maintained using a linked list and can be resumed using
    fg (foreground) or bg (background).

    The shell also supports multiple pipes and dynamic prompt change using PS1.

 * Concepts Used:
    * System Calls
    * Signal Handling
    * Linked List (For job control)
    * File & String Handling
*/

#include "header.h"

char prompt[30] = "minishell:~$"; //default prompt
//to store input/commands from user
char input_str[50];
char *external_commands[154];//array of strings to store ext_commands

int main(){
    //first clear the terminal
    system("clear");
    
    //first store ext_commands in 2d array
    extract_external_commands(external_commands);

    scan_input(prompt, input_str); //function call to get input

    return 0;
}