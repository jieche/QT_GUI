#include "UIDemo.h"

#include <codecvt>

#include "ui_UIDemo.h"
#include "quiwidget.h"
#include "savelog.h"
#include "QtXml\qdom.h"
#include "QCryptographicHash"

const QString OneMonthGuid = " C439BBE874AE4A80";
const QString ThreeMonthGuid = "C439BBE874AE4A81";
const QString OneYearGuid = "C439BBE874AE4A82";
const QString PermenetGuid = "C439BBE874AE4A83";

UIDemo::UIDemo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UIDemo)
{
    ui->setupUi(this);
    this->initForm();
	m_thread.start();
	m_Worker->moveToThread(&m_thread);

	m_cp_thread.start();
	m_FileCP->moveToThread(&m_cp_thread);

	qRegisterMetaType<QVector<int> >("QVector<int>");
	
    QUIWidget::setFormInCenter(this);


}

UIDemo::~UIDemo()
{
	delete ui;
}

void UIDemo::setStyle(const QString &str)
{
	static QString qss;

	if (qss == str) {
		return;
	}

	qss = str;
	QString paletteColor = str.mid(20, 7);
	qApp->setPalette(QPalette(QColor(paletteColor)));
	qApp->setStyleSheet(str);
	
}

void UIDemo::getDrivers()
{
	
}

void UIDemo::initForm()
{
    this->max = false;
    this->location = this->geometry();
    this->setProperty("form", true);
    this->setProperty("canMove", true);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
	
    IconHelper::Instance()->setIcon(ui->labIco, QChar(0xF099), 30);
    IconHelper::Instance()->setIcon(ui->btnMenu_Min, QChar(0xF068));
    IconHelper::Instance()->setIcon(ui->btnMenu_Max, QChar(0xF067));
    IconHelper::Instance()->setIcon(ui->btnMenu_Close, QChar(0xF00d));

    ui->widgetTitle->setProperty("form", "title");
    ui->widgetTop->setProperty("nav", "top");
    ui->labTitle->setText(m_title);
    ui->labTitle->setFont(QFont("Microsoft Yahei", 20));
    this->setWindowTitle(ui->labTitle->text());

	getDrivers();
    QSize icoSize(32, 32);
    int icoWidth = 85;

    //设置顶部导航按钮
    QList<QToolButton *> tbtns = ui->widgetTop->findChildren<QToolButton *>();
    foreach (QToolButton *btn, tbtns)
	{
        btn->setIconSize(icoSize);
        btn->setMinimumWidth(icoWidth);
        btn->setCheckable(true);
    }
	on_btnNew_clicked();
}

void UIDemo::on_btnNew_clicked()
{
	QString fileName = ":/qss/psblack.css";

	if (!fileName.isEmpty()) {
		QFile file(fileName);

		if (file.open(QFile::ReadOnly)) {
			QString str = file.readAll();
			setStyle(str);
		}
	}
}

void UIDemo::on_btnMenu_Min_clicked()
{
    showMinimized();
}

void UIDemo::on_btnMenu_Max_clicked()
{
    if (max) {
        this->setGeometry(location);
        this->setProperty("canMove", true);
    } else {
        location = this->geometry();
        this->setGeometry(qApp->desktop()->availableGeometry());
        this->setProperty("canMove", false);
    }

    max = !max;
}

void UIDemo::on_btnMenu_Close_clicked()
{
    close();
	exit(0);
}

void UIDemo::on_btn_generate_clicked()
{
	QString cpuNo = getWMIC("wmic cpu get processorid");
	ui->lineEdit_cpu->setText(cpuNo);

	QString guid = "C439BBE874AE4A83";
	ui->lineEdit_key->setText(md5(cpuNo + guid).left(12));
	
}

QString UIDemo::md5(QString key)
{
	QCryptographicHash md5(QCryptographicHash::Md5);
	md5.addData(key.toUtf8());
	return  QString(md5.result().toHex().toUpper());
}

QString UIDemo::getWMIC(const QString &cmd)
{
	//获取cpu名称：wmic cpu get Name
	//获取cpu核心数：wmic cpu get NumberOfCores
	//获取cpu线程数：wmic cpu get NumberOfLogicalProcessors
	//查询cpu序列号：wmic cpu get processorid
	//查询主板序列号：wmic baseboard get serialnumber

	//查询BIOS序列号：wmic bios get serialnumber
	//查看硬盘：wmic diskdrive get serialnumber

	QProcess p;
	p.start(cmd);
	p.waitForFinished();
	QString result = QString::fromLocal8Bit(p.readAllStandardOutput());
	QStringList list = cmd.split(" ");
	result = result.remove(list.last(), Qt::CaseInsensitive);
	result = result.replace("\r", "");
	result = result.replace("\n", "");
	result = result.simplified();
	return result;

}
