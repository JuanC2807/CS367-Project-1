/* This is the only file you will be editing.
 * - wlf_sched.c (Wlf Scheduler Library Code)
 * - Copyright of Starter Code: Prof. Kevin Andrea, George Mason University. All Rights Reserved
 * - Copyright of Student Code: You!  
 * - Copyright of ASCII Art: Modified from an uncredited author's work:
 * -- https://www.asciiart.eu/animals/wolves
 * - Restrictions on Student Code: Do not post your code on any public site (eg. Github).
 * -- Feel free to post your code on a PRIVATE Github and give interviewers access to it.
 * -- You are liable for the protection of your code from others.
 * - Date: Jan 2025
 */

/* CS367 Project 1, Spring Semester, 2025
 * Fill in your Name, GNumber, and Section Number in the following comment fields
 * Name: Juan Carlos Garcia Solis
 * GNumber:  G01391273
 * Section Number: CS367-005             (Replace the _ with your section number)
 */

/* wlf CPU Scheduling Library
                     .
                    / V\
                  / `  /
                 <<   |
                 /    |
               /      |
             /        |
           /    \  \ /
          (      ) | |
  ________|   _/_  | |
<__________\______)\__)
*/
 
/* Standard Library Includes */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
/* Unix System Includes */
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <pthread.h>
#include <sched.h>
/* DO NOT CHANGE THE FOLLOWING INCLUDES - Local Includes 
 * If you change these, it will not build on Zeus with the Makefile
 * If you change these, it will not run in the grader
 */
#include "wlf_sched.h"
#include "strawhat_scheduler.h"
#include "strawhat_support.h"
#include "strawhat_process.h"
/* DO NOT CHANGE ABOVE INCLUDES - Local Includes */

/* Feel free to create any definitions or constants you like! */

/* Feel free to create any helper functions you like! */
void free_queue(Wlf_queue_s *queue);

void free_queue(Wlf_queue_s *queue){
    if (queue == NULL) return;
    Wlf_process_s *temp = queue->head;
    Wlf_process_s *prev = NULL;

    while (temp!= NULL){
        prev = temp;
        temp = temp->next;

        free(prev->cmd);
        prev->cmd = NULL;
        free(prev);
    }
    queue->head = queue->tail = NULL;
    free(queue);
}

/*** Wlf Library API Functions to Complete ***/

/* Initializes the Wlf_schedule_s Struct and all of the Wlf_queue_s Structs
 * Follow the project documentation for this function.
 * Returns a pointer to the new Wlf_schedule_s or NULL on any error.
 * - Hint: What does malloc return on an error?
 */
Wlf_schedule_s *wlf_initialize() {

    //Allocates memory for high queue
    Wlf_queue_s * high = malloc(sizeof(Wlf_queue_s));
    if (high == NULL){
        return NULL;
    }
    //initialize head, tail to NULL and set count to 0
    high->head = NULL;
    high->tail =NULL;
    high->count = 0;

    //Allocates memory for normal queue
    Wlf_queue_s * normal = malloc(sizeof(Wlf_queue_s));
    if (normal == NULL){
        return NULL;
    }
    //initialize head, tail to NULL and set count to 0
    normal->head = NULL;
    normal->tail =NULL;
    normal->count = 0;
    //Allocates memory for terminated queue
    Wlf_queue_s * terminated = malloc(sizeof(Wlf_queue_s));
    if (terminated == NULL){
        return NULL;
    }
    //initialize head, tail to NULL and set count to 0
    terminated->head = NULL;
    terminated->tail = NULL;
    terminated->count = 0;
    //Allocates memory for overall schedule
    Wlf_schedule_s * da_schedule = malloc(sizeof(Wlf_schedule_s));
    if (da_schedule == NULL){
        return NULL;
    }

    //Initialize all queues in overall queue
    da_schedule->ready_queue_high = high;
    da_schedule->ready_queue_normal = normal;
    da_schedule->terminated_queue = terminated;

  return da_schedule;// return overall schedule
}

/* Allocate and Initialize a new Wlf_process_s with the given information.
 * - Malloc and copy the command string, don't just assign it!
 * Follow the project documentation for this function.
 * - You may assume all arguments within data are Legal and Correct for this Function Only
 * Returns a pointer to the Wlf_process_s on success or a NULL on any error.
 */
