#ifndef UIDEMO2_H
#define UIDEMO2_H

#include <QDialog>

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
	QFileInfoList allfile(QTreeWidgetItem *root, QString path); //参数为主函数中添加的item和路径名;
private:
    Ui::UIDemo2 *ui;
    bool max;
    QRect location;

private slots:
    void initForm();
	void buttonClick();
    void btnClick();

		private slots:
    void on_btnMenu_Min_clicked();
    void on_btnMenu_Max_clicked();
    void on_btnMenu_Close_clicked();
};

#endif // UIDEMO2_H
