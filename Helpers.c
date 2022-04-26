#include <stdio.h> //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <assert.h>
#include <string.h>
// typedef short bool;
#include <stdbool.h>
// ------------------------------------some global vars------------------------------------------------//
FILE* OutputFile = fopen("scheduler.txt", "w");
/////////////////////////////////////////////////   Start structs ////////////////////////////////////
// struct of the process
struct Process
{
    int id, parentId, arriavalTime, runTime, waitingTime, remainingTime, priority;
    bool running, lastProcess;
};
typedef struct Process Process;

struct Message
{
	int Mtype;
	Process CurrentProcess;
};
typedef struct Message Message;
///////////////////////////////////////////////  End structs //////////////////////////////////////////


/*
Function explaintion:-
	This function is used to complie the file and run it and it will be used to create the processes also
	and run the schdular file and the clk file
	Function Parameters:-
		1-File Name :the name of the file that will be complied and run
		2-args[]: Array to put the parameter if exist in it that will be sent to the file
*/


void RunAndComplie(char* FileName,char* arg_0, char* arg_1, char* arg_3)
{
	char * Complie;
	Complie=(char*)malloc((20+2*sizeof(FileName)*sizeof(char)));
	strcpy(Complie,"gcc ");
	strcat(Complie,FileName);
	strcat(Complie,".c ");
	strcat(Complie,"-o ");
	strcat(Complie,FileName);
	strcat(Complie,".out");
	system(Complie);
	char FileNameOut[80];
	strcpy(FileNameOut, FileName);
	//fflush(stdout);
	//printf("\n%s\n", FileName);
	printf("\n");
    strcat(FileNameOut,".out");
    execl(FileNameOut,FileName, arg_0, arg_1, NULL);
}


//------------------------------------------------------------------------------------------------------------------------//
/*
Function explaintion:-
	This function will be used in send and recieve the processed between the process generator that will read it 
	and the schudlar that will take and and create it
	Function Parameters:-
		1-Identifier :- the unique of the message queue to control which will use the queue
*/


int InitMsgQueue(char Idenitfier)
{
	key_t key=ftok("keyFile",Idenitfier);
	int megid=msgget(key, 0666 | IPC_CREAT);
	if(megid==-1)
	{
		printf("error in creation in message queue");	
		exit(-1);
	}
	return megid;

}


//------------------------------------------------------------------------------------------------------------------------//
/*
Function explaintion:-
	This function will be used mainly in the remaining time of the process and it will be shared by the schdular and the 
	running process 
	Function Parameters:-
		1-Idenitfier:- the unique of the share memory to control which one will use it 
		2-IdShm:- it is pointer will be sent to put the shm id in it
	The return value:
		the function will return the address of the shared memory to use it in write and read
*/


void * InitShm(char Idenitfier, int * IdShm)
{
    key_t Key = ftok("keyFile", Idenitfier);
    int ShmId = shmget(Key, 4096, 0666 | IPC_CREAT);
    *IdShm = ShmId;

    if (ShmId == -1)
    {
        perror("error in creation in shared memory");
        exit(-1);
    }

    void * ShmAddr  = shmat(ShmId, (void *)0, 0);
    return ShmAddr;
}

/*
two Functions :
	these two functions used in send and receive to and from the message queue
	Commen Parameters:
		MsgId: the id of the queue to control it
	Send: 
		ArrivedProcess: it the process will be arrive at this moment
	Rec:
		return Process that sent be the process generator
*/

Process RecMsg(int MsgId)
{
	Message ResMessage;
	int Flag=msgrcv(MsgId, &ResMessage, sizeof(ResMessage.CurrentProcess), 0, IPC_NOWAIT);
	if(Flag==-1)
		printf("\nerror in reciving\n");
		return NULL;
	return ResMessage.CurrentProcess;

}


void SendMsg(int MsgId,Process ArrivedProcess)
{
	Message SendMessage;
	SendMessage.CurrentProcess=ArrivedProcess;
	int Flag=msgsnd(MsgId, &SendMessage, sizeof(SendMessage.CurrentProcess), !IPC_NOWAIT);
	if(Flag==-1)
		printf("\nerror in Sending\n");
}

//------------------------------------------------------------------------------------------------------------------------//



#define true 1
#define false 0

#define SHKEY 300


///==============================
//don't mess with this variable//
int * shmaddr;                 //
//===============================



// get clock function
int getClk()
{
    return *shmaddr;
}


/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
*/
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *) shmat(shmid, (void *)0, 0);
}


/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
*/

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}



//---------------------------------------some functions needed by the HPF--------------------------------//
void StartProcess(Process* CurrentProcess)
{
	*CurrentProcess.id=fork();
	if(*CurrentProcess.id==0)
	{
		RunAndComplie("process",NULL,NULL);
	}
	fprintf(OutputFile, "At time %d process %d started arr %d total %d remain %d wait %d\n",
            getClk(), *CurrentProcess.id, *CurrentProcess.arrivalTime, *CurrentProcess.runTime,
            *CurrentProcess.runTime, getClk() - *CurrentProcess.arrivalTime);
}
// must take queue as parameter
void ReadyProcessExist(int MsgId)
{
	Process* RecProcess;
	while(1)
	{
		RecProcess=NULL;
		*RecProcess=RecMsg(MsgId);
		if(RecProcess!=NULL)
		{
			// push it in the queue 
		}
		else 
		{
			break;
		}
	}
}


void FinishProcess(Process FinishedProcess)
{
	int FinishTime=getClk();
	double WTA = (getClk() - FinishedProcess.arrivalTime) * 1.0 / FinishedProcess.runTime;
    int wait = (getClk() - FinishedProcess.arrivalTime) - FinishedProcess.runTime;
    fprintf(OutputFile, "At time %d process %d finished arr %d total %d remain 0 wait %d TA %d WTA %.2f\n", getClk(),
            FinishedProcess.id, FinishedProcess.arrivalTime, FinishedProcess.runTime,
            wait, getClk() - FinishedProcess.arrivalTime, WTA); 
}


void StopProcess(Process p);
void ContinueProcess(Process p);
int comparePriorities(int,int);
