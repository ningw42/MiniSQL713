#include "API_Module.h"
#include "Interpreter.h"
#include "Catalog_Manager.h"

extern CatalogManager cm;

void APIMoudule::API(SQLstatement s)
{
	if (s.type == CREATE_DATABASE){

	}
	else if (s.type == CREATE_TABLE){
		cm.API_Catalog(s);
	}
	else if (s.type == CREATE_INDEX){

	}
	else if (s.type == DROP_DATABASE){

	}
	else if (s.type == DROP_TABLE){

	}
	else if (s.type == DROP_INDEX){

	}
	else if (s.type == SELECT){

	}
	else if (s.type == SELECT_WHERE){

	}
	else if (s.type == INSERT){
		cm.API_Catalog(s);
	}
	else if (s.type == DELETE){

	}
	else if (s.type == DELETE_WHERE){

	}
	else if (s.type == USE){

	}
	else if (s.type == EXECFILE){
		fstream fin("sql.txt");
		string sql;
		string temp;
		bool finish = false;
		sql = "";
		while (fin >> temp){
			sql = sql + ' ' + temp;
			if (sql.at(sql.length() - 1) == ';'){
				sql.erase(0, 1);
				sql = lower(sql);
				cout << sql << endl;
				sql = Interpreter(sql);
				SQLstatement s(sql);
				API(s);
				sql = "";
			}
		}
	}
	else if (s.type == QUIT){
		exit(0);
	}
	else if (s.type == HELP){
		getHelp();
	}
	else if (s.type == ILLEGAL){
		cout << "wrong SQL statement." << endl;
	}
}

void APIMoudule::getHelp()
{
	fstream fin("help.txt");
	string s;
	while (getline(fin, s))
	{
		cout << s << endl;
	}
}

//获取用户输入
string APIMoudule::read_input()
{
	string SQL;
	string temp;
	bool finish = false;
	SQL = "";
	while (!finish)
	{
		cin >> temp;
		SQL = SQL + ' ' + temp;
		if (SQL.at(SQL.length() - 1) == ';'){
			SQL.erase(0, 1);
			finish = true;
		}
	}
	//将输入大写转化为小写
	SQL = lower(SQL);
	//返回用户输入
	return SQL;
}

//改为小写
string APIMoudule::lower(string s)
{
	transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}