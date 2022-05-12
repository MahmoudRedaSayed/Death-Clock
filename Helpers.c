#include "headers.h"

/******************************* Global Variables *******************************/
FILE *OutputFile;

/******************************* Start Data Structures *******************************/
struct Node
{
	struct Node *Next;
	Process data;
};
typedef struct Node Node;

/*
Function explaintion:-
	This function is used to push a process into the queue
	Function Parameters:-
		1- q : the head node of the queue
		2- p: the process to be pushed
		3- PriorityFlag : this flag to determine which the priority for,
						  if flag : 0  ( no priority, acts as a normal queue FCFS)
						  if flag : 1  ( Priority on the the process priority)
						  if flag : 2  ( Priority on the the process remaining time)

*/
void push(Node **q, Process p, int PriorityFlag) // PriorityFlag:1 ( priority is on) , PriorityFlag:0 ( priority is off), PriorityFlag:2 ( priority is remaining Time)
{
	if (PriorityFlag == 0) // act as a normal queue
	{
		if (*q == NULL) // if the queue is empty
		{
			*q = (Node *)malloc(sizeof(Node));
			(*q)->data = p;
			(*q)->Next = NULL;
		}
		else
		{
			Node *temp = *q;
			while (temp->Next != NULL)
			{
				temp = temp->Next;
			}
			temp->Next = (Node *)malloc(sizeof(Node));
			temp->Next->Next = NULL;
			temp->Next->data = p;
		}
	}
	else if (PriorityFlag == 1) // Priority on the the process priority
	{
		if (*q == NULL) // if the queue is empty
		{
			*q = (Node *)malloc(sizeof(Node));
			(*q)->data = p;
			(*q)->Next = NULL;
		}
		else
		{
			if (p.ArriavalTime == (*q)->data.ArriavalTime)
			{
				// check priorities
				if (p.Priority <= (*q)->data.Priority)
				{
					Node *temp = (Node *)malloc(sizeof(Node));
					temp->data = p;
					temp->Next = *q;
					*q = temp;
				}
				else
				{
					Node *temp = *q;
					Node *prev = temp;
					while (temp != NULL && (p.ArriavalTime == temp->data.ArriavalTime && p.Priority < temp->data.Priority))
					{
						prev = temp;
						temp = temp->Next;
					}
					Node *inserted = (Node *)malloc(sizeof(Node));
					prev->Next = inserted;
					inserted->Next = temp;
				}
			}
			else if (p.ArriavalTime > (*q)->data.ArriavalTime)
			{
				Node *temp = *q;
				Node *prev = temp;

				while (temp != NULL && p.ArriavalTime > temp->data.ArriavalTime)
				{
					prev = temp;
					temp = temp->Next;
				}

				if (temp == NULL) // then, insert in the last of the queue
				{
					Node *inserted = (Node *)malloc(sizeof(Node));
					inserted->data = p;
					prev->Next = inserted;
					inserted->Next = NULL;
				}
				else // check if there is a process with the same priority or not
				{
					while (temp != NULL && p.Priority > temp->data.Priority)
					{
						prev = temp;
						temp = temp->Next;
					}
					// here, we got the right place to insert
					Node *inserted = (Node *)malloc(sizeof(Node));
					inserted->data = p;
					prev->Next = inserted;
					inserted->Next = temp;
				}
			}
		}
	}

	else if(PriorityFlag == 2)		// priority is on the remaining time
	{
		if (*q == NULL)
		{
			*q = (Node *)malloc(sizeof(Node));
			(*q)->data = p;
			(*q)->Next = NULL;
		}
		else
		{

			// check remaining time
			if (p.RemainingTime < (*q)->data.RemainingTime)
			{
				Node *temp = (Node *)malloc(sizeof(Node));
				temp->data = p;
				temp->Next = *q;
				*q = temp;
			}
			else
			{
				Node *temp = *q;
				Node *prev = temp;
				while (temp != NULL && p.RemainingTime >= temp->data.RemainingTime)
				{
					prev = temp;
					temp = temp->Next;
				}
				Node *inserted = (Node *)malloc(sizeof(Node));
				inserted->data = p;
				prev->Next = inserted;
				inserted->Next = temp;
			}
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------//
/*
Function explaintion:-
	This function is used to remove the first process from the queue
	Function Parameters:-
		1- q : the head node of the queue
*/
Process pop(Node **q)
{
	if (*q != NULL)
	{
		Process obj = (*q)->data;
		*q = (*q)->Next;
		return obj;
	}
	else // reutrn any dummy process
	{
		Process p;
		p.ArriavalTime = -1; // indicator
		return p;
	}
}
//------------------------------------------------------------------------------------------------------------------------//
/*
Function explaintion:-
	This function is used to get the first process from the queue
	Function Parameters:-
		1- q : the head node of the queue
*/
Process peek(Node *q)
{
	if (q != NULL)
	{
		return q->data;
	}
	// return any dummy process
	Process p;
	p.ArriavalTime = -1;
	return p;
}
//------------------------------------------------------------------------------------------------------------------------//
/*
Function explaintion:-
	This function is used to check whether the queue is empty or not
	Function Parameters:-
		1- q : the head node of the queue
*/
bool isEmpty(Node *q)
{
	return q == NULL;
}

/******************************* End Data Structures *******************************/

/******************************* Some Important Functions *******************************/

/*
Function explaintion:-
	This function is used to complie the file and run it and it will be used to create the processes also
	and run the schdular file and the clk file
	Function Parameters:-
		1-File Name :the name of the file that will be complied and run
		2-args: the needed arguments to put the parameter if exist in it that will be sent to the file
*/
void RunAndComplie(char *FileName, char *arg_0, char *arg_1, char *arg_2, char *arg_3)
{
	char *Complie;
	Complie = (char *)malloc((20 + 2 * sizeof(FileName) * sizeof(char)));
	strcpy(Complie, "gcc ");
	strcat(Complie, FileName);
	strcat(Complie, ".c ");
	strcat(Complie, "-o ");
	strcat(Complie, FileName);
	strcat(Complie, ".out");
	system(Complie);
	char FileNameOut[80];
	strcpy(FileNameOut, FileName);
	printf("\n");
	strcat(FileNameOut, ".out");
	execl(FileNameOut, FileName, arg_0, arg_1, arg_2, arg_3, NULL);
}

//------------------------------------------------------------------------------------------------------------------------//
/*
Function explaintion:-
	This function will be used in send and recieve the processed between the process generator that will read it
	and the schudlar that will take and and create it
	Function Parameters:-
		1-Identifier :- the unique of the message queue to control which will use the queue
*/

int InitMsgQueue(char Idenitfier)
{
	key_t key = ftok("keyFile", Idenitfier);
	int megid = msgget(key, 0666 | IPC_CREAT);
	if (megid == -1)
	{
		printf("error in creation in message queue");
		exit(-1);
	}
	return megid;
}

//------------------------------------------------------------------------------------------------------------------------//
/*
Function explaintion:-
	This function will be used mainly in the remaining time of the process and it will be shared by the schdular and the
	running process
	Function Parameters:-
		1-Idenitfier:- the unique of the share memory to control which one will use it
		2-IdShm:- it is pointer will be sent to put the shm id in it
	The return value:
		the function will return the address of the shared memory to use it in write and read
*/

void *InitShm(char Idenitfier, int *IdShm)
{
	key_t Key = ftok("keyFile", Idenitfier);
	int ShmId = shmget(Key, 4096, 0666 | IPC_CREAT);
	*IdShm = ShmId;

	if (ShmId == -1)
	{
		perror("error in creation in shared memory");
		exit(-1);
	}

	void *ShmAddr = shmat(ShmId, (void *)0, 0);
	return ShmAddr;
}
//------------------------------------------------------------------------------------------------------------------------//
/*
two Functions :
	these two functions used in send and receive to and from the message queue
	Commen Parameters:
		MsgId: the id of the queue to control it
	Send:
		ArrivedProcess: it the process will be arrive at this moment
	Rec:
		return Process that sent be the process generator
*/

Process RecMsg(int MsgId)
{

	Message ResMessage;
	int Flag = msgrcv(MsgId, &ResMessage, sizeof(ResMessage.CurrentProcess), 0, IPC_NOWAIT);

	// if failed to receive, return a dummy process
	if (Flag == -1)
	{
		Process *p = (Process *)malloc(sizeof(Process));
		p->ArriavalTime = -1; // just an indicator
		return *p;
	}

	return ResMessage.CurrentProcess;
}

void SendMsg(int MsgId, Process ArrivedProcess)
{
	Message SendMessage;
	SendMessage.CurrentProcess = ArrivedProcess;
	int Flag = msgsnd(MsgId, &SendMessage, sizeof(SendMessage.CurrentProcess), !IPC_NOWAIT);

	if (Flag == -1)
	{
		printf("\nerror in Sending\n");
	}
}

//------------------------------------------------------------------------------------------------------------------------//
/*
Function explaintion:-
	This function starts the prcess by forking and running the process file
	Function Parameters:-
		1-CurrentProcess:- The process to be run
*/
void StartProcess(Process *CurrentProcess)
{
	OutputFile = fopen("scheduler.txt", "a");

	if (!OutputFile)
		perror("\nCan't open the scheduler text file\n");

	CurrentProcess->Id_1 = fork();

	if (CurrentProcess->Id_1 == 0)
	{
		RunAndComplie("process", NULL, NULL, NULL, NULL);
	}

	fprintf(OutputFile, "At time %d process %d started arr %d total %d remain %d wait %d\n",
			getClk(), CurrentProcess->Id_2, CurrentProcess->ArriavalTime, CurrentProcess->RunTime,
			CurrentProcess->RunTime, getClk() - CurrentProcess->ArriavalTime);
	fclose(OutputFile);
}
//------------------------------------------------------------------------------------------------------------------------//
/*
Function explaintion:-
	This function checks if a new process is sent, reveive it, and push it in the ready queue
	Function Parameters:-
		1-Queue:- The ready queue ( for the ready processes )
		2-Flag:-  A flag sent by ref to know if the receiving process is succeeded or not
		3-Priority:- to determine the priority type of the passes queue
		4-ProcessesHashTime:- An array to store the number of processes that arrived at time x, (index = x)
*/
void ReadyProcessExist(int MsgId, Node **Queue, int *Flag, int Priority, int* last)
{

	Process RecProcess;
	int index;

	RecProcess = RecMsg(MsgId);
	push(Queue, RecProcess, Priority);
	printf("process : %d\n", RecProcess.Id_2);

	// if (RecProcess.Id_2 == 1)
	// {
	// 	*last = -1;
	// }
	

	return;
	
}
//------------------------------------------------------------------------------------------------------------------------//
/*
Function explaintion:-
	This function finishes prints the info of the process after terminating
	Function Parameters:-
		1-FinishedProcess:- The process that want to be terminated
		2-ShmAddr:-  the shared memory address
*/
void FinishProcess(Process *FinishedProcess, int *ShmAddr)
{
	OutputFile = fopen("scheduler.txt", "a");

	int FinishTime = getClk();
	double WTA = (getClk() - FinishedProcess->ArriavalTime) * 1.0 / FinishedProcess->RunTime;
	int wait = (getClk() - FinishedProcess->ArriavalTime) - FinishedProcess->RunTime;
	fprintf(OutputFile, "At time %d process %d finished arr %d total %d remain 0 wait %d TA %d WTA %.2f\n", getClk(),
			FinishedProcess->Id_2, FinishedProcess->ArriavalTime, FinishedProcess->RunTime,
			wait, getClk() - FinishedProcess->ArriavalTime, WTA);

	fclose(OutputFile);
}
//------------------------------------------------------------------------------------------------------------------------//
/*
Function explaintion:-
	This function stops the passed process by sending a SIGSTOP to the process,
	and prints the info of the process
	Function Parameters:-
		1-p:- The process that want to be terminated
*/
void StopProcess(Process p)
{
	OutputFile = fopen("scheduler.txt", "a");
	kill(p.Id_1, SIGSTOP);

	fprintf(OutputFile, "At time %d process %d stopped arr %d total %d remain %d\n", getClk(),
			p.Id_2, p.ArriavalTime, p.RunTime, p.RemainingTime);
	fclose(OutputFile);
}
//------------------------------------------------------------------------------------------------------------------------//
/*
Function explaintion:-
	This function resumes the passed process by sending a SIGCONT to the process,
	and prints the time which the process resumed its work at
	Function Parameters:-
		1-p:- The process that want to be resumed
*/
void ContinueProcess(Process p)
{
	OutputFile = fopen("scheduler.txt", "a");
	kill(p.Id_1, SIGCONT);
	fprintf(OutputFile, "At time %d process %d resumed arr %d total %d remain %d\n", getClk(),
			p.Id_2, p.ArriavalTime, p.RunTime, p.RemainingTime);
	fclose(OutputFile);
}
//------------------------------------------------------------------------------------------------------------------------//
/*
Function explaintion:-
	This function to wait for one second
*/
void nextSecondWaiting(int *lastSecond)
{
    while (*lastSecond == getClk())
        ;
    *lastSecond = getClk();
}

//------------------------------------------------------------------------------------------------------------------------//


//------------------------------------------------------------------------------------------------------------------------//
/*
Function explaintion:-
	This function compares between two passed variables,
		returns 1 if the first number is the bigger,
		otherwirse, returns 0
	Function Parameters:-
		1-First:- The first number
		2-Second:- The second number
*/
int comparePriorities(int First, int Second)
{
	if (First > Second)
		return 1;
	else
		return 0;
}
