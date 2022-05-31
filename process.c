#include "Helpers.c"

int main(int agrc, char * argv[])
{
    initClk();
    printf("process starts , clk : %d\n", getClk());
    
    int last = -1; // -1 => just to enter in the first time
    int ShmId = 0;
    int ShmId2 = 0;
    int *RemainingTime = (int *)InitShm('m', &ShmId);
    int *ShmAdrr2 = (int *)InitShm('n', &ShmId2);
    *ShmAdrr2 = 0;

    while (*RemainingTime > 0)
    {
        while (last==getClk())
        ;
        if(last!=getClk())
        {
            *RemainingTime -= 1;
            last=getClk();
        }
        *ShmAdrr2 = 1;
    }
    
    // destroyClk(false);
    return 0;
}
