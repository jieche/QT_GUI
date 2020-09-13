#include "Worker.h"

Worker::Worker(QObject *parent /*= 0*/)
{
}

bool Worker::isMatch(const QString str, const QString& pattern)
{
	const QRegularExpression regularExpression(pattern);
	const QRegularExpressionMatch match = regularExpression.match(str);
	if (match.hasMatch())
	{
		return true;
	}
	return false;
}

void Worker::traverseRecusionDir(QString dirPath, QString pattern)
{
	QDir dir(dirPath);
	QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);   //获取当前所有目录

	for (int i = 0; i != folder_list.size(); ++i)         //自动递归添加各目录到上一级目录
	{

		QString namepath = folder_list.at(i).absoluteFilePath();    //获取路径
		QFileInfo folderinfo = folder_list.at(i);
		QString name = folderinfo.fileName();      //获取目录名
		const QRegularExpression regularExpression(pattern);
		const QRegularExpressionMatch match = regularExpression.match(name);
		if (match.hasMatch())
		{
			m_mutex.lock();
			m_fileInfoList.push_back(folder_list.at(i));
			m_mutex.unlock();
		}
		else
		{
			traverseRecusionDir(namepath, pattern);  //进行递归
		}
	}
}

QFileInfoList Worker::allfile(QTreeWidgetItem *root, QString path)         //参数为主函数中添加的item和路径名
{

	/*添加path路径文件*/
	QDir dir(path);          //遍历各级子目录
	QDir dir_file(path);    //遍历子目录中所有文件
	dir_file.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);        //获取当前所有文件
	dir_file.setSorting(QDir::Size | QDir::Reversed);
	QFileInfoList list_file = dir_file.entryInfoList();
	for (int i = 0; i < list_file.size(); ++i) {       //将当前目录中所有文件添加到treewidget中
		QFileInfo fileInfo = list_file.at(i);
		QString name2 = fileInfo.fileName();
		m_mutex.lock();
		QTreeWidgetItem* child = new QTreeWidgetItem(QStringList() << name2);
		child->setIcon(0, QIcon(":/file/image/link.ico"));
		child->setCheckState(1, Qt::Checked);
		root->addChild(child);
		m_mutex.unlock();
	}


	//QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);   //获取当前所有目录

	for (int i = 0; i != folder_list.size(); i++)         //自动递归添加各目录到上一级目录
	{

		QString namepath = folder_list.at(i).absoluteFilePath();    //获取路径
		QFileInfo folderinfo = folder_list.at(i);
		QString name = folderinfo.fileName();      //获取目录名
		m_mutex.lock();
		QTreeWidgetItem* childroot = new QTreeWidgetItem(QStringList() << name);
		childroot->setIcon(0, QIcon(":/file/image/link.ico"));
		childroot->setCheckState(1, Qt::Checked);
		root->addChild(childroot);              //将当前目录添加成path的子项
		m_mutex.unlock();
		QFileInfoList child_file_list = allfile(childroot, namepath);          //进行递归
		//file_list.append(child_file_list);
		//file_list.append(name);
	}
	//return file_list;
	return QFileInfoList();
}

void Worker::doWork(const QString &parameter)
{
	QString result;
	m_fileInfoList.clear();
	/* ... here is the expensive or blocking operation ... */
	traverseRecusionDir(m_dirPath, m_pattern);
	for (const auto& dir : m_fileInfoList)
	{
		m_mutex.lock();
		QTreeWidgetItem *root = new QTreeWidgetItem(m_treeWidget);
		root->setText(0, dir.fileName());
		root->setToolTip(0,dir.absoluteFilePath());
		m_mutex.unlock();
		//allfile(root, dir.absoluteFilePath());
	}
	
	emit resultReady(result);
}


