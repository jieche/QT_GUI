#ifndef UIDEMO2_H
#define UIDEMO2_H

#include <QDialog>
#include <QStringList>


#include "file_copy.h"
#include "QFileInfo"
#include "Worker.h"

namespace Ui {
class UIDemo2;
}

class UIDemo2 : public QDialog
{
    Q_OBJECT

public:
    explicit UIDemo2(QWidget *parent = 0);

	void setStyle(const QString &str);

	void on_btnNew_clicked();
	~UIDemo2();
	//获取盘符
	void getDrivers();
	void traverseDir(QString dirPath) const;
    static bool isMatch(QString str, const QString& pattern);
	void traverseRecusionDir(QString dirPath, QString pattern);
	QFileInfoList allfile(QTreeWidgetItem *root, QString path); //参数为主函数中添加的item和路径名;
private:
    Ui::UIDemo2 *ui;
    bool max;
    QRect location;
    /**
	 * \brief 产品文件夹信息列表
	 */
	QFileInfoList    m_fileInfoList;
	Worker        *m_Worker =new Worker;
	QThread       m_thread;
	SFileCopy     *m_FileCP = new SFileCopy;
	QThread		  m_cp_thread;

private slots:
    void initForm();
    void btnClick();

private slots:
    void on_btnMenu_Min_clicked();
    void on_btnMenu_Max_clicked();
    void on_btnMenu_Close_clicked();
	void readXML();
	void searchSlot();
	void copySlot();
	void logSlot(QString log);
signals:
	void process();
	void processCP();
private:
	QString m_srcPath;
	QString m_desPath;
	QString m_desDiskFlag;
	QString m_desDiskName;
	bool m_isSearching = false;
	bool m_isCopying = false;
	QFileInfoList  m_DirList;
};

#endif // UIDEMO2_H
