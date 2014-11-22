#ifndef _API_MODULE_H_
#define _API_MODULE_H_

#include "MiniSQL.h"

class APIMoudule
{
private:
	string lower(string s);
	void getHelp();

	//从表t中获得indexname为an的Index指针
	Attribute* getAttributebyi(Table *t, string in);
public:
	APIMoudule(){};
	~APIMoudule(){};

	//调用其它模块处理SQLstatement
	void API(SQLstatement &s);
	
	//获取用户输入
	string read_input();
};

#endif
