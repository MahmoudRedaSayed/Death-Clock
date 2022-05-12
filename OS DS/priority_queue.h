

#ifndef _priorty_queue
#define _priorty_queue

#include "PriortyNode.h"
#include "priortyQueueADT.h"

using namespace std;


template <typename T>
class priority_Queue :public priortyQueueADT<T>
{
private:
	
	PriortyNode<T>* backPtr;
	PriortyNode<T>* frontPtr;
public:
	priority_Queue()
	{
		backPtr = nullptr;
		frontPtr = nullptr;
		
	}
	bool isEmpty() const
	{
		return (frontPtr == nullptr);
	}
	bool enqueue( T newEntry, double r_priorty)
	{

		PriortyNode<T>* newNodePtr = new PriortyNode<T>(newEntry, r_priorty);
		if (isEmpty())
			frontPtr = newNodePtr;
		else
		{

			PriortyNode<T>* ptr = frontPtr;
			if (frontPtr->getPriorty() < newNodePtr->getPriorty())
			{
				newNodePtr->setNext(frontPtr);
				frontPtr = newNodePtr;
			}
			else
			{
				while (ptr->getNext() && ptr->getNext()->getPriorty() > newNodePtr->getPriorty())
				{
					ptr = ptr->getNext();
				}
				newNodePtr->setNext(ptr->getNext());
				ptr->setNext(newNodePtr);
			}

		}
		return true;
	}

	bool dequeue(T& frntEntry)
	{
		if (isEmpty())
			return false;

		PriortyNode<T>* nodeToDeletePtr = frontPtr;
		frntEntry = frontPtr->getItem();

		frontPtr = frontPtr->getNext();

		if (nodeToDeletePtr == backPtr)
			backPtr = nullptr;


		delete nodeToDeletePtr;

		return true;

	}
	bool peek(T& frntEntry) const
	{
		if (isEmpty())
			return false;

		frntEntry = frontPtr->getItem();

		return true;

	}
	~priority_Queue()
	{
		T temp;
		while (dequeue(temp));
	}



	priority_Queue(const priority_Queue<T>& LQ)
	{
		PriortyNode<T>* NodePtr = LQ.frontPtr;
		if (!NodePtr)
		{
			frontPtr = backPtr = nullptr;
			return;
		}

		PriortyNode<T>* ptr = new PriortyNode<T>(NodePtr->getItem(), NodePtr->getPriorty());
		frontPtr = backPtr = ptr;
		NodePtr = NodePtr->getNext();


		while (NodePtr)
		{
			PriortyNode<T>* ptr = new PriortyNode<T>(NodePtr->getItem(), NodePtr->getPriorty());
			backPtr->setNext(ptr);
			backPtr = ptr;
			NodePtr = NodePtr->getNext();
		}
	}

};
#endif