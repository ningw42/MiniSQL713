#include "Buffer_Manager.h"

#define EMPTY '@'

#include "MiniSQL.h"
extern int fukcount;
enum PointerType{ FATHER, LEFT, RIGHT };
class DeletePos{
public:
	int block;
	int offset;
	DeletePos(){}
	DeletePos(int block, int offset) : block(block), offset(offset){}
};
template <class KEY>
class Data{
public:
	KEY key;
	int BlockInFile;
	int Offset;
	Data() : BlockInFile(0), Offset(0){}
	Data(KEY Key, int block, int off) : key(Key), BlockInFile(block), Offset(off){}
};
template <class KEY>
class InnerData{
public:
	KEY key;
	int data;
	InnerData() : data(-1){}
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
	InnerNode(int bufferNum) :Node(bufferNum){
		recordNum = 0;
	}
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
	
	Data<KEY> pop();
	Data<KEY> getfront();
};
template <class KEY>
class IndexManager{
public:
	void createIndex(Table & tableinfo, Index & indexinfo);
	InnerData<KEY> insertValue(Index& indexinfor, Data<KEY> node, int blockOffset = 0);
	char * selectEqual(const Table& tableinfor, const Index& indexinfor, KEY key, int blockOffset = 0);
	void dropIndex(const string& indexName);
	DeletePos selectEqualForDelete(const Table& tableinfor, const Index& indexinfor, KEY key, int blockOffset = 0);
	vector<char *>  selectFromTo(Table& table, Index & indexinfo, KEY start,KEY end,int blockOffset = 0);
	void createIndexforString(Table & tableinfo, Index & indexinfo);
	char * selectEqualForString(const Table& tableinfor, const Index& indexinfor,string key, int blockOffset=0);
};

extern BufferManager bm;
template <class KEY>
bool Node<KEY>::isRoot(int buffernum){
	return bm.bufferBlock[bufferNum].value[0] == '1';
	}

template <class KEY> 
int Node<KEY>::getRecordNum(int bufferNum){
	
	int num;
	char * temp = new char[4];
	for(int i = 2; i<6; i++){
		temp[i-2] = bm.bufferBlock[bufferNum].value[i];
	}
	num = *((int *)temp);
	delete[] temp;
	return num;
}
template <class KEY>
int Node<KEY>::getPointer(PointerType pp){
	char* temp = new char[4];
	int num;
	switch (pp){
	case FATHER:
		for(int i = 6;i<10;i++)
			temp[i-6] = bm.bufferBlock[bufferNum].value[i];	//new
		num = *((int *)temp);
		delete[] temp;
		return num;
	case LEFT:
		for(int i = 10;i<14;i++){
			temp[i-10] = bm.bufferBlock[bufferNum].value[i];
		}
		num = *((int *)temp);
		delete[] temp;
		return num;
	case RIGHT:
		for(int i = 14;i<18;i++){
			temp[i-14] = bm.bufferBlock[bufferNum].value[i];
		}
		num = *((int *)temp);
		delete[] temp;
		return num;
	}
}
template <class KEY>
InnerNode<KEY>::InnerNode(int bufferNumber, const Index& index){
	bufferNum = bufferNumber;

	Root = isRoot(bufferNumber);
	
	int count = getrecordNum(bufferNumber);
	pointFather = getPointer(FATHER);
	recordNum = 0;
	int position = 18;
	KEY tempkey;
	int tempdata;
	char * temp = new char[sizeof(KEY)];
	for(int i = 0;i<count;i++){
		for(int j = 0;j<sizeof(KEY);j++){
			temp [j] = bm.bufferBlock[bufferNum].value[position+j];
			}
		tempkey = *((KEY *)temp);
		position =position + sizeof(KEY);
		for(int k = 0;k<sizeof(int);k++){
			temp [k] = bm.bufferBlock[bufferNum].value[position+k];
			}
		tempdata = *((int *)temp);
		position = position + sizeof(int);
		InnerData<KEY> data(tempkey,tempdata);
		insert(data);
	}
	delete[] temp;
}
template <class KEY>
InnerNode<KEY>::~InnerNode(){
	if(isRoot(bufferNum)) {
		bm.bufferBlock[bufferNum].value[0] = '1';
	}
	else{
		bm.bufferBlock[bufferNum].value[0] = '0';
	}
	bm.bufferBlock[bufferNum].value[1] = '0';
	char* temp;
	temp = (char *)(&recordNum);
	for(int i = 0;i<4;i++){
		bm.bufferBlock[bufferNum].value[2+i] = temp [i];
	}
	char* temp1;

	temp1 = (char *)(&pointFather);
	for (int i = 6; i<10; i++){
		bm.bufferBlock[bufferNum].value[i] = temp1[i - 6];
	}
	if(nodelist.size() == 0){
		cout << "Error!" << endl;
		exit(0);
	}
	list< InnerData<KEY> > ::iterator i;
	int position =18;
	for(i = nodelist.begin();i!=nodelist.end();i++){
		KEY key = (*i).key;
		char * temp;
		temp = (char *)(&key);
		for(int i = 0;i<sizeof(KEY);i++){
			bm.bufferBlock[bufferNum].value[position+i] = temp [i];
		}
		position +=sizeof(KEY);
		char *temp2;
		int tdata = (*i).data;
		temp2 = (char *)(&tdata);
		for(int i = 0;i<sizeof(int);i++){
			bm.bufferBlock[bufferNum].value[position+i] = temp2 [i];
		}
		position +=sizeof(int);
	}
}
template <class KEY>
void InnerNode<KEY>::insert(InnerData<KEY> data){
	recordNum++;
	list<InnerData<KEY>>::iterator i = nodelist.begin();
	if(nodelist.size() == 0)
		nodelist.insert(i, data);
	else{
		for(i = nodelist.begin(); i!=nodelist.end(); i++)
			if((*i).key > data.key) break;
		nodelist.insert(i, data);
	}
	
}


