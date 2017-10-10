#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QDebug>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    QTranslator translator;
    translator.load(":/translator/qt_zh_CN.qm");

    a.installTranslator(&translator);
    MainWindow w;
    w.show();

    return a.exec();
}