Wlf_process_s *wlf_create(Wlf_create_data_s *data) {
    Wlf_process_s * process_node = malloc(sizeof(Wlf_process_s));
    if (process_node == NULL){
        return NULL;
    }
    process_node->cmd = malloc(sizeof(char) * strlen(data->original_cmd)+1);
    if (process_node->cmd == NULL){
        return NULL;
    }
    process_node->pid = data->pid;
    strncpy(process_node->cmd,data->original_cmd, strlen(data->original_cmd)+1);
    //
    process_node->state = 0;
    //
    process_node->state |= 0x8000;
    if (data->is_critical == 1){
        process_node->state |= 0x1800;
    }
    if (data->is_high == 1){
        process_node->state |= 0x0800;
    }
    process_node->age = 0;
    process_node->next = NULL;

  return process_node; /* Replace This Line with your Code */
}

/* Inserts a process into the appropriate Ready Queue (singly linked list).
 * Follow the project documentation for this function.
 * - Do not create a new process to insert, insert the SAME process passed in.
 * Returns a 0 on success or a -1 on any error.
 */
int wlf_enqueue(Wlf_schedule_s *schedule, Wlf_process_s *process) {
    // if schedule or process are NULL return -1
    if (schedule == NULL || process == NULL){
        return -1;
    }

    //Set Flag to Ready
    process->state = ((process->state) | (1 << 15));

    process->next = NULL;
    //Checks if Critical flag is on to know where to insert
    if ((process->state) & 1 << 12){
        //If the linked list is empty assign the first node
        if (schedule->ready_queue_high->head == NULL){
            schedule->ready_queue_high->head = schedule->ready_queue_high->tail = process;
        }else{
            //If the list is not empty insert node at the end of the list
            schedule->ready_queue_high->tail->next = process;
            schedule->ready_queue_high->tail = process;
        }
        //Increment counter of high queue
        schedule->ready_queue_high->count++;
    }
    // check high flag to know where to insert
    else if((process->state) & 1 << 11){
        if (schedule->ready_queue_high->head == NULL){
            //If list is empty assign first node
            schedule->ready_queue_high->head = schedule->ready_queue_high->tail = process;
        }else{
            //If not empty insert node at the end of the list
            schedule->ready_queue_high->tail->next = process;
            schedule->ready_queue_high->tail = process;
        }
        //Increment high count
        schedule->ready_queue_high->count++;
    }else{
        //Go through list and find first node without critical or high flags and insert
        if (schedule->ready_queue_normal->head == NULL){
            //if list is empty insert at start
            schedule->ready_queue_normal->head = schedule->ready_queue_normal->tail = process;
        }else{
            // if not empty assign at the end of list
            schedule->ready_queue_normal->tail->next = process;
            schedule->ready_queue_normal->tail = process;
        }
        // increment at normal queue count
        schedule->ready_queue_normal->count++;
    }


  return 0;
}

/* Returns the number of items in a given Wlf Queue (singly linked list).
 * Follow the project documentation for this function.
 * Returns the number of processes in the list or -1 on any errors.
 */
int wlf_count(Wlf_queue_s *queue) {
    if (queue == NULL){
        return -1;
    }
  return queue->count; //return the current count of queue
}


/* Selects the best process to run from the Ready Queues (singly linked list).
 * Follow the project documentation for this function.
 * Returns a pointer to the process selected or NULL if none available or on any errors.
 * - Do not create a new process to return, return a pointer to the SAME process selected.
 * - Return NULL if the ready queues were both empty OR if there were any errors.
 */
Wlf_process_s *wlf_select(Wlf_schedule_s *schedule) {
    if ( (schedule == NULL) || ((schedule->ready_queue_high == NULL) && (schedule->ready_queue_normal == NULL))){
        return NULL;
    }

    //Initialize a temp pointer
    Wlf_process_s * temp;
    if (schedule->ready_queue_high != NULL && schedule->ready_queue_high->head != NULL){
        temp = schedule->ready_queue_high->head;
        Wlf_process_s * prev = NULL;

        //While loops runs through the linked list to find a critical process if found it will remove it from the list and return a pointer to the removed node
        while (temp != NULL){
            //Only want to look for node with critical flag set
            if (temp->state & 0x1000){
                //if the critical node was found at the beginning of the list remove it
                if (prev == NULL){
                    schedule->ready_queue_high->head = temp->next;
                }else{ // this else is to check the rest of the list if the node was not found at the beginning
                    prev->next = temp->next;
                }
                temp->next=NULL;
                schedule->ready_queue_high->count -= 1;
                temp->age=0;
                temp->state = ((temp->state & ~(0xE000))^0x4000);

                return temp;
            }
            //Move to next node
            prev = temp;
            temp = temp->next;
        }
        // since the high list is also populated with high nodes that are not critical I want to grab the first process in the list
        if (schedule->ready_queue_high->head != NULL){
            temp = schedule->ready_queue_high->head;
            schedule->ready_queue_high->head = temp->next;
            temp->next=NULL;
            //
            schedule->ready_queue_high->count -= 1;
            temp->age=0;
            temp->state = ((temp->state & ~(0xE000))^0x4000);
            return temp;
        }
    }

    if (schedule->ready_queue_normal != NULL && schedule->ready_queue_normal->head != NULL){
        temp = schedule->ready_queue_normal->head;
        schedule->ready_queue_normal->head = temp->next;
        temp->next=NULL;
        schedule->ready_queue_normal->count -= 1;
        temp->state = ((temp->state & ~(0xE000))^0x4000);;
        return temp;
    }
    return NULL;
}

