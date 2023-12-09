#!/bin/bash


# set some global variables
nieghbor_occupied=0         # start with false
time_is_up=0                # start with false
missed_tasks_count=0


# print out the process id of this running script 
echo "Current process pid: $$"

# read from the user the nieghbor pid 
echo "Please enter the nieghbor pid: "
read NIEGHBOR_PID

function calculation {

    echo "Running calculations."
    while [ $time_is_up -ne 1 ]; do
        echo "." >> ./worker_alpha.log
    done
}

function _alarm {
    sleep $1
    time_is_up=1
    kill -SIGALRM $2
}


function _logme {
    echo "nieghbor_occupied? $nieghbor_occupied"
    echo "time_is_up? $time_is_up"
    echo "missed_tasks_count? $missed_tasks_count"
    echo "signal received? $1"
}

# declare our signal handlers
function sigusr_one_handler {
    # log out some info     
    _logme SIGUSR1

    # when we receive a sig usr 1
    # this means that we need to 
    # do our calculations.
    time_is_up=0

    # So first of all, check if
    # the nieghbor process is 
    # occupied
    if [ $nieghbor_occupied -eq 1 ]; then
        # if occupied, then only queue the
        # missed task, then return
        (( missed_tasks_count++ ))
        return
    fi

    # if not occupied, then do send to 
    # nieghbor process a sigusr2 signal
    # to inform him that we are about to
    # work.
    kill -SIGUSR2 $NIEGHBOR_PID 

    # if missed tasks count > 0, decrement 
    if [ $missed_tasks_count -gt 0 ]; then
        (( missed_tasks_count-- ))
    fi

    # set an alarm of the current process
    # with our custom _alarm function in 
    # about 10s.
    _alarm 10 $$ &

    # call the calc function 
    calculation 

    return
} 


function sigusr_two_handler {

    # log out some info     
    _logme SIGUSR2

    # when we receive the sigusr2
    # signal, this means that either
    # the nieghbor process is about
    # to do some work, or it just 
    # finished.
    if [ $nieghbor_occupied -eq 0 ]; then
        # then the nieghbor is about to 
        # do something, halt self!
        nieghbor_occupied=1
        time_is_up=1
        
        return
    fi

    # otherwise, in this case the nieghbor 
    # just finished its task, unlock self
    time_is_up=0
    nieghbor_occupied=0

    # and tackle any missed task if any
    if [ $missed_tasks_count -gt 0 ]; then
        # send to self a sigusr1 to do some
        # calculations
        kill -SIGUSR1 $$  

        # return immidiatily
        return
    fi
    
    return
}


function sigusr_alrm_handler {

    # log out some info     
    _logme SIGALRM

    # when we recieve a sigalrm
    # this means that the calculation
    # time is up, and we need to stop
    # and drop off from the cpu
    time_is_up=1
    
    # notify the nieghbor that the 
    # cpu is free now
    kill -SIGUSR2 $NIEGHBOR_PID

    return
}



# trap the signals that interest us
trap sigusr_one_handler SIGUSR1
trap sigusr_two_handler SIGUSR2
trap sigusr_alrm_handler SIGALRM
trap "echo 'Interropted, terminating process.'; exit 2" SIGINT
trap "echo 'Killed, terminating process.'; exit 2" SIGTERM


# make our script a deamon, so as it does not exit
# and does not consume resources, we will do that 
# with read, and not with sleep, since sleep is 
# actually consuming ressources.
# link: https://stackoverflow.com/questions/92802/what-is-the-linux-equivalent-to-dos-pause
while read; do
    echo "Continue to run as deamon. (Waiting for next signal)"; 
done



