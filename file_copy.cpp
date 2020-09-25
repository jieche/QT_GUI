#include "file_copy.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

#include "MySql.h"
#include "savelog.h"

SFileCopy::SFileCopy(QObject *parent) : QObject(parent)
{
	m_createfile = new QDir();
}

SFileCopy::~SFileCopy()
{
	if (m_createfile) {
		m_createfile = Q_NULLPTR;
		delete m_createfile;
	}
}

bool SFileCopy::copyFileToPath(QString sourceDir, QString toDir, bool coverFileIfExist)
{
	toDir.replace("\\", "/");
	if (sourceDir == toDir) {
		return true;
	}
	if (!QFile::exists(sourceDir)) {
		return false;
	}
	bool exist = m_createfile->exists(toDir);
	if (exist) {
		if (coverFileIfExist) {
			m_createfile->remove(toDir);
		}
	}


	if (!QFile::copy(sourceDir, toDir)) {
		return false;
	}
	return true;
}

bool SFileCopy::copyDirectoryFiles(const QString &fromDir, const QString &toDir, bool coverFileIfExist)
{
	QDir sourceDir(fromDir);
	QDir targetDir(toDir);
	QString log = "拷贝目录:" + fromDir + "->" + toDir + "\n";
	qInfo() << log;
	emit sigLog(log);
	if (!targetDir.exists()) {    /**< 如果目标目录不存在，则进行创建 */
		if (!targetDir.mkdir(targetDir.absolutePath())) {
			return false;
		}
	}
	QFileInfoList fileInfoList = sourceDir.entryInfoList();


	if (m_firstRead) {
		int isfileTMP = 0;
		qDebug() << "a copyDirectoryFiles:" << fileInfoList.count();
		foreach(QFileInfo fileInfo, fileInfoList) {
			if (fileInfo.isFile()) {
				isfileTMP++;
			}
		}
		m_total = fileInfoList.count() - 2 - isfileTMP; // 2为.和..
		m_value = 0;
		m_firstRead = false;
		qDebug() << "a copyDirectoryFiles:" << fileInfoList.count() << m_total << isfileTMP;
		emit sigCopyDirStation(m_value / m_total);
		if (m_value == m_total) {
			m_firstRead = true;
			emit sigCopyDirStation(1);
			emit sigCopyDirOver();
		}
	}
	else {
		m_value++;
		qDebug() << "a copyDirectoryFiles:" << m_value << m_total;
		emit sigCopyDirStation(m_value / m_total);
		if ((m_value / m_total == 1) || (m_value > m_total) || (m_value == m_total)) {
			m_firstRead = true;
			emit sigCopyDirOver();
		}
	}
	foreach(QFileInfo fileInfo, fileInfoList) {
		if (fileInfo.fileName() == "." || fileInfo.fileName() == "..") {
			continue;
		}
		if (fileInfo.isDir())
		{    /**< 当为目录时，递归的进行copy */
			if (!copyDirectoryFiles(fileInfo.filePath(), targetDir.filePath(fileInfo.fileName()), coverFileIfExist)) {
				return false;
			}
		}
		else {            /**< 当允许覆盖操作时，将旧文件进行删除操作 */
			if (coverFileIfExist && targetDir.exists(fileInfo.fileName())) {
				targetDir.remove(fileInfo.fileName());
			}
			/// 进行文件copy
			QString log = "拷贝文件:" + fileInfo.absoluteFilePath() + "->" + targetDir.absoluteFilePath(fileInfo.fileName()) + "\n";
			qInfo() << log;
			emit sigLog(log);
			//m_textEdit->appendPlainText(log);
			if (!QFile::copy(fileInfo.filePath(), targetDir.filePath(fileInfo.fileName()))) {
				return false;
			}
		}
	}
	return true;
}

void SFileCopy::doWork()
{
	//创建 时间戳文件夹
	QDateTime datetime = QDateTime::currentDateTime();
	QString timestr = datetime.toString("yyyyMMddHHmmss");  // 文件名称为“年月日时分秒”
	QDir targetDir(m_desPath + "/" + timestr);
	if (!targetDir.exists()) {    /**< 如果目标目录不存在，则进行创建 */
		if (!targetDir.mkdir(targetDir.absolutePath())) {
			return;
		}
	}
	MySql  db;
	for (const auto& dir : m_srcFileList)
	{
		QString product_type = "0";
		QString storage_path = dir.absoluteFilePath();
		QString storage_time = datetime.currentDateTime().toString("yyyyMMddHHmmss");
		QString burn_start_time = datetime.currentDateTime().toString("yyyyMMddHHmmss");
		QString Stor_state = "2";
		copyDirectoryFiles(dir.absoluteFilePath(), m_desPath + "/" + timestr + "/" + dir.fileName());
		QString burn_end_time = datetime.currentDateTime().toString("yyyyMMddHHmmss");
		Stor_state = "1";
		QString remark = dir.fileName();
		QString sql = QString("insert into zc_stor_info(product_type,storage_path,storage_time,burn_start_time,burn_end_time,Stor_state,remark) values(%1,'%2',%3,'%4','%5',%6,'%7')")
			.arg(product_type)//1
			.arg(storage_path)//2
			.arg(storage_time)//3
			.arg(burn_start_time)//4
			.arg(burn_end_time)//5
			.arg(Stor_state)//6
			.arg(remark);//7
		db.insert(sql);
	}
}
