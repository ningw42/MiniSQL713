#include "Record_Manager.h"

//bool RecordManager::createTable(Table & table)
//{}
//bool RecordManager::insertValue(Table & table, string values)
//{}
//bool RecordManager::dropTable(Table & table)
//{}
//bool RecordManager::deleteWithwhere(Table & table, vector<Condition> & conditions)
//{}
//bool RecordManager::deleteWithoutwhere(Table & table)
//{}
int RecordManager::selectWithwhere(Table & table, const vector<Attribute> & attributes, const vector<Condition> & conditions)
{
	attributeValuesMap.clear();				// 清空返回列表					可能存在内存泄露		TO-DO
	int readSize = table.tupleLength + 1;	// tuple长度 + 一位lazy delete位
	int selectAttrNum;						// 选择属性个数
	vector<Attribute> selectAttrbute;		// 选取属性列表

	if (attributes[0].name == "*")		
	{
		// 全选的情况
		selectAttrNum = table.attributes.size();
		selectAttrbute = table.attributes;
	}
	else
	{
		// 非全选
		selectAttrNum = attributes.size();
		selectAttrbute = attributes;
	}

	vector<string> * cluster = new vector<string>[selectAttrNum];	// 属性值列表的集合		new
	string * allAttrValuestemp = new string[table.attriNum];		// 用于					new(deleted)

	for (/*read from start to end*/;;)
	{
		// read tuple here
		for (int i = 0; i < table.attriNum; i++)
		{
			int count = 0;	// 选择属性的下标
			string value;	// 取出每个属性的值 并且string化
			
			allAttrValuestemp[i] = value;	// 暂存当前遍历的tuple的属性值
			if (contains(selectAttrbute, table.attributes[i]))
			{
				// 记录是第几个需要的属性
				cluster[count].push_back(/*把值放入列表*/);
				count++;
			}
		}
		if (!satisfy(table.attributes, conditions, allAttrValuestemp))	// 判断读取的记录是否符合条件			
		{													// 可能存在空vector调用pop_back()		TO-DO
			// 移除不满足条件的tuple（之前无条件放入）
			for (int i = 0; i < selectAttrNum; i++)
			{
				cluster[i].pop_back();
			}
		}
	}

	if (cluster[0].size())	// 有选出的属性
	{
		for (int i = 0; i < selectAttrNum; i++)
		{
			attributeValuesMap.insert(std::pair<string, vector<string> >(selectAttrbute[i].name, cluster[i]));
		}
		delete[] allAttrValuestemp;
		return cluster[0].size();
	}
	else
	{
		delete[] cluster;
		delete[] allAttrValuestemp;
		return 0;
	}
}

int RecordManager::selectWithoutwhere(Table & table, const vector<Attribute> & attributes)
{
	attributeValuesMap.clear();				// 清空返回列表					可能存在内存泄露		TO-DO
	int readSize = table.tupleLength + 1;	// tuple长度 + 一位lazy delete位
	int selectAttrNum;						// 选择属性个数
	vector<Attribute> selectAttrbute;		// 选取属性列表

	if (attributes[0].name == "*")
	{
		// 全选的情况
		selectAttrNum = table.attributes.size();
		selectAttrbute = table.attributes;
	}
	else
	{
		// 非全选
		selectAttrNum = attributes.size();
		selectAttrbute = attributes;
	}

	vector<string> * cluster = new vector<string>[selectAttrNum];	// 属性值列表的集合		new
	char * reader = new char[readSize];								// 读取buffer			new

	for (/*read from start to end*/;;)
	{
		// read tuple here
		for (int i = 0; i < table.attriNum; i++)
		{
			// 取出每个属性的值
			int count = 0;
			if (contains(selectAttrbute, table.attributes[i]))
			{
				// 记录是第几个需要的属性
				cluster[count].push_back(/*把值放入列表*/);
				count++;
			}
		}
	}

	if (cluster[0].size())	// 有选出的属性
	{
		for (int i = 0; i < selectAttrNum; i++)
		{
			attributeValuesMap.insert(std::pair<string, vector<string> >(selectAttrbute[i].name, cluster[i]));
		}
		return cluster[0].size();
	}
	else
	{
		delete[] cluster;
		return 0;
	}
}