template <class KEY>
InnerData<KEY> InnerNode<KEY>::pop(){
	recordNum--;
	InnerData<KEY> tmpt = nodelist.back();
	nodelist.pop_back();
	return tmpt;
}
template <class KEY>
InnerData<KEY> InnerNode<KEY>::getfront(){
		return nodelist.front();
	}
template <class KEY>
int InnerNode<KEY>::getrecordNum(int buffernum){
	char * temp = new char[4];
	int num;
	for(int i = 2;i<6;i++){
		temp[i-2] = bm.bufferBlock[bufferNum].value[i];		// new
	}
	num = *((int *)temp);
	delete[] temp;
	return num;
}


template <class KEY>
Leaf<KEY>::Leaf(int bufnum){
	bufferNum = bufnum;
	recordNum = 0;
	nextSibling = lastSibling = 0;
}
template <class KEY>
Leaf<KEY>::Leaf(int bufnum,const Index& indexinfo){
	bufferNum = bufnum;
	Root = isRoot(bufnum);
	pointFather = getPointer(FATHER);
	nextSibling = getPointer(RIGHT);
	lastSibling = getPointer(LEFT);
	columnLength = indexinfo.columnLength;
	int position = 18;
	int recordCount = getRecordNum(bufnum);
	recordNum = 0;
	char * temp = new char[sizeof(KEY)];
	for(int i = 0; i < recordCount; i++)
	{
		KEY key ;
		int tempblock,tempoff;
		for(int j = 0;j<sizeof(KEY);j++){
			temp [j] = bm.bufferBlock[bufferNum].value[position+j];
		}
		key = *((KEY *)temp);
		position +=sizeof(KEY);
		for(int j = 0;j<sizeof(int);j++){
			temp [j] = bm.bufferBlock[bufferNum].value[position+j];
		}
		tempblock = *((int *)temp);
		position +=sizeof(int);
		for(int j = 0;j<sizeof(int);j++){
			temp [j] = bm.bufferBlock[bufferNum].value[position+j];
		}
		tempoff = *((int *)temp);
		position +=sizeof(int);
		Data<KEY> newdata(key,tempblock,tempoff);
		insert(newdata);
	}
	delete[] temp;
}
template <class KEY>
Leaf<KEY>::~Leaf(){
	//cout << "start" << endl;
	if(Root) bm.bufferBlock[bufferNum].value[0] = '1';
	else bm.bufferBlock[bufferNum].value[0] = '0';
	bm.bufferBlock[bufferNum].value[1] = '1';
	
	char* temp1;
	
	temp1 = (char *)(&pointFather);
	for(int i = 6;i<10;i++){
		bm.bufferBlock[bufferNum].value[i] = temp1 [i-6];
	}
	char* temp2;
	
	temp2 = (char *)(&lastSibling);
	for(int i = 10;i<14;i++){
		bm.bufferBlock[bufferNum].value[i] = temp2 [i-10];
	}
	char* temp3;
	
	temp3 = (char *)(&nextSibling);
	for(int i = 14;i<18;i++){
		bm.bufferBlock[bufferNum].value[i] = temp3 [i-14];
	}

	//cout << "write pointer" << endl;

	if(nodelist.size() == 0){
		cout << "Error!" << endl;
		exit(0);
	}
	list< Data<KEY> >::iterator itor;
	int position =18;
	for(itor = nodelist.begin();itor!=nodelist.end();itor++){
		KEY key = (*itor).key;
		char * temp;
		temp = (char *)(&key);
		for(int i = 0;i<sizeof(KEY);i++){
			bm.bufferBlock[bufferNum].value[position+i] = temp [i];
		}
		position +=sizeof(KEY);
		char *temp2;
		temp2 = (char *)(&((*itor).BlockInFile));
		for(int i = 0;i<sizeof(int);i++){
			bm.bufferBlock[bufferNum].value[position+i] = temp2 [i];
		}
		position +=sizeof(int);
		char *temp3;
		temp3 = (char *)(&((*itor).Offset));
		for(int i = 0;i<sizeof(int);i++){
			bm.bufferBlock[bufferNum].value[position+i] = temp3 [i];
		}
		position +=sizeof(int);
	}
	//cout << "value" << endl;
	char* temp4;
	temp4 = (char *)(&recordNum);
	for (int i = 0; i<4; i++){
		bm.bufferBlock[bufferNum].value[2 + i] = temp4[i];
	}
	//cout << "end" << endl;
	//cout << fukcount++ << endl;
}
template <class KEY>
void Leaf<KEY>::insert(Data<KEY> data){
	recordNum++;
	list<Data<KEY>>::iterator i = nodelist.begin();
	if(nodelist.size() == 0){
		nodelist.insert(i, data);
		return;
	}

	else{
		for(i = nodelist.begin(); i!=nodelist.end(); i++)
			if((*i).key > data.key) break;
	}
	nodelist.insert(i, data);
	
}


