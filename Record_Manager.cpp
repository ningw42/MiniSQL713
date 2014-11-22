#include "Record_Manager.h"
#include "Buffer_Manager.h"
#include "Index_Manager.cpp"
#include "Catalog_Manager.h"
#include <time.h>

BufferManager bm;
extern CatalogManager cm;
extern IndexManager<int> imInt;
extern IndexManager<float> imFloat;
extern IndexManager<string> imString;
//bool RecordManager::createTable(Table & table)
//{}
bool RecordManager::dropTable(string & tableName)
{
	bm.setInvalid(tableName + ".table");
	remove(("./bm/" + tableName + ".table").c_str());
	return true;
}

int RecordManager::deleteWithwhere(Table & table, vector<Condition> & conditions)
{
	bm.scanIn(table);
	int deleteLength = table.tupleLength + 1;	// 检查的长度
	int count = 0;								// 删除的行数
	string * allAttrValuestemp = new string[table.attriNum];	// 暂存同行的各属性值		new

	for (int bufferIndex = 0; bufferIndex < MAXBLOCKNUMBER; bufferIndex++)	// 遍历buffer
	{
		if (bm.bufferBlock[bufferIndex].filename == table.name + ".table")	// 属于表的块
		{
			for (int blockIndex = 0; blockIndex <= BLOCKSIZE - deleteLength; blockIndex += deleteLength)	// 有个坑 =号
			{
				int visibleBit = bm.bufferBlock[bufferIndex].value[blockIndex];	// 取得visibleBit
				if (visibleBit == 1)
				{
					int positionOffset = 1;	// 跳过visibleBit
					for (int i = 0; i < table.attriNum; i++)
					{
						string value = toString(bm.bufferBlock[bufferIndex].value + blockIndex + positionOffset, table.attributes[i].length, table.attributes[i].type);
						// 取出每个属性的值 并且string化
						allAttrValuestemp[i] = value;
						positionOffset += table.attributes[i].length;	// 下个属性
					}
					if (satisfy(table.attributes, conditions, allAttrValuestemp))
					{
						// set visable bit to 0(false);
						bm.bufferBlock[bufferIndex].value[blockIndex] = (char)false;
						bm.bufferBlock[bufferIndex].isWritten = true;
						count++;
					}
				}
				else if (visibleBit == 64)	// visibleBit == '@' （空）
				{
					break;
				}
			}
		}
	}
	table.recordNum -= count;
	delete[] allAttrValuestemp;
	return count;
}

int RecordManager::deleteWithoutwhere(Table & table)
{
	bm.scanIn(table);
	int deleteLength = table.tupleLength + 1;	// 检查的长度
	int count = 0;								// 删除的行数

	// read Buffer and set "visable bit" to 0(false);									TO-DO
	for (int bufferIndex = 0; bufferIndex < MAXBLOCKNUMBER; bufferIndex++)	// 遍历buffer
	{
		if (bm.bufferBlock[bufferIndex].filename == table.name + ".table")	// 属于表的块
		{
			for (int blockIndex = 0; blockIndex <= BLOCKSIZE - deleteLength; blockIndex += deleteLength)	// 有个坑 =号
			{
				int visibleBit = bm.bufferBlock[bufferIndex].value[blockIndex];	// 取得visibleBit
				if (visibleBit == 1)
				{
					// set visable bit to 0(false);
					bm.bufferBlock[bufferIndex].value[blockIndex] = (char)false;
					bm.bufferBlock[bufferIndex].isWritten = true;
					count++;
				}
				else if (visibleBit == 64)	// visibleBit == '@' （空）
				{
					break;
				}
			}
		}
	}
	// 维护table的recordNum
	table.recordNum -= count;
	return count;
}

