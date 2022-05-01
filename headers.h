#include <stdio.h>
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
#include <stdbool.h>
#define true 1
#define false 0
#define SHKEY 300


///==============================
//don't mess with this variable//
int * shmaddr;                 //
//===============================

/******************************** Structs Delcaration ***********************************************/
// Process Struct
struct Process
{
    int Id_1,   // for forking
        Id_2,   // for input
        ArriavalTime,
        RunTime,
        WaitingTime,
        RemainingTime,
        Priority;
    bool LastProcess;
};
typedef struct Process Process;

// Message Struct
struct Message
{
	long mtype;
	Process CurrentProcess;
};
typedef struct Message Message;


/*********************************** Functions ***********************************************/

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




// initialization of a shared memroy segment, and return its id
int initializeSharedMemory(void*sharedMemoryAddress, int project_id)
{
    int shmid;
    key_t sharedMemoryKey = ftok("keyFile", project_id);    //ftok - converts a pathname and a project identifier to a System V IPC key
    shmid = shmget(sharedMemoryKey, 4096, IPC_CREAT | 0666);

    if (shmid == -1)
    {
        perror("Failed to create shared Memory");
        exit(-1);
    }else   printf("Created a shared memory segment with id : %d\n", shmid);

    sharedMemoryAddress = shmat(shmid, (void*)0, 0);    // attach to the shm

    return shmid;
}


