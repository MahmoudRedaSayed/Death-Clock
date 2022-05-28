#include "Helpers.c"

int main(int agrc, char * argv[])
{
    initClk();
    printf("process starts , clk : %d\n", getClk());
    union Semun semun;

    int sem1 = semget('m', 1, 0666 | IPC_CREAT);
    semun.val = 0; /* initial value of the semaphore, Binary semaphore */
    if (semctl(sem1,/* semnum */ 0, SETVAL, semun) == -1)       // semnum = 0, because we have only one here so it is the first index
                                                                // and this is just initialization
    {
        perror("Error in semctl");
        exit(-1);
    }
    int last = -1; // -1 => just to enter in the first time
    int ShmId = 0;
    int ShmId2 = 0;
    int *RemainingTime = (int *)InitShm('m', &ShmId);
    int *ShmAdrr2 = (int *)InitShm('n', &ShmId2);
    *ShmAdrr2 = 0;
    sem1=0;
    while (*RemainingTime > 0)
    {
        while (last==getClk())
        ;
        if(last!=getClk())
        {
            *RemainingTime -= 1;
            last=getClk();
        }
        printf("semBeforUP:%d\n", sem1);
        *ShmAdrr2 = 1;
    }
    
    // destroyClk(false);
    return 0;
}
