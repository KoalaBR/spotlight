#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QTranslator>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString locale = QLocale::system().name();

    QTranslator translator;
    if (locale.startsWith("de_"))
    {
        if (!translator.load("spotlight_de.qm"))
            QMessageBox::warning(NULL, "Warnung", "spotlight_de.qm fehlt!");
    }
    else
    {
        if (!translator.load("spotlight_en.qm"))
            QMessageBox::warning(NULL, "Warning", "Missing spotlight_en.qm");
    }
    qApp->installTranslator(&translator);
    MainWindow w;
    w.show();

    return a.exec();
}
