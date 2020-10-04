#ifndef UIDemo_H
#define UIDemo_H

#include <QDialog>
#include <QStringList>


#include "file_copy.h"
#include "QFileInfo"
#include "Worker.h"

namespace Ui {
class UIDemo;
}

class UIDemo : public QDialog
{
    Q_OBJECT

public:
    explicit UIDemo(QWidget *parent = 0);

	void setStyle(const QString &str);

	void on_btnNew_clicked();
	~UIDemo();
	//获取盘符
	void getDrivers();
	QFileInfoList allfile(QTreeWidgetItem *root, QString path); //参数为主函数中添加的item和路径名;
private:
    Ui::UIDemo *ui;
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
	void refreshSlot();
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
	QStringList  m_patternList;
};

#endif // UIDemo_H
