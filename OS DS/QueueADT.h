#ifndef QUEUE_ADT_
#define QUEUE_ADT_

template<class ItemType>
class QueueADT
{
public:
	virtual bool isEmpty() const = 0;
	virtual bool enqueue(const ItemType& newEntry) = 0;
	virtual bool dequeue(ItemType& FrontEntry) = 0;
	virtual bool peek(ItemType& FrontEntry) const = 0;
	virtual ~QueueADT() { }
};
#endif