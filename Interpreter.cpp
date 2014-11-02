#include "Interpreter.h"
using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////
//获取用户输入，并对输入作有效性检查，若正确，返回语句的内部表示形式
string Interpreter(string statement)
{
	string SQL;
	string temp;
	int start=0, end;
	if (statement.empty()){
		cout << "syntax error: empty statement!" << endl;
		SQL = "99";
		return SQL;
	}
	else
		SQL = lower(statement);
	//获取输入的第一个单词
	start = SQL.find_first_not_of(' ',0);
	end = SQL.find_first_of(' ',start);
	temp = SQL.substr(start, end - start);//第一个词
	//获取第二个单词
	start = end + 1;

	//若为create语句
	if(temp=="create")
		SQL=create_clause(SQL,start);
	//若为drop语句
	else if(temp=="drop")
		SQL=drop_clause(SQL,start);
	//若为select语句
	else if(temp=="select")
		SQL=select_clause(SQL,start);
	//若为insert语句
	else if(temp=="insert")
		SQL=insert_clause(SQL,start);
	//若为delete语句
	else if(temp=="delete")
		SQL=delete_clause(SQL,start);
	//若为use语句
	else if(temp=="use")
		SQL=use_clause(SQL,start);
	//若为execfile语句
	else if(temp=="execfile")
		SQL=execfile_clause(SQL,start);
	//若为quit语句
	else if(temp=="quit")
		SQL=quit_clause(SQL,start);
	//获取帮助
	else if(temp=="help")
		SQL="80";
	//若为非法语句
	else
	{
		cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
		SQL="99";
	}
	//返回输入语句的内部形式
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//改为小写
string lower(string s)
{
	transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create语句是否有效
string create_clause(string SQL,int start)
{
	string temp;
	int end;
	//获取第二个单词
	end = SQL.find_first_of(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;  //start从第三个词开始

	//若无，打印出错信息
	if (start == 0 || temp.empty())
	{
		cout<<"syntax error: syntax error for create statement!"<<endl;
		SQL="99";
	}
	//若为database,继续验证
	else if(temp=="database")
		SQL=create_database(SQL,start);
	//若为table,继续验证
	else if(temp=="table")
		SQL=create_table(SQL,start);
	//若为index,继续验证
	else if(temp=="index")
		SQL=create_index(SQL,start);
	//若为错误信息，打印出错信息
	else
	{
		cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
		SQL="99";
	}
	//返回create语句的内部形式
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create database语句是否有效
string create_database(string SQL, int start)
{
	string temp;
	int end;
	//获取第三个单词
	end = SQL.find_first_of(';', start);
	temp = SQL.substr(start, end - start);  //数据库名
	start = end + 1;  //start从第四个词开始

	//若无，打印出错信息
	if (start == 0 || temp.empty())
	{
		cout << "error: database name has not been specified!" << endl;
		SQL = "99";
	}
	else
	{
		//若为非法信息，打印出错信息
		if (SQL.at(end) != ';' || end != SQL.length() - 1)
		{
			cout << "error12:" << temp << "---is not a valid database name!" << endl;
			SQL = "99";
		}
		//返回create database语句的内部形式
		else
			SQL = "00" + temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create table语句是否有效
string create_table(string SQL,int start)
{
	string temp,sql,T;
	int index,end,length;
	//获取表名
	end = SQL.find_first_of('(', start);
	temp = SQL.substr(start, end - start);  //表名
	start = end + 1;  //start从第四个词开始
	index=start;
	if((end=SQL.Find('(',start))==-1)
	{
		cout<<"error: missing ( in the statement!"<<endl;
		SQL="99";
		return SQL;
	}
	temp=SQL.Mid(start,end-start);  //表名
	start=end+1;   // (
	if(!temp.IsEmpty())
	{
		while(SQL.GetAt(start)==' ')
			start++;
		length=temp.GetLength()-1;
		while(temp.GetAt(length)==' ')
			length--;
		temp=temp.Left(length+1);      //?
	}
	//若无，打印出错信息
	if (start == 0 || temp.empty())
	{
		cout<<"error: error in create table statement!"<<endl;
		SQL="99";
		return SQL;
	}
	//若为非法信息，打印出错信息
	else if(temp.Find(' ')!=-1)   //名字中有空格
	{
		cout<<"error: "<<temp<<"---is not a valid table name!"<<endl;
		SQL="99";
		return SQL;
	}
	else
	{
		sql=temp+",";
		//获取每个属性
		while((end=SQL.Find(',',start))!=-1)
		{
			temp=SQL.Mid(start,end-start);  //第一个属性
			start=end+1;
			//若有空属性，打印出错信息
			if (start == 0 || temp.empty())
			{
				cout<<"error: error in create table statement!"<<endl;
				SQL="99";
				return SQL;
			}
			//保存属性
			else
			{
				sql=get_attribute(temp,sql);
				if(sql=="99")
					return sql;
			}
			while(SQL.GetAt(start)==' ')
				start++;
		}
		//获得最后属性
		temp=SQL.Mid(start,SQL.GetLength()-start-1);
		length=temp.GetLength()-1;
		while(temp.GetAt(length)!=')'&&length>=0)
			length--;
		//若无，打印出错信息
		if(length<1)
		{
			cout<<"error: error in create table statement!"<<endl;
			SQL="99";
			return SQL;
		}
		//存储属性
		else
		{
			temp=temp.Left(length);
			end=SQL.Find(' ',start);
			T=SQL.Mid(start,end-start);
			start=end+1;
			//若为主键定义
			if(T=="primary")
			{
				//判断是否有关键字key
				temp+=")";
				while(SQL.GetAt(start)==' ')
					start++;
				end=SQL.Find('(',start);
				T=SQL.Mid(start,end-start);
				start=end+1;
				length=T.GetLength()-1;
				while(T.GetAt(length)==' ')
					length--;
				T=T.Left(length+1);
				//若为空，打印出错信息
				if(T.IsEmpty())
				{
					cout<<"syntax error: syntax error in create table statement!"<<endl;
					cout<<"\t missing key word key!"<<endl;
					SQL="99";
					return SQL;
				}
				//若有，继续验证
				else if(T=="key")
				{
					//获取主键属性名
					while(SQL.GetAt(start)==' ')
						start++;
					end=SQL.Find(')',start);
					T=SQL.Mid(start,end-start);
					length=T.GetLength()-1;
					while(T.GetAt(length)==' ')
						length--;
					T=T.Left(length+1);
					//若无，打印出错信息
					if(T.IsEmpty())
					{
						cout<<"error : missing primary key attribute name!"<<endl;
						SQL="99";
						return SQL;
					}
					//若为非法信息，打印出错信息
					else if(T.Find(' ')!=-1)
					{
						cout<<"error : "<<T<<"---is not a valid primary key attribute name!"<<endl;
						SQL="99";
						return SQL;
					}
					//保存主键
					else
					{
						sql+=T+" #,";
						SQL="01"+sql;
					}
				}
				//若为非法信息，打印出错信息
				else
				{
					cout<<"error : "<<T<<"---is not a valid key word!"<<endl;
					SQL="99";
					return SQL;
				}
			}
			//若为一般属性
			else   //还可以不定义主键？
			{
				sql=get_attribute(temp,sql);
				if(sql=="99")
				{
					SQL="99";
					return SQL;
				}
				else
					SQL="01"+sql;
			}
		}
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//获得属性
string get_attribute(string temp,string sql)
{
	int start=0,end,index;
	string T,C;
	temp+=" ";
	//获得属性名
	end=temp.Find(' ',start);
	T=temp.Mid(start,end-start);
	start=end+1;
	sql+=T+" ";
	//获得数据类型
	while(temp.GetAt(start)==' ')
		start++;
	end=temp.Find(' ',start);
	T=temp.Mid(start,end-start);
	start=end+1;
	//若无，打印出错信息
	if(T.IsEmpty())
	{
		cout<<"error : error in create table statement!"<<endl;
		sql="99";
		return sql;
	}
	//若为int
	else if(T=="int")
		sql+="+";
	//若为float
	else if(T=="float")
		sql+="-";
	//其他
	else
	{
		index=T.Find('(');   //不是）？
		C=T.Left(index);
		index++;
		//若有误，打印出错信息
		if(C.IsEmpty())
		{
			cout<<"error: "<<T<<"---is not a valid data type definition!"<<endl;
			sql="99";
			return sql;
		}
		//若为char
		else if(C=="char")
		{
			C=T.Mid(index,T.GetLength()-index-1);
			if(C.IsEmpty())
			{
				cout<<"error: the length of the data type char has not been specified!"<<endl;
				sql="99";
				return sql;
			}
			else
				sql+=C;
		}
		//若为非法信息，打印出错信息
		else
		{
			cout<<"error: "<<C<<"---is not a valid key word!"<<endl;
			sql="99";
			return sql;
		}
	}
	//是否有附加信息
	while(start<temp.GetLength()&&temp.GetAt(start)==' ')
		start++;
	if(start<temp.GetLength())
	{
		//若为unique定义，保存信息
		end=temp.Find(' ',start);
		T=temp.Mid(start,end-start);
		if(T=="unique")
		{
			sql+=" 1,";
		}
		//若为非法信息，打印出错信息
		else
		{
			cout<<"error: "<<temp<<"---is not a valid key word!"<<endl;
			sql="99";
			return sql;
		}
	}
	//若无附加信息
	else
		sql+=" 0,";
	return sql;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create index语句是否有效
string create_index(string SQL,int start)
{
	string temp,sql;
	int end;
	//获取第三个单词
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//若无，打印出错信息
	if (start == 0 || temp.empty())
	{
		cout<<"syntax error: syntax error for create index statement!"<<endl;
		SQL="99";
	}
	else
	{
		sql=temp;
		//获取第四个单词
		while(SQL.GetAt(start)==' ')
			start++;
		end=SQL.Find(' ',start);
		temp=SQL.Mid(start,end-start);
		start=end+1;
		//若无，打印出错信息
		if (start == 0 || temp.empty())
		{
			cout<<"syntax error: syntax error for create index statement!"<<endl;
			SQL="99";
		}
		//若为on,继续验证
		else if(temp=="on")
			SQL=create_index_on(SQL,start,sql);
		//若为非法信息，打印非法信息
		else
		{
			cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
			SQL="99";
		}
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证create index on语句是否有效
string create_index_on(string SQL,int start,string sql)
{
	string temp;
	int end,length;
	//获取表名
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find('(',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//若无，打印出错信息
	if (start == 0 || temp.empty())
	{
		cout<<"syntax error: syntax error for create index statement!"<<endl;
		cout<<"\t missing ( !"<<endl;
		SQL="99";
		return SQL;
	}
	else
	{
		//检验是否为有效文件名
		length=temp.GetLength()-1;
		while(temp.GetAt(length)==' ')
			length--;
		temp=temp.Left(length+1);
		//有效                            //?
		if(temp.Find(' ')==-1)
		{
			sql+=" "+temp;
			//获取属性名
			while(SQL.GetAt(start)==' ')
				start++;
			end=SQL.Find(')',start);
			temp=SQL.Mid(start,end-start);
			start=end+1;
			//若无，打印出错信息
			if (start == 0 || temp.empty())
			{
				cout<<"syntax error: syntax error for create index statement!"<<endl;
				cout<<"\t missing ) !"<<endl;
				SQL="99";
				return SQL;
			}
			else
			{
				//检验是否为有效属性名
				length=temp.GetLength()-1;
				while(temp.GetAt(length)==' ')
					length--;
				temp=temp.Left(length+1);
				//有效
				if(temp.Find(' ')==-1)
				{
					sql+=" "+temp;
					while(SQL.GetAt(start)==' ')
						start++;
					if(SQL.GetAt(start)!=';'||start!=SQL.GetLength()-1)
					{
						cout<<"syntax error: syntax error for quit!"<<endl;
						SQL="99";
						return SQL;
					}
					//返回create index语句的内部形式
					else
						SQL="02"+sql;
				}
				//无效,打印出错信息
				else
				{
					cout<<"error:"<<" "<<temp<<"---is not a valid attribute name!"<<endl;
					SQL="99";
					return SQL;
				}
			}
		}
		//无效,打印出错信息
		else
		{
			cout<<"error:"<<" "<<temp<<"---is not a valid table name!"<<endl;
			SQL="99";
			return SQL;
		}
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop语句是否有效
string drop_clause(string SQL,int start)
{

	string temp;
	int end;
	//获取第二个单词
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;  //start从第三个词开始
	//若无，打印出错信息
	if(string)
	{
		cout<<"syntax error: syntax error for drop statement!"<<endl;
		SQL="99";
	}
	//若为database,继续验证
	else if(temp=="database")
		SQL=drop_database(SQL,start);
	//若为table,继续验证
	else if(temp=="table")
		SQL=drop_table(SQL,start);
	//若为index,继续验证
	else if(temp=="index")
		SQL=drop_index(SQL,start);
	//若为错误信息，打印出错信息
	else
	{
		cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
		SQL="99";
	}
	//返回drop语句的内部形式
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop database语句是否有效
string drop_database(string SQL,int start)
{
	string temp;
	int end;
	//获取第三个单词
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//若无，打印出错信息
	if(string)
	{
		cout<<"error: database name has not been specified!"<<endl;
		SQL="99";
	}
	else
	{
		while(SQL.GetAt(start)==' ')
			start++;
		//若为非法信息，打印出错信息
		if(SQL.GetAt(start)!=';'||start!=SQL.GetLength()-1)
		{
			cout<<"error:"<<SQL.Mid(index,SQL.GetLength()-index-2)<<"---is not a valid database name!"<<endl;
			SQL="99";
		}
		//返回drop database语句的内部形式
		else
			SQL="10"+temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop table语句是否有效
string drop_table(string SQL,int start)  //drop table 表名 ;
{
	string temp;
	int end;
	//获取第三个单词
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);   //temp=table名
	start=end+1;
	//若无，打印出错信息


	if(string)
	{
		cout<<"error: table name has not been specified!"<<endl;
		SQL="99";
	}
	else
	{
		while(SQL.GetAt(start)==' ')
			start++;
		//若为非法信息，打印出错信息
		if(SQL.GetAt(start)!=';'||start!=SQL.GetLength()-1)
		{
			cout<<"error:"<<SQL.Mid(index,SQL.GetLength()-index-2)<<"---is not a valid table name!"<<endl;
			SQL="99";
		}
		//返回drop table语句的内部形式
		else
			SQL="11"+temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证drop index语句是否有效
string drop_index(string SQL,int start)  //drop index 索引名 ;
{
	string temp;
	int end;
	//获取第三个单词
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//若无，打印出错信息

	if(string)
	{
		cout<<"error: index name has not been specified!"<<endl;
		SQL="99";
	}
	else
	{
		while(SQL.GetAt(start)==' ')
			start++;
		//若为非法信息，打印出错信息
		if(SQL.GetAt(start)!=';'||start!=SQL.GetLength()-1)
		{
			cout<<"error:"<<SQL.Mid(index,SQL.GetLength()-index-2)<<"---is not a valid index name!"<<endl;
			SQL="99";
		}
		//返回drop index语句的内部形式
		else
			SQL="12"+temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证select 语句是否有效
string select_clause(string SQL,int start)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
//获得属性组或文件组的每一项
string get_part(string temp,string sql,string kind)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证insert语句是否有效
string insert_clause(string SQL,int start)  //insert into 表名 values ( 值1 , 值2 , … , 值n ) ;
{
	string temp,sql;
	int end;
	//获取第二个单词
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);  //temp = into
	start=end+1;
	//若无，打印出错信息
	if(string)
	{
		cout<<"syntax error: syntax error for insert statement!"<<endl;
		SQL="99";
	}
	else if (temp != 'into')
	{
		cout<<"syntax error: syntax error for insert statement!"<<endl;
		SQL="99";

	}
	else
	{
		//获取第三个单词
		while(SQL.GetAt(start)==' ')
			start++;
		index=start;
		end=SQL.Find(' ',start);
		temp=SQL.Mid(start,end-start);  //temp = table名
		sql += temp;   //sql = table名
		start=end+1;
		//若无，打印出错信息
		if(string)
		{
			cout<<"syntax error: syntax error for insert statement!"<<endl;
			SQL="99";
		}
		else
		{

			//获取第三个单词
			while(SQL.GetAt(start)==' ')
				start++;
			index=start;
			end=SQL.Find(' ',start);
			temp=SQL.Mid(start,end-start);  //temp = value
			start=end+1;                    //strat = (
			//若无，打印出错信息
			if(string)
			{
				cout<<"syntax error: syntax error for insert statement!"<<endl;
				SQL="99";
			}
			else if (temp != "value")
			{
				cout<<"syntax error: syntax error for insert statement!"<<endl;
				SQL="99";
			}
			else
			{
				SQL=insert_into_values(SQL,start,sql);
			}
		}
	}

	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证insert into values语句是否有效
string insert_into_values(string SQL,int start,string sql)
{
	string temp;                      //insert into 表名 values ( 值1 , 值2 , … , 值n ) ;
	string T;
	int end;
	//获取(
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);  //temp = (
	start=end+1;
	//若无，打印出错
	if(string)
	{
		cout<<"syntax error: syntax error for insert into statement!"<<endl;
		SQL="99";
	}
	else if(temp != '(')
	{
		cout<<"syntax error: syntax error for insert into statement!"<<endl;
		SQL="99";
	}
	else
	{
		end=SQL.Find(' ',start);
		temp=SQL.Mid(start,end-start);  //temp = ( 值1 , 值2 , … , 值n )
		start=end+1;
		//若无，打印出错
		if(string)
		{
			cout<<"syntax error: syntax error for insert into statement!"<<endl;
			SQL="99";
		}
		else
		{
			SQL="30"+sql+' '+temp;
		}
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证delete语句是否有效
string delete_clause(string SQL,int start)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证 delete from where 语句是否有效
string delete_from_where(string SQL,int start,string sql)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
//将表达式转化为内部形式
string get_expression(string temp,string sql)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
//获取表达式组的每个表达式
string get_each(string T,string sql,string condition)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证use语句是否有效
string use_clause(string SQL,int start)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证execfile语句是否有效
string execfile_clause(string SQL,int start)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
//验证quit语句是否有效
string quit_clause(string SQL,int start)
{

}
