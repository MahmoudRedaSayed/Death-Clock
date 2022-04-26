#ifndef _NODE
#define _NODE
template < class T>
class Node
{
private:
	T item;
	Node<T>* next;
public:
	Node()
	{
		next = nullptr;

	}
	Node(const T& r_Item)
	{
		item = r_Item;
		next = nullptr;
	}
	Node(const T& r_Item, Node<T>* nextNodePtr)
	{
		item = r_Item;
		next = nextNodePtr;
	}
	void setItem(const T& r_Item)
	{
		item = r_Item;
	}

	void setNext(Node<T>* nextNodePtr)
	{
		next = nextNodePtr;
	}
	const T& getItem() const
	{
		return item;
	}

	Node<T>* getNext() const
	{
		return next;
	}
};
#endif