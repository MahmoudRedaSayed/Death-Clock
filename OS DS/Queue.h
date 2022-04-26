#pragma once
#include "Node.h"
#include "QueueADT.h"
template <typename T>
class Queue :public QueueADT<T>
{
private:
	Node<T>* backPtr;
	Node<T>* frontPtr;
public:
	Queue()
	{
		backPtr = nullptr;
		frontPtr = nullptr;
	}
	bool isEmpty() const
	{
		return (frontPtr == nullptr);
	}
	bool enqueue(const T& newEntry)
	{
		Node<T>* newNodePtr = new Node<T>(newEntry);

		if (isEmpty())
			frontPtr = newNodePtr;
		else
			backPtr->setNext(newNodePtr);

		backPtr = newNodePtr;
		return true;
	}

	bool dequeue(T& frntEntry)
	{
		if (isEmpty())
			return false;
		Node<T>* nodeToDeletePtr = frontPtr;
		frntEntry = frontPtr->getItem();
		frontPtr = frontPtr->getNext();

		if (nodeToDeletePtr == backPtr)
			backPtr = nullptr;
		delete nodeToDeletePtr;
		return true;

	}
	bool  peek(T& frntEntry) const
	{
		if (isEmpty())
			return false;

		frntEntry = frontPtr->getItem();
		return true;

	}
	~Queue()
	{
		T temp;
		while (dequeue(temp));
	}

	Queue(const Queue<T>& LQ)
	{
		Node<T>* NodePtr = LQ.frontPtr;
		if (!NodePtr)
		{
			frontPtr = backPtr = nullptr;
			return;
		}

		Node<T>* ptr = new Node<T>(NodePtr->getItem());
		frontPtr = backPtr = ptr;
		NodePtr = NodePtr->getNext();
		while (NodePtr)
		{
			Node<T>* ptr = new Node<T>(NodePtr->getItem());
			backPtr->setNext(ptr);
			backPtr = ptr;
			NodePtr = NodePtr->getNext();
		}
	}
};