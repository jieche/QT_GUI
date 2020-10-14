#include "file_copy.h"
#include "MySql.h"
#include "savelog.h"
#include "Postgres.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include "QSqlQuery"


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
	qDebug() << log;
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
		qDebug() << "a copyDirectoryFiles:" << m_total << isfileTMP;
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
			qDebug() << log;
			emit sigLog(log);
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
		if (targetDir.mkdir(targetDir.absolutePath())) {
			qDebug() << "创建目标时间文件夹";
			emit sigLog("创建目标时间文件夹");
		}
		else
		{
			return ;
		}
	}
	
	
	Postgres db_post;
	MySql  db;

	for (const auto& dir : m_srcFileList)
	{
		int product_type = 0;
		QString storage_path = m_desPath + "/" + timestr + "/" + dir.fileName();

		 //查找对应表
		QString dataType = dir.fileName().split("_").at(0);//截取类型
		QString sql_tab = QString("select attribute_table from data_model where model_name ='%1'").arg(dataType);
		qDebug() << sql_tab;
		QString tabName = db_post.selectOne(sql_tab);
		QString sql_id = QString("select productid from %1 where productname ='%2'").arg(tabName).arg(dir.fileName());
		qDebug() << sql_id;
		QString product_id_str = db_post.selectOne(sql_id);
		int product_id = -1;
		if(!product_id_str.isEmpty())
		{
			product_id = product_id_str.toInt();
		}

		QString storage_time = datetime.currentDateTime().toString("yyyyMMddHHmmss");
		QString burn_start_time = datetime.currentDateTime().toString("yyyyMMddHHmmss");
		QString Stor_state = "2";
		copyDirectoryFiles(dir.absoluteFilePath(), m_desPath + "/" + timestr + "/" + dir.fileName());
		QString burn_end_time = datetime.currentDateTime().toString("yyyyMMddHHmmss");
		Stor_state = "1";
		QString remark = dir.fileName();
		QString sql = QString("insert into zc_stor_info(product_id,product_type,storage_path,storage_time,burn_start_time,burn_end_time,Stor_state,remark) values('%1','%2','%3','%4','%5',%6,'%7','%8')")
			.arg(product_id)//1
			.arg(product_type)//2
			.arg(storage_path)//3
			.arg(storage_time)//4
			.arg(burn_start_time)//5
			.arg(burn_end_time)//6
			.arg(Stor_state)//7
			.arg(remark);//8
		qDebug() << sql;
		QVariant stor_info_id =  db.insert(sql);
		{
			QString log_type ="1";//1：存储日志；2：回迁日志
			QString log_code ="0";//日志编码
			QString log_title = "";//日志标题
			QString log_create_time = datetime.currentDateTime().toString("yyyyMMddHHmmss");;
			QString log_remark;
			QString log_context ="";

			QString sql_log = QString("insert into zc_log_info(  stor_info_id, log_type, log_code, log_title,log_create_time, remark,log_context ) \
															values(      '%1',        '%2',     '%3'  , '%4',     '%5'  ,         '%6','%7')")
				.arg(stor_info_id.toString())//1
				.arg(log_type)//2
				.arg(log_code)//3
				.arg(log_title)//4
				.arg(log_create_time)//5
				.arg(log_remark)//6
				.arg(log_context);//7
			qDebug() << sql_log;
			db.insert(sql_log);
		}
	}
}

