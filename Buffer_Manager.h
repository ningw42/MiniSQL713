#ifndef __BufferManager_H__
#define __BufferManager_H__

#define BLOCKSIZE 4096
#define MAXBLOCKNUMBER 1000

#include "MiniSQL.h"
#include "Index_Manager.h"

class buffer{
	friend class BufferManager;
	buffer(){}
public:
	string filename;
	int blockOffset;
	int LRU;
	char value[BLOCKSIZE + 1];
	bool isValid;
	bool isWritten;

	void init();
	string getvalue(int start, int end);
	char getvalue(int pos);
};

class insertPos{
public:
	int bufferNUM;
	int position;
};

class BufferManager{
public:
	BufferManager();
	~BufferManager();

	buffer bufferBlock[MAXBLOCKNUMBER];
	void writeBack(int bufferNum);										// 把buffer中的内容写回到文件中
	int getbufferNum(string filename, int blockOffset); 				// 获得特定文件中特定块在buffer中的序号,如果没有就将块导入buffer
	int getIfIsInBuffer(string filename, int blockOffset); 				// 获得特定文件中特定块在buffer中的序号,如果没有就返回-1
	void readBlock(string filename, int blockOffset, int bufferNum); 	// 读取特定块到buffer[bufferNum]中
	void writeBlock(int bufferNum);										// 标记buffer[bufferNum]的写入位，同时更新LRU
	void LRU(int bufferNum);											// LRU
	int getEmptyBuffer(); 												// 得到一个空的buffer的序号（如果buffer满了按照LRU进行替换）
	int getEmptyBufferExcept(string filename); 							// 得到一个空的buffer的序号(替换的时候不替换包含特定filename数据的buffer
	insertPos getInsertPosition(Table& fileinformation);				// 如果想向某文件进行写入，调用这个函数获得写入位置
	int addBlockInFile(Table& fileinformation);							// 为一个文件在buffer中新开辟一个块（为了写入）
	int addBlockInFile(Index& indexinfor); 								// 为一个index文件在buffer中新开辟一个块（为了写入）
	void scanIn(Table fil); 											// 将整个文件读入buffer中
	void setInvalid(string filename); 									// 如果文件被删除，将里面的所有相关块置为invalid
};
#endif