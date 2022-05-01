#include <stdio.h>
#include <stdlib.h>
#include"Helpers.c"
struct Node 
{ 
        Node * Next;
        Process data; 
};
typedef struct Node Node;

void push(Node* q, Process p, int PriorityFlag)         // PriorityFlag:1 ( priority is on)
{
        if (PriorityFlag != 1)
        {
                if (q == NULL)
                {
                        q = (Node*)malloc(sizeof(Node));
                        q->data = p;
                }else
                {
                        Node* temp = q;
                while (temp->Next != NULL)
                {
                        temp = temp->Next;
                }
                        temp->Next = (Node*)malloc(sizeof(Node));
                        temp->Next->data = p;   
                }  
        }else
        {
        if (q == NULL)
                {
                        q = (Node*)malloc(sizeof(Node));
                        q->data = p;
                }else
                {
                        if (p.priority < q->data.priority)
                        {
                                Node* temp = (Node*)malloc(sizeof(Node));
                                temp->data = p;
                                temp->Next = q;
                                q = temp;
                        }
                        else
                        {
                                Node*temp = q;
                                Node* prev=temp;
                                while (! temp->data.priority > p.priority)
                                {       prev = temp;
                                        temp = temp->Next;
                                }
                                Node* inserted = (Node*)malloc(sizeof(Node));
                                inserted->data = p;
                                inserted->Next = temp;
                                prev->Next = inserted;
                        }     
                }  
        }
}

Process* pop(Node* q)
{
        if (q != NULL)
        {
                Process obj = q->data;
                q = q->Next;
                return &obj;
        }else
        {
                return NULL;
        }
        
}

bool isEmpty(Node* q)
{
        return q == NULL;
}






















 
// Node* createQueue(Process Data) 
// {  
//         Node* temp = (Node*)malloc(sizeof(Node));
//         temp->data = Data;
//         temp->next = NULL;
//         return temp;
// } 
// void Dequeue(Node *Q) 
// { 
//         /* If Queue size is zero then it is empty. So we cannot pop */ 
//         if(Q==NULL) 
//         { 
//                 printf("Queue is Empty n"); 
//                 return; 
//         } 
//         else 
//         { 
//                 Q=Q->Next;
//         } 
//         return; 
// } 
// Process front(Node*Q) 
// { 
//         if(Q==NULL) 
//         { 
//                 printf("Queue is Empty n"); 
//                 return NULL;
//         } 
//         /* Return the element which is at the front*/ 
//         return Q->data; 
// } 
// void Enqueue(Node*Q,Process element) 
// { 
//         Node * temp=Q;
//         while(temp->Next==NULL)
//         {
//                 temp=temp->Next;
//         }
//         temp->Next=(Node*)malloc(sizeof(Node));
//         temp->Next->data=element;
// }