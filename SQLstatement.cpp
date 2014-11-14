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
		return MYINT;
	else if (s == "-")
		return MYFLOAT;
	else
		return MYCHAR;
}
void SQLstatement::outputinfo()
{
	cout << "----------Table----------" << endl;
	cout << "SQL type : " << type << endl;
	cout << "tableName : " << tableName << endl;
	cout << "indexName : " << indexName << endl;
	cout << "insert value : " << content << endl;
	cout << "---------Attribute---------" << endl;
	for (size_t i = 0; i < attributes.size(); i++)
	{
		cout << "name: " << attributes[i].name << endl;
		cout << "type: " << attributes[i].type << endl;
		cout << "length: " << attributes[i].length << endl;
		cout << "isPrimary: " << attributes[i].isPrimaryKey << endl;
		cout << "isUnique: " << attributes[i].isUnique << endl;
	}
	cout << "----------Conditions----------" << endl;
	for (size_t i = 0; i < conditions.size(); i++)
	{
		cout << "attrName : " << conditions[i].attribute.name << endl;
		cout << "relation type : " << conditions[i].relationType << endl;
		cout << "value : " << conditions[i].value << endl;
	}
}
RELATION_TYPE SQLstatement::getRelationType(string oper)
{
	if (oper == "=") return EQUAL;
	else if (oper == ">") return GREATER;
	else if (oper == "<") return SMALLER;
	else if (oper == ">=") return GREATER_EQUAL;
	else if (oper == "<=") return SMALLER_EQUAL;
	else if (oper == "!=") return NOT_EQUAL;
	// 有个坑 ： 操作符不合法没有处理；					TO-DO
}
Condition SQLstatement::genCondition(string exp)
{
	string op = "<>=!";
	int front = exp.find_first_of(op, 0);
	int end = exp.find_first_not_of(op, front);

	string attrName = exp.substr(0, front);
	RELATION_TYPE relationType = getRelationType(exp.substr(front, end - front));
	string value = exp.substr(end);

	//cout << attr << endl << oper << endl << value << endl;
	return Condition(Attribute(attrName), value, relationType);
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
		end = SQL.find_first_of(' ', start);
		tableName = SQL.substr(start, end - start);
		start = end + 1;

		while (true)
		{
			if (!start) break;
			end = SQL.find_first_of(',', start);
			temp = SQL.substr(start, end - start);
			// select属性添加
			attributes.push_back(Attribute(temp));
			start = end + 1;
		}
		break;
	case SELECT_WHERE:
		int conditionstart;	// 记录条件开始位置
		end = SQL.find_first_of(' ', start);
		tableName = SQL.substr(start, end - start);
		start = end + 1;
		conditionstart = SQL.find_first_of(' ', start);
		//cout << conditionstart << endl;

		// 属性解析
		while (true)
		{
			if (!start) break;
			end = SQL.find_first_of(',', start);
			if (end == -1)
				temp = SQL.substr(start, conditionstart - start);
			else
				temp = SQL.substr(start, end - start);
			// select属性添加
			attributes.push_back(Attribute(temp));
			start = end + 1;
		}

		// 条件解析
		start = conditionstart + 1;
		while (true)
		{
			if (!start) break;	// 条件取完
			end = SQL.find_first_of("&|", start);
			temp = SQL.substr(start, end - start);	// 取条件

			// 生成并放入vector
			conditions.push_back(genCondition(temp));

			start = end + 1;
		}
		break;
	case INSERT:
		end = SQL.find_first_of(' ', start);
		tableName = SQL.substr(start, end - start);
		start = end + 1;	// 读取表名
		// insert内容
		content = SQL.substr(start);
		break;
	case DELETE:
		tableName = SQL.substr(start);
		// 读取表名
		break;
	case DELETE_WHERE:
		end = SQL.find_first_of(' ', start);
		tableName = SQL.substr(start, end - start);
		start = end + 1; 
		
		// where条件生成
		while (true)
		{
			if (!start) break;	// 条件取完
			end = SQL.find_first_of("&|", start);
			temp = SQL.substr(start, end - start);	// 取条件

			// 生成并放入vector
			conditions.push_back(genCondition(temp));

			start = end + 1;
		}
		break;
	case CREATE_DATABASE:break;
	case CREATE_TABLE:
		end = SQL.find_first_of(',', start);
		tableName = SQL.substr(start, end - start); 
		start = end + 1;	// 读取表名

		// 属性解析
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
				if (attrtype == MYCHAR)
				{
					length = atoi(SQL.substr(first + 1, second - first - 1).c_str());
				}
				attributes.push_back(Attribute(attrname, attrtype, length, false, attrisUnique));
			}
			start = end + 1;
		}
		break;
	case CREATE_INDEX:
		end = SQL.find_first_of(' ', start);
		indexName = SQL.substr(start, end - start);
		start = end + 1;	// 读取index名
		end = SQL.find_first_of(' ', start);
		tableName = SQL.substr(start, end - start);
		start = end + 1;
		attributes.push_back(Attribute(SQL.substr(start)));
		break;
	case DROP_DATABASE:break;
	case DROP_TABLE:
		tableName = SQL.substr(start);
		// 读取表名
		break;
	case DROP_INDEX:
		indexName = SQL.substr(start);
		// 取得indexName
		break;
	case USE:break;
	case EXECFILE:
		tableName = SQL.substr(start);
		// 用tableName存放执行的文件名
		break;
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
	attributes.clear();
	conditions.clear();
}