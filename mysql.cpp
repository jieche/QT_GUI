#include "MySql.h"
#include <QDir>
#include<QDebug>
#include <QSqlQuery>
#include <QSqlError>

MySql::MySql()
{
	CreateConnection();
}

MySql::~MySql()
{
	closeDb();
}

bool  MySql::CreateConnection()
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName("localhost");
	db.setPort(3306);
	db.setDatabaseName("test");
	db.setUserName("root");
	db.setPassword("123456");
	if (db.open())
	{
		m_db = db;
	}
	else
	{
		qDebug() << "opened error";
	}
	return 0;
}

void  MySql::closeDb()
{
	if (m_db.isOpen())
	{
		m_db.close();
	}
}

void MySql::insert(QString sql )
{
	QSqlQuery qsQuery = QSqlQuery(m_db);
	qsQuery.prepare(sql);
	qsQuery.exec();
	CommitDB(qsQuery);
}

void  MySql::requestAccessToken(void)
{
	mutex.lock();                           //如果已经lock且还未unlock，一直等待，直到unlock，然后lock//
}

void  MySql::releaseAccessToken(void)
{
	mutex.unlock();
}

bool  MySql::CommitDB(QSqlQuery& qsQuery)
{
	if (qsQuery.isActive())//成功执行sql语句 isActive会返回true,否则返回false
	{
		m_db.commit();
		return true;
	}
	else
	{
		m_db.rollback();
		return false;
	}
}
