#include "Helpers.c"
/******************************* Global Variables *******************************/
int MsgQueueId,
    SharedMemoryId,
    SharedMemoryId2,
    CountProcesses,
    LastArrivalTime,
    ReceivedCounter,
    flag,
    last;
chosenAlgorithm;
int *ShmAddr;
int *ShmAddr2;
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
    ShmAddr2 = InitShm('n', &SharedMemoryId2);
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
    int flag = 1, flag2 = 1;

    ReceivedCounter = CountProcesses;
    CurrentProcess.LastProcess = false; // to enter the first iteration of the next loop
    *ShmAddr = -1;
    *ShmAddr2 = 0;
    union Semun semun;
    int sem1 = semget('m', 1, 0666 | IPC_CREAT);
    sem1 = 0;
    semun.val = 0;                                         /* initial value of the semaphore, Binary semaphore */
    if (semctl(sem1, /* semnum */ 0, SETVAL, semun) == -1) // semnum = 0, because we have only one here so it is the first index
                                                           // and this is just initialization
    {
        perror("Error in semctl");
        exit(-1);
    }

    while (!(*ShmAddr == -1 && CurrentProcess.LastProcess))
    {
        // last = getClk();
        // printf("shmaddr:%d, clk : %d\n", *ShmAddr, last);
        if (*ShmAddr2 == 1 || flag == 1)
        {
            if (*ShmAddr == 0)
            {
                FinishProcess(&CurrentProcess, ShmAddr);
                printf("After Finish :clock:%d, shmaddr:%d\n", getClk(), *ShmAddr);
                *ShmAddr = -1;
                flag = 0;
                flag2 = 0;
                // up(sem1);
                *ShmAddr2 = 0;
                flag++;
                flag2 = 0;
                last = getClk();
                nextSecondWaiting(&last);
            }

            // wait until the clock changes
            // nextSecondWaiting(&last);
            if (flag2 = 1)
            {
                if (*ShmAddr == -1 && !isEmpty(q))
                {
                    // pop a new process to run it
                    CurrentProcess = pop(&q);
                    *ShmAddr = CurrentProcess.RunTime;
                    printf("clock:%d, shmaddr:%d\n", getClk(), *ShmAddr);
                    StartProcess(&CurrentProcess);
                    flag += 1;
                }
            }
        }

        // if (*ShmAddr == 0)
        // {
        //     FinishProcess(&CurrentProcess, ShmAddr);
        //     printf("After Finish :clock:%d, shmaddr:%d\n", getClk(), *ShmAddr);
        //     *ShmAddr = -1;
        // }

        // wait until the clock changes
        // nextSecondWaiting(&last);

        // if (*ShmAddr == -1 && !isEmpty(q))
        // {
        //     // pop a new process to run it
        //     CurrentProcess = pop(&q);
        //     *ShmAddr = CurrentProcess.RunTime;
        //     printf("clock:%d, shmaddr:%d\n", getClk(), *ShmAddr);
        //     StartProcess(&CurrentProcess);
        // }
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
        flag = 1,
        x = 0,
        var = 0,
        last = -1,
        flag3 = 0,
        ProcessesHashTime[LastArrivalTime];

    *ShmAddr = -1;
    *ShmAddr2 == 1 ;
    ReceivedCounter = CountProcesses;
    CurrentProcess.RemainingTime = -2; // to prevent it enter the block of code that finishes the process in the start

    last = -1;
    while (!(*ShmAddr == -1 && CurrentProcess.LastProcess && isEmpty(q)))
    {
        if(*ShmAddr2 == 1 || flag == 1)
        {
            *ShmAddr2=0;
            CurrentQuantum++;
        // check whether the current process is finished or not
        if (*ShmAddr == 0)
        {
            FinishProcess(&CurrentProcess, ShmAddr);
            if (isEmpty(q))
                {
                    flag3 = 1;
                }
            *ShmAddr = -1;
            CurrentQuantum = 0; // reset
            last=getClk();
        nextSecondWaiting(&last);
        }

        // if the quantum is over ==> switch to another process if any
        // the condition (*ShmAddr <= 0 || CurrentQuantum == 0)
        // *ShmAddr <= 0  => to manage the finished process
        // CurrentQuantum == 0 => to manage the stopped process and it possible to be done in the finished process also
        if ((*ShmAddr < 0 || CurrentQuantum == quantum))
        {
            // The condition (CurrentQuantum == quantum && !*ShmAddr<= 0)
            // CurrentQuantum == quantum && !*ShmAddr<= 0 => to make sure that doesn't come from the finish part
            // that the value of the share memory will not be equal zero and this process will be stopped
            if (CurrentQuantum == quantum && !(*ShmAddr < 0)) // if true => there was a process already running
            {
                StopProcess(CurrentProcess);
                CurrentProcess.RemainingTime = *ShmAddr;
                printf("Process :%d,remining:%d\n", CurrentProcess.Id_2, *ShmAddr);
                push(&q, CurrentProcess, 0);
                last=getClk();
                nextSecondWaiting(&last);
            }
            // switch to another process, or get the first arrived process to work
            if (!isEmpty(q))
            {
                CurrentProcess = pop(&q);
            // update the remaining time in the memory
            *ShmAddr = CurrentProcess.RemainingTime;
            // check if this is the first time to start or conitue
            if (CurrentProcess.RemainingTime == CurrentProcess.RunTime)
            {
                StartProcess(&CurrentProcess);
                flag3 = 0;
                flag++;
                printf("Process : %d, shmaddr:%d\n", CurrentProcess.Id_2, *ShmAddr);
            }
            else
            {
                ContinueProcess(CurrentProcess);
            }
            CurrentQuantum = 0; // reset
            }
            
            
        }
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
    *ShmAddr2 == 1;

    int LowestRemainingTime = 0,
        condition = 0,
        flag = 1,
        counter = 0,
        last = -1,
        flag3 = 0;
    int ProcessesHashTime[LastArrivalTime + 1];

    *ShmAddr = -1;
    CurrentProcess.LastProcess = false;
    ReceivedCounter = CountProcesses;

    while (!(*ShmAddr == -1 && CurrentProcess.LastProcess && isEmpty(q)))
    {
        if (*ShmAddr2 == 1 || flag == 1 || peek(q).LastProcess || flag3 == 1)
        {
            *ShmAddr2 = 0;
            if (*ShmAddr == 0)
            {
                FinishProcess(&CurrentProcess, ShmAddr);
                if (isEmpty(q))
                {
                    flag3 = 1;
                }

                *ShmAddr = -1;
                last = getClk();
                nextSecondWaiting(&last);
                flag++;
                printf("shmaddr2:%d\n", *ShmAddr2);
            }

            // wait until the clock changes

            if (!isEmpty(q))
            {
                // compare
                LowestRemainingTime = peek(q).RemainingTime;
                // printf("Lowest:%d, clk:%d \n", LowestRemainingTime, getClk());
                // printf("ShmAddr:%d\n", *ShmAddr);
                if (LowestRemainingTime < *ShmAddr) // if shared memory is -1 it will not go into it
                {
                    CurrentProcess.RemainingTime = *ShmAddr;
                    push(&q, CurrentProcess, 2);
                    printf("stoppp\n");
                    StopProcess(CurrentProcess);
                    last = getClk();
                    nextSecondWaiting(&last);
                    *ShmAddr = -1;
                }

                if (*ShmAddr == -1)
                {
                    CurrentProcess = pop(&q);
                    *ShmAddr = CurrentProcess.RemainingTime;
                    if (CurrentProcess.RemainingTime == CurrentProcess.RunTime)
                    {
                        StartProcess(&CurrentProcess);
                        flag3 = 0;
                        flag++;
                    }
                    else
                    {
                        ContinueProcess(CurrentProcess);
                    }
                }
            }
        }
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
        // int x = peek(q).RemainingTime;
        // printf("LowestRecv: %d\n", x);
        break;
    case 3:
        ReadyProcessExist(MsgQueueId, &q, &flag, 0, &last);
        break;
    }

    // just to confirm
    signal(SIGUSR1, checkRecieving);
}
