#include "Postgres.h"
#include <QDir>
#include<QDebug>
#include <qdom.h>
#include <QSqlQuery>
#include <QSqlError>

Postgres::Postgres()
{
	readXML();
	CreateConnection();
	
}

Postgres::~Postgres()
{
	closeDb();
}

bool  Postgres::CreateConnection()
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL","postgressql");
	db.setHostName(m_HostName);
	db.setPort(m_HostPort);
	db.setDatabaseName(m_DatabaseName);
	db.setUserName(m_UserName);
	db.setPassword(m_Password);
	if (db.open())
	{
		m_db = db;
		qDebug() << "opened sucess Postgres" ;
	}
	else
	{
		QString text = db.lastError().driverText();
		qDebug() << "opened error"<< db.lastError();
	}
	return 0;
}

void  Postgres::closeDb()
{
	if (m_db.isOpen())
	{
		m_db.close();
	}
}

QVariant Postgres::insert(QString sql )
{
	QSqlQuery qsQuery = QSqlQuery(m_db);
	qsQuery.prepare(sql);
	qsQuery.exec();
	CommitDB(qsQuery);
	return qsQuery.lastInsertId();
}

QString Postgres::selectOne(QString sql)
{
	QSqlQuery qsQuery = QSqlQuery(m_db);
	qsQuery.prepare(sql);
	qsQuery.exec();
	CommitDB(qsQuery);
	while (qsQuery.next())
	{
		return qsQuery.value(0).toString();
	}
	return "";
}

void  Postgres::requestAccessToken(void)
{
	mutex.lock();                           //如果已经lock且还未unlock，一直等待，直到unlock，然后lock//
}

void  Postgres::releaseAccessToken(void)
{
	mutex.unlock();
}

bool  Postgres::CommitDB(QSqlQuery& qsQuery)
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

void Postgres::readXML()
{
	//打开或创建文件
	QFile file("config.xml"); //相对路径、绝对路径、资源路径都行
	if (!file.open(QFile::ReadOnly))
		return;

	QDomDocument doc;
	if (!doc.setContent(&file))
	{
		file.close();
		return;
	}
	file.close();

	QDomElement root = doc.documentElement(); //返回根节点
	QDomNode node = root.firstChild(); //获得第一个子节点
	while (!node.isNull())  //如果节点不空
	{
		if (node.isElement()) //如果节点是元素
		{
			QDomElement e = node.toElement(); //转换为元素，注意元素和节点是两个数据结构，其实差不多
			if (e.tagName() == "postgres")
			{
				m_HostName = e.attribute("hostName").trimmed();
				m_HostPort = e.attribute("hostPort").toInt();
				m_DatabaseName = e.attribute("databaseName").trimmed();
				m_UserName = e.attribute("UserName").trimmed();
				m_Password = e.attribute("password").trimmed();
				break;
			}
		}
		node = node.nextSibling(); //下一个兄弟节点,nextSiblingElement()是下一个兄弟元素，都差不多
	}
	file.close();
}

