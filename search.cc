#include <malloc.h>

int headerSize(void)
{
	return sizeof(headerData);
}

int tupleSize(void)
{
	return keyLength + sizeof(void *);
}

keyType* getKeyAddr(int pos)
{
	keyType* key = (keyType*)(data + pos * tupleSize() + sizeof(long));
	return key;
}



long* getLinkAddr(int pos)
{
	long* link = firstTuple()+pos*tupleSize();
	return link;
}

bool searchForKey(object& theObject, int& ptr)
{
	int i=0;

	while (i < activeKeys)
	{	
		if (theObject.isLess(getKeyAddr(i))
		{
			ptr = i;
			return TRUE;
		}
		i++;
	}
	ptr=i;
	return FALSE;
}

