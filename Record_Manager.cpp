#include "Record_Manager.h"
#include "Buffer_Manager.h"

extern BufferManager bm;
// 坑1:primaryKey unique

//bool RecordManager::createTable(Table & table)
//{}
bool RecordManager::dropTable(string & tableName)
{
	bm.setInvalid(tableName + ".table");
	remove((tableName + ".table").c_str());
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

int RecordManager::selectWithwhere(Table & table, const vector<Attribute> & attributes, const vector<Condition> & conditions)
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

	vector<string> * cluster = new vector<string>[selectAttrNum];	// 属性值列表的集合		new需要在API中delete
	string * allAttrValuestemp = new string[table.attriNum];		// 用于存放当前行的值		new(deleted)

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

void RecordManager::outputMap(int tupleCount)
{
	//int max = 0;
	//int attrCount = 0;
	//for (auto& result : attributeValuesMap)
	//	attrCount++;
	//for (int i = 0; i < tupleCount; i++)
	//{
	//	int length = 0;
	//	for (auto& result : attributeValuesMap) {
	//		length += result.second[i].length();
	//	}
	//	if (length > max)
	//	{
	//		max = length;
	//	}
	//}
	//max += attrCount << 2;
	//for (int i = 0; i < max; i++)
	//{
	//	cout << '-';
	//}
	//cout << endl;

	for (auto& result : attributeValuesMap) {
		cout << result.first << "\t";
	}
	cout << endl;
	for (int i = 0; i < tupleCount; i++)
	{
		for (auto& result : attributeValuesMap) {
			cout << result.second[i] << "\t";
		}
		cout << endl;
	}
}