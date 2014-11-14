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
	attributeValuesMap.clear();			// 清空返回列表					可能存在内存泄露		TO-DO
	int readSize = table.tupleLength;	// tuple长度
	int selectAttrNum;					// 选择属性个数
	vector<Attribute> selectAttrbute;	// 选取属性列表

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

	for (/*read from start to end*/;;)
	{
		// read tuple here
		for (int i = 0; i < table.attriNum; i++)
		{
			// 取出每个属性的值
			int count = 0;	// 选择属性的下标
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

int RecordManager::selectWithoutwhere(Table & table, const vector<Attribute> & attributes)
{
	attributeValuesMap.clear();			// 清空返回列表					可能存在内存泄露		TO-DO
	int readSize = table.tupleLength;	// tuple长度
	int selectAttrNum;					// 选择属性个数
	vector<Attribute> selectAttrbute;	// 选取属性列表

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