#ifndef __RECORD_MANAGER_H__
#define __RECORD_MANAGER_H__

#include "MiniSQL.h"

class RecordManager
{
public:
	AttributeValuesMap attributeValuesMap;
	RecordManager(){};
	~RecordManager(){};

	bool contains(const vector<Attribute> & attributes, const Attribute & attr);

	bool createTable(Table & table);
	bool insertValue(Table & table, string values);
	bool dropTable(Table & table);
	int selectWithwhere(Table & table, const vector<Attribute> & attributes, const vector<Condition> & conditions);
	int selectWithoutwhere(Table & table, const vector<Attribute> & attributes);
	bool deleteWithwhere(Table & table, vector<Condition> & conditions);
	bool deleteWithoutwhere(Table & table);
};
#endif