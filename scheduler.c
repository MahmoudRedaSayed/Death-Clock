#include "Helpers.c"
/******************************* Global Variables *******************************/
int MsgQueueId,
    SharedMemoryId,
    CountProcesses,
    LastArrivalTime,
    ReceivedCounter,
    flag,
    last;
    chosenAlgorithm;
int *ShmAddr;
bool LastProcess;
Node *q = NULL;

void clearResources(int);
void checkRecieving(int signum);

int main(int argc, char *argv[])
{
    initClk();
    signal(SIGINT, clearResources); // upon termination release the clock resources.
    signal(SIGUSR1, checkRecieving);

    printf("Scheduler is starting\n");

    MsgQueueId = InitMsgQueue('q');
    ShmAddr = InitShm('m', &SharedMemoryId);
    chosenAlgorithm = atoi(argv[1]);

    int quantum = atoi(argv[2]),
        NumOfProcesses = atoi(argv[3]);
    LastArrivalTime = atoi(argv[4]);

    CountProcesses = NumOfProcesses;

    // start the algorithm
    switch (chosenAlgorithm)
    {
    case 1:
        HPF();
        break;
    case 2:
        SRTN();
        break;
    case 3:
        RoundRobin(quantum);
        break;
    }

    destroyClk(true);
}

//------------------------------------------------------------------------------------------------------------------------//
/*
Function explaintion:-
    This function is the implementation of the Highest Priority First Algorithm,
    it selects the highest priority process of the ready queue and start it until it is finished,
    takes the next highest priority and so on ....
*/
void HPF()
{
    printf("Start HPF Algo\n");

    Process CurrentProcess;

    last = -1;
    int timeConsume = 0;

    ReceivedCounter = CountProcesses;
    CurrentProcess.LastProcess = false; // to enter the first iteration of the next loop
    *ShmAddr = -1;

    while (!(*ShmAddr == -1 && CurrentProcess.LastProcess))
    {
        // last = getClk();
        printf("shmaddr:%d\n", *ShmAddr);
        if (*ShmAddr == 0)
        {
            FinishProcess(&CurrentProcess, ShmAddr);
            *ShmAddr = -1;
            
        }

        if (*ShmAddr == -1 && !isEmpty(q))
        {
            // pop a new process to run it
            CurrentProcess = pop(&q);
            *ShmAddr = CurrentProcess.RunTime;
            StartProcess(&CurrentProcess);
        }

        // wait until the clock changes
        nextSecondWaiting(&last);
    }
}
//------------------------------------------------------------------------------------------------------------------------//
/*
Function explaintion:-
    This function is the implementation of the Round Robin Algorithm,
    Each process is assigned a fixed time slot in a cyclic way.
    Algorithm Parameters:-
        1-quantum:- The time quantum
*/

