#include <stdio.h>
#include <stdlib.h>
#include"Helpers.c"
 
 struct Node
{
    Process data;
    struct node* next;
} ;
typedef struct Node Node;

// Function to Create A New Node
Node* newNode(Process d)
{
    Node* temp = (Node*)malloc(sizeof(Node));
    temp->data = d;
    temp->next = NULL;
    return temp;
}
 
// Return the value at head
Process peek(Node** head)
{
    return (*head)->data;
}
 
// Removes the element with the
// highest priority form the list
void pop(Node** head)
{
    Node* temp = *head;
    (*head) = (*head)->next;
    free(temp);
}
 
// Function to push according to priority
void push(Node** head, Process d)
{
    Node* start = (*head);
 
    // Create new Node
    Node* temp = newNode(d);
 
    // Special Case: The head of list has lesser
    // priority than new node. So insert new
    // node before head node and change head node.
    if ((*head)->data->priority > temp->data->priority) {
 
        // Insert New Node before head
        temp->next = *head;
        (*head) = temp;
    }
    else {
 
        // Traverse the list and find a
        // position to insert new node
        while (start->next != NULL &&
            start->next->data->priority < temp->data->priority) {
            start = start->next;
        }
 
        // Either at the ends of the list
        // or at required position
        temp->next = start->next;
        start->next = temp;
    }
}
 
// Function to check is list is empty
int isEmpty(Node** head)
{
    return (*head) == NULL;
}
 