bool RecordManager::insertValue(Table & table, const string & values)
{
	//clock_t time_start = clock();
	bm.scanIn(table);
	//clock_t time_end = clock();
	//cout << (double)(time_end - time_start) / CLOCKS_PER_SEC << endl;
	int writeLength = table.tupleLength + 1;
	char * tempData = new char[writeLength];	// new
	int currentPos = 0;							// 当前tempData的写入位置
	int start = 0;
	int end = 0;

	// primaryKey和Unique检查还没做

	bool visable = true;	// lazy delete 用
	currentPos = copyinto(tempData, (char *)&visable, currentPos, 1);

	for (size_t i = 0; i < table.attributes.size(); i++)
	{
		end = values.find_first_of(',', start);
		string tempValue = values.substr(start, end - start);
		start = end + 1;

		switch (table.attributes[i].type)
		{
		case MYINT:
		{
			int tempInt = stoi(tempValue);
			currentPos = copyinto(tempData, (char *)&tempInt, currentPos, 4);
			break;
		}
		case MYFLOAT:
		{
			float tempFloat = stof(tempValue);
			currentPos = copyinto(tempData, (char *)&tempFloat, currentPos, 4);
			break;
		}
		case MYCHAR:
		{
			string tempString = tempValue.substr(1, tempValue.length() - 2);	// 去引号
			currentPos = copyinto(tempData, tempString.c_str(), currentPos, table.attributes[i].length);	// 可能存在越界
			break;
		}
		default:break;
		}
	}
	//bool visablebit = tempData[0];
	//int intg = *(int *)(tempData + 1);
	//float flo = *(float *)(tempData + 5);
	//cout << visablebit << endl << intg << endl << flo << endl << tempData + 9 << endl;

	// 将tempData写入真正的buffer												
	// 维护table的recordNum


	if (ConstraintCheck(tempData, table))
	{
		insertPos insertPos = bm.getInsertPosition(table);
		int bufferIndex = insertPos.bufferNUM;
		int blockIndex = insertPos.position;
		for (int i = 0; i < writeLength; i++)
		{
			bm.bufferBlock[bufferIndex].value[blockIndex + i] = tempData[i];
		}
		table.recordNum++;
		delete[] tempData;
		return true;
	}
	else
	{
		delete[] tempData;
		return false;
	}
}

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

	vector<string> * cluster = new vector<string>[selectAttrNum];	// 属性值列表的集合		new需要在API中delete
	string * allAttrValuestemp = new string[table.attriNum];		// 用于存放当前行的值		new(deleted)

	Condition cond = indexAvailable(table, conditions);
	if (cond.relationType != UNDEFINED_RELATION)
	{
		string indexName;
		for (size_t i = 0; i < table.attributes.size(); i++)
		{
			if (table.attributes[i].name == cond.attribute.name)
			{
				indexName = table.attributes[i].indexName;
				break;
			}
		}
		Index indexinfo = *cm.findIndex(indexName);
		char * dataBuffer = NULL;
		switch (cond.attribute.type)
		{
		case MYINT:
			dataBuffer = imInt.selectEqual(table, indexinfo, stoi(cond.value));
			break;
		case MYFLOAT:
			dataBuffer = imFloat.selectEqual(table, indexinfo, stof(cond.value));
			break;
		case MYCHAR:
			dataBuffer = imString.selectEqual(table, indexinfo, cond.value.substr(1, cond.value.length() - 2));
			break;
		default:
			break;
		}

		int visibleBit = dataBuffer[0];	// 取得visibleBit
		if (visibleBit == 1)
		{
			int positionOffset = 1;	// 跳过visibleBit
			int count = 0;	// 选择属性的下标
			for (int i = 0; i < table.attriNum; i++)
			{
				string value = toString(dataBuffer + positionOffset, table.attributes[i].length, table.attributes[i].type);
				allAttrValuestemp[i] = value;	// 暂存当前遍历的tuple的属性值
				if (contains(selectAttrbute, table.attributes[i]))
				{
					// 记录是第几个需要的属性
					cluster[count].push_back(value);/*把值放入列表*/
					count++;
				}
				positionOffset += table.attributes[i].length;
			}
		}
		cout << "index" << endl;
		delete dataBuffer;
	}
	else
	{
		bm.scanIn(table);
		for (int bufferIndex = 0; bufferIndex < MAXBLOCKNUMBER; bufferIndex++)	// 遍历buffer
		{
			if (bm.bufferBlock[bufferIndex].filename == table.name + ".table")	// 属于表的块
			{
				for (int blockIndex = 0; blockIndex <= BLOCKSIZE - readSize; blockIndex += readSize)	// 有个坑 =号
				{
					int visibleBit = bm.bufferBlock[bufferIndex].value[blockIndex];	// 取得visibleBit
					if (visibleBit == 1)
					{
						int positionOffset = 1;	// 跳过visibleBit
						int count = 0;	// 选择属性的下标
						for (int i = 0; i < table.attriNum; i++)
						{
							string value = toString(bm.bufferBlock[bufferIndex].value + blockIndex + positionOffset, table.attributes[i].length, table.attributes[i].type);
							allAttrValuestemp[i] = value;	// 暂存当前遍历的tuple的属性值
							if (contains(selectAttrbute, table.attributes[i]))
							{
								// 记录是第几个需要的属性
								cluster[count].push_back(value);/*把值放入列表*/
								count++;
							}
							positionOffset += table.attributes[i].length;
						}

						//for (int i = 0; i < table.attriNum; i++)
						//{
						//	cout << "allAttrValuestemp[" << i << "] = " << allAttrValuestemp[i] << endl;
						//}

						if (!satisfy(table.attributes, conditions, allAttrValuestemp))	// 判断读取的记录是否符合条件			
						{													// 可能存在空vector调用pop_back()		TO-DO
							// 移除不满足条件的tuple（之前无条件放入）
							for (int i = 0; i < selectAttrNum; i++)
							{
								cluster[i].pop_back();
							}
						}
					}
					else if (visibleBit == 64)	// visibleBit == '@' （空）
					{
						break;
					}
				}
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
	bm.scanIn(table);
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

	vector<string> * cluster = new vector<string>[selectAttrNum];	// 属性值列表的集合		new 有待delete

	for (int bufferIndex = 0; bufferIndex < MAXBLOCKNUMBER; bufferIndex++)	// 遍历buffer
	{
		if (bm.bufferBlock[bufferIndex].filename == table.name + ".table")	// 属于表的块
		{
			for (int blockIndex = 0; blockIndex <= BLOCKSIZE - readSize; blockIndex += readSize)	// 有个坑 =号
			{
				int visibleBit = bm.bufferBlock[bufferIndex].value[blockIndex];	// 取得visibleBit
				if (visibleBit == 1)
				{
					int positionOffset = 1;	// 跳过visibleBit
					int count = 0;	// 选择属性的下标
					for (int i = 0; i < table.attriNum; i++)
					{
						string value = toString(bm.bufferBlock[bufferIndex].value + blockIndex + positionOffset, table.attributes[i].length, table.attributes[i].type);
						if (contains(selectAttrbute, table.attributes[i]))
						{
							// 记录是第几个需要的属性
							cluster[count].push_back(value);/*把值放入列表*/
							count++;
						}
						positionOffset += table.attributes[i].length;
					}
				}
				else if (visibleBit == 64)	// visibleBit == '@' （空）
				{
					break;
				}
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


bool RecordManager::hasIndex(const Table & table, const string & attrName)
{
	for (size_t i = 0; i < table.attributes.size(); i++)
	{
		if (table.attributes[i].name == attrName && table.attributes[i].indexName != "")
			return true;
	}
	return false;
}

Condition RecordManager::indexAvailable(const Table & table, const vector<Condition> & conditions)
{
	for (size_t i = 0; i < conditions.size(); i++)
	{
		if (conditions[i].relationType == EQUAL && hasIndex(table, conditions[i].attribute.name))
		{
			return conditions[i];
		}
	}
	return Condition(Attribute(), "", UNDEFINED_RELATION);
}

bool RecordManager::isExist(char * data, int currentPos, int length, TYPE type, const Table & table)
{
	// 字符化后比较
	string key = toString(data + currentPos, length, type);
	int readSize = table.tupleLength + 1;

	for (int bufferIndex = 0; bufferIndex < MAXBLOCKNUMBER; bufferIndex++)	// 遍历buffer
	{
		if (bm.bufferBlock[bufferIndex].filename == table.name + ".table")	// 属于表的块
		{
			for (int blockIndex = 0; blockIndex <= BLOCKSIZE - readSize; blockIndex += readSize)	// 有个坑 =号
			{
				int visibleBit = bm.bufferBlock[bufferIndex].value[blockIndex];	// 取得visibleBit
				if (visibleBit)
				{
					string existValue = toString(bm.bufferBlock[bufferIndex].value + blockIndex + currentPos, length, type);
					if (existValue == key)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool RecordManager::ConstraintCheck(char * tempData, const Table & table)
{
	int currentPos = 1;
	for (size_t i = 0; i < table.attributes.size(); i++)
	{
		bool isUnique = table.attributes[i].isUnique;
		bool isPrimay = table.attributes[i].isPrimaryKey;
		int length = table.attributes[i].length;
		TYPE type = table.attributes[i].type;
		if (isPrimay)
		{
			if (isExist(tempData, currentPos, length, type, table))
			{
				cout << table.attributes[i].name << " is primary key. " << toString(tempData + currentPos, length, type) << " can't be inserted" << endl;
				return false;
			}
		}
		else if (isUnique)
		{
			if (isExist(tempData, currentPos, length, type, table))
			{
				cout << table.attributes[i].name << " is unique. " << toString(tempData + currentPos, length, type) << " can't be inserted" << endl;
				return false;
			}
		}
		currentPos += table.attributes[i].length;
	}
	return true;
}

string RecordManager::toString(const char * source, int length, TYPE type)
{
	string value = "";
	switch (type)
	{
	case MYINT:
		value = to_string(*((int *)source));
		break;
	case MYFLOAT:
		value = to_string(*((float *)source));
		break;
	case MYCHAR:
		value = string(source);
		break;
	default:
		break;
	}
	return value;
}

int RecordManager::copyinto(char * buffer, const char * from, int start, int length)
{
	int i;
	for (i = 0; i < length; i++)
	{
		buffer[i + start] = from[i];
	}
	return i + start;
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
			return i + 1;
		}
	}
	return 0;
}

int RecordManager::findIndexOf(const vector<Attribute> & attributes, const Condition & condition)
{
	for (size_t i = 0; i < attributes.size(); i++)
	{
		if (attributes[i].name == condition.attribute.name)
		{
			return i;
		}
	}
	return 0;
	// 有坑
}

bool RecordManager::satisfy(const vector<Attribute> & attributes, const vector<Condition> & conditions, const string * allAttrValues)
{
	//for (size_t i = 0; i < attributes.size(); i++)
	//{
	//	if (int indexofcond = hasCondition(conditions, attributes[i].name))
	//	{
	//		if (!satisfy(conditions[indexofcond - 1], allAttrValues[i], attributes[i].type))	// 若conditions维护了对应attribute的属性，则不需要第三个参数
	//		{
	//			return false;
	//		}
	//	}
	//}
	//return true;

	for (size_t i = 0; i < conditions.size(); i++)
	{
		int attrIndex = findIndexOf(attributes, conditions[i]);
		if ((!satisfy(conditions[i], allAttrValues[attrIndex], attributes[attrIndex].type)))
		{
			return false;
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
			if (value == cond.value.substr(1, cond.value.length() - 2))
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
			if (value != cond.value.substr(1, cond.value.length() - 2))
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
			if (value > cond.value.substr(1, cond.value.length() - 2))
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
			if (value >= cond.value.substr(1, cond.value.length() - 2))
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
			if (value < cond.value.substr(1, cond.value.length() - 2))
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
			if (value <= cond.value.substr(1, cond.value.length() - 2))
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

void RecordManager::outputMap(int tupleCount, const Table & table)
{
	int max = 0;
	int attrCount = table.attriNum;
	for (int i = 0; i < tupleCount; i++)
	{
		int length = 0;
		for (auto& result : attributeValuesMap) {
			length += result.second[i].length();
		}
		if (length > max)
		{
			max = length;
		}
	}
	int length = max * attrCount;
	for (int i = 0; i < length; i++)
	{
		cout << '-';
	}
	cout << endl;

	for (auto& result : attributeValuesMap) {
		cout << "\t" << result.first << "\t";
	}
	cout << endl;
	for (int i = 0; i < tupleCount; i++)
	{
		for (auto& result : attributeValuesMap) {
			cout << "\t"  << result.second[i] << "\t";
		}
		cout << endl;
	}
}