void RoundRobin(int quantum)
{
    printf("Round Robin Algorithm Starts\n");
    Process CurrentProcess;

    int CurrentQuantum = 0,
        index = 0,
        flag = 0,
        x = 0,
        var = 0,
        last = -1,
        ProcessesHashTime[LastArrivalTime];

    *ShmAddr = -1;
    ReceivedCounter = CountProcesses;
    CurrentProcess.RemainingTime = -2; // to prevent it enter the block of code that finishes the process in the start

    last = -1;

    while (!(*ShmAddr == -1 && CurrentProcess.LastProcess && isEmpty(q)))
    {
        nextSecondWaiting(&last);
        CurrentQuantum++;
        // check whether the current process is finished or not
        if (*ShmAddr == 0)
        {
            printf("clk:%d\n", getClk());
            FinishProcess(&CurrentProcess, ShmAddr);
            *ShmAddr = -1;
            CurrentQuantum = 0; // reset
            // if (CountProcesses == 0)
            // {
            //     break;
            // }
        }

        // if the quantum is over ==> switch to another process if any
        // the condition (*ShmAddr <= 0 || CurrentQuantum == 0)
        // *ShmAddr <= 0  => to manage the finished process
        // CurrentQuantum == 0 => to manage the stopped process and it possible to be done in the finished process also
        if ((*ShmAddr < 0 || CurrentQuantum == quantum) && !isEmpty(q))
        {
            // The condition (CurrentQuantum == quantum && !*ShmAddr<= 0)
            // CurrentQuantum == quantum && !*ShmAddr<= 0 => to make sure that doesn't come from the finish part
            // that the value of the share memory will not be equal zero and this process will be stopped
            if (CurrentQuantum == quantum && !*ShmAddr < 0) // if true => there was a process already running
            {
                StopProcess(CurrentProcess);
                CurrentProcess.RemainingTime = *ShmAddr;
                push(&q, CurrentProcess, 0);
            }
            // switch to another process, or get the first arrived process to work
            CurrentProcess = pop(&q);
            // update the remaining time in the memory
            *ShmAddr = CurrentProcess.RemainingTime;
            // check if this is the first time to start or conitue
            if (CurrentProcess.RemainingTime == CurrentProcess.RunTime)
                StartProcess(&CurrentProcess);
            else
                ContinueProcess(CurrentProcess);
            CurrentQuantum = 0; // reset
        }
        
    }
}
//------------------------------------------------------------------------------------------------------------------------//
/*
Function explaintion:-
    This function is the implementation of the Shortest Remaining Time Next Algorithm,
    This Algorithm is the preemptive version of SJF scheduling.
    At the arrival of every process, the schedules select the process with the least remaining burst time among the list of available processes and the running process.
*/
void SRTN()
{
    printf("SRTN starts\n");

    Process CurrentProcess;
    Process *temp;
    Node *Priority_Q = NULL;

    int LowestRemainingTime = 0,
        condition = 0,
        flag = 0,
        counter = 0,
        last = -1;
    int ProcessesHashTime[LastArrivalTime + 1];

    *ShmAddr = -1;
    CurrentProcess.LastProcess = false;
    ReceivedCounter = CountProcesses;

    while (!(*ShmAddr == -1 && CurrentProcess.LastProcess && isEmpty(q)))
    {
        if (*ShmAddr == 0)
        {
            FinishProcess(&CurrentProcess, ShmAddr);
            *ShmAddr = -1;
            // if (CountProcesses == 0)
            // {
            //     break;
            // }
        }

        if (!isEmpty(q))
        {
            while (!isEmpty(q))
            {
                Process p = pop(&q);
                printf("id : %d\n" ,p.Id_2);
            }
            
            // compare 
            LowestRemainingTime = peek(q).RemainingTime;
            printf("Lowest:%d\n", LowestRemainingTime);
            printf("ShmAddr:%d\n", *ShmAddr);
            if (LowestRemainingTime < *ShmAddr) // if shared memory is -1 it will not go into it
            {
                CurrentProcess.RemainingTime = *ShmAddr;
                push(&q, CurrentProcess, 2);
                printf("stoppp\n");
                StopProcess(CurrentProcess);
            }
            CurrentProcess = pop(&q);
            *ShmAddr = CurrentProcess.RemainingTime;
            if (CurrentProcess.RemainingTime == CurrentProcess.RunTime)
            {
                StartProcess(&CurrentProcess);
                
            }
            else
            {
                ContinueProcess(CurrentProcess);
                
            }
        }
        // wait until the clock changes
        nextSecondWaiting(&last);
    }
}

//------------------------------------------------------------------------------------------------------------------------//
/*
Function explaintion:-
    This function clears the resources
*/
void clearResources(int signum)
{
    msgctl(MsgQueueId, IPC_RMID, (struct msqid_ds *)0);
    shmctl(SharedMemoryId, IPC_RMID, NULL);
    signal(SIGINT, SIG_DFL);
}
//------------------------------------------------------------------------------------------------------------------------//
/*
Function explaintion:-
    This function checks if the process generator sent a process or not by SIGUSR1
*/
void checkRecieving(int signum)
{
    // start the algorithm
    switch (chosenAlgorithm)
    {
    case 1:
        ReadyProcessExist(MsgQueueId, &q, &flag, 1, &last);
        break;
    case 2:
        ReadyProcessExist(MsgQueueId, &q, &flag, 2, &last);
        break;
    case 3:
        ReadyProcessExist(MsgQueueId, &q, &flag, 0, &last);
        break;
    }

    // just to confirm
    signal(SIGUSR1, checkRecieving);
}
