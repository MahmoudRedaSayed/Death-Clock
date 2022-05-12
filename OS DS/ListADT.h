#ifndef List_ADT
#define List_ADT

template<class ItemType>
class ListADT
{
public:

	virtual bool isEmpty() const = 0;


	virtual bool insert(int pos, const ItemType& newEntry) = 0;

	virtual const ItemType& getEntry(int pos)const = 0;
	virtual bool remove(int pos) = 0;

	virtual void clear() = 0;


	virtual ~ListADT() { }
};
#endif