/* Ages all Process nodes in the Ready Queue - Normal and Promotes any that are Starving.
 * If the Ready Queue - Normal is empty, return 0.  (Success if nothing to do)
 * Follow the specification for this function.
 * Returns a 0 on success or a -1 on any error.
 */

int wlf_promote(Wlf_schedule_s *schedule) {
    if (schedule == NULL || schedule->ready_queue_normal == NULL || schedule->ready_queue_high == NULL){
        return -1;
    }

    Wlf_process_s * temp;
    Wlf_process_s * prev = NULL;
    temp = schedule->ready_queue_normal->head;

    while (temp != NULL){
        temp->age+=1;
        temp = temp->next;
    }

    prev = NULL;
    temp = schedule->ready_queue_normal->head;
    while (temp != NULL){
        //
        if ((temp->age >= STARVING_AGE)){
            //
            if (prev == NULL){
                schedule->ready_queue_normal->head = temp->next;
            }else{
                prev->next = temp->next;
            }

            if (temp == schedule->ready_queue_normal->tail){
                schedule->ready_queue_normal->tail=prev;
            }
            temp->next=NULL;
            //
            schedule->ready_queue_normal->count --;
            //
            if (schedule->ready_queue_high->head==NULL){
                schedule->ready_queue_high->head = temp;
                schedule->ready_queue_high->tail = temp;
            }else{
                schedule->ready_queue_high->tail->next = temp;
                schedule->ready_queue_high->tail = temp;
            }
            schedule->ready_queue_high->count++;

            if (prev==NULL){
                temp = schedule->ready_queue_normal->head;
            }else{temp = prev->next;}
        }else{
            //Move on to the next node
            prev = temp;
            temp = temp->next;
        }
    }
    return 0;
}


/* This is called when a process exits normally that was just Running.
 * Put the given node into the Terminated Queue and set the Exit Code 
 * - Do not create a new process to insert, insert the SAME process passed in.
 * Follow the project documentation for this function.
 * Returns a 0 on success or a -1 on any error.
 */
int wlf_exited(Wlf_schedule_s *schedule, Wlf_process_s *process, int exit_code) {
    if (schedule == NULL || process == NULL || schedule->terminated_queue == NULL){
        return -1;
    }

    process->state = (process->state & ~(0xE000))^0x2000;
    process->state = (process->state & ~0x00FF);
	
	//Copies exit_code
	process->state |= exit_code;


    process->next = NULL;
    if (schedule->terminated_queue->head == NULL){
        schedule->terminated_queue->head = schedule->terminated_queue->tail = process;
    }else{
        //If the list is not empty insert node at the end of the list
        schedule->terminated_queue->tail->next = process;
        schedule->terminated_queue->tail = process;
    }
    schedule->terminated_queue->count++;

  return 0; /* Replace This Line with your Code */
}

/* This is called when the OS terminates a process early. 
 * - This will either be in your Ready Queue - High or Ready Queue - Normal.
 * - The difference with wlf_exited is that this process is in one of your Queues already.
 * Remove the process with matching pid from either Ready Queue and add the Exit Code to it.
 * - You have to check both since it could be in either queue.
 * Follow the project documentation for this function.
 * Returns a 0 on success or a -1 on any error.
 */
