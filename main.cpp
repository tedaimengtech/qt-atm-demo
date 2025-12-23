#include "mainwindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "dataoperator.h"
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "atm-bank_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    //检查数据库
    auto instance = dataOperator::getInstance();
    if(!(instance.initDatabase() && instance.initTables())) {
        QMessageBox::warning(nullptr,"数据库读取失败","请检查数据库文件database.db",QMessageBox::Ok);
        return 0;
    }
    MainWindow w;
    w.show();
    return a.exec();
}
