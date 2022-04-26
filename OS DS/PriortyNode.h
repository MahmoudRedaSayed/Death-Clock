
#ifndef Priorty_Node
#define Priorty_Node
template < class T>
class PriortyNode
{
private:
	double priorty;
	T item;
	PriortyNode<T>* next;
public:
	PriortyNode();
	PriortyNode( T  r_Item, double  r_priorty);
	PriortyNode(const T & r_Item, double  r_priorty,PriortyNode<T>* nextNodePtr);
	void setItem(const T & r_Item);
	void setPriorty(const T &r_priorty);
	double getPriorty()const;
	void setNext(PriortyNode<T>* nextNodePtr);
	T getItem() const;
	PriortyNode<T>* getNext() const;
};
#endif

template < class T>
PriortyNode<T>::PriortyNode()
{
	next = nullptr;
}

template < class T>
PriortyNode<T>::PriortyNode(const T  r_Item, double  r_priorty)
{
	item = r_Item;
	priorty = r_priorty;
	next = nullptr;
}

template < class T>
PriortyNode<T>::PriortyNode(const T& r_Item, double  r_priorty,PriortyNode<T>* nextNodePtr)
{
	item = r_Item;
	priorty = r_priorty;
	next = nextNodePtr;
}
template < class T>
void PriortyNode<T>::setItem(const T& r_Item)
{
	item = r_Item;
}
template < class T>
void PriortyNode<T>::setPriorty(const T &r_priorty)
{
	priorty = r_priorty;
}
template < class T>
void PriortyNode<T>::setNext(PriortyNode<T>* nextNodePtr)
{
	next = nextNodePtr;
}

template < class T>
T PriortyNode<T>::getItem() const
{
	return item;
}

template < class T>
double  PriortyNode<T>::getPriorty() const
{
	return priorty;
}

template < class T>
PriortyNode<T>* PriortyNode<T>::getNext() const
{
	return next;
}