#include <string>
#include<iostream>
#include<fstream> 
#include<vector>
#include"bufferManager.h"


#define  BLOCKSIZE  1000
#define EMPTY '@'
#define MAXBLOCKNUMBER 100
using namespace std;

class buffer{
public:
	bool isWritten;
	bool isValid;
	string  filename;
	int blockOffset;
	int LRUvalue;
	char values[BLOCKSIZE+1];
	buffer(){
		initialize();
	}
	void initialize(){
		isWritten = 0;
		isValid = 0;
		filename = "NULL";
		blockOffset = 0;
		LRUvalue = 0;
		for(int i = 0;i<BLOCKSIZE;i++)
			values[i] = EMPTY;
		values[BLOCKSIZE] = '\0';
	}
	string getvalues(int startpos,int endpos){
		string tmp = "";
		if(startpos>=0&&startpos <=endpos&&endpos<=BLOCKSIZE){
			for(int i = startpos;i<endpos;i++){
				tmp +=values[i];
			}
		}
		return tmp;
	}
	char getvalues(int pos){
		if(pos>=0&&pos<=BLOCKSIZE){
			return values[pos];
		}
		return '\0';
	}
	

	};
class insertPos{
public:
	int bufferNUM;
	int position;
};




class BufferManager{
public:
	friend class RecoedManager;
	friend class IndexManager;
	friend class Leaf;
	friend class Branch;
	friend class BPlusTree;


	BufferManager(){
		for (int i = 0;i<MAXBLOCKNUMBER; i++)
			bufferBlock[i].initialize();
	}
	~BufferManager(){
		for(int i = 0; i< MAXBLOCKNUMBER; i++)
			flashBack(i);
	}
private:
public:
	buffer bufferBlock[MAXBLOCKNUMBER];
	void flashBack(int bufferNum){
		if(!bufferBlock[bufferNum].isWritten)return;
		string filename = bufferBlock[bufferNum].filename;
		fstream fout(filename.c_str(), ios::in|ios::out);
		
		fout.seekp(BLOCKSIZE*bufferBlock[bufferNum].blockOffset, fout.beg);
		fout.write(bufferBlock[bufferNum].values, BLOCKSIZE);
		bufferBlock[bufferNum].initialize();
		fout.close();
	}
	int getbufferNum(string filename ,int blockOffset){
		int bufferNum = getIfIsInBuffer(filename,blockOffset);
		if(bufferNum = -1){
			bufferNum = getEmptyBufferExcept(filename);
			readBlock(filename,blockOffset,bufferNum);
		}
		return bufferNum;
	}
	int getIfIsInBuffer(string filename ,int blockOffset){
		for (int bufferNum = 0;bufferNum<MAXBLOCKNUMBER;bufferNum++)
			if(bufferBlock[bufferNum].filename == filename && bufferBlock[bufferNum].blockOffset == blockOffset)	return bufferNum;
		return -1;	
	}
	void readBlock(string filename,int blockOffset,int bufferNum){
		bufferBlock[bufferNum].isValid =1;
		bufferBlock[bufferNum].isWritten = 0;
		bufferBlock[bufferNum].filename = filename;
		bufferBlock[bufferNum].blockOffset = blockOffset;
		fstream fin(filename.c_str(),ios::in);
		fin.seekp(BLOCKSIZE*blockOffset,fin.beg);
		fin.read(bufferBlock[bufferNum].values,BLOCKSIZE);
		fin.close();
	}
	void writeBlock(int bufferNum){
		bufferBlock[bufferNum].isWritten = 1;
		useBlock(bufferNum);
	}

	void useBlock(int bufferNum){
		for(int i =1;i<MAXBLOCKNUMBER;i++){
			if(i == bufferNum ){
				bufferBlock[bufferNum].LRUvalue = 0;
				bufferBlock[i].isValid = 1;
			}
			else
				bufferBlock[bufferNum].LRUvalue++;

		}}
	int getEmptyBuffer() {
		int bufferNum = 0;
		int highestLRUvalue = bufferBlock[0].LRUvalue;
		for(int i = 0;i<MAXBLOCKNUMBER;i++)
		{
			if(!bufferBlock[i].isValid){
				bufferBlock[i].initialize();
				bufferBlock[i].isValid = 1;
				return i;
			}
			else if(highestLRUvalue < bufferBlock[i].LRUvalue)
			{
				highestLRUvalue = bufferBlock[i].LRUvalue;
				bufferNum = i;
			}
		}
		flashBack(bufferNum);
		bufferBlock[bufferNum].isValid = 1;
		return bufferNum;
	}

