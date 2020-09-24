#include "uidemo2.h"

#include <codecvt>

#include "ui_uidemo2.h"
#include "quiwidget.h"
#include "mysql.h"

#include "QRegularExpression"
#include "savelog.h"
#include "QtXml\qdom.h"

UIDemo2::UIDemo2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UIDemo2)
{
    ui->setupUi(this);
	readXML();
	SaveLog::Instance()->start();//启动日志钩子
    this->initForm();
	m_thread.start();
	m_Worker->moveToThread(&m_thread);

	m_cp_thread.start();
	m_FileCP->moveToThread(&m_cp_thread);

	
    QUIWidget::setFormInCenter(this);
	connect(&m_thread, &QThread::finished, m_Worker, &QObject::deleteLater);
	connect(this, &UIDemo2::process, m_Worker, &Worker::doWork);
	connect(m_Worker,&Worker::searchFinish,this,[=](){
		m_isSearching = false;
		ui->btn_search->setChecked(false);
		m_DirList = m_Worker->getFileList();
	});

	connect(&m_cp_thread, &QThread::finished, m_FileCP, &QObject::deleteLater);
	connect(this, &UIDemo2::processCP, m_FileCP, &SFileCopy::doWork);
	connect(m_FileCP, &SFileCopy::sigCopyDirOver, this, [=]() {
		m_isCopying = false;
		ui->btnCopy->setChecked(false);
	});
	connect(m_FileCP, &SFileCopy::sigCopyDirStation, this, [=](float value) {
		ui->progressBar->setValue(value*100);
	});
	/*connect(m_FileCP, &SFileCopy::sigLog, this, [=](QString log) {
		ui->plainTextEdit->appendPlainText(log);
	}, Qt::QueuedConnection);*/
	connect(m_FileCP, &SFileCopy::sigLog, this, &UIDemo2::logSlot, Qt::QueuedConnection);
}


void UIDemo2::setStyle(const QString &str)
{
	static QString qss;

	if (qss == str) {
		return;
	}

	qss = str;
	QString paletteColor = str.mid(20, 7);
	qApp->setPalette(QPalette(QColor(paletteColor)));
	qApp->setStyleSheet(str);
	/*ui->widgetPanel->setStyleSheet(QString("QFrame#boxPanel{border-width:0px;background:%1;}"
		"QFrame#gboxDevicePanel,QFrame#gboxDeviceTitle{padding:3px;}")
		.arg(paletteColor));

#ifndef demo
	ui->txtMain->setText(str);
#endif*/
}

void UIDemo2::logSlot(QString log)
{
	ui->plainTextEdit->appendPlainText(log);
}



UIDemo2::~UIDemo2()
{
	SaveLog::Instance()->stop();
	
	m_thread.quit();
	m_thread.wait();

	m_cp_thread.quit();
	m_cp_thread.wait();
    delete ui;
}

