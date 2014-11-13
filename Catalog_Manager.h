#ifndef __MiniSQL__Catalog__Manager__
#define __MiniSQL__Catalog__Manager__

#include "MiniSQL.h"

class CatalogManager
{
	vector<Table> tableList;					// ����Ϣ�б�
	//vector<Table>::iterator tableListItor;		// ����Ϣ�б������
	int tableNum;								// �����Ŀ
public:
	CatalogManager(){};
	~CatalogManager(){};
	bool API_Catalog(SQLstatement sql);

	Table* findTable(string tn);
	bool createTable(SQLstatement sql);
	bool checkInsert(Table *t, string value);
	void pushBack_tableList(Table &t);
	void update_tableNum();
	bool save_tableInfo(vector<Table> &tl, bool add);
	bool checkType(Attribute *a, string v);
	bool checkAttribute(Table *t, vector<Attribute> *a);
	bool checkCondition(Table *t, vector<Condition> *c);
	bool createIndex(string in, Table *t, Attribute *a);
	bool checkIndex(Table *t, string in, bool drop);
	bool dropIndex(Table *t, string in);
	bool dropTable(Table *t);
	
	//bool insertAttri(Table& table, string attriName, int type, int length, bool isPrimaryKey = false, bool isUnique = false);
	//bool initiaTable(Table& table);
		
	bool deleteAttri(Table &table, string attriName);
	int getAttriNum(Table &table, string attriName);
};

#endif /* defined(__MiniSQL__Catalog__Manager__) */