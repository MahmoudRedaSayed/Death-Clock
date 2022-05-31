#include "Helpers.c"
#include <math.h>
/******************************* Global Variables *******************************/
int MsgQueueId,
    SharedMemoryId,
    SharedMemoryId2,
    CountProcesses,
    flag,
    NumOfProcesses,
    ClockAtFinishing;
last,
    chosenAlgorithm;
int *ShmAddr;
int *ShmAddr2;
Node *WaitingQueue = NULL; // waiting queue
Node *processesWTA = NULL;
Node *ReadyQueue = NULL; // Processes that are currently running, and we push in it when memory can hold it.

void clearResources(int);
void checkRecieving(int signum);
double calculateSD(Node **head);

int main(int argc, char *argv[])
{
    ClockAtFinishing = 0; // initialization
    initClk();
    buddyInit();
    printFirstLineInFile();
    signal(SIGINT, clearResources); // upon termination release the clock resources.
    signal(SIGUSR1, checkRecieving);

    printf("Scheduler is starting\n");

    MsgQueueId = InitMsgQueue('q');
    ShmAddr = InitShm('m', &SharedMemoryId);
    ShmAddr2 = InitShm('n', &SharedMemoryId2);
    chosenAlgorithm = atoi(argv[1]);

    int quantum = atoi(argv[2]);
    NumOfProcesses = 0;
    CountProcesses = 0;
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
    case 4:
        FCFS();
        break;
    case 5:
        HPFP();
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

    CurrentProcess.LastProcess = false; // to enter the first iteration of the next loop
    *ShmAddr = -1;
    *ShmAddr2 = 0;

    while (*ShmAddr == -1 || CountProcesses > 0)
    {
        traverseQueue(&WaitingQueue, &ReadyQueue, 1);
        if (*ShmAddr2 == 1 || flag == 1)
        {
            flag2 = 1;
            if (*ShmAddr == 0)
            {
                deallocate(CurrentProcess.startIndex, CurrentProcess.nominalSize);
                FinishProcess(&CurrentProcess, ShmAddr);
                push(&processesWTA, CurrentProcess, 0);
                ClockAtFinishing = getClk();
                traverseQueue(&WaitingQueue, &ReadyQueue, 1);
                NumOfProcesses++;
                CountProcesses--;
                *ShmAddr = -1;
                flag = 0;
                *ShmAddr2 = 0;
                flag++;
                flag2 = 0;
                CurrentProcess.LastProcess = false;
                last = getClk();
                nextSecondWaiting(&last);
            }

            if (flag2 == 1)
            {
                if (*ShmAddr == -1 && !isEmpty(ReadyQueue))
                {
                    // pop a new process to run it
                    CurrentProcess = pop(&ReadyQueue);
                    *ShmAddr = CurrentProcess.RunTime;
                    traverseQueue(&WaitingQueue, &ReadyQueue, 1);
                    StartProcess(&CurrentProcess);
                    flag += 1;
                }
            }
        }
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
        flag = 1,
        last = -1;

    *ShmAddr = -1;
    *ShmAddr2 == 1;
    CurrentProcess.RemainingTime = -2; // to prevent it enter the block of code that finishes the process in the start
    CurrentProcess.LastProcess = false;
    last = -1;
    while (*ShmAddr == -1 || CountProcesses > 0)
    {
        traverseQueue(&WaitingQueue, &ReadyQueue, 0);

        if (*ShmAddr2 == 1 || flag == 1)
        {
            *ShmAddr2 = 0;
            CurrentQuantum++;
            // check whether the current process is finished or not
            if (*ShmAddr == 0)
            {
                deallocate(CurrentProcess.startIndex, CurrentProcess.nominalSize);
                FinishProcess(&CurrentProcess, ShmAddr);
                push(&processesWTA, CurrentProcess, 0);
                ClockAtFinishing = getClk();
                NumOfProcesses++;
                CountProcesses--;
                if (isEmpty(ReadyQueue))
                {
                    flag = 1;
                }
                *ShmAddr = -1;
                CurrentQuantum = 0; // reset
                last = getClk();
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
                    if (isEmpty(ReadyQueue))
                        CurrentQuantum = 0;
                    if (!isEmpty(ReadyQueue))
                    {
                        CurrentProcess.RemainingTime = *ShmAddr;
                        StopProcess(&CurrentProcess);
                        push(&ReadyQueue, CurrentProcess, 0);
                    }
                    last = getClk();
                    nextSecondWaiting(&last);
                }

                // switch to another process, or get the first arrived process to work
                if (!isEmpty(ReadyQueue))
                {
                    CurrentProcess = pop(&ReadyQueue);
                    // update the remaining time in the memory
                    *ShmAddr = CurrentProcess.RemainingTime;

                    // check if this is the first time to start or conitue
                    if (CurrentProcess.RemainingTime == CurrentProcess.RunTime)
                    {
                        StartProcess(&CurrentProcess);
                        flag++;
                    }
                    else
                    {
                        ContinueProcess(&CurrentProcess);
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
    *ShmAddr2 == 1;

    int LowestRemainingTime = 0,
        flag = 1,
        last = -1;

    *ShmAddr = -1;
    CurrentProcess.LastProcess = false;

    while (*ShmAddr == -1 || CountProcesses > 0)
    {
        traverseQueue(&WaitingQueue, &ReadyQueue, 2);
        if (*ShmAddr2 == 1 || flag == 1)
        {
            *ShmAddr2 = 0;
            if (*ShmAddr == 0)
            {
                deallocate(CurrentProcess.startIndex, CurrentProcess.nominalSize);
                FinishProcess(&CurrentProcess, ShmAddr);
                push(&processesWTA, CurrentProcess, 0);
                ClockAtFinishing = getClk();
                traverseQueue(&WaitingQueue, &ReadyQueue, 2);
                flag++;
                if (isEmpty(ReadyQueue))
                {
                    flag = 1;
                }

                NumOfProcesses++;
                CountProcesses--;
                *ShmAddr = -1;
                last = getClk();
                nextSecondWaiting(&last);
            }

            if (!isEmpty(ReadyQueue))
            {
                // compare
                LowestRemainingTime = peek(ReadyQueue).RemainingTime;
                if (LowestRemainingTime < *ShmAddr && LowestRemainingTime != -1) // if shared memory is -1 it will not go into it
                {
                    CurrentProcess.RemainingTime = *ShmAddr;
                    StopProcess(&CurrentProcess);
                    push(&ReadyQueue, CurrentProcess, 2);
                    last = getClk();
                    nextSecondWaiting(&last);
                    *ShmAddr = -1;
                }

                if (*ShmAddr == -1)
                {
                    CurrentProcess = pop(&ReadyQueue);
                    *ShmAddr = CurrentProcess.RemainingTime;
                    if (CurrentProcess.RemainingTime == CurrentProcess.RunTime)
                    {
                        traverseQueue(&WaitingQueue, &ReadyQueue, 2);
                        StartProcess(&CurrentProcess);
                        flag++;
                    }
                    else
                    {
                        ContinueProcess(&CurrentProcess);
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------//

/*Function explaintion:-
    This function is the implementation of the Highest Priority First Algorithm,
    it selects the highest priority process of the ready queue and start it until it is finished,
    takes the next highest priority and so on ....
*/
void FCFS()
{
    printf("Start FCFS Algo\n");

    Process CurrentProcess;
    last = -1;
    int flag = 1, flag2 = 1;

    CurrentProcess.LastProcess = false; // to enter the first iteration of the next loop
    *ShmAddr = -1;
    *ShmAddr2 = 0;

    while (*ShmAddr == -1 || CountProcesses > 0)
    {
        traverseQueue(&WaitingQueue, &ReadyQueue, 0);
        if (*ShmAddr2 == 1 || flag == 1)
        {
            flag2 = 1;
            if (*ShmAddr == 0)
            {
                deallocate(CurrentProcess.startIndex, CurrentProcess.nominalSize);
                FinishProcess(&CurrentProcess, ShmAddr);
                push(&processesWTA, CurrentProcess, 0);
                ClockAtFinishing = getClk();
                traverseQueue(&WaitingQueue, &ReadyQueue, 0);
                NumOfProcesses++;
                CountProcesses--;
                *ShmAddr = -1;
                flag = 0;
                *ShmAddr2 = 0;
                flag++;
                flag2 = 0;
                CurrentProcess.LastProcess = false;
                last = getClk();
                nextSecondWaiting(&last);

                if (isEmpty(ReadyQueue))
                {
                    flag = 1;
                }
            }

            if (flag2 == 1)
            {
                if (*ShmAddr == -1 && !isEmpty(ReadyQueue))
                {
                    // pop a new process to run it
                    CurrentProcess = pop(&ReadyQueue);
                    *ShmAddr = CurrentProcess.RunTime;
                    traverseQueue(&WaitingQueue, &ReadyQueue, 0);
                    StartProcess(&CurrentProcess);
                    flag += 1;
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

void HPFP()
{
    printf("HPFP starts\n");

    Process CurrentProcess;
    *ShmAddr2 == 1;

    int LowestPriority = 0,
        flag = 1,
        last = -1;

    *ShmAddr = -1;
    CurrentProcess.LastProcess = false;
    CurrentProcess.Priority = -1;

    while (*ShmAddr == -1 || CountProcesses > 0)
    {
        traverseQueue(&WaitingQueue, &ReadyQueue, 1);
        if (*ShmAddr2 == 1 || flag == 1)
        {
            traverseQueue(&WaitingQueue, &ReadyQueue, 1);
            *ShmAddr2 = 0;
            if (*ShmAddr == 0)
            {
                deallocate(CurrentProcess.startIndex, CurrentProcess.nominalSize);
                FinishProcess(&CurrentProcess, ShmAddr);
                push(&processesWTA, CurrentProcess, 0);
                ClockAtFinishing = getClk();
                CurrentProcess.Priority = -1;
                traverseQueue(&WaitingQueue, &ReadyQueue, 1);
                flag++;
                if (isEmpty(ReadyQueue))
                {
                    flag = 1;
                }
                NumOfProcesses++;
                CountProcesses--;
                *ShmAddr = -1;
                last = getClk();
                nextSecondWaiting(&last);
            }
            if (!isEmpty(ReadyQueue))
            {
                // compare
                traverseQueue(&WaitingQueue, &ReadyQueue, 1);
                LowestPriority = peek(ReadyQueue).Priority;
                if (LowestPriority < CurrentProcess.Priority && LowestPriority != 11) // if shared memory is -1 it will not go into it
                {
                    CurrentProcess.RemainingTime = *ShmAddr;
                    StopProcess(&CurrentProcess);
                    push(&ReadyQueue, CurrentProcess, 1);
                    last = getClk();
                    nextSecondWaiting(&last);
                    *ShmAddr = -1;
                }

                if (*ShmAddr == -1)
                {
                    CurrentProcess = pop(&ReadyQueue);
                    *ShmAddr = CurrentProcess.RemainingTime;
                    if (CurrentProcess.RemainingTime == CurrentProcess.RunTime)
                    {
                        traverseQueue(&WaitingQueue, &ReadyQueue, 1);
                        StartProcess(&CurrentProcess);
                        flag++;
                    }
                    else
                    {
                        ContinueProcess(&CurrentProcess);
                    }
                }
            }
        }
    }
}

/*
Function explaintion:-
    This function clears the resources
*/
void clearResources(int signum)
{
    double std = 0.0;
    std = calculateSD(&processesWTA);
    SchedulerPref(NumOfProcesses, ClockAtFinishing, std);
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
        ReadyProcessExist(MsgQueueId, &WaitingQueue, &flag, 1, &last, &CountProcesses);
        break;
    case 2:
        ReadyProcessExist(MsgQueueId, &WaitingQueue, &flag, 2, &last, &CountProcesses);
        break;
    case 3:
        ReadyProcessExist(MsgQueueId, &WaitingQueue, &flag, 0, &last, &CountProcesses);
        break;
    case 4:
        ReadyProcessExist(MsgQueueId, &WaitingQueue, &flag, 0, &last, &CountProcesses);
        break;
    case 5:
        ReadyProcessExist(MsgQueueId, &WaitingQueue, &flag, 1, &last, &CountProcesses);
        break;
    }
    // just to confirm
    signal(SIGUSR1, checkRecieving);
}

/* linked list functions */


/////////////  memory section /////////////
struct linkedList *freeList[11];


/*
Function explanation:-
    Initializes the array free list that has linked lists, each holding the starting index of the free blocks of that size
    the size is 2^i where i is the index of that list in the array
*/
void buddyInit()
{
    // Create Lists to hold starting indeces of blocks of that size (2^i)
    // Where i is the index of that linked list in the Free List Array
    for (int i = 3; i <= 10; i++)
    {
        freeList[i] = creatLinkedList();
    }
    // Store the largest Portion of memory (RAW, not used at all)
    InsertOrdered(freeList[10], 0);
}
/*
Function explanation:-
    This function splits a block of memory into two smaller blocks of half the size of the original block
    Example: Block 1024 bytes -> Split into two 512 bytes blocks.
*/

void splitMemory(int blockSize, int startIndex)
{
    // First StartIndex
    int insertElement = startIndex + blockSize / 2;

    // Index of the list to insert the split memory in
    int insertIndex = ceil(log2(blockSize)) - 1;

    printf("%f\n", log2(blockSize));
    printf("First Insert Index in split: %d \n", insertIndex);

    // Store right child
    InsertOrdered(freeList[insertIndex], insertElement);

    // Second StartIndex which can be further split
    insertElement = startIndex;

    printf("Second Insert Index in split: %d \n", insertIndex);
    // Store left child
    InsertOrdered(freeList[insertIndex], insertElement);

    // Delete Parent
    DeleteNode(freeList[insertIndex + 1], startIndex);
}
/*
Function explanation:-
    - Allocates memory for a process given its size, and returns the start index of that allocated block
    - If no memory is available for allocation, it returns -1
*/

/*
Assumption: 
    - No Process is smaller than 8 bytes.
*/
int allocate(int size)
{
    int firstNONEmptyIndex = 11;

    int requiredPower = ceil(log2(size));
    if (requiredPower < 3)
    {
        requiredPower = 3;
    }
    int splitCount = requiredPower;

    // To get the first Non Empty list, to start splitting from
    for (int i = requiredPower; i <= 10; i++)
    {
        if (freeList[i] && isempty(freeList[i]))
        {
            splitCount++;
        }
        else
        {
            firstNONEmptyIndex = i;
            break;
        }
    }
    // No match At all (Should Insert in Waiting Queue)   [In Scheduler]
    if (firstNONEmptyIndex == 11)
    {
        printf("\nFailed to Allocate memory for process with size: %d\n", (int)pow(2, requiredPower));
        return -1;
    }

    else if (firstNONEmptyIndex == requiredPower)
    {
        int startIndex = freeList[requiredPower]->head->data;
        DeleteNode(freeList[requiredPower], startIndex);

        printf("\nAllocated %d bytes from memory from Index %d, till %d\n", (int)pow(2, requiredPower), startIndex, (int)pow(2, requiredPower) + startIndex - 1);
      
        return startIndex;
    }
    ///////////////////////////////////////Till Here////////////////////////////////////////////////////////////////

    // Loop Till we Reach the size we want
    else
    {
        for (int i = splitCount; i > requiredPower; i--)
        {
            printf("%d \n", i);
            int startIndex = freeList[i]->head->data;
            int splitSize = pow(2, i);
            printf("%d \n", i);
            printf("size: %d \n", splitSize);
            printf("startIndex: %d \n", startIndex);
            splitMemory(splitSize, startIndex);

            // To get the list to split from, next time
            // i--;
        }

        // delete Node from free list and return the start index to be stored in process.startIndex
        int startIndex = freeList[requiredPower]->head->data;
        DeleteNode(freeList[requiredPower], startIndex);

        printf("\nAllocated %d bytes from memory from Index %d, till %d\n", (int)pow(2, requiredPower), startIndex, (int)pow(2, requiredPower) + startIndex - 1);
        return startIndex;
    }
}
/*
Function Explanation: 
    - Gets a process size and frees its block in memory, then insert in free list.
    - Then merges neigbouring blocks (Buddies).
    - If even index, merge with the block after, if odd index, merge with the block before
*/

void deallocate(int startIndex, int size)
{

    int isEven = 0;
    int insertIndex = ceil(log2(size));
    if (insertIndex < 3)
    {
        insertIndex = 3;
    }
    size = (int)pow(2, insertIndex);

    printf("Inside Deallocate, power is %d, and size = %d\n", insertIndex, size);
    if (insertIndex == 10)
    {
        InsertOrdered(freeList[10], 0);
        printf("\nFreed %d bytes from memory starting from %d till %d\n", size, startIndex, startIndex + size - 1);
        Traverse(freeList[10]);
        printf("\n");
        return;
    }

    if (findNode(freeList[insertIndex], startIndex))
    {
        printf("Sorry, deallocation Failed\n");
    }
    // Check if index is even*memSize or odd*memSize
    else
    {
        while (insertIndex < 10)
        {

            int address = startIndex / size;
            isEven = 1;
            if (address % 2 == 0)
            {

                // Check if next address is present
                int nextAddress = (address + 1) * size;
                printf("NEXT ADDRESS: %d\n", nextAddress);

                if (findNode(freeList[insertIndex], nextAddress) == 1)
                {
                    DeleteNode(freeList[insertIndex], nextAddress);

                    // To delete the one inserted in previous iteration
                    if (findNode(freeList[insertIndex], startIndex) == 1)
                    {
                        DeleteNode(freeList[insertIndex], startIndex);
                    }
                    printf("\nFreed %d bytes from memory starting from %d till %d\n", size, startIndex, startIndex + size - 1);
                    printf("Merged two blocks of size = %d, and start index = %d \n", size, startIndex);
                    InsertOrdered(freeList[insertIndex + 1], startIndex);
                }
                else
                {
                    if (findNode(freeList[insertIndex], startIndex) == 0)
                    {
                        // If next is not present, Just Insert Current then
                        InsertOrdered(freeList[insertIndex], startIndex);
                    }
                    break;
                }
            }

            else
            {
                // Check if prev address is present
                int prevAddress = (address - 1) * size;
                printf("PREV ADDRESS: %d\n", prevAddress);
                if (findNode(freeList[insertIndex], prevAddress) == 1)
                {

                    printf("startIndex: %d, size: %d, insertIndex: %d\n", startIndex, size, insertIndex);
                    DeleteNode(freeList[insertIndex], prevAddress);
                    if (findNode(freeList[insertIndex], startIndex) == 1)
                    {
                        DeleteNode(freeList[insertIndex], startIndex);
                    }
                    printf("\nFreed %d bytes from memory starting from %d till %d\n", size, startIndex, startIndex + size - 1);
                    printf("Merged two blocks of size = %d, and start index = %d \n", size, prevAddress);
                    InsertOrdered(freeList[insertIndex + 1], prevAddress);
                    startIndex = prevAddress;
                }

                else
                {
                    if (findNode(freeList[insertIndex], startIndex) == 0)
                    {

                        printf("Inserted a block of size: %d into free list \n", (int)pow(2, insertIndex));
                        // If next is not present, Just Insert Current then
                        InsertOrdered(freeList[insertIndex], startIndex);
                    }

                    break;
                }
            }
            size *= 2;
            insertIndex++;
        }
    }
    
}

/*
Function Explanation: 
    Gets a process size and checks if there is a free memory block to hold it, by checking the free list.
*/

int isMemoryAvailable(int size)
{
    int requiredPower = ceil(log2(size));
    if (requiredPower < 3)
    {
        requiredPower = 3;
    }
    for (int i = requiredPower; i <= 10; i++)
    {
        if (!isempty(freeList[i]))
        {
            return 1;
        }
    }
    return 0;
}

/*
Function Explanation: 
    Traverses waiting queue, and if any process can be allocated in memory, allocates memory for it, 
    removes from waiting queue, and pushes into ready queue
*/
void traverseQueue(Node **head, Node **ReadyQueue, int flag) // flag : to determine the priority
{
    Process temp;
    Node *previous = *head;
    Node *current = *head;
    Process traversalProcess;
    int requiredPower;
    while (current != NULL)
    {
        // printf("Inside Traverse\n");
        traversalProcess = current->data;

        if (isMemoryAvailable(traversalProcess.nominalSize) == 1)
        {
            traversalProcess.RemainingTime = traversalProcess.RunTime;
            requiredPower = ceil(log2(traversalProcess.nominalSize));
            traversalProcess.actualSize = (int)pow(2, requiredPower);

            printf("Current Process to be allocated has ID %d\n", traversalProcess.Id_2);

            traversalProcess.startIndex = allocate(traversalProcess.nominalSize);
            push(ReadyQueue, traversalProcess, flag);
            MemoryAllocate(&traversalProcess);
            // remove this node from the queue

            if (current == *head)
            {
                *head = current->Next;
                current = *head;
                previous->Next = NULL;
                previous = *head;
            }
            else
            {
                previous->Next = current->Next;
                current->Next = NULL;
                current = previous->Next;
            }

            // current = current->Next;

            if (current != NULL)
            {
                printf("Previous points to process of size%d\n", previous->data.nominalSize);
                printf("Current points to process of size%d\n", current->data.nominalSize);
            }
            else
            {
                printf("current is NULL\n");
                return;
            }
        }
        else
        {
            // printf("No Memory Available\n");
            previous = current;
            // printf("Previous points to process of size%d\n", previous->data.nominalSize);
            current = current->Next;
            if (current == NULL)
            {
                return;
            }
            // printf("Next points to process of size%d\n", current->data.nominalSize);
        }
    }
}

double calculateSD(Node **head)
{
    double sum = 0.0, mean, SD = 0.0;
    int i;
    Node *temp = *head;
    double counter = 0;
    while (temp != NULL)
    {
        sum = sum + temp->data.WTA;
        temp = temp->Next;
        counter++;
    }
    temp = *head;
    mean = sum / counter;

    while (temp != NULL)
    {
        SD = SD + pow(temp->data.WTA - mean, 2);
        temp = temp->Next;
    }
    return sqrt(SD / counter);
}
