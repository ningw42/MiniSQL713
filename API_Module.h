#if !defined(_API_MODULE_H_)
#define _API_MODULE_H_

#include "MiniSQL.h"

void  API_Module(string SQL);

//��ʾ������Ϣ
void Help();

//���������ڵ�
void Insert_Index(string DB_Name, string Table_Name, index_info & index);

//ɾ�������ڵ�
void Delete_Index(string DB_Name, string Table_Name, index_info & index);

//���������ڵ�
void Find_Index(string DB_Name, string Table_Name, index_info & index);

//��ȡ������������Ϣ
void Get_Index(string DB_Name, string Table_Name, int & start, int & end, int type, index_info & index);

#endif
