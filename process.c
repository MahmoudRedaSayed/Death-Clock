#include "Helpers.c"

int main(int agrc, char * argv[])
{
    initClk();
    printf("process starts\n");
    int last = -1; // -1 => just to enter in the first time
    int ShmId = 0;
    int *RemainingTime = (int *)InitShm('m', &ShmId);

    while (*RemainingTime > 0)
    {
        while (last==getClk())
        ;
        if(last!=getClk())
        {
            *RemainingTime -= 1;
            last=getClk();
        }
    }
    
    // destroyClk(false);
    return 0;
}