template <class KEY>
Data<KEY> Leaf<KEY>::pop(){
	recordNum--;
	Data<KEY> tmpt = nodelist.back();
	nodelist.pop_back();
	return tmpt;
}
template <class KEY>
Data<KEY> Leaf<KEY>::getfront(){
	return nodelist.front();
}
template <class KEY>
void IndexManager<KEY>::createIndex(Table & tableinfo, Index & indexinfo){
	string filename = indexinfo.index_name + ".index";
	fstream  fout(("./bm/" + filename).c_str(), ios::out | ios::binary);
	fout.close();
	int blockNum;
	blockNum = bm.addBlockInFile(indexinfo);
	bm.bufferBlock[blockNum].filename = filename;
	bm.bufferBlock[blockNum].blockOffset = 0;
	bm.bufferBlock[blockNum].isWritten = 1;
	bm.bufferBlock[blockNum].isValid = 1;
	bm.bufferBlock[blockNum].value[0] = '1';   //block的第一位标记是否是根
	bm.bufferBlock[blockNum].value[1] = '1';  //block的第二位标记是否是叶节点
	int recordNum = 0;
	char * temp;
	temp = (char *)(&recordNum);
	for(int i = 0;i<4;i++){
		bm.bufferBlock[blockNum].value[2+i] = temp [i];
	}
	int initpt = 0;

	temp = (char *)(&initpt);
	for(int i = 0;i<sizeof(int);i++){
		bm.bufferBlock[blockNum].value[6+i] = temp [i];
	}
	for(int i = 0;i<sizeof(int);i++){
		bm.bufferBlock[blockNum].value[10+i] = temp [i];
	}
	for(int i = 0;i<sizeof(int);i++){
		bm.bufferBlock[blockNum].value[14+i] = temp [i];
	}

	filename = tableinfo.name + ".table";
	int length = tableinfo.tupleLength + 1;
	const int recordNumber = BLOCKSIZE / length;
	char * t = new char[indexinfo.tuplelength];

	for(int blockOffset = 0;blockOffset <tableinfo.blockNum; blockOffset++){
		cout << blockOffset << endl;
		int bufferNum = bm.getIfIsInBuffer(filename, blockOffset);
		if(bufferNum == -1){
			bufferNum = bm.getEmptyBuffer();
			bm.readBlock(filename, blockOffset, bufferNum);
		}
		for(int offset = 0; offset < recordNumber; offset ++){
			int position = offset * length ;
			if(bm.bufferBlock[bufferNum].value[position] == EMPTY||(bool *)bm.bufferBlock[bufferNum].value[position] ==false)continue;	//可以改进
			KEY colume;
			
			for(int k = 0;k<indexinfo.columnLength;k++){
				t[k] = bm.bufferBlock[bufferNum].value[position + indexinfo.startposition + k];
			}

			colume = *((KEY *)(t));

			
			Data<KEY> node(colume, blockOffset, offset);
			insertValue(indexinfo, node);
		}
	}
	delete[] t;
}
template<class KEY>
void IndexManager<KEY>::createIndexforString(Table & tableinfo, Index & indexinfo){
	string filename = indexinfo.index_name + ".index";
	fstream  fout(("./bm/" + filename).c_str(), ios::out | ios::binary);
	fout.close();
	int blockNum;
	blockNum = bm.addBlockInFile(indexinfo);
	bm.bufferBlock[blockNum].filename = filename;
	bm.bufferBlock[blockNum].blockOffset = 0;
	bm.bufferBlock[blockNum].isWritten = 1;
	bm.bufferBlock[blockNum].isValid = 1;
	bm.bufferBlock[blockNum].value[0] = '1';   //block的第一位标记是否是根
	bm.bufferBlock[blockNum].value[1] = '1';  //block的第二位标记是否是叶节点
	int recordNum = 0;
	char * temp;
	temp = (char *)(&recordNum);
	for (int i = 0; i<4; i++){
		bm.bufferBlock[blockNum].value[2 + i] = temp[i];
	}
	int initpt = 0;

	temp = (char *)(&initpt);
	for (int i = 0; i<sizeof(int); i++){
		bm.bufferBlock[blockNum].value[6 + i] = temp[i];
	}
	for (int i = 0; i<sizeof(int); i++){
		bm.bufferBlock[blockNum].value[10 + i] = temp[i];
	}
	for (int i = 0; i<sizeof(int); i++){
		bm.bufferBlock[blockNum].value[14 + i] = temp[i];
	}

	filename = tableinfo.name + ".table";
	int length = tableinfo.tupleLength + 1;
	const int recordNumber = BLOCKSIZE / length;
	char * t = new char[indexinfo.tuplelength];

	for (int blockOffset = 0; blockOffset <tableinfo.blockNum; blockOffset++){
		int bufferNum = bm.getIfIsInBuffer(filename, blockOffset);
		if (bufferNum == -1){
			bufferNum = bm.getEmptyBuffer();
			bm.readBlock(filename, blockOffset, bufferNum);
		}
		for (int offset = 0; offset < recordNumber; offset++){
			int position = offset * length;
			if (bm.bufferBlock[bufferNum].value[position] == EMPTY || (bool *)bm.bufferBlock[bufferNum].value[position] == false)continue;	//可以改进
			string colume;

			for (int k = 0; k<indexinfo.columnLength; k++){
				t[k] = bm.bufferBlock[bufferNum].value[position + indexinfo.startposition + k];
			}
			
				colume = string(t);
		
			Data<string> node(colume, blockOffset, offset);
			insertValue(indexinfo, node);
		}
	}
	delete[] t;
}


