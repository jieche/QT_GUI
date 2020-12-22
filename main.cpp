#include "demo/appinit.h"
#include <QApplication>


#include "savelog.h"
#include "uidemo.h"

int main(int argc, char *argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5,6,0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication a(argc, argv);
    a.setFont(QFont("Microsoft Yahei", 9));
    a.setWindowIcon(QIcon(":/key32.ico"));

  
    AppInit::Instance()->start();
	SaveLog::Instance()->start();//启动日志钩子

	UIDemo w;
	w.exec();
    return a.exec();
}
