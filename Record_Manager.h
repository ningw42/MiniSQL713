#ifndef __RECORD_MANAGER_H__
#define __RECORD_MANAGER_H__
#include "Minisql.h"

typedef map< string, vector<string> > SelectReturnList;
class RecordManager
{
public:
	RecordManager(){};
	~RecordManager(){};

	bool createTable(Table & table);
	bool insertValue(Table & table, string values);
	bool dropTable(Table & table);
	SelectReturnList selectWithwhere(Table & table, vector<Attribute> & attributes, vector<Condition> & conditions);
	SelectReturnList selectWithoutwhere(Table & table, vector<Attribute> & attributes);
	bool deleteWithwhere(Table & table, vector<Condition> & conditions);
	bool deleteWithoutwhere(Table & table);
};
#endif