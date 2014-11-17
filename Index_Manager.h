#ifndef __INDEX_MANAGER_H__
#define __INDEX_MANAGER_H__

#define EMPTY '@'
#define KEY int

#include "MiniSQL.h"

enum PointerType{ FATHER, LEFT, RIGHT };

class Data{
public:
	KEY key;
	int BlockInFile;
	int Offset;
	Data() : key(-65535), BlockInFile(0), Offset(0){}
	Data(KEY Key, int block, int off) : key(Key), BlockInFile(block), Offset(off){}
};

class InnerData{
public:
	KEY key;
	int data;
	InnerData() :key(-65535), data(0){}
	InnerData(KEY k, int ptrC) :key(k), data(ptrC){}
};

class Node{
public:
	int bufferNum;
	int pointFather;
	int recordNum;
	int columnLength;
	bool Root;
	Node(){}
	Node(int bufferNum) :bufferNum(bufferNum){}
	bool isRoot(int buffernum);
	int getRecordNum(int bufferNum);
	int getPointer(PointerType pp);
};

class InnerNode : public Node{
public:
	list<InnerData>nodelist;
	InnerNode();
	InnerNode(int bufferNum) :Node(bufferNum){}
	InnerNode(int bufferNumber, const Index& index);
	~InnerNode();
	void insert(InnerData data);
	InnerData pop();
	InnerData getfront();
	int getrecordNum(int buffernum);
};

class Leaf :public Node
{
public:
	int nextSibling;
	int lastSibling;
	list<Data> nodelist;
	Leaf(int bufnum);
	Leaf(int bufnum, const Index& indexinfo);
	~Leaf();
	void insert(Data data);
	Data pop();
	Data getfront();
};

class IndexManager{
public:
	void createIndex(Table & tableinfo, Index & indexinfo);
	InnerData insertValue(Index& indexinfor, Data node, int blockOffset = 0);
	char * selectEqual(const Table& tableinfor, const Index& indexinfor, KEY key, int blockOffset = 0);
	void dropIndex(Index& indexinfor);
};
#endif