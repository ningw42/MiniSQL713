#include "MiniSQL.h"
inline bool isUnique(string s)
{
	if (s == "1")
		return true;
	else
		return false;
}
inline TYPE getType(string s)
{
	if (s == "+")
		return INT;
	else if (s == "-")
		return FLOAT;
	else
		return CHAR;
}
void SQLstatement::outputinfo()
{
	for (size_t i = 0; i < attributes.size(); i++)
	{
		cout << tableName << endl;
		cout << "name: " << attributes[i].name << endl;
		cout << "type: " << attributes[i].type << endl;
		cout << "length: " << attributes[i].length << endl;
		cout << "isPrimary: " << attributes[i].isPrimaryKey << endl;
		cout << "isUnique: " << attributes[i].isUnique << endl;
	}
}
STATEMENT_TYPE SQLstatement::identify(string type_code)
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

SQLstatement::SQLstatement(string SQL)
{
	int start = 2;
	int end = 0;
	string temp;
	string type_code = SQL.substr(0, 2);
	this->type = identify(type_code);
	switch (this->type)
	{
	case SELECT:
		end = SQL.find_first_of(',', start);
		tableName = SQL.substr(start, end - start);
		break;
	case SELECT_WHERE:break;
	case INSERT:
		end = SQL.find_first_of(' ', start);
		tableName = SQL.substr(start, end - start);
		start = end + 1;	// 读取表名
		content = SQL.substr(start);
		//cout << temp << endl;
		break;
	case DELETE:break;
	case DELETE_WHERE:break;
	case CREATE_DATABASE:break;
	case CREATE_TABLE:
		end = SQL.find_first_of(',', start);
		tableName = SQL.substr(start, end - start); 
		start = end + 1;	// 读取表名
		while (true)
		{
			int first, second;
			end = SQL.find_first_of(',', start);
			if (end == -1) break;
			temp = SQL.substr(start, end - start);

			first = SQL.find_first_of(' ', start);
			second = SQL.find_first_of(' ', first + 1);
			if (second == -1)	// 读到primarykey声明
			{
				string primaryKey = SQL.substr(start, first - start);
				for (size_t i = 0; i < attributes.size(); i++)
				{
					if (attributes[i].name == primaryKey)
					{
						attributes[i].isPrimaryKey = true;
						break;
					}	// 可能存在primaryKey指明的属性不存在						TO-DO
				}
			}
			else
			{
				string attrname = SQL.substr(start, first - start);
				TYPE attrtype = getType(SQL.substr(first + 1, second - first - 1));
				bool attrisUnique = isUnique(SQL.substr(second + 1, end - second - 1));
				int length = 4;
				if (attrtype == CHAR)
				{
					length = atoi(SQL.substr(first + 1, second - first - 1).c_str());
				}
				attributes.push_back(Attribute(attrname, attrtype, length, false, attrisUnique));
			}
			start = end + 1;
		}
		break;
	case CREATE_INDEX:break;
	case DROP_DATABASE:break;
	case DROP_TABLE:break;
	case DROP_INDEX:break;
	case USE:break;
	case EXECFILE:break;
	case QUIT:break;
	case HELP:break;
	default:break;
	}
}
SQLstatement::SQLstatement(STATEMENT_TYPE type, string tableName)
{

}

SQLstatement::~SQLstatement()
{
}