	int getEmptyBufferExcept(string filename){
		int bufferNum = -1;
		int highestLRUvalue = bufferBlock[0].LRUvalue;
		for(int i = 0; i < MAXBLOCKNUMBER; i++)
		{
			if(!bufferBlock[i].isValid){
				bufferBlock[i].isValid = 1;
				return i;
			}
			else if(highestLRUvalue < bufferBlock[i].LRUvalue && bufferBlock[i].filename != filename)
			{
				highestLRUvalue = bufferBlock[i].LRUvalue;
				bufferNum = i;
			}
		}
		if(bufferNum == -1){
			cout << "All the buffers in the database system are used up. Sorry about that!" << endl;
			exit(0);
		}
		flashBack(bufferNum);
		bufferBlock[bufferNum].isValid = 1;
		return bufferNum;
	}

	insertPos getInsertPosition(Table& tableinfo){
		insertPos ipos;
		if(tableinfo.blockNum ==0){
			ipos.bufferNUM = addBlockInFile(tableinfo);
			ipos.position = 0;
			return ipos;
		}
		string filename = tableinfo.name + ".table";
		int length = tableinfo.tupleLength + 1;
		int blockOffset = tableinfo.blockNum - 1;
		int bufferNum = getIfIsInBuffer(filename, blockOffset);
		if(bufferNum ==-1){
			bufferNum = getEmptyBuffer();
			readBlock(filename, blockOffset, bufferNum);
		}
		const int recordNum = BLOCKSIZE/length;
		for (int offset = 0;offset < recordNum; offset ++){
			int position = offset*length;
			char  isEmpty = bufferBlock[bufferNum].values[position];
			if(isEmpty == EMPTY){//find an empty space
				ipos.bufferNUM = bufferNum;
				ipos.position = position;
				return ipos;
				}
		}
		//if the program run till here, the last block is full, therefor one more block is added
		 ipos.bufferNUM = addBlockInFile(tableinfo);
		 ipos.position = 0;
		 return ipos;}
	int addBlockInFile(Table& tableinfor){//add one more block in file for the table
		int bufferNum = getEmptyBuffer();	
		bufferBlock[bufferNum].initialize();
		bufferBlock[bufferNum].isValid = 1;
		bufferBlock[bufferNum].isWritten = 1;
		bufferBlock[bufferNum].filename = tableinfor.name + ".table";
		bufferBlock[bufferNum].blockOffset = tableinfor.blockNum++;
		return bufferNum;
	}
	int scanIn(Table tableinfo){
		string filename = tableinfo.name + ".table";
		fstream fin(filename.c_str(),ios::in);
		for(int blockOffset = 0; blockOffset < tableinfo.blockNum; blockOffset++){
			if(getIfIsInBuffer(filename, blockOffset) == -1){	//indicate that the data is not read in buffer yet
				int bufferNum = getEmptyBufferExcept(filename);
				readBlock(filename, blockOffset, bufferNum);
			}
		}
		fin.close();
	}
	void setInvalid(string filename){//when a file is deleted, a table or an index, all the value in buffer should be set invalid
		for(int i = 0; i < MAXBLOCKNUMBER; i++)
		{
			if(bufferBlock[i].filename == filename){
					bufferBlock[i].isValid = 0;
					bufferBlock[i].isWritten = 0;
			}
		}
	}
	

/*	int addBlockInFile(Index& indexinfor){//add one more block in file for the index
		string filename = indexinfor.index_name + ".index";
		int bufferNum = getEmptyBufferExcept(filename);
		bufferBlock[bufferNum].initialize();
		bufferBlock[bufferNum].isValid = 1;
		bufferBlock[bufferNum].isWritten = 1;
		bufferBlock[bufferNum].filename = filename;
		bufferBlock[bufferNum].blockOffset = indexinfor.blockNum++;
		return bufferNum;
		};*/};