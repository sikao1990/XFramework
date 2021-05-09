#include <stdio.h>
#include "../XPublic.h"

extern void showSqlResult(list< list<DBField> >& lst);

int main()
{
	REGISTER_DB_ENGINE("mysql", MysqlConn, MysqlAccess);
	DBConnPool dbPools;
	DBParam tagDB("mysql");
	//tagDB.param._adoParam.m_dbBase = "";
	//tagDB.param._adoParam.m_dbIp = "localhost";
	//tagDB.param._adoParam.m_dbName = "test";
	//tagDB.param._adoParam.m_user = "sa";
	//tagDB.param._adoParam.m_passwd = "zhubin123";
	dbPools.Init(tagDB);
	DBConnect* pDBConn = dbPools.GetNewConnect();
	DBAccess* pDBAccess = pDBConn->GetDBAccessProxy();

	//char szbuf[256] = {};
	//sprintf(szbuf, "insert into tb(name,fkb) values(%s,%d)", "\'hello\'", 3);
	//pDBAccess->Execute(szbuf);
	
	//vector<FieldInfo> fargs;
	//FieldInfo tmp;
	//tmp.fieldName = "name";
	//tmp.fieldType = DT_STRING;
	//fargs.push_back(tmp);
	//tmp.fieldName = "fkb";
	//tmp.fieldType = DT_INT;
	//fargs.push_back(tmp);
	//pDBAccess->Execute("insert into tb(name,fkb) values(?,%d)", fargs, TT_Sql,"world",4);
	//test code
	list< list<DBField> > reslst;
	//pDBAccess->Query("select * from userInfo", reslst);
	//printf("%d\n",reslst.size());
	//showSqlResult(reslst);
	//fargs.clear();
/*
	char data[4] = { 'A','A','A','A' };
	tmp.fieldName = "pic";
	tmp.fieldType = DT_BLOB;
	fargs.push_back(tmp);
	//tmp.fieldName = "";
	//tmp.fieldType = DT_INT;
	//fargs.push_back(tmp);
	pDBAccess->Execute("update example_blob set pic=? where id=99", fargs, TT_Sql,data,4);
*/
	//fargs.clear();
	//tmp.fieldName = "id";
	//tmp.fieldType = DT_INT;
	//fargs.push_back(tmp);
	//reslst.clear();
	//pDBAccess->Query("select * from example_blob where id=?", fargs, reslst, TT_Sql, 99);
	pDBAccess->Query("select pic from example_blob where id=99",reslst);
	printf("row:%d\n", reslst.size());
	showSqlResult(reslst);

	pDBConn->ReleaseDBAccessProxy(pDBAccess);
	dbPools.ReleaseConn(pDBConn);
    return 0;
}

void showSqlResult(list< list<DBField> >& lst)
{
	for (list< list<DBField> >::iterator it = lst.begin(); it != lst.end(); it++)
	{
		list<DBField>& row = *it;
		printf("row start:");
		for (list<DBField>::iterator itr = row.begin(); itr != row.end(); itr++)
		{
			switch (itr->fieldType)
			{
			case DT_INT:
				printf(" %s:%d", itr->fieldName.c_str(), itr->ptr.nInt32);
				break;
			case DT_INT64:
				printf(" %s:%lld", itr->fieldName.c_str(), itr->ptr.nInt64);
				break;
			case DT_STRING:
				printf(" %s:%s", itr->fieldName.c_str(), itr->ptr.pStr);
				break;
			case DT_DOUBLE:
				printf(" %s:%lf", itr->fieldName.c_str(), itr->ptr.lfDouble);
				break;
			case DT_BLOB:
				printf(" %s:", itr->fieldName.c_str());
				for (int i = 0; i < itr->len; i++)
					printf(" %x ",itr->ptr.pBlob[i]);
				break;
			}
		}
		puts("");
	}
}