#include "MiniSQL.h"
class SQLstatament
{
public:
	STATEMENT_TYPE type;
	string tableName;
	vector<Attribute> attributes;
	vector<Condition> conditions;
	//vector<RELATION_TYPE> relations;
	string content;

	SQLstatament(STATEMENT_TYPE type, string tableName);
	SQLstatament(string SQL);
	~SQLstatament();
	STATEMENT_TYPE identify(string type_code);
};

STATEMENT_TYPE SQLstatament::identify(string type_code)
{
	if (type_code == "99") return ILLEGAL;
	else if (type_code == "20") return SELECT;
	else if (type_code == "21") return SELECT_WHERE;
	else if (type_code == "30") return INSERT;
	else if (type_code == "40") return DELETE;
	else if (type_code == "41") return DELETE_WHERE;
	else if (type_code == "00") return CREATE_DATABASE;
	else if (type_code == "01") return CREATE_TABLE;
	else if (type_code == "02") return CREATE_INDEX;
	else if (type_code == "10") return DROP_DATABASE;
	else if (type_code == "11") return DROP_TABLE;
	else if (type_code == "12") return DROP_INDEX;
	else if (type_code == "50") return USE;
	else if (type_code == "60") return EXECFILE;
	else if (type_code == "70") return QUIT;
	else if (type_code == "80") return HELP;
	else return ILLEGAL;
}

SQLstatament::SQLstatament(string SQL)
{
	string type_code = SQL.substr(0, 1);
	this->type = identify(type_code);
}
SQLstatament::SQLstatament(STATEMENT_TYPE type, string tableName)
{

}

SQLstatament::~SQLstatament()
{
}