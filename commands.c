#include"header.h"

char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
						"set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
						"exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help","jobs","fg", "bg", NULL};

int main_pid = 0;
int status;
/*Function to store the external commands from file into 2d array*/
void extract_external_commands(char **external_commands){
    //open the file in read mode
    int fd = open("ext_cmd.txt",O_RDONLY);
    char buff[35], ch;    
    int i=0, flag=0;
    while(read(fd, &ch, 1)>0){   //now run a loop to store commands one by one
        if(ch == '\n'){
            buff[i]='\0';        //if \n found then terminate that string
            external_commands[flag]=malloc(i + 1);  //allocate memory for string according to size(i)

            strcpy(external_commands[flag], buff);  //store command in 2d array
            flag++;
            i=0; //reset i to store next command
        }
        else{
            buff[i++]=ch;  //if not \n, then keep on storing char in string buff
        }
    }
    //store last command if file doesn't end with newline
    if(i > 0)
    {
        buff[i] = '\0';
        external_commands[flag] = malloc(i + 1);
        strcpy(external_commands[flag], buff);
        flag++;
    }
    //lastly store NULL in 2d array and close the file
    external_commands[flag] = NULL;
    close(fd);
}


/*Function to get the command from input string*/
char *get_command(char *input_string){
    static char cmd[35];
    strcpy(cmd, input_string); //take backup of original string

    int i=0;  //run a loop to get command
    while(cmd[i]){
        if(cmd[i] == ' '){ //check until space only
            cmd[i] = '\0';
            break;
        }
        i++;
    }
    return cmd;  //return the command i.e upto space
}


/*Function to check the command type*/
int check_command_type(char *command){
    //first check for builtin commands 2d array
    int i=0;
    while(builtins[i] != NULL){
        if(strcmp(builtins[i], command)==0) //if cmd found in builtin array return that macro
            return BUILTIN;
        i++;
    }

    //if not builtin, check for external command 2d array
    i=0;
    while(external_commands[i] != NULL){
        if(strcmp(external_commands[i], command)==0)
            return EXTERNAL;            //if found in ext command array
        i++;
    }

    //if not found in both
    return NO_COMMAND;
}

/*Function to execute internal/builtin commands*/
void execute_internal_commands(char *input_string){
    //if its a exit
    if(strcmp(input_string, "exit")==0)    
        exit(0);
    //if pwd command
    else if(strcmp(input_string, "pwd")==0){   
        char buff[50];
        printf(CYAN"%s\n" RESET, getcwd(buff, 50));  //use getcwd() librabry call to print the pwd
    }
    //if cd command
    else if(strncmp(input_string, "cd", 2)==0){
        int ret = chdir(input_string+3);   //change directory using chdir(send only dir name)
        if(ret == 0){
            char buff[50];
            printf(CYAN"%s\n" RESET, getcwd(buff, 50));  //print it
        }
        else if(ret == -1){
            perror("cd");  //if directory not found
        }
    }
     //if echo $$ command
    else if(strcmp(input_string, "echo $$")==0){
        printf("%d\n",getpid());    //print the current process pid i.e mini-shell pid
        status = 0;
    }
    else if(strcmp(input_string, "echo $?") == 0){
        if(WIFEXITED(status))
            printf("%d\n",WEXITSTATUS(status)); //print the exit status of previous cmd
        
        else if(status == 127)
            printf("%d\n", status);

        else if(WIFSIGNALED(status))
            printf("%d\n", 127 + WTERMSIG(status));

        status = 0;
    }
    //if it's a echo $SHELL command
    else if(strcmp(input_string, "echo $SHELL")==0){
        printf("%s\n", getenv(input_string+6));
        status = 0;
    }
    //if it's a jobs command, print the stopped jobs
    else if(strcmp(input_str, "jobs") == 0){
        //print the linked_list
        print_job_list(head);
    }
    //for fg, continue the stopped jobs
    else if(strcmp(input_str, "fg")==0){   
        if(head==NULL){ //if there are no stopped jobs
            printf("fg: current: no such job\n");
        }
        else{
            jobs_t *temp = head;
            while(temp->link)    //first traverse to the last job
                temp = temp->link;

            kill(temp->pid, SIGCONT); //then continue that job
            printf("%s\n",temp->cmd);
            waitpid(temp->pid, NULL, WUNTRACED); //wait for it to complete in fg

            delete_last(&head);   //delete job that's executed
        }
    }
    //for bg, continue the stopped jobs in background
    else if(strcmp(input_str, "bg")==0){
        if(head==NULL){ //if there are no stopped jobs
            printf("bg: current: no such job\n");
        }
        else{
            
            jobs_t *temp = head;
            while(temp->link)    //first traverse to the last job
                temp = temp->link;

            kill(temp->pid, SIGCONT); //then continue that stopped job
            printf("[%d]  %s &\n",temp->count,temp->cmd);

            delete_last(&head);   //delete job that's executed
        }
    }
}

/*Function to execute external commands*/
void execute_external_commands(char *input_string){
    //first convert this ip string into 2d array for execvp()

    int row=0, i=0;
    while(input_string[i]){
        if(input_string[i]==' ') //finding row value accroding to space and '\0'
            row++;
        i++;
    }
    row++;
    char *argv[row+1];  //declare array of pointers to store external cmds for execvp()

    i=0;
    char temp_input[100];
    strcpy(temp_input, input_string);
    char *token = strtok(temp_input, " ");  //using strtok()
    while(token != NULL){
        argv[i++] = token;
        token = strtok(NULL, " ");
    }
    argv[i] = NULL;

    //now create a child process that will execute the external commands
    
    main_pid = fork(); //create a child to exec the commands

    if(main_pid > 0){
        //PARENT
        waitpid(main_pid, &status, WUNTRACED); //wait for child to complete
        main_pid = 0;
    }
    else if(main_pid == 0){
        //CHILD
        //register the signal to give its properties back
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        //check if pipes are present or not
        int j=0, ispipe=0;
        while(argv[j]){
            if(strcmp(argv[j], "|")==0){
                ispipe=1;
                break;
            }
            j++;
        }
        if(ispipe == 0){   //if pipe is not present
            execvp(argv[0], argv);
        }
        else{        //if pipe is present, perform n-pipe operation
            n_pipe_exec(argv, i);
        }
    }
}

/*Function to execute n pipes commands*/
void n_pipe_exec( char **argv, int row){
    int *command = malloc(row * sizeof(int)); //array to store command indices
    int ind = 1, pipe_count=0;
    command[0] = 0; //store first command index

    for(int i=0; i<row; i++){  //run a loop to check for pipes
        if(strcmp(argv[i],"|")==0){
            pipe_count++;
            command[ind++] = i+1;  //pipe found, store next index after pipe into command array
            argv[i] = NULL;    //also make it as NULL
        }
    }
    command = realloc(command, (pipe_count+1)*sizeof(int)); //realloc memeory according to no. of pipes

    int fd[2];
    //now run a loop for ind i.e command no. of times
    for(int i=0; i<ind; i++){

        if(i != ind-1)
            pipe(fd); //create pipe except for last child

        int pid = fork(); //create child

        if(pid > 0){
            //PARENT
            if(i != ind-1){
                dup2(fd[0], 0);
                close(fd[0]);
                close(fd[1]);
            }
            wait(NULL);
        }
        else if(pid == 0){
            //CHILD
            if(i != ind-1){
                close(fd[0]);
                dup2(fd[1], 1);
                close(fd[1]);
            }
            execvp(argv[command[i]], argv + command[i]);
            perror("execvp");
        }
        else{
            perror("fork");
        }
    }
    free(command);
    exit(0);
}