template <class KEY>
InnerData<KEY> IndexManager<KEY>::insertValue(Index& indexinfo, Data<KEY> node, int blockOffset){
	InnerData<KEY> reBranch;
	string filename = indexinfo.index_name + ".index";
	
	int bufferNum = bm.getbufferNum(filename, blockOffset);
	bool isLeaf = ( bm.bufferBlock[bufferNum].value[1] == '1' );
	if(isLeaf){
		Leaf<KEY> leaf(bufferNum, indexinfo);
		leaf.insert(node);
		


		const int RecordLength = indexinfo.columnLength + 8;
		const int MaxrecordNum = (BLOCKSIZE-18)/RecordLength;
		if( leaf.recordNum >MaxrecordNum ){
			//cout << "split1" << endl;
			if(leaf.Root){
				int rbufferNum = leaf.bufferNum;
				leaf.bufferNum = bm.addBlockInFile(indexinfo);
				int sbufferNum = bm.addBlockInFile(indexinfo);
				//cout << "split2" << endl;
				InnerNode<KEY> branchRoot(rbufferNum);
				Leaf<KEY> leafadd(sbufferNum);


				branchRoot.Root = 1;
				leafadd.Root = 0;
				leaf.Root = 0;
				//cout << "split3" << endl;
				branchRoot.pointFather = leafadd.pointFather = leaf.pointFather = 0;
				branchRoot.columnLength = leafadd.columnLength = leaf.columnLength;
				leafadd.nextSibling = leaf.nextSibling;
				leafadd.lastSibling = bm.bufferBlock[leaf.bufferNum].blockOffset;
				leaf.nextSibling = bm.bufferBlock[leafadd.bufferNum].blockOffset;
				while(leafadd.nodelist.size() < leaf.nodelist.size()){
					Data<KEY> tnode = leaf.pop();
					leafadd.insert(tnode);
				}
				
				InnerData<KEY> tmptNode1, tmptNode2;
				tmptNode1.key = leafadd.getfront().key;
				tmptNode1.data = bm.bufferBlock[leafadd.bufferNum].blockOffset;
				branchRoot.insert(tmptNode1);
				tmptNode2.key = leaf.getfront().key;
				tmptNode2.data = bm.bufferBlock[leaf.bufferNum].blockOffset;
				branchRoot.insert(tmptNode2);
				return reBranch;
				//cout << "split" << endl;
			}
			else{
				int bufferNum = bm.addBlockInFile(indexinfo);
				Leaf<KEY> leafadd(bufferNum);
				leafadd.Root = 0;
				leafadd.pointFather = leaf.pointFather;
				leafadd.columnLength = leaf.columnLength;


				leafadd.nextSibling = leaf.nextSibling;
				leafadd.lastSibling = bm.bufferBlock[leaf.bufferNum].blockOffset;
				leaf.nextSibling = bm.bufferBlock[leafadd.bufferNum].blockOffset;
				if(leafadd.nextSibling != 0){
					int bufferNum = bm.getbufferNum(filename, leafadd.nextSibling);
					Leaf<KEY> leafnext(bufferNum, indexinfo);
					leafnext.lastSibling = bm.bufferBlock[leafadd.bufferNum].blockOffset;
				}
				while(leafadd.nodelist.size() < leaf.nodelist.size()){
					Data<KEY> tnode = leaf.pop();
					leafadd.insert(tnode);
				}
				reBranch.key = leafadd.getfront().key;
				reBranch.data = leaf.nextSibling;
				return reBranch;
			}
		}
		else{//not need to split,just return
			return reBranch;
		}

	}
	else{//not a leaf
		InnerNode<KEY> branch(bufferNum, indexinfo);
		if (branch.nodelist.size() == 0){
			cout << "=======0" << endl;
		}
		list<InnerData<KEY>>::iterator i = branch.nodelist.begin();
		if((*i).key > node.key){
			(*i).key = node.key;
		}
		else{
			for(i = branch.nodelist.begin(); i != branch.nodelist.end(); i++)
				if((*i).key > node.key) break;
			i--;
		}
		InnerData<KEY> bnode = insertValue(indexinfo, node, (*i).data);//go down

		if(bnode.data == -1){
			return reBranch;
		}
		else{
			branch.insert(bnode);
			const int RecordLength = indexinfo.columnLength + 4;
			const int MaxrecordNum = (BLOCKSIZE-18) / RecordLength;
			if(branch.recordNum > MaxrecordNum){//need to split up
				if(branch.Root){
					int rbufferNum = branch.bufferNum;	// buffer number for new root
					branch.bufferNum = bm.addBlockInFile(indexinfo);	//find a new place for old branch
					int sbufferNum = bm.addBlockInFile(indexinfo);	// buffer number for sibling
					InnerNode<KEY> branchRoot(rbufferNum);	//new root
					InnerNode<KEY> branchadd(sbufferNum);	//sibling


					branchRoot.Root = 1;
					branchadd.Root = 0;
					branch.Root = 0;

					branchRoot.pointFather = branchadd.pointFather = branch.pointFather = 0;
					branchRoot.columnLength = branchadd.columnLength = branch.columnLength;

					while(branchadd.nodelist.size() < branch.nodelist.size()){
						InnerData<KEY> tnode = branch.pop();
						branchadd.insert(tnode);
					}

					InnerData<KEY> tmptNode;
					tmptNode.key = branchadd.getfront().key;
					tmptNode.data = bm.bufferBlock[branchadd.bufferNum].blockOffset;
					branchRoot.insert(tmptNode);
					tmptNode.key = branch.getfront().key;
					tmptNode.data = bm.bufferBlock[branch.bufferNum].blockOffset;
					branchRoot.insert(tmptNode);
					return reBranch;//here the function must have already returned to the top lay
				}
				else{//branch is not a root
					int bufferNum = bm.addBlockInFile(indexinfo);
					InnerNode<KEY> branchadd(bufferNum);
					branchadd.Root = 0;
					branchadd.pointFather = branch.pointFather;
					branchadd.columnLength = branch.columnLength;

					while(branchadd.nodelist.size() < branch.nodelist.size()){
						InnerData<KEY> tnode = branch.pop();
						branchadd.insert(tnode);
					}
					reBranch.key = branchadd.getfront().key;
					reBranch.data = bm.bufferBlock[bufferNum].blockOffset;
					return reBranch;
				}
			}
			else{//not need to split,just return
				return reBranch;
			}
		}
	}
	return reBranch;//here is just for safe
}
template <class KEY>
char * IndexManager<KEY>::selectEqual(const Table& tableinfor, const Index& indexinfor, KEY key, int blockOffset){
	string filename = indexinfor.index_name + ".index";
	char * temp = new char[tableinfor.tupleLength + 1];
	int bufferNum = bm.getbufferNum(filename, blockOffset);
	bool isLeaf = ( bm.bufferBlock[bufferNum].value[1] == '1' );
	if(isLeaf){
		Leaf<KEY> leaf(bufferNum, indexinfor);
		list< Data<KEY> >::iterator i = leaf.nodelist.begin();
		for(i = leaf.nodelist.begin(); i!= leaf.nodelist.end(); i++){
			if((*i).key == key){
				filename = indexinfor.table_name + ".table";
				int recordBufferNum = bm.getbufferNum(filename, (*i).BlockInFile);
				int position = (tableinfor.tupleLength + 1) * ((*i).Offset);
				
				for(int j = 0;j<indexinfor.tuplelength+1;j++){
					temp[j] = bm.bufferBlock[recordBufferNum].value[position + j];
				}
				
				return temp;
			}
		}
	}
	else{
		InnerNode<KEY> branch(bufferNum, indexinfor);
		list< InnerData<KEY> >::iterator i = branch.nodelist.begin();
		for(i = branch.nodelist.begin(); i != branch.nodelist.end(); i++){
			if((*i).key > key){
				if (i == branch.nodelist.begin()){
					return NULL;
				}
				i--;
				break;
			}
		}
		if(i == branch.nodelist.end()) i--;
		temp = selectEqual(tableinfor, indexinfor, key, (*i).data);
		return temp;
	}
	return NULL;
}
template <class KEY>
DeletePos IndexManager<KEY>::selectEqualForDelete(const Table& tableinfor, const Index& indexinfor, KEY key, int blockOffset){
	string filename = indexinfor.index_name + ".index";
	DeletePos result;
	
	int bufferNum = bm.getbufferNum(filename, blockOffset);
	bool isLeaf = (bm.bufferBlock[bufferNum].value[1] == '1');
	if (isLeaf){
		Leaf<KEY> leaf(bufferNum, indexinfor);
		list< Data<KEY> >::iterator i = leaf.nodelist.begin();
		for (i = leaf.nodelist.begin(); i != leaf.nodelist.end(); i++){
			if ((*i).key == key){
				result.block = (*i).BlockInFile;
				result.offset = (*i).Offset;
				return result;
			}
		}
	}
	else{
		InnerNode<KEY> branch(bufferNum, indexinfor);
		list< InnerData<KEY> >::iterator i = branch.nodelist.begin();
		for (i = branch.nodelist.begin(); i != branch.nodelist.end(); i++){
			if ((*i).key > key){
				if (i == branch.nodelist.begin()){
					return DeletePos(-1, -1);
				}
				i--;
				break;
			}
		}
		if (i == branch.nodelist.end()) i--;
		result = selectEqualForDelete(tableinfor, indexinfor, key, (*i).data);
		return result;
	}
	return DeletePos(-1, -1);
}

