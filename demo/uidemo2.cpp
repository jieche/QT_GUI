#include "uidemo2.h"

#include "ui_uidemo2.h"
#include "quiwidget.h"
#include "Controller.h"
#include "QRegularExpression"

UIDemo2::UIDemo2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UIDemo2)
{
    ui->setupUi(this);
    this->initForm();
	m_thread.start();
    QUIWidget::setFormInCenter(this);
	connect(&m_thread, &QThread::finished, m_Worker, &QObject::deleteLater);
	//connect(m_Worker, &Worker::resultReady, this, &Controller::handleResults);

}

UIDemo2::~UIDemo2()
{
	m_thread.quit();
	m_thread.wait();
    delete ui;
}

void UIDemo2::getDrivers()
{
	foreach(const QStorageInfo &storage, QStorageInfo::mountedVolumes()) {
		if (storage.isValid() && storage.isReady()) {
			if (!storage.isReadOnly()) 
			{
				QString path = storage.rootPath();
				auto btn = new QPushButton(ui->widgetLeft);
				btn->setText(storage.displayName() + "(" + path + ")");
				btn->setToolTip(storage.displayName() + "(" + path + ")");
				ui->layout_left->addWidget(btn, 0);
			}
		}
	}
	
	auto verticalSpacer_2 = new QSpacerItem(20, 456, QSizePolicy::Minimum, QSizePolicy::Expanding);
	ui->layout_left->addItem(verticalSpacer_2);
	
}


void UIDemo2::initForm()
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
    ui->widgetLeft->setProperty("nav", "left");
    ui->widgetTop->setProperty("nav", "top");
    ui->labTitle->setText("数据迁移平台");
    ui->labTitle->setFont(QFont("Microsoft Yahei", 20));
    this->setWindowTitle(ui->labTitle->text());

    ui->label->setStyleSheet("QLabel{font:30pt;}");
	getDrivers();
    QSize icoSize(32, 32);
    int icoWidth = 85;

    //设置顶部导航按钮
    QList<QToolButton *> tbtns = ui->widgetTop->findChildren<QToolButton *>();
    foreach (QToolButton *btn, tbtns) {
        btn->setIconSize(icoSize);
        btn->setMinimumWidth(icoWidth);
        btn->setCheckable(true);
        connect(btn, SIGNAL(clicked()), this, SLOT(buttonClick()));
    }

    //ui->btnMain->click();

    //设置左侧导航按钮
    QList<QPushButton *> btns = ui->widgetLeft->findChildren<QPushButton *>();
    foreach (QPushButton *btn, btns) {
        btn->setCheckable(true);
        connect(btn, SIGNAL(clicked()), this, SLOT(btnClick()));
    }

    ui->btn1->click();
}


bool UIDemo2::isMatch(const QString str, const QString& pattern)
{
	const QRegularExpression regularExpression(pattern);
	const QRegularExpressionMatch match = regularExpression.match(str);
	if (match.hasMatch()) 
	{
		return true;
	}
	return false;
}

void UIDemo2::traverseRecusionDir(QString dirPath,QString pattern)
{
	QDir dir(dirPath);
	QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);   //获取当前所有目录

	for (int i = 0; i != folder_list.size(); ++i)         //自动递归添加各目录到上一级目录
	{

		QString namepath = folder_list.at(i).absoluteFilePath();    //获取路径
		QFileInfo folderinfo = folder_list.at(i);
		QString name = folderinfo.fileName();      //获取目录名
		if(isMatch(name,pattern))
		{
			m_fileInfoList.push_back(folder_list.at(i));
		}
		else
		{
			traverseRecusionDir(namepath,pattern);  //进行递归
		}
	}
}

QFileInfoList UIDemo2::allfile(QTreeWidgetItem *root, QString path)         //参数为主函数中添加的item和路径名
{

	/*添加path路径文件*/
	QDir dir(path);          //遍历各级子目录
	QDir dir_file(path);    //遍历子目录中所有文件
	dir_file.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);        //获取当前所有文件
	dir_file.setSorting(QDir::Size | QDir::Reversed);
	QFileInfoList list_file = dir_file.entryInfoList();
	for (int i = 0; i < list_file.size(); ++i) {       //将当前目录中所有文件添加到treewidget中
		QFileInfo fileInfo = list_file.at(i);
		QString name2 = fileInfo.fileName();
		QTreeWidgetItem* child = new QTreeWidgetItem(QStringList() << name2);
		child->setIcon(0, QIcon(":/file/image/link.ico"));
		child->setCheckState(1, Qt::Checked);
		root->addChild(child);
	}


	QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);   //获取当前所有目录

	for (int i = 0; i != folder_list.size(); i++)         //自动递归添加各目录到上一级目录
	{

		QString namepath = folder_list.at(i).absoluteFilePath();    //获取路径
		QFileInfo folderinfo = folder_list.at(i);
		QString name = folderinfo.fileName();      //获取目录名
		QTreeWidgetItem* childroot = new QTreeWidgetItem(QStringList() << name);
		childroot->setIcon(0, QIcon(":/file/image/link.ico"));
		childroot->setCheckState(1, Qt::Checked);
		root->addChild(childroot);              //将当前目录添加成path的子项
		QFileInfoList child_file_list = allfile(childroot, namepath);          //进行递归
		file_list.append(child_file_list);
		file_list.append(name);
	}
	return file_list;
}

void UIDemo2::buttonClick()
{
    QToolButton *b = (QToolButton *)sender();
    QString name = b->text();

    QList<QToolButton *> btns = ui->widgetTop->findChildren<QToolButton *>();
    foreach (QToolButton *btn, btns) {
        if (btn == b) {
            btn->setChecked(true);
        } else {
            btn->setChecked(false);
        }
    }
    ui->label->setText(QString("你单击了顶部导航菜单\n%1").arg(name));
}

void UIDemo2::btnClick()
{
    QPushButton *b = (QPushButton *)sender();
    QString name = b->text();
	//name += QString("晶璃/");

    QList<QPushButton *> btns = ui->widgetLeft->findChildren<QPushButton *>();
    foreach (QPushButton *btn, btns) {
        if (btn == b) {
            btn->setChecked(true);
        } else {
            btn->setChecked(false);
        }
    }
	ui->treeWidget->clear();
	m_Worker->setPath("D:/");
	m_Worker->setPattern("李光耀");
	m_Worker->setTree(ui->treeWidget);
	m_Worker->moveToThread(&m_thread);
	connect(this, &UIDemo2::process, m_Worker, &Worker::doWork);
	emit process("");
    ui->label->setText(QString("你单击了左侧导航菜单\n%1").arg(name));
}

void UIDemo2::on_btnMenu_Min_clicked()
{
    showMinimized();
}

void UIDemo2::on_btnMenu_Max_clicked()
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

void UIDemo2::on_btnMenu_Close_clicked()
{
    close();
}
