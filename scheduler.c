#include "Helpers.c"
#include "PriorityQueue.c"
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
    
    printf("Scheduler is starting\n");

    msgQueueId = InitMsgQueue('q');
    shmAddr    = InitShm('m', &sharedMemoryId);
    
    int chosenAlgorithm = atoi(argv[1]),
        quantum = atoi(argv[2]),
        numOfProcesses = atoi(argv[3]);

    countProcesses = numOfProcesses;

    // test
    // printf("chosenAlgorithm:%d\n", chosenAlgorithm);
    // printf("quantum:%d\n", quantum);
    // printf("numOfProcesses:%d\n", countProcesses);
    // exit(-1);
    
    // TODO : Declare here a queue for the ready processes

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
    CurrentProcess.priority=0;
    *shmAddr=-1;
    Node* q = newNode(CurrentProcess);

    while(!(*shmAddr==-1&&isEmpty(&q)&&CurrentProcess.lastProcess))
    {
        // function to check if there is new process or not
        ReadyProcessExist(msgQueueId, q);
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
        if(!isEmpty(&q)&&*shmAddr==0)
        {
            // pop a new process to run it 
            CurrentProcess = peek(q);
            pop(q);
            *shmAddr=CurrentProcess.runTime;
            StartProcess(&CurrentProcess);
        }
    }
}

// // Round Robin Algorithm
// void RoundRobin(int quantum)
// {
    
//     Process currentProcess;
//     int currentQuantum = 0;

   
//    *shmAddr = -1;
//    Queue_Process *q = createQueue_Process(countProcesses); 


//     while(!currentProcess.lastProcess && *shmAddr != -1 /* && TODO: queue is not empty */  )
//     {
//         // check if there is a process arrived 
//         ReadyProcessExist(msgQueueId);

//         // check whether the current process is finished or not
//         if (*shmAddr == 0)
//         {
//             FinishProcess(currentProcess);
//             countProcesses--;
//             currentQuantum = 0;           // reset

//             if(countProcesses == 0)
//             {
//                 break;
//             }
//         }
        
//         currentQuantum++;

//         // if the quantum is over ==> switch to another process if any
//         if (currentQuantum == quantum  /* && TODO: queue is not empty */)
//         {
//             currentQuantum = 0;           // reset

//             if(*shmAddr > 0)
//             {
//                 currentProcess.remainingTime = *shmAddr;
//                 Enqueue_Process(q, currentProcess);
//                 StopProcess(currentProcess);
//             }

//             // switch to another process
//             Dequeue_Process(q);
//             currentProcess = front_Process(q);

//             // update the remaining time in the memory
//             *shmAddr = currentProcess.remainingTime;

//             // check if this is the first time to start or conitue
//             if (currentProcess.remainingTime == currentProcess.runTime)
//                 StartProcess(&currentProcess);
//             else
//                 ContinueProcess(currentProcess);
//         }

        
//     }

// }

// // Shortest Remaining Time Next
//  void SRTN()
// {
//     *shmAddr = -1;
//     Process CurrentProcess;
//     CurrentProcess.lastProcess = false;
//     *shmAddr=-1;
//     Queue_Process *q = createQueue_Process(countProcesses); 

//     while(!(*shmAddr==-1&&/* isEmptyOfQueue */CurrentProcess.lastProcess))
//     {
//         // function to check if there is new process or not
//         ReadyProcessExist(msgQueueId);

//         int highestPriorityInQueue = /*topOfQueue()*/; 
//         int flag = comparePriorities(*shmAddr, highestPriorityInQueue)  // flag : 1 --> switch
        
//         if(flag == 1)
//         {
//             // TODO : enqueu(CurrentProcess, pri)
            
//             CurrentProcess = top();
//             *shmAddr = CurrentProcess.remainigTime;
            
//             if (CurrentProcess.remainigTime == CurrentProcess.runTime)
//                 StartProcess(CurrentProcess);
//             else 
//                 ContinueProcess(CurrentProcess);
//         }

//         if(*shmAddr==0)
//         {
//             FinishProcess(CurrentProcess);
//             countProcesses--;
//             if(countProcesses == 0)
//             {
//                 break;
//             }
//         }

//         // function to finish the process
//         if(!isEmpty(queue)&&*shmAddr==0)
//         {
//             // pop a new process to run it 
//             CurrentProcess=pop(queue);
//             *shmAddr=CurrentProcess.runTime;
//             StartProcess(&CurrentProcess);
//         }
//     }
    

// }