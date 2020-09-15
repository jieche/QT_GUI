#include "demo/appinit.h"
#include <QApplication>

#include "uidemo2.h"

int main(int argc, char *argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5,6,0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication a(argc, argv);
    a.setFont(QFont("Microsoft Yahei", 9));
    a.setWindowIcon(QIcon(":/main.ico"));

  
    AppInit::Instance()->start();


	UIDemo2 w;
	w.exec();
    return a.exec();
}
