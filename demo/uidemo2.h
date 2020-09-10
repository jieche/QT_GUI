#ifndef UIDEMO2_H
#define UIDEMO2_H

#include <QDialog>

#include "Controller.h"
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
	Controller		*controller = nullptr;
	Worker        *m_Worker =new Worker;
	QThread       m_thread;

private slots:
    void initForm();
	void buttonClick();
    void btnClick();

private slots:
    void on_btnMenu_Min_clicked();
    void on_btnMenu_Max_clicked();
    void on_btnMenu_Close_clicked();
signals:
	void process(const QString &);
};

#endif // UIDEMO2_H
