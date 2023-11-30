

/*
*   This will be the program that spawns the workers and 
*   wait til they terminate.
*
*   This way we can pass to each worker its nighboor pid.
*
*
*   This will also expect a command line argument, which 
*   will be the lifespan of the whole workers, after that
*   amount of time (passed in seconds), this process will
*   terminate the workers
*
*   NOTE: I was thinking about fork and exec, but this way
*   I am stuck since I can not give the pid of the currently
*   spawned process to the next one ! 
*
*   The best way to do so, is to actually map out some shared
*   memory, and write a vector of pids which will contain all
*   the process that the current initiator process had spawned
*
*   This way, the workers will read from that shared memory, so
*   technically, no locking is requied, (it is read only), and 
*   figure out all the nieghboors pids, and that can be easily 
*   done just by traversing the array and regarding all the pids 
*   that are diffrent then the worker process pid as some neighboor
*   pid.
*
*
*   This will be a good practice, but just for testing sake, I will
*   attempt another approach that is more simplistic. I will make 
*   sure that each worker at first prints out its pid to the terminal
*   and then waits for my/user input. This way I can manually grap 
*   the pid of the neighboor and pass it to the correct worker, this
*   does not scale as the first solution, but it is rather simple to
*   implement and test. So using this solution, this program is actually
*   not needed.
*   
* */


#include <signal.h>
#include <unistd.h>
int workers_count = 2;
int main(int argc, char* argv[]){

    // fork and exec the workers (alpha and beta)
    pid_t pid = -1;
    for(int i = 0; i < workers_count; i++){
        pid = fork();
        
        if (pid == 0) {
            // this is the child process
            
            
        }else {
            // this is the parent process
        }
    }


    return 0;
}
