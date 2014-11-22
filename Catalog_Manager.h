#ifndef __MiniSQL__Catalog__Manager__
#define __MiniSQL__Catalog__Manager__

#include "MiniSQL.h"

class CatalogManager
{
private:
	vector<Table> tableList; // 表信息列表
	vector<Index> indexList; // 索引信息列表
	int tableNum; // 表的数目
	
	// 创建表，检查是否成功
	bool createTable(SQLstatement &sql); 
	// 创建索引
	bool createIndex(SQLstatement &sql, Table *t);
	
	
	// 将表加入tableList	
	void pushBack_tableList(Table &t); 
	// 将索引加入indexList 
	void pushBack_indexList(Index &i);

	// 更新表数目
	void update_tableNum(bool add);  

	// 检查插入是否合法
	//example of value:
	//1,1.0,"abc"
	bool checkInsert(Table *t, string value);
	// 检查类型是否正确 float 1.0f , char[] "ff"
	bool checkType(Attribute *a, string v);
	// 检查t中的属性和a中属性是否相符，index为真时还需检查是否为unique或primary
	bool checkAttribute(Table *t, vector<Attribute> *a, bool index); 
	// 检查t中的属性和c中属性是否相符
	bool checkCondition(Table *t, vector<Condition> *c); 
	 

	// 清除属性下的indexname
	bool clearIndex(Table *t, string in, bool drop);	 

	// 删除索引（从vector中，同时调用save_indexInfo）
	bool dropIndex(Table *t, Index *i); 
	// 删除表（从vector中，同时调用save_tableInfo）
	bool dropTable(Table *t); 

	// 把表t的更改写入文件 add真时更新文件，假时删除文件
	bool save_tableInfo(Table *t, bool add); 
	// 把索引i的更改写入文件 add真时更新文件，假时删除文件
	bool save_indexInfo(Index *i, bool add);

	// 把表的属性信息写入文件（savetable时调用）
	void writeAttribute(string fn, Attribute *a); 	

	// 从表t中获得name为an的Index指针
	Attribute* getAttributebya(Table *t, string an); 
	
public:
	string msg; // 传送消息

	CatalogManager(){};
	~CatalogManager(){};

	// API对cm的调用
	bool API_Catalog(SQLstatement &sql); 

	// 若找到表返回指针，否则返回NULL
	Table* findTable(string tn); 		
	// 若找到索引返回指针，否则返回NULL
	Index* findIndex(string in); 

	// 把最新的表信息写入文件
	void update_tableInfo();
	// 把最新的索引信息写入文件
	void update_indexInfo(); 
	//从文件中读入表信息
	void read_tableInfo(); 
	//从文件中读入索引信息
	void read_indexInfo(); 
 
};

#endif