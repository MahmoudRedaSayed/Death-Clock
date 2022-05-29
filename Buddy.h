// #include "Helpers.c"
// #include <math.h>
// int maxSize = 10;
// int minSize =3;
// struct linkedList * freeList[11];

// void buddyInit()
// {
//     //Create Lists to hold starting indeces of blocks of that size (2^i)
//     //Where i is the index of that linked list in the Free List
//     for(int i =  3; i<=10; i++)
//     {
//         freeList[i] = creatLinkedList();
//     }
//     //Store the largest Portion of memory (RAW, not used at all)
//     InsertOrdered(freeList[10], 0);
// }

// void splitMemory(int blockSize, int startIndex)
// {
//     //First StartIndex
//     int insertElement = startIndex+blockSize/2;

//     //Index of the list to insert the split memory in
//     int insertIndex = ceil(log2(blockSize))-1;

//     printf("%f\n", log2(blockSize));
//     printf("First Insert Index in split: %d \n", insertIndex);
    
//     //Store right child
//     InsertOrdered(freeList[insertIndex], insertElement);
    
//     //Second StartIndex which can be further split
//     insertElement = startIndex;

//     printf("First Insert Index in split: %d \n", insertIndex);
//     //Store left child
//     InsertOrdered(freeList[insertIndex], insertElement);

//     //Delete Parent
//     DeleteNode(freeList[insertIndex+1], startIndex);

// }

// //Return process.startIndex if it's not -1
// int allocate(int size)
// {
//     int firstNONEmptyIndex = 11;
    
//     int requiredPower = ceil(log2(size));
//     if(requiredPower<3) {
//         requiredPower = 3;
//     }
//     int splitCount = requiredPower;

//     //To get the first Non Empty list, to start splitting from
//     for (int i = requiredPower; i<=10; i++)
//     {
//         if(freeList[i] && isempty(freeList[i]))
//         {
//             splitCount++;
//         }
//         else
//         {
//             firstNONEmptyIndex = i;
//             break;
//         }
//     }
//     //No match At all (Should Insert in Waiting Queue)   [In Scheduler]
//     if(firstNONEmptyIndex == 11)
//     {
//         printf("\nFailed to Allocate memory for process with size: %d\n", (int)pow(2,requiredPower));
//         return -1;
//     }


//     ////////////////////////////////////That Section will probably be deleted./////////////////////////////////////////////////////////
//     // Exact Fit, no need for Loop (Can Do It With Loop Probably in Else Condition {will not enter loop and thus execute the section below})
//     else if (firstNONEmptyIndex == requiredPower)
//     {
//         int startIndex = freeList[requiredPower]->head->data;
//         DeleteNode(freeList[requiredPower], startIndex);


//         printf("\nAllocated %d bytes from memory from Index %d, till %d\n", (int)pow(2,requiredPower), startIndex, (int)pow(2,requiredPower)+startIndex-1);
//         //To be Stored in process.startIndex
//         //[Together with process.actualSize will make us not need map for indexes and sizes]
//         return startIndex;       
        
//     }
//     ///////////////////////////////////////Till Here////////////////////////////////////////////////////////////////


//     //Loop Till we Reach the size we want
//     else
//     {
//         for(int i=splitCount; i>requiredPower;i--)
//         {
//             printf("%d \n", i);
//             int startIndex = freeList[i]->head->data;
//             int splitSize = pow(2,i);
//             printf("%d \n", i);
//             printf("size: %d \n", splitSize);
//             printf("startIndex: %d \n", startIndex);
//             splitMemory(splitSize, startIndex);

            
//             //To get the list to split from, next time
//             //i--;
//         }

//         // delete Node from free list and return the start index to be stored in process.startIndex
//              printf("Hassan\n");
//              printf("DATA: %d\n", freeList[requiredPower]->head->data);
//         int startIndex = freeList[requiredPower]->head->data;
//         DeleteNode(freeList[requiredPower], startIndex);

//         printf("\nAllocated %d bytes from memory from Index %d, till %d\n", (int)pow(2,requiredPower), startIndex, (int)pow(2,requiredPower)+startIndex-1);
//         return startIndex; 
//     }   
// }

