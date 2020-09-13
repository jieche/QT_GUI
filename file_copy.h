#pragma once
#ifndef SFILECOPY_H
#define SFILECOPY_H

#include <QObject>
#include <QDir>

class SFileCopy : public QObject
{
	Q_OBJECT
public:
	explicit SFileCopy(QObject *parent = 0);
	~SFileCopy();

	//拷贝文件：
	bool copyFileToPath(QString sourceDir, QString toDir, bool coverFileIfExist);

	//拷贝文件夹：
	bool copyDirectoryFiles(const QString &fromDir, const QString &toDir, bool coverFileIfExist =true);
	void setSrcPath(QString path) { m_srcPath = path; };
	void setDesPath(QString path) { m_desPath = path; };
	
signals:
	void sigCopyDirStation(float num);
	void sigCopyDirOver();
public slots :
	void doWork();
private:
	QDir * m_createfile = Q_NULLPTR;
	float m_total = 0;
	float m_value = 0;
	bool m_firstRead = true;
	QString m_srcPath;
	QString m_desPath;
};

#endif // SFILECOPY_H