int wlf_killed(Wlf_schedule_s *schedule, pid_t pid, int exit_code) {
    if (schedule==NULL || schedule->ready_queue_high == NULL || schedule->ready_queue_normal == NULL || schedule->terminated_queue == NULL || pid < 0){
        return -1;
    }

    int pid_found = -1;

    //-00- Start of checking high que
    Wlf_process_s * temp;
    Wlf_process_s * prev = NULL;
    temp = schedule->ready_queue_high->head;
    while (temp!=NULL){
        if (temp->pid == pid){
            pid_found = 0;
            //Set states
            temp->state = (temp->state & ~(0xE000))^0x2000;
            temp->state = (temp->state & ~0x00FF) | exit_code;
            // END of Setting States

            //IMP 1 START
            if (prev == NULL){
                schedule->ready_queue_high->head = temp->next;
            }else{
                prev->next = temp->next;
            }

            if (temp == schedule->ready_queue_high->tail){
                schedule->ready_queue_high->tail=prev;
            }
            temp->next=NULL;
            //
            schedule->ready_queue_high->count--;
            //
            if (schedule->terminated_queue->head==NULL){
                schedule->terminated_queue->head = temp;
                schedule->terminated_queue->tail = temp;
            }else{
                schedule->terminated_queue->tail->next = temp;
                schedule->terminated_queue->tail = temp;
            }
            schedule->terminated_queue->count++;

            if (prev==NULL){
                temp = schedule->ready_queue_high->head;
            }else{temp = prev->next;}
            //IMP 2 END
            //
            //This line ends the initial check prompts the next node to be checked
            // Might want to return Zero here if we find it
        }else{
            //Goes to the next node in list
            prev = temp;
            temp = temp->next;
        }
    }
    //-00- End of checking high que

    //-11- Start of checking normal que
    prev = NULL;
    temp = schedule->ready_queue_normal->head;
    while (temp!=NULL){
        if (temp->pid == pid){
            pid_found = 0;
            //Set states
            temp->state = (temp->state & ~(0xE000))^0x2000;
            temp->state = (temp->state & ~0x00FF) | exit_code;
            // END of Setting States

            //IMP 2 START
            if (prev == NULL){
                schedule->ready_queue_normal->head = temp->next;
            }else{
                prev->next = temp->next;
            }

            if (temp == schedule->ready_queue_normal->tail){
                schedule->ready_queue_normal->tail=prev;
            }
            temp->next=NULL;
            //
            schedule->ready_queue_normal->count--;
            //
            if (schedule->terminated_queue->head==NULL){
                schedule->terminated_queue->head = temp;
                schedule->terminated_queue->tail = temp;
            }else{
                schedule->terminated_queue->tail->next = temp;
                schedule->terminated_queue->tail = temp;
            }
            schedule->terminated_queue->count++;

            if (prev==NULL){
                temp = schedule->ready_queue_normal->head;
            }else{temp = prev->next;}
            //IMP 2 END
            //
            //This line ends the initial check prompts the next node to be checked
            // Might want to return Zero here if we find it
        }else{
            //Goes to the next node in list
            prev = temp;
            temp = temp->next;
        }
    }
    //-11- End of checking normal que
  return pid_found; // we return -1 if node matching pid is not found in high or normal
}

/* This is called when StrawHat reaps a Terminated (Defunct) Process.  (reap command).
 * Remove and free the process with matching pid from the terminated Queue.
 * Follow the specification for this function.
 * Returns the exit_code on success or a -1 on any error (such as process not found).
 */
int wlf_reap(Wlf_schedule_s *schedule, pid_t pid) {
    if (schedule==NULL || schedule->terminated_queue == NULL || schedule->terminated_queue->head == NULL){
        return -1;
    }
    int exit_code = 0;
    Wlf_process_s * temp = schedule->terminated_queue->head;
    Wlf_process_s * prev = NULL;

    //if pid given is equal to 0 remove the first head process from terminated Queue
    if (pid == 0){
        schedule->terminated_queue->head = temp->next;
        schedule->terminated_queue->count--;
        exit_code = wlf_get_ec(temp);
        free(temp->cmd);
        free(temp);
        return exit_code;
        //remove the head of the terminated list
    }

        while (temp!=NULL){
            //If a process was found with the same pid remove it from the list
            if (temp->pid == pid){
                if (prev == NULL){
                    schedule->terminated_queue->head = temp->next;
                }else{
                    prev->next = temp->next;
                }
                schedule->terminated_queue->count--;
                exit_code = wlf_get_ec(temp);
                free(temp->cmd);
                free(temp);
                return exit_code;
                //remove the found process
            }else{
                prev = temp;
                temp = temp->next;
            }
        }

  return -1; // if pid was not found in the terminated list return -1
}

/* Gets the exit code from a terminated process.
 * (All Linux exit codes are between 0 and 255)
 * Follow the project documentation for this function.
 * Returns the exit code if the process is terminated.
 * If the process is not terminated, return -1.
 */
int wlf_get_ec(Wlf_process_s *process) {
    if (process == NULL || !(process->state & 0x2000)){
        return -1;
    }
  return (int)(process->state & 0x00FF);
}

/* Frees all allocated memory in the Wlf_schedule_s, all of the Queues, and all of their Nodes.
 * Follow the project documentation for this function.
 * Returns void.
 */
void wlf_cleanup(Wlf_schedule_s *schedule) {
    if (schedule == NULL)return;
    free_queue(schedule->ready_queue_high);
    free_queue(schedule->ready_queue_normal);
    free_queue(schedule->terminated_queue);
    free(schedule);
}
