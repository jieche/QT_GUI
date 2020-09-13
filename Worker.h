#pragma once
#include "QThread"
#include "QTreeWidget"
#include "QMutex"


class Worker : public QObject/*QObject*/
{
	Q_OBJECT
public:
	explicit Worker(QObject *parent = 0);

	void setPath(QString path) { m_dirPath = path; }
	void setPattern(QString pat) { m_pattern = pat; }
	void setTree(QTreeWidget* tree) { m_treeWidget = tree; }
	
private:
	static bool isMatch(const QString str, const QString& pattern);
	void traverseRecusionDir(QString dirPath, QString pattern);
	QFileInfoList allfile(QTreeWidgetItem *root, QString path); 
signals:
	void currentNum(int);
	void workDone();
	void resultReady(const QString &result);
	void searchFinish();
public slots:
	//void stopThread();
	void doWork();

protected:

private:
	volatile bool bStop = false;//易失性变量,用volatile进行申明
public:
	QFileInfoList getFileList() const { return m_fileInfoList; }
	/**
	* \brief 产品文件夹信息列表
	*/
	QFileInfoList    m_fileInfoList;
	QString m_dirPath;
	QString m_pattern;
	QTreeWidget* m_treeWidget =nullptr;
	QMutex m_mutex;

};

