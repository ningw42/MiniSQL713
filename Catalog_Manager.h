#ifndef __MiniSQL__Catalog__Manager__
#define __MiniSQL__Catalog__Manager__

#include "MiniSQL.h"
#include <fstream>
#include <iostream>

class CatalogManager
{
	vector<Table> tableList;					// ����Ϣ�б�
	vector<Table>::iterator tableListItor;		// ����Ϣ�б������
	int tableNum;								// �����Ŀ

public:
	CatalogManager();
	~CatalogManager();
	Table &createTable(string name, string primarykey);
	//bool insertAttri(Table& table, string attriName, int type, int length, bool isPrimaryKey = false, bool isUnique = false);
	bool initiaTable(Table& table);
	bool createIndex(string indexName, string tableName, string attriName);
	int findTable(string tableName);
	int findIndexTable(string indexName);
	int findIndexAttri(string indexName);
	bool dropTable(string tableName);
	bool dropIndex(string indexName);
	bool deleteAttri(Table &table, string attriName);
	int getAttriNum(Table &table, string attriName);
};

#endif /* defined(__MiniSQL__Catalog__Manager__) */