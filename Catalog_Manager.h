#ifndef __MiniSQL__Catalog__Manager__
#define __MiniSQL__Catalog__Manager__

#include "MiniSQL.h"

class CatalogManager
{
	vector<Table> tableList;					// 表信息列表
	vector<Index> indexList;                    // 索引信息列表
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
	bool checkAttribute(Table *t, vector<Attribute> *a, bool index);
	bool checkCondition(Table *t, vector<Condition> *c);
	Index* findIndex(string in);
	bool createIndex(SQLstatement &sql, Table *t);
	bool checkIndex(Table *t, string in, bool drop);
	void pushBack_indexList(Index &i);
	bool dropIndex(Table *t, Index *i);
	bool dropTable(Table *t);
	bool save_tableInfo(Table *t, bool add);
	void writeAttribute(string fn, Attribute *a);
	void read_tableInfo();
	void update_tableInfo();
	bool save_indexInfo(Index *i, bool add);
	void read_indexInfo();
	void update_indexInfo();

	//bool insertAttri(Table& table, string attriName, int type, int length, bool isPrimaryKey = false, bool isUnique = false);
	//bool initiaTable(Table& table);
	//bool deleteAttri(Table &table, string attriName);
	int getAttriNum(Table *table);
	
	string msg;
};

#endif /* defined(__MiniSQL__Catalog__Manager__) */