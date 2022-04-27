#include <stdio.h>
#include <stdlib.h>

#define QUEUE_TEMPLATE(ELTTYPE) \
typedef struct \
{\
        int capacity;\
        int size;\
        int front;\
        int rear;\
        ELTTYPE *elements;\
}Queue_##ELTTYPE;\
\
Queue_##ELTTYPE * createQueue_##ELTTYPE(int maxElements)\
{\
        /* Create a Queue */\
        Queue_##ELTTYPE *Q;\
        Q = (Queue_##ELTTYPE *)malloc(sizeof(Queue_##ELTTYPE));\
        /* Initialise its properties */\
        Q->elements = malloc(sizeof(ELTTYPE)*maxElements);\
        Q->size = 0;\
        Q->capacity = maxElements;\
        Q->front = 0;\
        Q->rear = -1;\
        /* Return the pointer */\
        return Q;\
}\
void Dequeue_##ELTTYPE(Queue_##ELTTYPE *Q)\
{\
        /* If Queue size is zero then it is empty. So we cannot pop */\
        if(Q->size==0)\
        {\
                printf("Queue is Empty\n");\
                return;\
        }\
        /* Removing an element is equivalent to incrementing index of front by one */\
        else\
        {\
                Q->size--;\
                Q->front++;\
                /* As we fill elements in circular fashion */\
                if(Q->front==Q->capacity)\
                {\
                        Q->front=0;\
                }\
        }\
        return;\
}\
ELTTYPE front_##ELTTYPE(Queue_##ELTTYPE *Q)\
{\
        if(Q->size==0)\
        {\
                printf("Queue is Empty\n");\
                exit(0);\
        }\
        /* Return the element which is at the front*/\
        return Q->elements[Q->front];\
}\
void Enqueue_##ELTTYPE(Queue_##ELTTYPE *Q,ELTTYPE element)\
{\
        /* If the Queue is full, we cannot push an element into it as there is no space for it.*/\
        if(Q->size == Q->capacity)\
        {\
                printf("Queue is Full\n");\
        }\
        else\
        {\
                Q->size++;\
                Q->rear++;\
                /* As we fill the queue in circular fashion */\
                if(Q->rear == Q->capacity)\
                {\
                        Q->rear = 0;\
                }\
                /* Insert the element in its rear side */ \
                Q->elements[Q->rear] = element;\
        }\
        return;\
}

QUEUE_TEMPLATE(int);
QUEUE_TEMPLATE(float);

int main()
{
        Queue_int *Q = createQueue_int(5);
        Queue_float *QF = createQueue_float(5);
 
        Enqueue_int(Q,1);
        Enqueue_int(Q,2);
        Enqueue_int(Q,3);
        Enqueue_int(Q,4);
        printf("Front element is %d\n",front_int(Q));
        Enqueue_int(Q,5);
        Dequeue_int(Q);
        Enqueue_int(Q,6);
        printf("Front element is %d\n",front_int(Q));

        Enqueue_float(QF,1);
        Enqueue_float(QF,2);
        Enqueue_float(QF,3);
        Enqueue_float(QF,4);
        printf("Front element is %f\n",front_float(QF));
        Enqueue_float(QF,5);
        Dequeue_float(QF);
        Enqueue_float(QF,6);
        printf("Front element is %f\n",front_float(QF));
}