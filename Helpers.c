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

/*
Function explaintion:-
	This function is used to complie the file and run it and it will be used to create the processes also
	and run the schdular file and the clk file
	Function Parameters:-
		1-File Name :the name of the file that will be complied and run
		2-args[]: Array to put the parameter if exist in it that will be sent to the file
*/


void RunAndComplie(string FileName,char* args[])
{
	char * Complie;
	Complie=(char*)malloc(13+2*sizeof(FileName)*sizeof(char));
	strcpy(Complie,"gcc ");
	strcat(Complie,FileName);
	strcat(Complie,".c ");
	strcat(Complie,"-o ");
	strcat(Complie,FileName);
	strcat(Complie,".out");
	system(Complie);
	execl(FileName,args[0],args[1]);
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
    key_t Key = ftok("keyfile", Idenitfier);
    int ShmId = shmget(Key, 4096, 0666 | IPC_CREAT);
    *IdShm = ShmId;

    if (ShmId!=-1)
    {
        perror("error in creation in shared memory");
        exit(-1);
    }

    void * ShmAddr  = shmat(ShmId, (void *)0, 0);
    return ShmAddr;
}


//------------------------------------------------------------------------------------------------------------------------//
