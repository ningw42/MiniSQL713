#ifndef _API_MODULE_H_
#define _API_MODULE_H_

#include "MiniSQL.h"

class APIMoudule
{
private:
	string lower(string s);
	void getHelp();

	//�ӱ�t�л��indexnameΪan��Indexָ��
	Attribute* getAttributebyi(Table *t, string in);
public:
	APIMoudule(){};
	~APIMoudule(){};

	//��������ģ�鴦��SQLstatement
	void API(SQLstatement &s);
	
	//��ȡ�û�����
	string read_input();
};

#endif
