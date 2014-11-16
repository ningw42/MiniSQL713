#ifndef __RECORD_MANAGER_H__
#define __RECORD_MANAGER_H__

#include "MiniSQL.h"

class RecordManager
{
public:
	AttributeValuesMap attributeValuesMap;
	RecordManager(){};
	~RecordManager(){};

	string toString(const char * source, int length, TYPE type);
	bool satisfy(const vector<Attribute> & attributes, const vector<Condition> & conditions, const string * allAttrValues);
	bool satisfy(const Condition & cond, const string & value);	// 需要维护condition里的attribute的所有信息
	bool satisfy(const Condition & cond, const string & value, const TYPE type);
	bool contains(const vector<Attribute> & attributes, const Attribute & attr);
	int hasCondition(const vector<Condition> & conditions, const string & attrName);
	int findIndexOf(const vector<Attribute> & attributes, const Condition & condition);
	int copyinto(char * buffer, const char * from, int start, int length);
	bool createTable(Table & table);
	bool insertValue(Table & table, const string & values);
	bool dropTable(string & tableName);
	int selectWithwhere(Table & table, const vector<Attribute> & attributes, const vector<Condition> & conditions);
	int selectWithoutwhere(Table & table, const vector<Attribute> & attributes);
	int  deleteWithwhere(Table & table, vector<Condition> & conditions);
	int deleteWithoutwhere(Table & table);

	void outputMap(int tupleCount);
};
#endif