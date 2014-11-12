#ifndef _API_MODULE_H_
#define _API_MODULE_H_

#include "MiniSQL.h"

class APIMoudule
{
public:
	APIMoudule(){};
	~APIMoudule(){};
	void API(SQLstatement s);
	void getHelp();
	//获取用户输入
	string read_input();
	//改为小写
	string lower(string s);

};

#endif
