#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString locale = QLocale::system().name();

    QTranslator translator;
    if (locale.startsWith("de_"))
        translator.load("spotlight_de.qm");
    else translator.load("spotlight_en.qm");
    qApp->installTranslator(&translator);
    MainWindow w;
    w.show();

    return a.exec();
}
