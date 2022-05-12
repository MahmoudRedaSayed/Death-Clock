
#ifndef priorty_Queue_ADT
#define priorty_Queue_ADT

template<class ItemType>
class priortyQueueADT
{
public:

	virtual bool isEmpty() const = 0;


	virtual bool enqueue( ItemType newEntry, double r_priorty) = 0;


	virtual bool dequeue(ItemType& FrontEntry) = 0;

	virtual bool peek(ItemType& FrontEntry) const = 0;


	virtual ~priortyQueueADT() { }
};
#endif