#include "Interpreter.h"
using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////
//��ȡ�û����룬������������Ч�Լ�飬����ȷ�����������ڲ���ʾ��ʽ
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
	//��ȡ����ĵ�һ������
	start = SQL.find_first_not_of(' ',0);
	end = SQL.find_first_of(' ',start);
	temp = SQL.substr(start, end - start);//��һ����
	//��ȡ�ڶ�������
	start = end + 1;

	//��Ϊcreate���
	if(temp=="create")
		SQL=create_clause(SQL,start);
	//��Ϊdrop���
	else if(temp=="drop")
		SQL=drop_clause(SQL,start);
	//��Ϊselect���
	else if(temp=="select")
		SQL=select_clause(SQL,start);
	//��Ϊinsert���
	else if(temp=="insert")
		SQL=insert_clause(SQL,start);
	//��Ϊdelete���
	else if(temp=="delete")
		SQL=delete_clause(SQL,start);
	//��Ϊuse���
	else if(temp=="use")
		SQL=use_clause(SQL,start);
	//��Ϊexecfile���
	else if(temp=="execfile")
		SQL=execfile_clause(SQL,start);
	//��Ϊquit���
	else if(temp=="quit")
		SQL=quit_clause(SQL,start);
	//��ȡ����
	else if(temp=="help")
		SQL="80";
	//��Ϊ�Ƿ����
	else
	{
		cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
		SQL="99";
	}
	//�������������ڲ���ʽ
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��ΪСд
string lower(string s)
{
	transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤create����Ƿ���Ч
string create_clause(string SQL,int start)
{
	string temp;
	int end;
	//��ȡ�ڶ�������
	end = SQL.find_first_of(' ', start);
	temp = SQL.substr(start, end - start);
	start = end + 1;  //start�ӵ������ʿ�ʼ

	//���ޣ���ӡ������Ϣ
	if (start == 0 || temp.empty())
	{
		cout<<"syntax error: syntax error for create statement!"<<endl;
		SQL="99";
	}
	//��Ϊdatabase,������֤
	else if(temp=="database")
		SQL=create_database(SQL,start);
	//��Ϊtable,������֤
	else if(temp=="table")
		SQL=create_table(SQL,start);
	//��Ϊindex,������֤
	else if(temp=="index")
		SQL=create_index(SQL,start);
	//��Ϊ������Ϣ����ӡ������Ϣ
	else
	{
		cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
		SQL="99";
	}
	//����create�����ڲ���ʽ
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤create database����Ƿ���Ч
string create_database(string SQL, int start)
{
	string temp;
	int end;
	//��ȡ����������
	end = SQL.find_first_of(';', start);
	temp = SQL.substr(start, end - start);  //���ݿ���
	start = end + 1;  //start�ӵ��ĸ��ʿ�ʼ

	//���ޣ���ӡ������Ϣ
	if (start == 0 || temp.empty())
	{
		cout << "error: database name has not been specified!" << endl;
		SQL = "99";
	}
	else
	{
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		if (SQL.at(end) != ';' || end != SQL.length() - 1)
		{
			cout << "error12:" << temp << "---is not a valid database name!" << endl;
			SQL = "99";
		}
		//����create database�����ڲ���ʽ
		else
			SQL = "00" + temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤create table����Ƿ���Ч
string create_table(string SQL,int start)
{
	string temp,sql,T;
	int index,end,length;
	//��ȡ����
	end = SQL.find_first_of('(', start);
	temp = SQL.substr(start, end - start);  //����
	start = end + 1;  //start�ӵ��ĸ��ʿ�ʼ
	index=start;
	if((end=SQL.Find('(',start))==-1)
	{
		cout<<"error: missing ( in the statement!"<<endl;
		SQL="99";
		return SQL;
	}
	temp=SQL.Mid(start,end-start);  //����
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
	//���ޣ���ӡ������Ϣ
	if (start == 0 || temp.empty())
	{
		cout<<"error: error in create table statement!"<<endl;
		SQL="99";
		return SQL;
	}
	//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
	else if(temp.Find(' ')!=-1)   //�������пո�
	{
		cout<<"error: "<<temp<<"---is not a valid table name!"<<endl;
		SQL="99";
		return SQL;
	}
	else
	{
		sql=temp+",";
		//��ȡÿ������
		while((end=SQL.Find(',',start))!=-1)
		{
			temp=SQL.Mid(start,end-start);  //��һ������
			start=end+1;
			//���п����ԣ���ӡ������Ϣ
			if (start == 0 || temp.empty())
			{
				cout<<"error: error in create table statement!"<<endl;
				SQL="99";
				return SQL;
			}
			//��������
			else
			{
				sql=get_attribute(temp,sql);
				if(sql=="99")
					return sql;
			}
			while(SQL.GetAt(start)==' ')
				start++;
		}
		//����������
		temp=SQL.Mid(start,SQL.GetLength()-start-1);
		length=temp.GetLength()-1;
		while(temp.GetAt(length)!=')'&&length>=0)
			length--;
		//���ޣ���ӡ������Ϣ
		if(length<1)
		{
			cout<<"error: error in create table statement!"<<endl;
			SQL="99";
			return SQL;
		}
		//�洢����
		else
		{
			temp=temp.Left(length);
			end=SQL.Find(' ',start);
			T=SQL.Mid(start,end-start);
			start=end+1;
			//��Ϊ��������
			if(T=="primary")
			{
				//�ж��Ƿ��йؼ���key
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
				//��Ϊ�գ���ӡ������Ϣ
				if(T.IsEmpty())
				{
					cout<<"syntax error: syntax error in create table statement!"<<endl;
					cout<<"\t missing key word key!"<<endl;
					SQL="99";
					return SQL;
				}
				//���У�������֤
				else if(T=="key")
				{
					//��ȡ����������
					while(SQL.GetAt(start)==' ')
						start++;
					end=SQL.Find(')',start);
					T=SQL.Mid(start,end-start);
					length=T.GetLength()-1;
					while(T.GetAt(length)==' ')
						length--;
					T=T.Left(length+1);
					//���ޣ���ӡ������Ϣ
					if(T.IsEmpty())
					{
						cout<<"error : missing primary key attribute name!"<<endl;
						SQL="99";
						return SQL;
					}
					//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
					else if(T.Find(' ')!=-1)
					{
						cout<<"error : "<<T<<"---is not a valid primary key attribute name!"<<endl;
						SQL="99";
						return SQL;
					}
					//��������
					else
					{
						sql+=T+" #,";
						SQL="01"+sql;
					}
				}
				//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
				else
				{
					cout<<"error : "<<T<<"---is not a valid key word!"<<endl;
					SQL="99";
					return SQL;
				}
			}
			//��Ϊһ������
			else   //�����Բ�����������
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
//�������
string get_attribute(string temp,string sql)
{
	int start=0,end,index;
	string T,C;
	temp+=" ";
	//���������
	end=temp.Find(' ',start);
	T=temp.Mid(start,end-start);
	start=end+1;
	sql+=T+" ";
	//�����������
	while(temp.GetAt(start)==' ')
		start++;
	end=temp.Find(' ',start);
	T=temp.Mid(start,end-start);
	start=end+1;
	//���ޣ���ӡ������Ϣ
	if(T.IsEmpty())
	{
		cout<<"error : error in create table statement!"<<endl;
		sql="99";
		return sql;
	}
	//��Ϊint
	else if(T=="int")
		sql+="+";
	//��Ϊfloat
	else if(T=="float")
		sql+="-";
	//����
	else
	{
		index=T.Find('(');   //���ǣ���
		C=T.Left(index);
		index++;
		//�����󣬴�ӡ������Ϣ
		if(C.IsEmpty())
		{
			cout<<"error: "<<T<<"---is not a valid data type definition!"<<endl;
			sql="99";
			return sql;
		}
		//��Ϊchar
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
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		else
		{
			cout<<"error: "<<C<<"---is not a valid key word!"<<endl;
			sql="99";
			return sql;
		}
	}
	//�Ƿ��и�����Ϣ
	while(start<temp.GetLength()&&temp.GetAt(start)==' ')
		start++;
	if(start<temp.GetLength())
	{
		//��Ϊunique���壬������Ϣ
		end=temp.Find(' ',start);
		T=temp.Mid(start,end-start);
		if(T=="unique")
		{
			sql+=" 1,";
		}
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		else
		{
			cout<<"error: "<<temp<<"---is not a valid key word!"<<endl;
			sql="99";
			return sql;
		}
	}
	//���޸�����Ϣ
	else
		sql+=" 0,";
	return sql;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤create index����Ƿ���Ч
string create_index(string SQL,int start)
{
	string temp,sql;
	int end;
	//��ȡ����������
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//���ޣ���ӡ������Ϣ
	if (start == 0 || temp.empty())
	{
		cout<<"syntax error: syntax error for create index statement!"<<endl;
		SQL="99";
	}
	else
	{
		sql=temp;
		//��ȡ���ĸ�����
		while(SQL.GetAt(start)==' ')
			start++;
		end=SQL.Find(' ',start);
		temp=SQL.Mid(start,end-start);
		start=end+1;
		//���ޣ���ӡ������Ϣ
		if (start == 0 || temp.empty())
		{
			cout<<"syntax error: syntax error for create index statement!"<<endl;
			SQL="99";
		}
		//��Ϊon,������֤
		else if(temp=="on")
			SQL=create_index_on(SQL,start,sql);
		//��Ϊ�Ƿ���Ϣ����ӡ�Ƿ���Ϣ
		else
		{
			cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
			SQL="99";
		}
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤create index on����Ƿ���Ч
string create_index_on(string SQL,int start,string sql)
{
	string temp;
	int end,length;
	//��ȡ����
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find('(',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//���ޣ���ӡ������Ϣ
	if (start == 0 || temp.empty())
	{
		cout<<"syntax error: syntax error for create index statement!"<<endl;
		cout<<"\t missing ( !"<<endl;
		SQL="99";
		return SQL;
	}
	else
	{
		//�����Ƿ�Ϊ��Ч�ļ���
		length=temp.GetLength()-1;
		while(temp.GetAt(length)==' ')
			length--;
		temp=temp.Left(length+1);
		//��Ч                            //?
		if(temp.Find(' ')==-1)
		{
			sql+=" "+temp;
			//��ȡ������
			while(SQL.GetAt(start)==' ')
				start++;
			end=SQL.Find(')',start);
			temp=SQL.Mid(start,end-start);
			start=end+1;
			//���ޣ���ӡ������Ϣ
			if (start == 0 || temp.empty())
			{
				cout<<"syntax error: syntax error for create index statement!"<<endl;
				cout<<"\t missing ) !"<<endl;
				SQL="99";
				return SQL;
			}
			else
			{
				//�����Ƿ�Ϊ��Ч������
				length=temp.GetLength()-1;
				while(temp.GetAt(length)==' ')
					length--;
				temp=temp.Left(length+1);
				//��Ч
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
					//����create index�����ڲ���ʽ
					else
						SQL="02"+sql;
				}
				//��Ч,��ӡ������Ϣ
				else
				{
					cout<<"error:"<<" "<<temp<<"---is not a valid attribute name!"<<endl;
					SQL="99";
					return SQL;
				}
			}
		}
		//��Ч,��ӡ������Ϣ
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
//��֤drop����Ƿ���Ч
string drop_clause(string SQL,int start)
{

	string temp;
	int end;
	//��ȡ�ڶ�������
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;  //start�ӵ������ʿ�ʼ
	//���ޣ���ӡ������Ϣ
	if(string)
	{
		cout<<"syntax error: syntax error for drop statement!"<<endl;
		SQL="99";
	}
	//��Ϊdatabase,������֤
	else if(temp=="database")
		SQL=drop_database(SQL,start);
	//��Ϊtable,������֤
	else if(temp=="table")
		SQL=drop_table(SQL,start);
	//��Ϊindex,������֤
	else if(temp=="index")
		SQL=drop_index(SQL,start);
	//��Ϊ������Ϣ����ӡ������Ϣ
	else
	{
		cout<<"syntax error:"<<" "<<temp<<"---is not a valid key word!"<<endl;
		SQL="99";
	}
	//����drop�����ڲ���ʽ
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤drop database����Ƿ���Ч
string drop_database(string SQL,int start)
{
	string temp;
	int end;
	//��ȡ����������
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//���ޣ���ӡ������Ϣ
	if(string)
	{
		cout<<"error: database name has not been specified!"<<endl;
		SQL="99";
	}
	else
	{
		while(SQL.GetAt(start)==' ')
			start++;
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		if(SQL.GetAt(start)!=';'||start!=SQL.GetLength()-1)
		{
			cout<<"error:"<<SQL.Mid(index,SQL.GetLength()-index-2)<<"---is not a valid database name!"<<endl;
			SQL="99";
		}
		//����drop database�����ڲ���ʽ
		else
			SQL="10"+temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤drop table����Ƿ���Ч
string drop_table(string SQL,int start)  //drop table ���� ;
{
	string temp;
	int end;
	//��ȡ����������
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);   //temp=table��
	start=end+1;
	//���ޣ���ӡ������Ϣ


	if(string)
	{
		cout<<"error: table name has not been specified!"<<endl;
		SQL="99";
	}
	else
	{
		while(SQL.GetAt(start)==' ')
			start++;
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		if(SQL.GetAt(start)!=';'||start!=SQL.GetLength()-1)
		{
			cout<<"error:"<<SQL.Mid(index,SQL.GetLength()-index-2)<<"---is not a valid table name!"<<endl;
			SQL="99";
		}
		//����drop table�����ڲ���ʽ
		else
			SQL="11"+temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤drop index����Ƿ���Ч
string drop_index(string SQL,int start)  //drop index ������ ;
{
	string temp;
	int end;
	//��ȡ����������
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);
	start=end+1;
	//���ޣ���ӡ������Ϣ

	if(string)
	{
		cout<<"error: index name has not been specified!"<<endl;
		SQL="99";
	}
	else
	{
		while(SQL.GetAt(start)==' ')
			start++;
		//��Ϊ�Ƿ���Ϣ����ӡ������Ϣ
		if(SQL.GetAt(start)!=';'||start!=SQL.GetLength()-1)
		{
			cout<<"error:"<<SQL.Mid(index,SQL.GetLength()-index-2)<<"---is not a valid index name!"<<endl;
			SQL="99";
		}
		//����drop index�����ڲ���ʽ
		else
			SQL="12"+temp;
	}
	return SQL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤select ����Ƿ���Ч
string select_clause(string SQL,int start)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
//�����������ļ����ÿһ��
string get_part(string temp,string sql,string kind)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤insert����Ƿ���Ч
string insert_clause(string SQL,int start)  //insert into ���� values ( ֵ1 , ֵ2 , �� , ֵn ) ;
{
	string temp,sql;
	int end;
	//��ȡ�ڶ�������
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);  //temp = into
	start=end+1;
	//���ޣ���ӡ������Ϣ
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
		//��ȡ����������
		while(SQL.GetAt(start)==' ')
			start++;
		index=start;
		end=SQL.Find(' ',start);
		temp=SQL.Mid(start,end-start);  //temp = table��
		sql += temp;   //sql = table��
		start=end+1;
		//���ޣ���ӡ������Ϣ
		if(string)
		{
			cout<<"syntax error: syntax error for insert statement!"<<endl;
			SQL="99";
		}
		else
		{

			//��ȡ����������
			while(SQL.GetAt(start)==' ')
				start++;
			index=start;
			end=SQL.Find(' ',start);
			temp=SQL.Mid(start,end-start);  //temp = value
			start=end+1;                    //strat = (
			//���ޣ���ӡ������Ϣ
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
//��֤insert into values����Ƿ���Ч
string insert_into_values(string SQL,int start,string sql)
{
	string temp;                      //insert into ���� values ( ֵ1 , ֵ2 , �� , ֵn ) ;
	string T;
	int end;
	//��ȡ(
	while(SQL.GetAt(start)==' ')
		start++;
	end=SQL.Find(' ',start);
	temp=SQL.Mid(start,end-start);  //temp = (
	start=end+1;
	//���ޣ���ӡ����
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
		temp=SQL.Mid(start,end-start);  //temp = ( ֵ1 , ֵ2 , �� , ֵn )
		start=end+1;
		//���ޣ���ӡ����
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
//��֤delete����Ƿ���Ч
string delete_clause(string SQL,int start)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤ delete from where ����Ƿ���Ч
string delete_from_where(string SQL,int start,string sql)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
//�����ʽת��Ϊ�ڲ���ʽ
string get_expression(string temp,string sql)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
//��ȡ���ʽ���ÿ�����ʽ
string get_each(string T,string sql,string condition)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤use����Ƿ���Ч
string use_clause(string SQL,int start)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤execfile����Ƿ���Ч
string execfile_clause(string SQL,int start)
{

}

/////////////////////////////////////////////////////////////////////////////////////////////
//��֤quit����Ƿ���Ч
string quit_clause(string SQL,int start)
{

}