bool RecordManager::contains(const vector<Attribute> & attributes, const Attribute & attr)
{
	for (size_t i = 0; i < attributes.size(); i++)
	{
		if (attributes[i].name == attr.name)
		{
			return true;
		}
	}
	return false;
}

int RecordManager::hasCondition(const vector<Condition> & conditions, const string & attrName)
{
	for (size_t i = 0; i < conditions.size(); i++)
	{
		if (conditions[i].attribute.name == attrName)
		{
			return i;
		}
	}
	return 0;
}

bool RecordManager::satisfy(const vector<Attribute> & attributes, const vector<Condition> & conditions, const string * allAttrValues)
{
	for (size_t i = 0; i < attributes.size(); i++)
	{
		if (int indexofcond = hasCondition(conditions, attributes[i].name))
		{
			if (!satisfy(conditions[indexofcond], allAttrValues[i], attributes[i].type))	// 若conditions维护了对应attribute的属性，则不需要第三个参数
			{
				return false;
			}
		}
	}
	return true;
}

bool RecordManager::satisfy(const Condition & cond, const string & value, const TYPE type)
{
	switch (cond.relationType)
	{
	case EQUAL:
		switch (type)
		{
		case MYINT:
			if (atoi(value.c_str()) == atoi(cond.value.c_str()))
				return true;
			else
				return false;
			break;
		case MYFLOAT:
			if (atof(value.c_str()) == atof(cond.value.c_str()))
				return true;
			else
				return false;
			break;
		case MYCHAR:
			if (value == cond.value)
				return true;
			else
				return false;
			break;
		default:break;
		}
		break;
	case NOT_EQUAL:
		switch (type)
		{
		case MYINT:
			if (atoi(value.c_str()) != atoi(cond.value.c_str()))
				return true;
			else
				return false;
			break;
		case MYFLOAT:
			if (atof(value.c_str()) != atof(cond.value.c_str()))
				return true;
			else
				return false;
			break;
		case MYCHAR:
			if (value != cond.value)
				return true;
			else
				return false;
			break;
		default:break;
		}
		break;
	case GREATER:
		switch (type)
		{
		case MYINT:
			if (atoi(value.c_str()) > atoi(cond.value.c_str()))
				return true;
			else
				return false;
			break;
		case MYFLOAT:
			if (atof(value.c_str()) > atof(cond.value.c_str()))
				return true;
			else
				return false;
			break;
		case MYCHAR:
			if (value > cond.value)
				return true;
			else
				return false;
			break;
		default:break;
		}
		break;
	case GREATER_EQUAL:
		switch (type)
		{
		case MYINT:
			if (atoi(value.c_str()) >= atoi(cond.value.c_str()))
				return true;
			else
				return false;
			break;
		case MYFLOAT:
			if (atof(value.c_str()) >= atof(cond.value.c_str()))
				return true;
			else
				return false;
			break;
		case MYCHAR:
			if (value >= cond.value)
				return true;
			else
				return false;
			break;
		default:break;
		}
		break;
	case SMALLER:
		switch (type)
		{
		case MYINT:
			if (atoi(value.c_str()) < atoi(cond.value.c_str()))
				return true;
			else
				return false;
			break;
		case MYFLOAT:
			if (atof(value.c_str()) < atof(cond.value.c_str()))
				return true;
			else
				return false;
			break;
		case MYCHAR:
			if (value < cond.value)
				return true;
			else
				return false;
			break;
		default:break;
		}
		break;
	case SMALLER_EQUAL:
		switch (type)
		{
		case MYINT:
			if (atoi(value.c_str()) <= atoi(cond.value.c_str()))
				return true;
			else
				return false;
			break;
		case MYFLOAT:
			if (atof(value.c_str()) <= atof(cond.value.c_str()))
				return true;
			else
				return false;
			break;
		case MYCHAR:
			if (value <= cond.value)
				return true;
			else
				return false;
			break;
		default:break;
		}
		break;
	default:break;
	}
}
