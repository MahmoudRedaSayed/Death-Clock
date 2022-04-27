#include <stdio.h>
#include <stdlib.h>
#include"Helpers.c"
 struct Node 
{ 
        Node * Next;
        Process data; 
};
typedef struct Node Node; 
 
Node* createQueue(Process Data) 
{  
        Node* temp = (Node*)malloc(sizeof(Node));
        temp->data = Data;
        temp->next = NULL;
        return temp;
} 
void Dequeue(Node *Q) 
{ 
        /* If Queue size is zero then it is empty. So we cannot pop */ 
        if(Q==NULL) 
        { 
                printf("Queue is Empty n"); 
                return; 
        } 
        else 
        { 
                Q=Q->Next;
        } 
        return; 
} 
Process front(Node*Q) 
{ 
        if(Q==NULL) 
        { 
                printf("Queue is Empty n"); 
                return NULL;
        } 
        /* Return the element which is at the front*/ 
        return Q->data; 
} 
void Enqueue(Node*Q,Process element) 
{ 
        Node * temp=Q;
        while(temp->Next==NULL)
        {
                temp=temp->Next;
        }
        temp->Next=(Node*)malloc(sizeof(Node));
        temp->Next->data=element;
}