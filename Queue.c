#include <stdio.h>
#include <stdlib.h>
#include"Helpers.c"
#define QUEUE_TEMPLATE(ELTTYPE)  
 struct Node 
{ 
        int capacity; 
        int size; 
        int front; 
        int rear; 
        ELTTYPE *elements; 
};
typedef struct Node Node; 
 
Node* createQueue(int maxElements) 
{
        /* Create a Queue */ 
        Node *Q; 
        Q = (Node*)malloc(sizeof(Node)); 
        /* Initialise its properties */ 
        Q->elements = malloc(sizeof(Node)*maxElements); 
        Q->size = 0; 
        Q->capacity = maxElements; 
        Q->front = 0; 
        Q->rear = -1; 
        /* Return the pointer */ 
        return Q; 
} 
void Dequeue(Node *Q) 
{ 
        /* If Queue size is zero then it is empty. So we cannot pop */ 
        if(Q->size==0) 
        { 
                printf("Queue is Empty n"); 
                return; 
        } 
        /* Removing an element is equivalent to incrementing index of front by one */ 
        else 
        { 
                Q->size--; 
                Q->front++; 
                /* As we fill elements in circular fashion */ 
                if(Q->front==Q->capacity) 
                { 
                        Q->front=0; 
                } 
        } 
        return; 
} 
Process front(Node*Q) 
{ 
        if(Q->size==0) 
        { 
                printf("Queue is Empty n"); 
                exit(0); 
        } 
        /* Return the element which is at the front*/ 
        return Q->elements[Q->front]; 
} 
void Enqueue(Node*Q,ELTTYPE element) 
{ 
        /* If the Queue is full, we cannot push an element into it as there is no space for it.*/ 
        if(Q->size == Q->capacity) 
        { 
                printf("Queue is Full n"); 
        } 
        else 
        { 
                Q->size++; 
                Q->rear++; 
                /* As we fill the queue in circular fashion */ 
                if(Q->rear == Q->capacity) 
                { 
                        Q->rear = 0; 
                } 
                /* Insert the element in its rear side */  
                Q->elements[Q->rear] = element; 
        } 
        return; 
}

