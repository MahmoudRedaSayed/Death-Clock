#include "Helpers.c"

/******************************* Global Variables *******************************/
int msgQueueId,
    sharedMemoryId;


void clearResources(int);

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);

    // 1. Read the input files.
    FILE* InputFile = fopen("processes.txt", "r");  // open file for read

    if (!InputFile)
        perror("Can't open the input file");

    char* Line = NULL;
    int LineSize = 0,
        CountProcesses = 0;
    
    // get the number of processes
    while (getline(&Line, &LineSize, InputFile) != -1)
        if (Line[0] != '#') // check if the line is commented or not
            CountProcesses++;   

    // close the input file
    fclose(InputFile);

    Process* AllProcesses = malloc(CountProcesses * sizeof(Process));
    int LastArrialTime = 0;

    // store the input data
    fopen("processes.txt", "r");
    int i = 0;
    Process input;
    while (getline(&Line, &LineSize, InputFile) != -1)
    {
        fscanf(InputFile,"%d", &input.Id_2);
        fscanf(InputFile,"%d", &input.ArriavalTime);
        fscanf(InputFile,"%d", &input.RunTime);
        fscanf(InputFile,"%d", &input.Priority);
        fscanf(InputFile,"%d", &input.nominalSize);
        printf("input %d\n", input.Id_2);
        AllProcesses[i].WaitingTime = 0;
        AllProcesses[i].RemainingTime = AllProcesses[i].RunTime;
        AllProcesses[i].LastProcess = false;

        if (input.RunTime != 0 )
        {
            AllProcesses[i] = input;
        }
        else
        {
            CountProcesses--;
            continue;
        }
        
        
        if (i == CountProcesses - 1)        // check if the last process or not
            {
                input.LastProcess = true;
                AllProcesses[i] = input;
            }
        
        i++;
    }
    fclose(InputFile);
    
    for (int i = 0; i < CountProcesses; i++)
    {
        AllProcesses[i].RemainingTime = AllProcesses[i].RunTime;
        printf("run : %d, rem: %d\n", AllProcesses[i].RunTime, AllProcesses[i].RemainingTime);
    }
    
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    char args[3][2];
    printf("Choose the algorithm :\n");
    printf("1-HPF\n2-SRTN\n3-RR\n4-FCFS\n5-HPFP\n");
    scanf("%s", args[0]);
    // validate the input
    while (atoi(args[0]) < 1 || atoi(args[0]) > 5)
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
        RunAndComplie("clk", NULL, NULL, NULL, NULL);
    else if (clkID == -1)
    {
        perror("Failed to fork the clock process");
        exit(-1);
    }

    // run the scheduler process
    char numOfProcesses[2];
    sprintf(numOfProcesses, "%d", CountProcesses);      // convert int to string
    char lastArriveTime[2];
    sprintf(lastArriveTime, "%d",LastArrialTime);      // convert int to string

    int SchedulerID = fork();

    if (SchedulerID == 0)
        RunAndComplie("scheduler", args[0], args[1], numOfProcesses, lastArriveTime);
    else if (SchedulerID == -1)
    {
        perror("Failed to fork the scheduler process");
        exit(-1);
    }

    // 4. Initialize clock
    initClk();

    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    int time;
    int sendCounter = 0;
    while (1)
    {
        time = getClk();
         
        for (int i = sendCounter; i < CountProcesses; i++)
        {
            if (AllProcesses[i].ArriavalTime == time)
            {
                SendMsg(msgQueueId, AllProcesses[i]);
                kill(SchedulerID, SIGUSR1);
                sendCounter++;
                AllProcesses[i].ArriavalTime = -1;
            }else break;

            
        }
        while (time == getClk())
        ;
    }
    
    // 7. Clear clock resources
    msgctl(msgQueueId, IPC_RMID, (struct msqid_ds *)0);
    shmctl(sharedMemoryId, IPC_RMID, NULL);
    //destroyClk(true);
}

void clearResources(int signum)
{
    msgctl(msgQueueId, IPC_RMID, (struct msqid_ds *)0);
    shmctl(sharedMemoryId, IPC_RMID, NULL);
    signal(SIGINT, SIG_DFL);
}