// //Gets Actual size of Process, and its startIndex
// //process.memSize should have the actual size not the given one in file?? 
// //(or can have another data member process.actualSize and send this to deallocate)
// void deallocate(int startIndex, int size)
// {
//     int insertIndex = ceil(log2(size));
//     int i = 10;
    

//     if(findNode(freeList[insertIndex], startIndex))
//     {
//         printf("Sorry, deallocation Failed\n");
//     }


//     //Check if index is even*memSize or odd*memSize
//     else
//     {


//         int address = startIndex/size;
//         if(address%2 ==0)
//         {
//             //Check if next address is present
//             int nextAddress = (address+1)*size;
//             printf("NEXT ADDRESS: %d\n", nextAddress);

//             if(findNode(freeList[insertIndex],nextAddress) ==1 )
//             {
//                 merge(startIndex,size, insertIndex, 1);   
//             }
//             else
//             {
//                 //If next is not present, Just Insert Current then
//                 InsertOrdered(freeList[insertIndex], startIndex);
//             }
//         }
//         else 
//         {
//             //Check if prev address is present
//             int prevAddress = (address-1)*size;
//             printf("PREV ADDRESS: %d\n", prevAddress);
//             if(findNode(freeList[insertIndex], prevAddress) ==1)
//             {

//                 printf("startIndex: %d, size: %d, insertIndex: %d\n", startIndex, size, insertIndex);
//                 merge(startIndex,size, insertIndex, 0);   
//             }

//             else
//             {
//                 printf("Inserted a block of size: %d into free list \n", (int)pow(2,insertIndex));
//                 //If next is not present, Just Insert Current then
//                 InsertOrdered(freeList[insertIndex], startIndex);
//             }
//         }

//         printf("\nFreed %d bytes from memory starting from %d till %d\n", size, startIndex, startIndex+size-1);
//     }
    
// }


// void buddyDestroy()
// {
//     //Free Head of Last index and
//     DeleteNode(freeList[10], 0);
//     for (int i = 3; i <= 10; i++)
//     {
//         //Free each Linked List
//         free(freeList[i]);
//     }
    
// }

// void merge(int startIndex, int size, int freeListIndex, int isEven)
// {
//     int address = startIndex/size;
//     if(isEven)
//     {
//         int nextAddress = (address+1)*size;
//         printf("Next Address is: %d, listIndex is : %d, size is: %d\n", nextAddress, freeListIndex, size);
//         if(findNode(freeList[freeListIndex],nextAddress) ==1 )
//         {
//             //If present, MERGE -> By deleting that address and inserting in the freeList Above
//             DeleteNode(freeList[freeListIndex],nextAddress);

            
//             InsertOrdered(freeList[freeListIndex+1], startIndex);
//             size *=2;
//             address = startIndex/size;
//             if(address%2==0)
//             {
                
//                 merge(startIndex, size, freeListIndex+1, 1);
//                 printf("Merged Block starting from: %d, of size %d\n", startIndex, size);
//             }
//             else
//             {
//                 merge(startIndex, size,freeListIndex+1,0);
//                 printf("Merged Block starting from: %d, of size %d\n", startIndex, size);
//             }
//         }
//         else
//         {
//             //If next is not present, Just Insert Current then
//             InsertOrdered(freeList[freeListIndex], startIndex);
//         }
//     }
//     else 
//     {
//             //Check if next address is present
//             int prevAddress = (address-1)*size;
//             if(findNode(freeList[freeListIndex], prevAddress) ==1)
//             {
//                 //If present, MERGE -> By deleting that address and inserting in the freeList Above
//                 DeleteNode(freeList[freeListIndex],prevAddress);
//                 InsertOrdered(freeList[freeListIndex+1], prevAddress);

//                 startIndex = prevAddress;
//                 size *=2;
//                 address = startIndex/size;
//                 printf("startIndex: %d, size: %d\n", startIndex,size);
//                 if(address%2==0)
//                 {
//                     merge(startIndex, size, freeListIndex+1, 1);
//                     printf("Merged Block starting from: %d, of size %d\n", startIndex, size);
//                 }
//                 else
//                 {
//                     merge(startIndex, size,freeListIndex+1,0);
//                     printf("Merged Block starting from: %d, of size %d\n", startIndex, size);
//                 }
//             }

//             else
//             {
//                 //If next is not present, Just Insert Current then
//                 InsertOrdered(freeList[freeListIndex], startIndex);
//             }
//     }
// }