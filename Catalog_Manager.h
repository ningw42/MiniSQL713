#ifndef __MiniSQL__Catalog__Manager__
#define __MiniSQL__Catalog__Manager__

#include "MiniSQL.h"

class CatalogManager
{
private:
	vector<Table> tableList; // ����Ϣ�б�
	vector<Index> indexList; // ������Ϣ�б�
	int tableNum; // �����Ŀ
	
	// ����������Ƿ�ɹ�
	bool createTable(SQLstatement &sql); 
	// ��������
	bool createIndex(SQLstatement &sql, Table *t);
	
	
	// �������tableList	
	void pushBack_tableList(Table &t); 
	// ����������indexList 
	void pushBack_indexList(Index &i);

	// ���±���Ŀ
	void update_tableNum(bool add);  

	// �������Ƿ�Ϸ�
	//example of value:
	//1,1.0,"abc"
	bool checkInsert(Table *t, string value);
	// ��������Ƿ���ȷ float 1.0f , char[] "ff"
	bool checkType(Attribute *a, string v);
	// ���t�е����Ժ�a�������Ƿ������indexΪ��ʱ�������Ƿ�Ϊunique��primary
	bool checkAttribute(Table *t, vector<Attribute> *a, bool index); 
	// ���t�е����Ժ�c�������Ƿ����
	bool checkCondition(Table *t, vector<Condition> *c); 
	 

	// ��������µ�indexname
	bool clearIndex(Table *t, string in, bool drop);	 

	// ɾ����������vector�У�ͬʱ����save_indexInfo��
	bool dropIndex(Table *t, Index *i); 
	// ɾ������vector�У�ͬʱ����save_tableInfo��
	bool dropTable(Table *t); 

	// �ѱ�t�ĸ���д���ļ� add��ʱ�����ļ�����ʱɾ���ļ�
	bool save_tableInfo(Table *t, bool add); 
	// ������i�ĸ���д���ļ� add��ʱ�����ļ�����ʱɾ���ļ�
	bool save_indexInfo(Index *i, bool add);

	// �ѱ��������Ϣд���ļ���savetableʱ���ã�
	void writeAttribute(string fn, Attribute *a); 	

	// �ӱ�t�л��nameΪan��Indexָ��
	Attribute* getAttributebya(Table *t, string an); 
	
public:
	string msg; // ������Ϣ

	CatalogManager(){};
	~CatalogManager(){};

	// API��cm�ĵ���
	bool API_Catalog(SQLstatement &sql); 

	// ���ҵ�����ָ�룬���򷵻�NULL
	Table* findTable(string tn); 		
	// ���ҵ���������ָ�룬���򷵻�NULL
	Index* findIndex(string in); 

	// �����µı���Ϣд���ļ�
	void update_tableInfo();
	// �����µ�������Ϣд���ļ�
	void update_indexInfo(); 
	//���ļ��ж������Ϣ
	void read_tableInfo(); 
	//���ļ��ж���������Ϣ
	void read_indexInfo(); 
 
};

#endif