template <class KEY>
char*  IndexManager<KEY>::selectEqualForString(const Table& tableinfor, const Index& indexinfor, string key, int blockOffset){
	string filename = indexinfor.index_name + ".index";
	char * temp = new char[tableinfor.tupleLength + 1];
	int bufferNum = bm.getbufferNum(filename, blockOffset);
	bool isLeaf = (bm.bufferBlock[bufferNum].value[1] == '1');
	if (isLeaf){
		Leaf<string> leaf(bufferNum, indexinfor);
		list< Data<string> >::iterator i = leaf.nodelist.begin();
		for (i = leaf.nodelist.begin(); i != leaf.nodelist.end(); i++){
			if ((*i).key == key){
				filename = indexinfor.table_name + ".table";
				int recordBufferNum = bm.getbufferNum(filename, (*i).BlockInFile);
				int position = (tableinfor.tupleLength + 1) * ((*i).Offset);

				for (int j = 0; j<indexinfor.tuplelength + 1; j++){
					temp[j] = bm.bufferBlock[recordBufferNum].value[position + j];
				}

				return temp;
			}
		}
	}
	else{
		InnerNode<string> branch(bufferNum, indexinfor);
		list< InnerData<string> >::iterator i = branch.nodelist.begin();
		for (i = branch.nodelist.begin(); i != branch.nodelist.end(); i++){
			if ((*i).key > key){
				i--;
				break;
			}
		}
		if (i == branch.nodelist.end()) i--;
		temp = selectEqualForString(tableinfor, indexinfor, key, (*i).data);
		return temp;
	}
	return NULL;
}

