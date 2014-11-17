#ifndef __MiniSQL__Catalog__Manager__
#define __MiniSQL__Catalog__Manager__

#include "MiniSQL.h"

class CatalogManager
{
	vector<Table> tableList; // 表信息列表
	vector<Index> indexList; // 索引信息列表
	int tableNum; // 表的数目
	
public:
	CatalogManager(){};
	~CatalogManager(){};
	bool API_Catalog(SQLstatement &sql); // API对cm的调用

	Table* findTable(string tn); // 找到表返回指针
	bool createTable(SQLstatement &sql); // 创建表
	bool checkInsert(Table *t, string value); // 检查插入
	void pushBack_tableList(Table &t); // 表加入tableList
	void update_tableNum(bool add); // 更新表数目
	bool checkType(Attribute *a, string v); // 检查类型
	bool checkAttribute(Table *t, vector<Attribute> *a, bool index); // 检查属性
	bool checkCondition(Table *t, vector<Condition> *c); // 检查条件
	Index* findIndex(string in); // 找到索引返回指针
	bool createIndex(SQLstatement &sql, Table *t); // 创建索引
	bool clearIndex(Table *t, string in, bool drop); // 清除索引
	void pushBack_indexList(Index &i); // 索引加入indexList
	bool dropIndex(Table *t, Index *i); // 删除索引
	bool dropTable(Table *t); // 删除表
	bool save_tableInfo(Table *t, bool add); // 存入表信息
	void writeAttribute(string fn, Attribute *a); // 存入属性
	void read_tableInfo(); // 读出表信息
	void update_tableInfo(); // 更新表信息
	bool save_indexInfo(Index *i, bool add); // 存入索引信息
	void read_indexInfo(); // 读出索引信息
	void update_indexInfo(); // 更新索引信息

	string msg; // 传送消息
};

#endif /* defined(__MiniSQL__Catalog__Manager__) */