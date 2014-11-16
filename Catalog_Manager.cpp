#include "Catalog_Manager.h"
#include "dirent.h"

bool CatalogManager::API_Catalog(SQLstatement &sql)
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
			msg = t->name;
			if (dropTable(t)){
				cout << msg << " dropped successfully." << endl;
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
	else if (sql.type == MYDELETE){
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

bool CatalogManager::createTable(SQLstatement &sql)
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
	update_tableNum(add);
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
				if (checkType(&(*iter2), iter->value)){
					iter->attribute.type = iter2->type;
					break;
				}
				else{
					cout << "wrong value type." << endl;
					return false;
				}
			}
		}
		if (flag == false){
			cout << "wrong attribute name." << endl;
			return false;
		}
	}
	return true;
}

void CatalogManager::pushBack_tableList(Table &t)
{
	tableList.push_back(t);
}

void CatalogManager::update_tableNum(bool add)
{
	if (add){
		tableNum++;
	}
	else{
		tableNum--;
	}
}

bool CatalogManager::checkType(Attribute *a, string v)
{
	TYPE vt;
	if ((v.find("'") == 0) && (v.find_last_of("'") == v.length() - 1))
		vt = TYPE::MYCHAR;
	else if (v.find('.') != string::npos)
		vt = TYPE::MYFLOAT;
	else
		vt = TYPE::MYINT;
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
			if (!save_tableInfo(t, add)){
				return false;
			}
			iter = tableList.erase(iter);
			update_tableNum(add);
			break;
		}
		else
			iter++;
	} // 误删SQLstatement对象attribute造成段错误
	return true;
}

bool CatalogManager::save_tableInfo(Table *t, bool add)
{
	if (add){
		ofstream fout;
		fout.open("./cm/" + t->name, ios::trunc);
		if (fout){
			vector<Attribute>::iterator iter;
			fout.close();
			for (iter = t->attributes.begin(); iter != t->attributes.end(); iter++){
				writeAttribute(t->name, &(*iter));
			}
			fout.open("./cm/" + t->name, ios::_Nocreate | ios::app);
			fout << ";" << endl;
			fout << t->attriNum << endl;
			fout << t->blockNum << endl;
			fout << t->primaryKey << endl;
			fout << t->recordNum << endl;
			fout << t->tupleLength << endl;
			fout.close();
			return true;
		}
		else{
			cout << "open file failed." << endl;
			fout.close();
			return false;
		}
	}
	else{
		if (remove(("./cm/" + t->name).c_str())){
			cout << t->name << " not exist." << endl;
			return false;
		}
		else{
			cout << t->name << " removed." << endl;
			return true;
		}
	}
}

void CatalogManager::writeAttribute(string fn, Attribute *a)
{
	ofstream fout;
	fout.open("./cm/" + fn, ios::_Nocreate | ios::app);
	if (fout){
		fout << ":" << endl;
		fout << a->indexName << endl;
		fout << (a->isPrimaryKey ? "1" : "0") << endl;
		fout << (a->isUnique ? "1":"0") << endl;
		fout << a->length << endl;
		fout << a->name << endl;
		fout << static_cast<int>(a->type) << endl;
	}
	else{
		cout << fn << "not exist." << endl;
	}
	fout.close();
}

void CatalogManager::read_TableInfo()
{
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir("./cm/")) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			string fn(ent->d_name);
			ifstream fin("./cm/" + fn, ios::_Nocreate);
			if (fin){
				vector<Attribute> va;
				Table *t = new Table();
				string s;
				getline(fin, s);
				if (s == ""){
					cout << "wrong file." << endl;
					exit(0);
				}
				while (s == ":"){
					Attribute *a = new Attribute();
					getline(fin, a->indexName);
					getline(fin, s);
					a->isPrimaryKey = (s == "1") ? true : false;
					getline(fin, s);
					a->isUnique = (s == "1") ? true : false;
					getline(fin, s);
					a->length = stoi(s);
					getline(fin, a->name);
					getline(fin, s);
					a->type = TYPE(stoi(s));
					getline(fin, s);
					va.push_back(*a);
				}
				t->attributes = va;
				vector<Attribute>(va).swap(va);
				getline(fin, s);
				t->attriNum = stoi(s);
				getline(fin, s);
				t->blockNum = stoi(s);
				t->name = fn;
				getline(fin, t->primaryKey);
				getline(fin, s);
				t->recordNum = stoi(s);
				getline(fin, s);
				t->tupleLength = stoi(s);
				tableList.push_back(*t);
			}
		}
		closedir(dir);
	}
	else {
		cout << "directory cm not exist." << endl;
	}
	tableNum = tableList.size();
}

void CatalogManager::update_tableInfo()
{
	bool add = true;
	vector<Table>::iterator iter;
	for (iter = tableList.begin(); iter != tableList.end(); iter++){
		save_tableInfo(&(*iter), add);
	}
}