template <class KEY>
void IndexManager<KEY>::dropIndex(const string& indexName){
	string filename = indexName + ".index";
	if( remove(("./bm/" +filename).c_str()) != 0 )
		perror( "Error " );
	else
		bm.setInvalid(filename);
}

template <class KEY>
vector<char *>  IndexManager<KEY>::selectFromTo(Table& table, Index & indexinfo, KEY start,KEY end,int blockOffset ){
	vector<char *>result;
	string filename = indexinfor.index_name + ".index";
		int bufferNum = buf.getbufferNum(filename, blockOffset);
		bool isLeaf = ( buf.bufferBlock[bufferNum].values[1] == '1' );
		if(isLeaf){
			do{
				Leaf<KEY> leaf(bufferNum, indexinfor);
				list< Data<KEY> >::iterator i;
				for(i = leaf.nodelist.begin(); i!= leaf.nodelist.end(); i++){
					if((*i).key >= start){
						if((*i).key > end){
							return result;
						}
						filename = indexinfor.table_name + ".table";
						int recordBufferNum = buf.getbufferNum(filename, (*i).BlockInFile);
						int position = (tableinfor.totalLength +1)* ((*i).Offset);
						       //maybe wrong
						char * temp = new char[tableinfor.totalLength +1];
						
						for(int j = 0;j<tableinfor.totalLength+1;j++){
							temp[j] = bm.bufferBlock[recordBufferNum].value[position + j];
						}
						result.insert (temp);
						
					}
				}
				if(leaf.nextSibling != 0){
					filename = indexinfor.index_name + ".index";
					bufferNum = buf.getbufferNum(filename, leaf.nextSibling);
				}
				else{
					return result;
				}
			}while(1);
		}
		else{
			InnerNode<KEY> branch(bufferNum, indexinfor);
			list< InnerData<KEY> >::iterator i = branch.nodelist.begin();
			if((*i).key > start){
				result = selectFromTo(tableinfor, indexinfor,start,end, (*i).data);
				return result;
			}
			else{
				for(i = branch.nodelist.begin(); i != branch.nodelist.end(); i++){
					if((*i).key > start){
						i--;
						break;
					}
				}
				result = selectBetween(tableinfor, indexinfor, start, end, (*i).data);
				return result;
			}
		}
		return result;
}