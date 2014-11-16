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
	bool API_Catalog(SQLstatement &sql);

	Table* findTable(string tn);
	bool createTable(SQLstatement &sql);
	bool checkInsert(Table *t, string value);
	void pushBack_tableList(Table &t);
	void update_tableNum(bool add);
	bool checkType(Attribute *a, string v);
	bool checkAttribute(Table *t, vector<Attribute> *a);
	bool checkCondition(Table *t, vector<Condition> *c);
	bool createIndex(string in, Table *t, Attribute *a);
	bool checkIndex(Table *t, string in, bool drop);
	bool dropIndex(Table *t, string in);
	bool dropTable(Table *t);
	bool save_tableInfo(Table *t, bool add);
	void writeAttribute(string fn, Attribute *a);
	void read_TableInfo();
	void update_tableInfo();
	
	//bool insertAttri(Table& table, string attriName, int type, int length, bool isPrimaryKey = false, bool isUnique = false);
	//bool initiaTable(Table& table);
	//bool deleteAttri(Table &table, string attriName);
	int getAttriNum(Table *table);
};

#endif /* defined(__MiniSQL__Catalog__Manager__) */