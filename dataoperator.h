#pragma once
#pragma comment(lib, "Qt6Sql.lib")
#include <qlineedit.h>
#include <QString>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQueryModel>
#include <QtSql/QSqlQuery>

class dataOperator
{
    QSqlDatabase db;
public:
    struct userInfo {
        QString account = nullptr;
        QString name = nullptr;
        QString id = nullptr;
        unsigned long long balance = 0;
        unsigned int errorCount = 0;
        QString errors = nullptr;
    };
    explicit dataOperator();
    bool initDatabase();
    userInfo userLogin(const QString &account, const QString &password);

    userInfo getUserName(const QString &account);

    QString addBalance(const QString &account, unsigned long long amount);

    QString dropBalance(const QString &account, unsigned long long amount, QString &password);
    static dataOperator& getInstance();
    ~dataOperator();
    bool initTables();
    bool userExistCheck(const QString &account);

    bool userLockCheck(const QString &account);

    QString createUser(const userInfo &user, const QString &password);
    QString setUserPassword(userInfo user, const QString &newPassword);
    QString validateUserPassword(const QString &account, const QString &password);
    QString updateUserInfo(const userInfo &user);
    QString updateBalance(const QString &account, long long amount);
private:
    enum class queryMode{createUser,updateUserInfo,setUserPassword,userExistCheck,getUserInfo,updateBalance};
    QSqlQuery getNewQuery(const userInfo &user, queryMode mode, const QString &password = nullptr, const long long &balance = 0);
    userInfo queryUserInfo(const QString &account);

};
