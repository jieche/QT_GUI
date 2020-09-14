#pragma once
#include <QWidget>
#include <QSqlDatabase>
#include <QtCore/QMutex>


class MySql
{
public:
	MySql();
	~MySql();

private:
	bool CreateConnection();      //创建数据库连接//
	bool isDirExist();            //路径是否存在//

	QString connectionName;       //连接名称//
	QString dbDir;                //.db文件路径//
	QSqlDatabase m_db;
	QMutex mutex;                 //多线程资源访问控制//
public:
	void closeDb();               //关闭数据库//

	void insert(QString sql); //新增
	bool MyDelete(int InputId);                               //删除//
private:
	void  requestAccessToken(void);                           //加锁//
	void  releaseAccessToken(void);                           //解锁//
	bool CommitDB(QSqlQuery& qsQuery);
private:
	QString m_HostName;
	QString m_DatabaseName;
	QString m_UserName;
	QString m_Password;

};
