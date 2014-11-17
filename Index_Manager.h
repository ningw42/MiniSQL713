#ifndef __INDEX_MANAGER_H__
#define __INDEX_MANAGER_H__

#define EMPTY '@'


#include "MiniSQL.h"


enum PointerType{ FATHER, LEFT, RIGHT };

template <class KEY>
class Data{
public:
	KEY key;
	int BlockInFile;
	int Offset;
	Data() : key(-65535), BlockInFile(0), Offset(0){}
	Data(KEY Key, int block, int off) : key(Key), BlockInFile(block), Offset(off){}
};
template <class KEY>
class InnerData{
public:
	KEY key;
	int data;
	InnerData() :key(-65535), data(0){}
	InnerData(KEY k, int ptrC) :key(k), data(ptrC){}
};
template <class KEY>
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
template <class KEY>
class InnerNode : public Node<KEY>{
public:
	list<InnerData<KEY>>nodelist;
	InnerNode();
	InnerNode(int bufferNum) :Node(bufferNum){}
	InnerNode(int bufferNumber, const Index& index);
	~InnerNode();
	void insert(InnerData<KEY> data);
	InnerData<KEY> pop();
	InnerData<KEY> getfront();
	int getrecordNum(int buffernum);
};
template <class KEY>
class Leaf :public Node<KEY>
{
public:
	int nextSibling;
	int lastSibling;
	list<Data<KEY>> nodelist;
	Leaf(int bufnum);
	Leaf(int bufnum, const Index& indexinfo);
	~Leaf();
	void insert(Data<KEY> data);
	Data pop();
	Data getfront();
};
template <class KEY>
class IndexManager{
public:
	void createIndex(Table & tableinfo, Index & indexinfo);
	InnerData<KEY> insertValue(Index& indexinfor, Data<KEY> node, int blockOffset = 0);
	char * selectEqual(const Table& tableinfor, const Index& indexinfor, KEY key, int blockOffset = 0);
	void dropIndex(Index& indexinfor);
};
#endif