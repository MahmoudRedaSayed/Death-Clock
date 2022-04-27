#include "Helpers.c"
void clearResources(int);
void tostring(char str[], int num);

/*------------------   Global Variables -----------------------*/
int msgQueueId,
    sharedMemoryId;

/*------------------  End Global Variables ----------------------*/

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.

    FILE* inputFile = fopen("processes.txt", "r");  // open file for read
    char* line = NULL;
    int lineSize = 0,
        countProcesses = 0;
    
    if (!inputFile)
        perror("Can't open the input file");
    

    // get the number of processes
    while (getline(&line, &lineSize, inputFile) != -1)
        // check if the line is commented or not
        if (line[0] != '#') countProcesses++;

    // close the input file
    fclose(inputFile);

    Process* allProcesses = malloc(countProcesses * sizeof(Process));

    // store the input data
    fopen("processes.txt", "r");
    int i = 0;
    while (getline(&line, &lineSize, inputFile) != -1)
    {
        fscanf(inputFile,"%d", &allProcesses[i].id);
        fscanf(inputFile,"%d", &allProcesses[i].arriavalTime);
        fscanf(inputFile,"%d", &allProcesses[i].runTime);
        fscanf(inputFile,"%d", &allProcesses[i].priority);

        allProcesses[i].running = 0;
        allProcesses[i].waitingTime = 0;
        allProcesses[i].remainingTime = allProcesses[i].runTime;
        allProcesses[i].lastProcess = false;

        if (i == countProcesses - 1)
            allProcesses[i].lastProcess = true;
        
        i++;

        
    }
    fclose(inputFile);

    // test input
    // for (int i = 0; i < countProcesses; i++)
    // {
    //     printf("Process %i:\n", i );
    //     printf("id :%i, arriavalTime: %i, runTime: %i, priority : %i\n", allProcesses[i].id, allProcesses[i].arriavalTime, allProcesses[i].runTime, allProcesses[i].priority);
    // }
    
    
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    char args[3][2];
    printf("Choose the algorithm :\n");
    printf("1-HPF\n2-SRTN\n3-RR\n");
    scanf("%s", args[0]);
    // validate the input
    while (atoi(args[0]) < 1 || atoi(args[0]) > 3)
    {
        printf("Wrong Choice, try again\n");
        scanf("%s", args[0]);
    }
    // if chosed RR, ask him for the quantum
    if (atoi(args[0]) == 3)   // RR
    {
        scanf("%s", args[1]);       // get the quantum

        while (atoi(args[1]) <= 0)
        {
            printf("Enter a valid quantum, please!\n");
            scanf("%s", args[1]);
        }
    }else 
        strcpy(args[1], "0");
     
    // 3. Initiate and create the scheduler and clock processes.
    msgQueueId     = InitMsgQueue('q');
    int *shmAddr   = InitShm('m', &sharedMemoryId);

    // run the clock process
    char* clkArgs[2];
    clkArgs[0] = NULL;
    clkArgs[1] = NULL;
    int clkID = fork();
    if (clkID == 0)
        RunAndComplie("clk", NULL, NULL, NULL);
    else if (clkID == -1)
    {
        perror("Failed to fork the clock process");
        exit(-1);
    }
    // run the scheduler process
    char numOfProcesses[2];
    sprintf(numOfProcesses, "%d", countProcesses);      // convert int to string
    //printf("%s\n", numOfProcesses);exit(-1);

    int schedulerID = fork();
    if (schedulerID == 0)
        RunAndComplie("scheduler", args[0], args[1], numOfProcesses);
    else if (schedulerID == -1)
    {
        perror("Failed to fork the scheduler process");
        exit(-1);
    }

    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.

    int time;
    bool done = false;
    while (! done)
    {
         time = getClk();
        for (int i = 0; i < countProcesses; i++)
        {
            if (allProcesses[i].arriavalTime == time)
                SendMsg(msgQueueId, allProcesses[i]);

            if (allProcesses[i].lastProcess)
                done = true;
        } 
    }

    // 7. Clear clock resources
    msgctl(msgQueueId, IPC_RMID, (struct msqid_ds *)0);
    shmctl(sharedMemoryId, IPC_RMID, NULL);
    destroyClk(true);
}

void clearResources(int signum)
{
    msgctl(msgQueueId, IPC_RMID, (struct msqid_ds *)0);
    shmctl(sharedMemoryId, IPC_RMID, NULL);
    signal(SIGINT, SIG_DFL);
}