void UIDemo2::getDrivers()
{
	//清空horizontalLayout布局内的所有元素
	QLayoutItem *child;
	while ((child = ui->layout_left->takeAt(0)) != 0)
	{
		//setParent为NULL，防止删除之后界面不消失
		if (child->widget())
		{
			child->widget()->setParent(NULL);
		}
		delete child;
	}
	
	foreach(const QStorageInfo &storage, QStorageInfo::mountedVolumes())
	{
		//磁盘名称 -》替换盘符
		QString diskName = m_desPath.split(":").at(0).trimmed();
		if(diskName == storage.displayName())
		{
			m_desDiskFlag = storage.rootPath().split(":").at(0);
			m_desDiskName = storage.displayName();
			QString  path = ui->label_des->text().split(":").at(1);
			
			ui->label_des->setText(QString("目的地址：%1").arg(storage.displayName() + "(" + m_desDiskFlag +":"+path + ")"));
		    
			if(m_desPath.left(m_desDiskName.length()) == m_desDiskName)
			{
				m_desPath.replace(0, m_desDiskName.length(), m_desDiskFlag);
			}
			
		}
		
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

    //ui->label->setStyleSheet("QLabel{font:30pt;}");
	getDrivers();
    QSize icoSize(32, 32);
    int icoWidth = 85;

    //设置顶部导航按钮
    QList<QToolButton *> tbtns = ui->widgetTop->findChildren<QToolButton *>();
    foreach (QToolButton *btn, tbtns) {
        btn->setIconSize(icoSize);
        btn->setMinimumWidth(icoWidth);
        btn->setCheckable(true);
    }
     //connect(ui->btn_search, SIGNAL(clicked()), this, SLOT(buttonClick()));

    //ui->btnMain->click();

    //设置左侧导航按钮
    QList<QPushButton *> btns = ui->widgetLeft->findChildren<QPushButton *>();
    foreach (QPushButton *btn, btns) {
        btn->setCheckable(true);
        connect(btn, SIGNAL(clicked()), this, SLOT(btnClick()));
    }

    //ui->btn1->click();
	on_btnNew_clicked();
}

void UIDemo2::on_btnNew_clicked()
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

bool UIDemo2::isMatch(const QString str, const QString& pattern)
{
	//身份证 ^[1-9]\d{5}(18|19|20)\d{2}((0[1-9])|(1[0-2]))(([0-2][1-9])|10|20|30|31)\d{3}[0-9Xx]$
	//邮箱  ^([A-Za-z0-9_\-\.])+\@([A-Za-z0-9_\-\.])+\.([A-Za-z]{2,4})$
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

void UIDemo2::btnClick()
{
    QPushButton *b = (QPushButton *)sender();
    QString path = b->text();
	path = path.split("(").at(1);
	path = path.split(")").at(0);
	m_srcPath = path;
    QList<QPushButton *> btns = ui->widgetLeft->findChildren<QPushButton *>();
    foreach (QPushButton *btn, btns) {
        if (btn == b) {
            btn->setChecked(true);
        } else {
            btn->setChecked(false);
        }
    }

    ui->label_src->setText(QString("源地址：%1").arg(b->text()));
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
	exit(0);
}

void UIDemo2::readXML()
{
	//打开或创建文件
	QFile file("config.xml"); //相对路径、绝对路径、资源路径都行
	if (!file.open(QFile::ReadOnly))
		return;

	QDomDocument doc;
	if (!doc.setContent(&file))
	{
		file.close();
		return;
	}
	file.close();

	QDomElement root = doc.documentElement(); //返回根节点
	QDomNode node = root.firstChild(); //获得第一个子节点
	while (!node.isNull())  //如果节点不空
	{
		if (node.isElement()) //如果节点是元素
		{
			QDomElement e = node.toElement(); //转换为元素，注意元素和节点是两个数据结构，其实差不多
			if (e.tagName() == "info")
			{
				auto path = e.attribute("path");
				ui->label_des->setText(QString("目的地址：%1").arg(path));
				m_desPath = path.trimmed();
				auto pattern = e.attribute("pattern");
				ui->lineEdit->setText(pattern);
			}
		}
		node = node.nextSibling(); //下一个兄弟节点,nextSiblingElement()是下一个兄弟元素，都差不多
	}
	file.close();
}

void UIDemo2::searchSlot()
{
	QToolButton *b = (QToolButton *)sender();
	QString name = b->text();

	/*QList<QToolButton *> btns = ui->widgetTop->findChildren<QToolButton *>();
	foreach(QToolButton *btn, btns) {
		if (btn == b) {
			btn->setChecked(true);
		}
		else {
			btn->setChecked(false);
		}
	}*/
	ui->treeWidget->clear();
	m_Worker->setPath(m_srcPath);
	m_Worker->setPattern(ui->lineEdit->text());
	m_Worker->setTree(ui->treeWidget);
	
	if(m_isSearching ==false)
	{
		emit process();
		m_isSearching = true;
	}
}

void UIDemo2::copySlot()
{
	QToolButton *b = (QToolButton *)sender();

	m_FileCP->setSrcPath(m_DirList);
	m_FileCP->setDesPath(m_desPath);
	if (m_isCopying == false)
	{
		emit processCP();
		m_isCopying = true;
	}
}

void UIDemo2::refreshSlot()
{
	getDrivers();
	QToolButton *b = (QToolButton *)sender();
	b->setChecked(false);
}

