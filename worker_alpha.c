#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

bool neighbor_occupied = false;
pid_t neighbor_pid = -1;

// keep track if the calculation should
// continue or not (alarm up or not ?)
bool time_is_up = false;

// keep track of our missed tasks count
int missed_tasks_count = 0;


void calculation();
void _logme(char *signal);

// declare our handler for sigusr1
void handle_sigusr_one(int sig);
// declare our handler for sigusr2
void handle_sigusr_two(int sig);
// declare our handler for alrm
void handle_sigusr_alrm(int sig);



int main(int argc, char *argv[]) {

    // print imidiatly the current process pid
    printf("Current pid: %d\n", getpid());

    // scan for user input
    printf("Insert the neighbor pid: ");
    scanf("%d", &neighbor_pid);

    // confirm the usre input
    printf("Nieghbor pid: %d\n", neighbor_pid);

    // set our signals handlers
    signal(SIGUSR1, &handle_sigusr_one);
    signal(SIGUSR2, &handle_sigusr_two);
    signal(SIGALRM, &handle_sigusr_alrm);

    // pause, and run in the background
    // as default behaviour
    while (true) {
        // put the pause function in a
        // while true loop to be applicable
        // even after the first signal reception
        pause();
    }

    return 0;
}

void calculation() {
    printf("Running.\n");
    while (!time_is_up) {
    }
}

void _logme(char *recieved_signal){
   printf("nieghboor occupied? %s", neighbor_occupied ? "true":"false");
   printf("missed tasks? %d", missed_tasks_count);
   printf("time is up? %s", time_is_up ? "true":"false");
   printf("recieved signal? %s", recieved_signal);
}


void handle_sigusr_one(int sig) {

    // log some info
    _logme("SIGUSR1");

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

void handle_sigusr_two(int sig) {

    // log some info
    _logme("SIGUSR2");

    // if sigusr2 is received, and
    // the boolean flag is set to false
    // this means that the neighbor
    // process is currently working
    // and this one should wait
    if (!neighbor_occupied) {
        neighbor_occupied = true;

        // then make sure that any
        // going calculation stops!
        time_is_up = true;
        return;
    }

    // otherwise, this means that
    // the neighbor process had
    // just finished its work
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
    }
}

void handle_sigusr_alrm(int sig) {

    // log some info
    _logme("SIGALRM");


    // if the alrm is on, this
    // means that the work should
    // be wrapped up, time's up!
    // first, inform the neighbor
    // process that the cpu will be
    // free,
    time_is_up = true;
    kill(neighbor_pid, SIGUSR2);
    return;
}

