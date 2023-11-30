/*
 * worker_beta is just a worker called beta since it is the second to
 * be implemented.
 *
 * This worker program will illustrate a solution for the third exercice
 * in the 2022-2023 exam handout, about managing two worker processes in
 * presence of a mono core cpu, without any one of them crushing the other
 * (i.e, one can only take out the cpu if the other one pulled of from it)
 * also, we need to make sure that these processes stay as deamons by default
 * and activate to do the processing as soon as a SIGUSR1 signal is fired and
 * received.
 * */

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


// set the global boolean var, that indicates 
// wheter if the neighbor worker process is 
// currently occupied or not.
bool neighbor_occupied = false;

// keep track of the neighbor pid
pid_t neighbor_pid = -1;

// keep track if the calculation should
// continue or not (alarm up or not ?)
bool time_is_up = false;

// keep track of our missed tasks count
int missed_tasks_count = 0;

// delcare our calculattion function
void calculation();


// declare our handler for sigusr1
void handle_sigusr_one(int sig){

    /* for debugging */
    // printf("recieved a sigusr_one\n");
    // printf("neighbor_occupied ?: %s\n", neighbor_occupied ? "true": "false");
    // printf("queued tasks: %d\n", missed_tasks_count);

    // make sure that the calculation 
    // will go on if possible
    time_is_up = false;


    if (!neighbor_occupied) {

        // send a sigusr2 to the second
        // worker process (the neighbor)
        // to inform it that the current 
        // one is going to take the lead
        kill(neighbor_pid, SIGUSR2);

        // since this can be a retry call
        // decrement the upcomming retry 
        // calls count by one.
        if (missed_tasks_count > 0)
            missed_tasks_count--;
        

        // set the alarm so as
        // the calculation does not overflow
        // in time, normaly it takes <= 10s
        alarm(10); 

        // do our calculation
        calculation();

    } else {
        // the neighbor is already doing
        // something, and should not be 
        // distracted, so we will just
        // register this task as not done
        // and try again later
        missed_tasks_count++;

        // then either pause here, or let the 
        // while loop in the main function do 
        // the trick as a fallback
        // pause()
    }

}
// declare our handler for sigusr2
void handle_sigusr_two(int sig){

    /* for debugging */
    // printf("recieved a sigusr_two\n");
    // printf("neighbor_occupied ?: %s\n", !neighbor_occupied ? "true": "false");
    // printf("queued tasks: %d\n", missed_tasks_count);

    // if sigusr2 is received, and
    // the boolean flag is set to false
    // this means that the neighbor
    // process is currently working
    // and this one should wait
    if(!neighbor_occupied){
        neighbor_occupied = true;

        // then make sure that any 
        // going calculation stops!
        time_is_up = true;

    }
    // otherwise, this means that
    // the neighbor process had
    // just finished its work
    else{
        neighbor_occupied = false;

        // if so, then the cpu is 
        // free, and we can take 
        // it up,

        if (missed_tasks_count > 0) {
            // if there is missed tasks
            // then just fire up 
            // a retry call to current 
            // process via the current
            // pid and a sigusr1 signal
            kill(getpid(), SIGUSR1);
            return;

            // NOTE: this is a bit dangerous
            // since the superviser process 
            // can send a sigusr1 at the 
            // same time, thus one of the processes
            // will be erased. We can bypass this
            // by calling the calculation method 
            // here without firing up another
            // signal, but we will need to decrement
            // our missed taks counter and also send
            // a sigusr2 to the neighbor
        }
    }
}
// declare our handler for alrm
void handle_sigusr_alrm(int sig){

    /* for debugging */
    // printf("recieved a sigusr_alrm\n");
    // printf("neighbor_occupied ?: %s\n", neighbor_occupied ? "true": "false");
    // printf("queued tasks: %d\n", missed_tasks_count);

    // if the alrm is on, this 
    // means that the work should
    // be wrapped up, time's up! 
    // first, inform the neighbor 
    // process that the cpu will be
    // free
    time_is_up = true;
    kill(neighbor_pid, SIGUSR2);
    return;
}



int main(int argc, char* argv[]){

    // print imidiatly the current process pid
    printf("Current pid: %d\n", getpid());


    // scan for user input
    printf("Insert the neighbor pid: ");
    scanf("%d",&neighbor_pid);

    // confirm the usre input
    printf("Nieghboor pid: %d\n", neighbor_pid);

    
    // set our signals handlers
    // for SIGUSR1
    signal(SIGUSR1, &handle_sigusr_one);
    // for SIGUSR2
    signal(SIGUSR2, &handle_sigusr_two);
    // for SIGALRM
    signal(SIGALRM, &handle_sigusr_alrm);
    
    
    // pause, and run in the background
    // as default behaviour
    while (true) {
        // put the pause function in a 
        // while true loop to be applicable
        // even after the first signal reception
        pause();
    }
    // return 
    return 0;
}


void calculation(){
    printf("Running.\n");
    while (!time_is_up) {
    }
}



