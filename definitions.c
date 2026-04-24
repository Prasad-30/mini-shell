#include"header.h"


/*Function to insert the stopped jobs at last in linked list*/
int insert_last(jobs_t **head, char *input_str){

    count++;

    jobs_t *newnode = malloc(sizeof(jobs_t));
    if(!newnode){
        printf("Failed to create newnode!\n");
        return FAILURE;
    }
    //handle the data assigning part
    newnode->count = count;
    newnode->pid = main_pid;
    strcpy(newnode->cmd, input_str);
    newnode->link = NULL;

    if((*head) == NULL){  //if list is empty
        (*head) = newnode;
        printf("[%d]+     %d      Stopped    %s\n",(*head)->count, (*head)->pid, (*head)->cmd);
        return SUCCESS;
    }

    //if list is not empty
    jobs_t *temp = (*head);
    while(temp->link){
        temp = temp->link;
    }
    temp->link = newnode;
    printf("[%d]+     %d      Stopped    %s\n",newnode->count, newnode->pid, newnode->cmd);
    return SUCCESS;
}

/*Function to print the jobs list*/
int print_job_list(jobs_t *temp){
    if(temp == NULL){
        return FAILURE;
    }   
    while(temp){
        printf("[%d]    Stopped    %s\n", temp->count, temp->cmd);
        temp = temp->link;
    }
    return SUCCESS;
}

/*Function to delete the last stopped job in Linked list*/
int delete_last(jobs_t **head){
    //if list is empty
    if((*head) == NULL){
        printf("No jobs found!\n");
        return FAILURE;
    }
    if((*head)->link == NULL){ //if only one node
        free(*head);
        (*head) = NULL;
        return SUCCESS;
    }
    jobs_t *temp = *head, *prev=NULL;
    while(temp->link){
        prev = temp;
        temp = temp->link;
    }
    free(temp);
    prev->link = NULL;
    temp = NULL;
    return SUCCESS;
}