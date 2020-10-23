#pragma once
#include <qdom.h>

class XML_config
{
public:
	static XML_config* instance()
	{
		static XML_config config;
		return &config;
	}
private:
	XML_config() : m_MySql_HostPort(0), m_post_HostPort(0)
	{
		read_xml();
	}

	void read_xml()
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
				if (e.tagName() == "info")
				{
					auto path = e.attribute("path");
					auto title = e.attribute("title");
					if (!title.isEmpty())
					{
						m_title = title;
					}
					auto linuxPath = e.attribute("linux");
					if (!linuxPath.isEmpty())
					{
						m_desDiskLinuxPath = linuxPath;
					}
					m_desPath = path.trimmed();
					m_desPath.replace('\\', '/');
				}
				else if (e.tagName() == "mysql")
				{
					m_MySql_HostName = e.attribute("hostName").trimmed();
					m_MySql_HostPort = e.attribute("hostPort").toInt();
					m_MySql_DatabaseName = e.attribute("databaseName").trimmed();
					m_MySql_UserName = e.attribute("UserName").trimmed();
					m_MySql_Password = e.attribute("password").trimmed();
				}
				else if (e.tagName() == "postgres")
				{
					m_post_HostName = e.attribute("hostName").trimmed();
					m_post_HostPort = e.attribute("hostPort").toInt();
					m_post_DatabaseName = e.attribute("databaseName").trimmed();
					m_post_UserName = e.attribute("UserName").trimmed();
					m_post_Password = e.attribute("password").trimmed();
				}
				else if (e.tagName() == "pattern")
				{
					QString pattern = e.attribute("p");
					m_patternList.push_back(pattern);
				}
				else if (e.tagName() == "prefix")
				{
					QString key = e.attribute("key");
					m_prefixMap[key] = e.attribute("model_name");
				}
				else if (e.tagName() == "contain")
				{
					QString key = e.attribute("key");
					m_containMap[key] = e.attribute("model_name");
				}
			}
			node = node.nextSibling(); //下一个兄弟节点,nextSiblingElement()是下一个兄弟元素，都差不多
		}
		file.close();
	}
public:
	QString     title() const { return m_title; }
	QString     desDiskLinuxPath() const {return m_desDiskLinuxPath; }
	QString     desPath() const { return m_desPath; }
	QStringList patternList() const { return m_patternList; }
	
	QString		MySql_HostName() const { return m_MySql_HostName; }
	int			MySql_HostPort() const { return m_MySql_HostPort; }
	QString		MySql_DatabaseName() const{ return m_MySql_DatabaseName; }
	QString		MySql_UserName() const{ return m_MySql_UserName; }
	QString		MySql_Password() const{ return m_MySql_Password; }
	
private:
	QString       m_title = "数据迁徙平台";
	QString       m_desDiskLinuxPath;
	QString       m_desPath;
	QStringList	  m_patternList;
	
	QString		  m_MySql_HostName;
	int			  m_MySql_HostPort;
	QString		  m_MySql_DatabaseName;
	QString		  m_MySql_UserName;
	QString		  m_MySql_Password;

	QString		  m_post_HostName;
	int			  m_post_HostPort;
	QString		  m_post_DatabaseName;
	QString		  m_post_UserName;
	QString		  m_post_Password;

	QMap<QString, QString> m_prefixMap;
	QMap<QString, QString> m_containMap;

	

	

	
};
