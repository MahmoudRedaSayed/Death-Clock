#include "Helpers.c"

/*-----------------------------  Start Global Variables ---------------------------------------------*/
int msgQueueId,
    sharedMemoryId;
int *shmAddr;
bool LastProcess;

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
        quantum = atoi(argv[2]);

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
        /* code */
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
            if(CurrentProcess.lastProcess==true)
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
