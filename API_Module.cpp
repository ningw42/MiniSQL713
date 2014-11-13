#include "API_Module.h"
#include "Catalog_Manager.h"
#include "Interpreter.h"

extern CatalogManager cm;

void APIMoudule::API(SQLstatement &s)
{
	if (s.type == CREATE_TABLE){
		if (cm.API_Catalog(s)){
			// 调record
		}
		else{
			cout << "create table failed" << endl;
		}
	}
	else if (s.type == CREATE_INDEX){
		if (cm.API_Catalog(s)){
			// 调index
		}
		else{
			cout << "create index failed" << endl;
		}
	}
	else if (s.type == DROP_TABLE){
		if (cm.API_Catalog(s)){
			// 调index和record
		}
		else{
			cout << "drop table failed" << endl;
		}
	}
	else if (s.type == DROP_INDEX){
		if (cm.API_Catalog(s)){
			// 调index
		}
		else{
			cout << "create index failed" << endl;
		}
	}
	else if (s.type == SELECT){
		if (cm.API_Catalog(s)){
			cout << "right select statement." << endl;
			// 调index和record
		}
		else{
			cout << "select failed." << endl;
		}
	}
	else if (s.type == SELECT_WHERE){
		if (cm.API_Catalog(s)){
			cout << "right select where statement." << endl;
			// 调index和record
		}
		else{
			cout << "select failed." << endl;
		}
	}
	else if (s.type == INSERT){
		if (cm.API_Catalog(s)){
			cout << "right insert statement." << endl;
			// 调record和index
		}
		else{
			cout << "insert failed." << endl;
		}
	}
	else if (s.type == DELETE){
		if (cm.API_Catalog(s)){
			cout << "right delete statement." << endl;
			// 调record和index
		}
		else{
			cout << "insert failed." << endl;
		}
	}
	else if (s.type == DELETE_WHERE){
		if (cm.API_Catalog(s)){
			cout << "right delete where statement." << endl;
			// 调record和index
		}
		else{
			cout << "insert failed." << endl;
		}
	}
	else if (s.type == EXECFILE){
		fstream fin(s.tableName);
		if (fin){
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
		else{
			cout << "no such file." << endl;
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