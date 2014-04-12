#include "dbobj.h"
#include "test.h"
#include <malloc.h>

main()
{
	fred myobj;

	dbObject dbo("test/dbfiles/","fred");
	dbo.add(myobj);
}

