#include "Catalog_Manager.h"

bool CatalogManager::API_Catalog(SQLstatement sql)
{
	if (sql.type == CREATE_TABLE){
		if (findTable(sql.tableName)){
			cout << sql.tableName << " existed." << endl;
			return false;
		}
		else{
			if (createTable(sql)){
				cout << sql.tableName << " created successfully." << endl;
				return true;
			}
			else{
				cout << "created failed." << endl;
				return false;
			}
		}
	}
	else if (sql.type == CREATE_INDEX){
		Table *t = findTable(sql.tableName);
		if (t){
			// µ÷index
			// createindex();
		}
		else{
			cout << sql.tableName << " not exist." << endl;
			return false;
		}
	}
	else if (sql.type == DROP_TABLE){
		Table *t = findTable(sql.tableName);
		if (t){
			// µ÷index
			// droptable();
		}
		else{
			cout << sql.tableName << " not exist." << endl;
			return false;
		}
	}
	else if (sql.type == DROP_INDEX){
		Table *t = findTable(sql.tableName);
		if (t){
			// µ÷index
			// droptable();
		}
		else{
			cout << sql.tableName << " not exist." << endl;
			return false;
		}
	}
	else if (sql.type == SELECT){
		Table *t = findTable(sql.tableName);
		if (t){
			if (checkAttribute(t, &sql.attributes)){
				return true;
			}
			else{
				return false;
			}
		}
		else{
			cout << sql.tableName << " not exist." << endl;
			return false;
		}
	}
	else if (sql.type == SELECT_WHERE){
		Table *t = findTable(sql.tableName);
		if (t){
			if (checkAttribute(t, &sql.attributes) && checkCondition(t, &sql.conditions)){
				return true;
			}
			else{
				return false;
			}
		}
		else{
			cout << sql.tableName << " not exist." << endl;
			return false;
		}
	}
	else if (sql.type == INSERT){
		Table *t = findTable(sql.tableName);
		if (t){
			if (checkInsert(t, sql.content)){
				return true;
			}
			else{
				return false;
			}
		}
		else{
			cout << sql.tableName << " not exist." << endl;
			return false;
		}
	}
	else if (sql.type == DELETE){
		Table *t = findTable(sql.tableName);
		if (t){
			return true;
		}
		else{
			cout << sql.tableName << " not exist." << endl;
			return false;
		}
	}
	else if (sql.type == DELETE_WHERE){
		Table *t = findTable(sql.tableName);
		if (t){
			if (checkCondition(t, &sql.conditions)){
				return true;
			}
			else{
				return false;
			}
		}
		else{
			cout << sql.tableName << " not exist." << endl;
			return false;
		}
	}
}

Table* CatalogManager::findTable(string tn)
{
	vector<Table> ::iterator iter;
	for (iter = tableList.begin(); iter != tableList.end(); iter++)
	{
		if (iter->name == tn)
		{
			return &(*iter);
		}
	}
	return NULL;
}

bool CatalogManager::createTable(SQLstatement sql)
{
	Table *table = new Table();
	table->attributes = sql.attributes;
	vector<Attribute>::iterator iter;
	for (iter = table->attributes.begin(); iter != table->attributes.end(); iter++)
	{
		table->attriNum++;
		table->tupleLength += iter->length;
		if (iter->isPrimaryKey){
			table->primaryKey = iter->name;
		}
	}
	table->name = sql.tableName;
	pushBack_tableList(*table);
	update_tableNum();
	return save_tableInfo(tableList);
}

bool CatalogManager::checkInsert(Table *t, string value)
{
	int vn = 1;
	int start = 0;
	int end;
	while ((end = value.find_first_of(",", start)) > 0){;
		vn++;
		start = end + 1;
	}
	if (t->attriNum == vn){
		if (vn == 1){
			return checkType(t->attributes.data(), value);
		}
		vector<Attribute>::iterator iter;
		start = 0;
		for (iter = t->attributes.begin(); iter != t->attributes.end(); iter++)
		{
			end = value.find_first_of(",", start);
			if (end == string::npos){
				end = value.length();
			}
			string eachv = value.substr(start, end - start);
			start = end + 1;
			if (!checkType(&(*iter), eachv)){
				cout << "wrong type." << endl;
				return false;
			}
		}
		return true;
	}
	else{
		cout << "wrong values number." << endl;
		return false;
	}
}

bool CatalogManager::checkAttribute(Table *t, vector<Attribute> *a)
{
	vector<Attribute>::iterator iter;
	for (iter = a->begin(); iter != a->end(); iter++){
		if (iter->name == "*"){
			return true;
		}
		vector<Attribute>::iterator iter2;
		bool flag = false;
		for (iter2 = t->attributes.begin(); iter2 != t->attributes.end(); iter2++){
			if (iter->name == iter2->name){
				flag = true;
				break;
			}
		}
		if (flag == false){
			cout << "wrong attribute." << endl;
			return false;
		}
	}
	return true;
}

bool CatalogManager::checkCondition(Table *t, vector<Condition> *c)
{
	vector<Condition>::iterator iter;
	for (iter = c->begin(); iter != c->end(); iter++){
		vector<Attribute>::iterator iter2;
		bool flag = false;
		for (iter2 = t->attributes.begin(); iter2 != t->attributes.end(); iter2++){
			if (iter->attribute.name == iter2->name){
				flag = true;
				break;
			}
		}
		if (flag == false){
			cout << "wrong condition." << endl;
			return false;
		}
	}
	return true;
}

void CatalogManager::pushBack_tableList(Table &t)
{
	tableList.push_back(t);
}

void CatalogManager::update_tableNum()
{
	tableNum++;
}

bool CatalogManager::save_tableInfo(vector<Table> &tl)
{
	return true;
}

bool CatalogManager::checkType(Attribute *a, string v)
{
	TYPE vt;
	if (v.find("'") == 0)
		vt = CHAR;
	else if (v.find('.') != string::npos)
		vt = FLOAT;
	else
		vt = INT;
	if (a->type == vt){
		return true;
	}
	else
		return false;
}