#ifndef __MINISQL__H__
#define __MINISQL__H__

#include <iostream>
#include <string>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <direct.h>
#include <algorithm>
#include <vector>
#include <map>
#include <list>


using namespace std;

#define FILE_COUNTER 1
#define FILE_INFO_LENGTH 671
#define ATTR_INFO_LENGTH 20
#define FILE_HEAD_LENGTH 31

typedef map< string, vector<string> > AttributeValuesMap;

enum TYPE { MYINT = 0, MYFLOAT = 1, MYCHAR = 2 };
enum RELATION_TYPE { EQUAL, NOT_EQUAL, GREATER, GREATER_EQUAL, SMALLER, SMALLER_EQUAL, AND, OR};
enum STATEMENT_TYPE { CREATE_DATABASE, CREATE_TABLE, CREATE_INDEX, DROP_TABLE, DROP_INDEX, DROP_DATABASE, SELECT, SELECT_WHERE, INSERT, MYDELETE, DELETE_WHERE, USE, EXECFILE, QUIT, HELP, ILLEGAL};

//定义表的索引节点信息
struct index_info
{
	string index_name;
	int length;
	char type;
	long offset;
	string value;
};

//定义属性信息
class Attribute
{
public:
	friend class CatalogManager;
	friend class Table;
	string name = "";
	string indexName = "";      //index名
	TYPE type = MYINT;
	int length = 0;
	bool isPrimaryKey = false;
	bool isUnique = false;

	Attribute(){}
	Attribute(const Attribute & from) :name(from.name), indexName(from.indexName), type(from.type), length(from.length), isPrimaryKey(from.isPrimaryKey), isUnique(from.isUnique){}
	Attribute(string name) :name(name){}
	Attribute(string name, TYPE type, int length, bool isPrimaryKey = false, bool isUnique = false) :name(name), type(type), length(length), isPrimaryKey(isPrimaryKey), isUnique(isUnique){}
	~Attribute(){}
};


//定义条件信息
class Condition
{
public:
	Attribute attribute;
	string value;
	RELATION_TYPE relationType;

	Condition(){}
	Condition(Attribute attribute, string value, RELATION_TYPE relationType) :attribute(attribute), value(value), relationType(relationType){}
	~Condition(){}
};

//定义表信息
class Table
{
public:
	string name;							// 表名
	int blockNum = 0;						// 在name.table中占用的block数
	int recordNum = 0;						// 记录条数
	int attriNum = 0;						// 属性数
	int tupleLength = 0;					// 每条记录的长度
	string primaryKey;						// 主键名字
	//int freeNum = 0;						// 有几条被删除的记录
	vector<Attribute> attributes;			// 指向元数据链表的指针
	//vector<Attribute>::iterator AttriItor;	// Attribute的iterator
	//vector<string> data;					// 指向数据链表的指针
	//long dataBlockInFile = -1;			// data开头在file中的块的位置（每张表的数据一定是从一块的开头开始）
	//vector<string> emptyList;				// 指向等待删除链表的指针（这东西到底干吗用）

	Table(){}
	Table(string name, string primaryKey) :name(name), primaryKey(primaryKey){}
	~Table(){}
};

// 语句类
class SQLstatement
{
public:
	STATEMENT_TYPE type;
	string tableName;
	string indexName;
	vector<Attribute> attributes;
	vector<Condition> conditions;
	//vector<RELATION_TYPE> relations;
	string content;

	SQLstatement(STATEMENT_TYPE type, string tableName);
	SQLstatement(string SQL);
	~SQLstatement();
	void outputinfo();
	Condition genCondition(string exp);
	RELATION_TYPE getRelationType(string oper);
	STATEMENT_TYPE identify(string type_code);
};

// 定义索引信息
class Index
{
public:
	string index_name;
	string table_name;
	int startposition;
	int tuplelength;
	int columnLength;
	int IndexOffset;
	Index() :IndexOffset(0){}
};
#endif