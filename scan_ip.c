#include"header.h"

jobs_t *head = NULL;
int count = 0;

/*Signal handler*/
void signal_handler(int signum){
    
    if(signum == SIGINT){    
        if(main_pid == 0){
            printf(GREEN"\n%s" RESET,prompt);
            fflush(stdout);
        }
    }
    if(signum == SIGTSTP){
        status = 127+SIGTSTP;
        if(main_pid == 0){
            printf(GREEN"\n%s" RESET,prompt);
            fflush(stdout);
        }
        else{
            insert_last(&head, input_str);
            
        }
    }
    if(signum == SIGCHLD){  //for bg command
        //clear the resources after child completes to avoid becoming it zombie
        waitpid(-1, &status, WNOHANG);
    }
}


/*Function definition to read the input*/
void scan_input(char *prompt, char *input_str){
    //register the signals
    signal(SIGINT, signal_handler);
    signal(SIGTSTP, signal_handler);
    signal(SIGCHLD, signal_handler); //register the SIGCHLD signal, so when childs status changes this signal is sent to parent
    while (1)
    {   
        printf(GREEN "%s" RESET,prompt);//print the default prompt
        if(scanf("%[^\n]", input_str) == 0){
            getchar();   // remove newline
            continue;    // skip loop
        }
        getchar();
        //first check if its a env var PS1
        if(strncmp(input_str, "PS1=", 4) == 0){
            if(check_spaces(input_str)==1) //call function to check for spaces
                strcpy(prompt, input_str+4); //change the prompt
            else
                printf(RED"PS1: command not found\n"RESET);
        }
        //if its a other commands
        else{
            char *command = get_command(input_str); //first get the command from input string upto ' ' only
            //check the type of command
            int type = check_command_type(command);
            if(type == BUILTIN){    //if it's a builtin command
                execute_internal_commands(input_str);
            }
            else if(type == EXTERNAL){  //if it's a external command
                execute_external_commands(input_str);
            }
            else{
                status = 127;
                printf("%s: command not found\n",command); //if command is invalid or not found
            }
        }
    }   
}

/*Function to check for spaces and removing trailing spaces*/
int check_spaces(char *str){
    int len = strlen(str);
    while(len>4){           //logic to remove trailing spaces
        if(isspace(str[len-1])){
            str[len-1] = '\0';
            len--;
        }
        else
            break;
    }
    int i=0; //now check is there any spaces in result string
    while((str)[i] !='\0'){
        if(isspace(str[i++]))
            return 0; //space found
    }

    return 1;  //valid prompt
}
