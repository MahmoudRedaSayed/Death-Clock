#include "Helpers.c"

/* Modify this file as needed*/
int * remainingtime;
int * shmid;
int last=-1;

int main(int agrc, char * argv[])
{
    initClk();
    remainingtime=(int *)InitShm("m",shmid);
    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    while (*remainingtime >= 0)
    {
        while (last==getClk());
        if(last!=getClk())
        {
            *remainingtime--;
            last=getClk();
        }
        
    }
    
    destroyClk(false);
    
    return 0;
}
