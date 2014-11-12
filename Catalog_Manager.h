#ifndef __MiniSQL__Catalog__Manager__
#define __MiniSQL__Catalog__Manager__

#include "MiniSQL.h"

class CatalogManager
{
	vector<Table> tableList;					// 表信息列表
	//vector<Table>::iterator tableListItor;		// 表信息列表迭代器
	int tableNum;								// 表的数目
public:
	CatalogManager(){};
	~CatalogManager(){};
	bool API_Catalog(SQLstatement sql);

	Table* findTable(string tn);
	bool createTable(SQLstatement sql);
	bool checkInsert(Table *t, string value);
	void pushBack_tableList(Table &t);
	void update_tableNum();
	bool save_tableInfo(vector<Table> &tl);
	bool checkType(Attribute *a, string v);
	bool checkAttribute(Table *t, vector<Attribute> *a);
	bool checkCondition(Table *t, vector<Condition> *c);

	//bool insertAttri(Table& table, string attriName, int type, int length, bool isPrimaryKey = false, bool isUnique = false);
	//bool initiaTable(Table& table);
	bool createIndex(string indexName, string tableName, string attriName);
	int findIndexTable(string indexName);
	int findIndexAttri(string indexName);
	bool dropTable(string tableName);
	bool dropIndex(string indexName);
	bool deleteAttri(Table &table, string attriName);
	int getAttriNum(Table &table, string attriName);
};

#endif /* defined(__MiniSQL__Catalog__Manager__) */