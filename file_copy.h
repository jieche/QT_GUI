#pragma once

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
	bool copyDirectoryFiles(const QString &fromDir, const QString &toDir, bool coverFileIfExist = true);
	quint64 dirFileSize(const QString &path);
	void setSrcPath(QFileInfoList path) { m_srcFileList = path; };
	void setDesPath(QString path) { m_desPath = path; };
	void setDesDiskName(QString name) { m_desDiskName = name; };
	void setDesLinuxPath(QString path) { m_desLinuxPath = path; };
	void setSrcDiskTag(QString tag) { m_srcTag = tag; };
	void setPrefixMap(QMap<QString, QString> m) { m_prefixMap = m; };
	void setContainMap(QMap<QString, QString> m) { m_containMap = m; };
private:
	QString find_model_name(QString dirName);
signals:
	void sigCopyDirStation(float num);
	void sigCopyDirOver();
	void sigLog(QString content);
public slots :
	void doWork();
private:
	QDir * m_createfile = Q_NULLPTR;
	float m_total = 0;
	float m_value = 0;
	bool m_firstRead = true;
	QFileInfoList m_srcFileList;
	QString m_desPath;
	QString m_desDiskName;
	QString m_desLinuxPath;
	QString m_srcTag;
	QMap<QString, QString> m_prefixMap;
	QMap<QString, QString> m_containMap;
};

