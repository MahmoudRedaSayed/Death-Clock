#ifndef Linked_List
#define Linked_List
#include "Node.h"
#include "ListADT.h"
template<class T>
class LinkedList : public ListADT<T>
{
	Node<T>* Head;
	int itemcount;
public:
	LinkedList()
	{
		Head = nullptr;
		itemcount = 0;
	}
	bool isEmpty() const
	{
		return (!Head);
	}

	int getLength() const
	{
		return itemcount;
	}

	bool insert(int pos, const T& newEntry)
	{
		if (pos >= 1 && pos <= itemcount + 1)
		{
			itemcount++;
			Node<T>* newNode = new Node<T>(newEntry);

			if (pos == 1)
			{
				newNode->setNext(Head);
				Head = newNode;
			}
			else
			{

				Node<T>* ptr = Head;
				for (int i = 1; i < pos - 1; i++)
				{
					ptr = ptr->getNext();

				}
				newNode->setNext(ptr->getNext());
				ptr->setNext(newNode);
			}
		}
		return false;
	}

	const T& getEntry(int pos)const
	{
		if (pos >= 1 && pos <= itemcount)
		{
			Node<T>* ptr = Head;
			for (int i = 1; i < pos; i++)
			{
				ptr = ptr->getNext();
			}
			return ptr->getItem();
		}
		return 0;
	}
	bool remove(int pos)
	{
		if (pos >= 1 && pos <= itemcount)
		{
			Node<T>* deleteNode;
			if (pos == 1)
			{
				deleteNode = Head;
				Head = Head->getNext();
				delete deleteNode;
				itemcount--;
				return true;
			}
			else
			{
				Node<T>* ptr = Head;
				for (int i = 1; i < pos - 1; i++)
				{
					ptr = ptr->getNext();
				}
				deleteNode = ptr->getNext();
				ptr->setNext(ptr->getNext()->getNext());

			}
			delete deleteNode;
			itemcount--;
			return true;
		}
		else
		{
			return false;
		}
	}

	void clear()
	{
		while (!isEmpty())
		{
			remove(1);
		}
	}


	~LinkedList()
	{
		clear();

	}
};
#endif