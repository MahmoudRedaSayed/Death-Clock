#include "Helpers.c"

/*-----------------------------  Start Global Variables ---------------------------------------------*/
int msgQueueId,
    sharedMemoryId;
int *shmAddr;
bool LastProcess;
int countProcesses;

/*-----------------------------   End Global Variables ----------------------------------------------*/

/*------------------------------ Start Functions Prototypes -----------------------------------------*/
void clearResources(int signum);
void HPF();

/*------------------------------ End Functions Prototypes -----------------------------------------*/



int main(int argc, char * argv[])
{
    initClk();
    signal(SIGINT, clearResources); //upon termination release the clock resources.
    
    //TODO implement the scheduler :)

    msgQueueId = InitMsgQueue('q');
    shmAddr    = InitShm('m', &sharedMemoryId);
    
    int chosenAlgorithm = atoi(argv[1]),    // needs checking 0 or 1 ?
        quantum = atoi(argv[2]),
        numOfProcesses = atoi(argv[3]);

    countProcesses = numOfProcesses;
    
    // TODO : Declare here a queue for the ready processes

    switch (chosenAlgorithm)
    {
    case 1:
        HPF();
        break;
    case 2:
        /* code */
        break;
    case 3:
        RoundRobin(quantum);
        break;
    }
    
    printf("\nrunning Schedular\n");
    destroyClk(true);
}

// clear the resources
void clearResources(int signum)
{
    msgctl(msgQueueId, IPC_RMID, (struct msqid_ds *)0);
    shmctl(sharedMemoryId, IPC_RMID, NULL);
    signal(SIGINT, SIG_DFL);
}

// HPF
// 1- check every time if there is a process coming or not
// 2- 
void HPF()
{
    Process CurrentProcess;
    CurrentProcess.lastProcess=false;
    *shmAddr=-1;

    while(!(*shmAddr==-1&&isEmpty(queue)&&CurrentProcess.lastProcess))
    {
        // function to check if there is new process or not
        ReadyProcessExist(msgQueueId);
        if(*shmAddr==0)
        {
            FinishProcess(CurrentProcess);
            countProcesses--;
            if(countProcesses == 0)
            {
                break;
            }
        }
            // function to finish the process
        if(!isEmpty(queue)&&*shmAddr==0)
        {
            // pop a new process to run it 
            CurrentProcess=pop(queue);
            *shmAddr=CurrentProcess.runTime;
            StartProcess(&CurrentProcess);
        }
    }
}

// Round Robin Algorithm
void RoundRobin(int quantum)
{
    
    Process currentProcess;
    int currentQuantum = 0;

   
   *shmAddr = -1;
    // TODO : declare queue


    while(!currentProcess.lastProcess && *shmAddr != -1 /* && TODO: queue is not empty */  )
    {
        // check if there is a process arrived 
        ReadyProcessExist(msgQueueId);

        // check whether the current process is finished or not
        if (*shmAddr == 0)
        {
            FinishProcess(currentProcess);
            countProcesses--;
            currentQuantum = 0;           // reset

            if(countProcesses == 0)
            {
                break;
            }
        }
        
        currentQuantum++;

        // if the quantum is over ==> switch to another process if any
        if (currentQuantum == quantum  /* && TODO: queue is not empty */)
        {
            currentQuantum = 0;           // reset

            if(*shmAddr > 0)
            {
                currentProcess.remainingTime = *shmAddr;
                /* TODO : enqueu the current process */
                StopProcess(msgQueueId);
            }

            // switch to another process
            currentProcess = /* TODO : dequeu() */;

            // update the remaining time in the memory
            *shmAddr = currentProcess.remainingTime;

            // check if this is the first time to start or conitue
            if (currentProcess.remainingTime == currentProcess.runTime)
                StartProcess(msgQueueId);
            else
                ContinueProcess(msgQueueId);
        }

        
    }

}

// Shortest Remaining Time Next
void SRTN()
{
    *shmAddr = -1;
    Process CurrentProcess;
    CurrentProcess.lastProcess=false;
    *shmAddr=-1;
    /* TODO : initialize a priority queue here*/

    while(!(*shmAddr==-1&&isEmpty(queue)&&CurrentProcess.lastProcess))
    {
        // function to check if there is new process or not
        ReadyProcessExist(msgQueueId);

        int highestPriorityInQueue = /*topOfQueue()*/; 
        int flag = comparePriorities(*shmAddr, highestPriorityInQueue)  // flag : 1 --> switch
        
        if(flag == 1)
        {
            // TODO : enqueu(CurrentProcess, pri)
            
            CurrentProcess = top();
            *shmAddr = CurrentProcess.remainigTime;
            
            if (CurrentProcess.remainigTime == CurrentProcess.runTime)
                StartProcess(CurrentProcess);
            else 
                ContinueProcess(CurrentProcess);
        }

        if(*shmAddr==0)
        {
            FinishProcess(CurrentProcess);
            countProcesses--;
            if(countProcesses == 0)
            {
                break;
            }
        }

        // function to finish the process
        if(!isEmpty(queue)&&*shmAddr==0)
        {
            // pop a new process to run it 
            CurrentProcess=pop(queue);
            *shmAddr=CurrentProcess.runTime;
            StartProcess(&CurrentProcess);
        }
    }
    

}