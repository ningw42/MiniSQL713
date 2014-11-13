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
				// 调index
				cout << sql.tableName << " created successfully." << endl;
				return true;
			}
			else{
				return false;
			}
		}
	}
	else if (sql.type == CREATE_INDEX){
		Table *t = findTable(sql.tableName);
		if (t){
			if (checkAttribute(t, &sql.attributes)){
				if (createIndex(sql.indexName, t, sql.attributes.data())){
					// 调index;
					return true;
				}
				else{
					return false;
				}
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
	else if (sql.type == DROP_TABLE){
		Table *t = findTable(sql.tableName);
		if (t){
			// 调index、record
			string tn = t->name;
			if (dropTable(t)){
				cout << tn << " dropped successfully." << endl;
				return true;
			}
			else{
				return true;
			}
		}
		else{
			cout << sql.tableName << " not exist." << endl;
			return false;
		}
	}
	else if (sql.type == DROP_INDEX){
		vector<Table>::iterator iter;
		for (iter = tableList.begin(); iter != tableList.end(); iter++){
			if (dropIndex(&(*iter), sql.indexName)){
				// 调index
				return true;
			}
		}
		return false;
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
	bool add = true;
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
	return save_tableInfo(table, add);
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

bool CatalogManager::createIndex(string in, Table *t, Attribute *a)
{
	bool drop = false;
	if (checkIndex(t, in, drop)){
		cout << in << " existed." << endl;
		return false;
	}
	vector<Attribute>::iterator iter;
	for (iter = t->attributes.begin(); iter != t->attributes.end(); iter++){
		if (iter->name == a->name && (iter->isPrimaryKey || iter->isUnique)){
			iter->indexName = in;
			cout << in << " created successfully." << endl;
			return true;
		}
		else {
			cout << iter->name << " not primary or unique." << endl;
			return false;
		}
	}
	return false;
}

bool CatalogManager::checkIndex(Table *t, string in, bool drop)
{
	vector<Attribute>::iterator iter;
	for (iter = t->attributes.begin(); iter != t->attributes.end(); iter++){
		if (iter->indexName == in){
			if (drop){
				iter->indexName = "";
			}
			return true;
		}
	}
	return false;
}

bool CatalogManager::dropIndex(Table *t, string in)
{
	bool drop = true;
	if (checkIndex(t, in, drop)){
		cout << in << " dropped successfully." << endl;
		return true;
	}
	cout << in << " not exist." << endl;
	return false;
}

bool CatalogManager::dropTable(Table *t)
{
	bool add = false;
	vector<Table>::iterator iter;
	for (iter = tableList.begin(); iter != tableList.end();)
	{
		if (iter->name == t->name){
			Table *t = &(*iter);
			iter = tableList.erase(iter);
			if (!save_tableInfo(t, add)){
				return false;
			}
			delete t;
			break;
		}
		else
			iter++;
	}
	return true;
}

bool CatalogManager::save_tableInfo(Table *t, bool add)
{
	if (add){
		ofstream fout(t->name, ios::trunc);
		if (fout){
			vector<Attribute>::iterator iter;
			for (iter = t->attributes.begin(); iter != t->attributes.end(); iter++){
				writeAttribute(t->name, &(*iter));
				fout << t->attriNum << endl;
				fout << t->blockNum << endl;
				fout << t->primaryKey << endl;
				fout << t->recordNum << endl;
				fout << t->tupleLength << endl;
			}
			return true;
		}
		else{
			cout << "open file failed." << endl;
			return false;
		}
	}
	else{
		ofstream fout(t->name, ios::_Nocreate);
		if (fout){
			
		}
		else{
			cout << "open file failed." << endl;
			return false;
		}
		/*vector<Table>::iterator iter;
		for (iter = tl.begin(); iter != tl.end(); iter++){
			ofstream fout(iter->name, ios::);
			vector<Attribute>::iterator iter2;
			for (iter2 = iter->attributes.begin(); iter2 != iter->attributes.end(); iter2++){
				writeAttribute(iter->name, &(*iter2));
				fout << iter->attriNum << endl;
				fout << iter->blockNum << endl;
				fout << iter->primaryKey << endl;
				fout << iter->recordNum << endl;
				fout << iter->tupleLength << endl;
			}
		}*/
	}
}

void CatalogManager::writeAttribute(string fn, Attribute *a)
{
	ofstream fout(fn);
	fout << a->indexName << endl;
	fout << a->isPrimaryKey << endl;
	fout << a->isUnique << endl;
	fout << a->length << endl;
	fout << a->name << endl;
	fout << a->type << endl;
}