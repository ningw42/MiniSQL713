#include "Buffer_Manager.h"
#define EMPTY '@'

void buffer::init(){
	isWritten = 0;
	isValid = 0;
	LRU = 0;
	blockOffset = 0;
	filename = "NULL";
	for (int i = 0; i<BLOCKSIZE; i++)
		value[i] = EMPTY;
	value[BLOCKSIZE] = '\0';
}


string buffer::getvalue(int start, int end){
	string  temp = "";
	if (start>0 && start <= end&&end <= BLOCKSIZE){
		for (int i = start; i<end; i++){
			temp += value[i];
		}
	}
	return temp;
}


char buffer::getvalue(int pos){
	if (pos >= 0 && pos<BLOCKSIZE)
		return value[pos];
	return '\0';
}

BufferManager::BufferManager(){
	for (int i = 0; i<MAXBLOCKNUMBER; i++)
		bufferBlock[i].init();
}

BufferManager::~BufferManager(){
	for (int i = 0; i<MAXBLOCKNUMBER; i++)
		writeBack(i);
}

void BufferManager::writeBack(int bufferNum){
	if (!bufferBlock[bufferNum].isWritten)return;
	string filename = bufferBlock[bufferNum].filename;
	fstream  fs(filename.c_str(), ios::in | ios::out);
	fs.seekp(BLOCKSIZE*bufferBlock[bufferNum].blockOffset, fs.beg);
	fs.write(bufferBlock[bufferNum].value, BLOCKSIZE);
	bufferBlock[bufferNum].init();
	fs.close();
}

int BufferManager::getbufferNum(string filename, int blockOffset){
	int bufferNum = getIfIsInBuffer(filename, blockOffset);
	if (bufferNum == -1){
		bufferNum = getEmptyBufferExcept(filename);
		readBlock(filename, blockOffset, bufferNum);
	}
	return bufferNum;
}

int BufferManager::getIfIsInBuffer(string filename, int blockOffset){
	for (int bufferNum = 0; bufferNum<MAXBLOCKNUMBER; bufferNum++)
	if (bufferBlock[bufferNum].filename == filename && bufferBlock[bufferNum].blockOffset == blockOffset)
		return bufferNum;
	return -1;
}

void BufferManager::readBlock(string filename, int blockOffset, int bufferNum){
	fstream fs(filename.c_str(), ios::in);
	bufferBlock[bufferNum].isValid = 1;
	bufferBlock[bufferNum].isWritten = 0;
	bufferBlock[bufferNum].filename = filename;
	bufferBlock[bufferNum].blockOffset = blockOffset;
	fs.seekp(BLOCKSIZE*blockOffset, fs.beg);
	fs.read(bufferBlock[bufferNum].value, BLOCKSIZE);
	fs.close();
}

void BufferManager::writeBlock(int bufferNum){
	bufferBlock[bufferNum].isWritten = 1;
	LRU(bufferNum);
}

void BufferManager::LRU(int bufferNum){
	for (int i = 0; i<MAXBLOCKNUMBER; i++){
		if (i == bufferNum){
			bufferBlock[bufferNum].LRU = 0;
			bufferBlock[i].isValid = 1;
		}
		else
			bufferBlock[bufferNum].LRU++;
	}
}

int BufferManager::getEmptyBuffer(){
	int bufferNum = 0;
	int highestLRU = bufferBlock[0].LRU;
	for (int i = 0; i<MAXBLOCKNUMBER; i++)
	{
		if (!bufferBlock[i].isValid){
			bufferBlock[i].init();
			bufferBlock[i].isValid = 1;
			return i;
		}
		else if (highestLRU < bufferBlock[i].LRU)
		{
			highestLRU = bufferBlock[i].LRU;
			bufferNum = i;
		}
	}
	writeBack(bufferNum);
	bufferBlock[bufferNum].isValid = 1;
	return bufferNum;
}



int BufferManager::getEmptyBufferExcept(string filename){
	int bufferNum = -1;
	int highestLRU = bufferBlock[0].LRU;
	for (int i = 0; i < MAXBLOCKNUMBER; i++)
	{
		if (!bufferBlock[i].isValid){
			bufferBlock[i].isValid = 1;
			return i;
		}
		else if (highestLRU < bufferBlock[i].LRU && bufferBlock[i].filename != filename)
		{
			highestLRU = bufferBlock[i].LRU;
			bufferNum = i;
		}
	}
	if (bufferNum == -1){
		cout << "we need more space for buffer" << endl;
		exit(0);
	}
	writeBack(bufferNum);
	bufferBlock[bufferNum].isValid = 1;
	return bufferNum;
}

insertPos  BufferManager::getInsertPosition(Table & fileinformation){
	insertPos ipos;
	if (fileinformation.blockNum == 0){
		ipos.bufferNUM = addBlockInFile(fileinformation);
		ipos.position = 0;
		return ipos;
	}
	string filename = fileinformation.name + ".table";
	int length = fileinformation.tupleLength;
	int blockOffset = fileinformation.blockNum - 1;
	int bufferNum = getIfIsInBuffer(filename, blockOffset);
	if (bufferNum == -1){
		bufferNum = getEmptyBuffer();
		readBlock(filename, blockOffset, bufferNum);
	}
	const int recordNum = BLOCKSIZE / length;
	for (int offset = 0; offset < recordNum; offset++){
		int position = offset*length;
		char  isEmpty = bufferBlock[bufferNum].value[position];
		if (isEmpty == EMPTY){
			ipos.bufferNUM = bufferNum;
			ipos.position = position;
			return ipos;
		}
	}

	ipos.bufferNUM = addBlockInFile(fileinformation);
	ipos.position = 0;
	return ipos;
}

int BufferManager::addBlockInFile(Table & fil){
	int bufferNum = getEmptyBuffer();
	bufferBlock[bufferNum].init();
	bufferBlock[bufferNum].isValid = 1;
	bufferBlock[bufferNum].isWritten = 1;
	bufferBlock[bufferNum].filename = fil.name + ".table";
	bufferBlock[bufferNum].blockOffset = fil.blockNum++;
	return bufferNum;
}
void BufferManager::scanIn(Table fil){
	string filename = fil.name + ".table";
	fstream fin(filename.c_str(), ios::in);
	for (int blockOffset = 0; blockOffset < fil.blockNum; blockOffset++){
		if (getIfIsInBuffer(filename, blockOffset) == -1){
			int bufferNum = getEmptyBufferExcept(filename);
			readBlock(filename, blockOffset, bufferNum);
		}
	}
	fin.close();
}

void BufferManager::setInvalid(string filename){
	for (int i = 0; i < MAXBLOCKNUMBER; i++)
	{
		if (bufferBlock[i].filename == filename){
			bufferBlock[i].isValid = 0;
			bufferBlock[i].isWritten = 0;
		}
	}
}
/*
int addBlockInFile(Index& indexinfor){
string filename = indexinfor.index_name + ".index";
int bufferNum = getEmptyBufferExcept(filename);
bufferBlock[bufferNum].initialize();
bufferBlock[bufferNum].isValid = 1;
bufferBlock[bufferNum].isWritten = 1;
bufferBlock[bufferNum].filename = filename;
bufferBlock[bufferNum].blockOffset = indexinfor.blockNum++;
return bufferNum;
};

*/


