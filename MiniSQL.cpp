#include "MiniSQL.h"
#include "Interpreter.h"
//#include "API_Module.h"
using namespace std;

int main(void)
{
	string SQL;
	//打印软件信息
	cout<<endl;
	cout<<"\t\t*************************************************"<<endl;
	cout<<"\t\t**                   MiniSQL                   **"<<endl;
	cout<<"\t\t**                Version (1.0)                **"<<endl;
	cout<<"\t\t**                                             **"<<endl;
	cout<<"\t\t**     copyright(2014) all right reserved!     **"<<endl;
	cout<<"\t\t*************************************************"<<endl;
	cout<<endl<<endl;
	while(1)
	{
		cout << "MiniSQL-->> ";
		SQL = Interpreter();
		cout << SQL << endl;
		//API_Module(SQL);
	}
	return 0;
}

