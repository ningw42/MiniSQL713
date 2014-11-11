#include "Catalog_Manager.h"

int CatalogManager :: findTable(string tableName)
{
	for (int i = 0; i < tableList.size(); i++)
	{
		if (tableList[i].name == tableName)
		{
			return i;
		}
	}
	return -1;
}