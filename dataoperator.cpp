#include "dataoperator.h"
#include "libbcrypt/include/bcrypt/BCrypt.hpp"
#include <QSqlError>
#include <QStandardPaths>
dataOperator::dataOperator() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    //找一个可以存储数据库的位置
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    db.setDatabaseName(path+"database.db");
    initDatabase();
}

bool dataOperator::initDatabase() {
    return db.open();
}
//生成各类sql查询参数
QSqlQuery dataOperator::getNewQuery(const userInfo& user, const queryMode mode, const QString& password,const long long &balance) {
    if (!db.isOpen()) initDatabase();
    auto query = QSqlQuery(db);
    switch (mode) {
        case queryMode::createUser:
            query.prepare("insert into users (account, name, id, password_crypted, balance, error_count) "
                  "values (:account, :name, :id, :password_crypted, :balance, :error_count)");
            query.bindValue(":account", user.account);
            query.bindValue(":name", user.name);
            query.bindValue(":id", user.id);
            query.bindValue(":password_crypted", QString::fromStdString(BCrypt::generateHash(password.toStdString())));
            query.bindValue(":balance", user.balance);
            query.bindValue(":error_count", user.errorCount);
            break;
        case queryMode::updateUserInfo:
            query.prepare("update users set name = :name, id = :id, balance = :balance, error_count = :error_count where account = :account;");
            query.bindValue(":account", user.account);
            query.bindValue(":balance", user.balance);
            query.bindValue(":name", user.name);
            query.bindValue(":id", user.id);
            query.bindValue(":error_count", user.errorCount);
            break;
        case queryMode::setUserPassword:
            query.prepare("update users set password_crypted = :password_crypted, error_count = :error_count where account = :account and name = :name and id = :id;");
            query.bindValue(":account", user.account);
            query.bindValue(":password_crypted", QString::fromStdString(BCrypt::generateHash(password.toStdString())));
            query.bindValue(":name", user.name);
            query.bindValue(":id", user.id);
            query.bindValue(":error_count", user.errorCount);
            break;
        case queryMode::userExistCheck:
            query.prepare("select * from users where account = :account;");
            query.bindValue(":account", user.account);
            break;
        case queryMode::getUserInfo:
            query.prepare("select * from users where account = :account;");
            query.bindValue(":account", user.account);
            break;
        case queryMode::updateBalance:
            query.prepare("update users set balance = balance + :balance where account = :account");
            query.bindValue(":account", user.account);
            query.bindValue(":balance", balance);
            break;
    }
    return query;
}
//此处用于生成一个唯一的Instance，在其他任何地方都可以调用
dataOperator& dataOperator::getInstance(){
    static dataOperator instance;
    return instance;
}
dataOperator::~dataOperator() {
    db.close();
}
//检查表信息是否存在，不存在就创建新表
bool dataOperator::initTables() {
    if (!db.isOpen()) initDatabase();
    auto query = QSqlQuery(db);
    query.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='users';");
    if (!query.next()) {
        // 表不存在，直接创建
        return query.exec(
            "CREATE TABLE IF NOT EXISTS users ("
            "    account CHAR(19) PRIMARY KEY CHECK("
            "        length(account) = 19 AND "
            "        account GLOB '[0-9]*'"
            "    ),"
            "    name VARCHAR(20) CHECK(length(name) <= 20),"
            "    id CHAR(18) CHECK(length(id) = 18),"
            "    password_crypted CHAR(60) NOT NULL CHECK(length(password_crypted) = 60),"
            "    balance DECIMAL(15,2) DEFAULT 0.00 CHECK(balance >= 0),"
            "    error_count INTEGER DEFAULT 0 CHECK(error_count >= 0 AND error_count <= 3)"
            ");"
        );
    }
    return true;
}
//判断该用户是否存在
bool dataOperator::userExistCheck(const QString &account) {
    userInfo user;
    user.account = account;
    auto query = getNewQuery(user, queryMode::userExistCheck);
    query.exec();
    return query.next();
}
//判断用户是否被锁定
bool dataOperator::userLockCheck(const QString &account) {
    auto user = queryUserInfo(account);
    if (user.errors != nullptr) return true; //查询出问题,视为锁定
    return user.errorCount >= 3;
}
//获取用户信息
dataOperator::userInfo dataOperator::queryUserInfo(const QString &account) {
    userInfo user;
    //如果用户不存在
    if (!userExistCheck(account)) {
        user.errors = "用户不存在";
        return user;
    }
    user.account = account;
    auto query = getNewQuery(user, queryMode::getUserInfo);
    //如果没成功
    if (!query.exec()) {
        user.errors = "数据库查询失败：" + query.lastError().text();
        return user;
    }
    //查询不到数据
    if (!query.next()) {
        user.errors = "数据库无法查询该用户记录：" + query.lastError().text();
        return user;
    }
    user = {
        .account = account,
        .name = query.value("name").toString(),
        .id = query.value("id").toString(),
        .balance = query.value("balance").toULongLong(),
        .errorCount = query.value("error_count").toUInt(),
        .errors = nullptr
    };
    return user;
}
//创建用户
QString dataOperator::createUser(const userInfo &user, const QString &password) {
    if (userExistCheck(user.account)) {
        //用户存在，返回错误
        return "用户已存在";
    }
    auto query = getNewQuery(user, queryMode::createUser, password);
    return query.exec() ? nullptr : "数据库存储失败：" + query.lastError().text();
}
//更新用户信息
QString dataOperator::updateUserInfo(const userInfo &user) {
    if (!userExistCheck(user.account)) {
        //用户不存在，返回错误
        return "用户已存在";
    }
    auto query = getNewQuery(user, queryMode::updateUserInfo);
    return query.exec() ? nullptr : "数据库存储失败：" + query.lastError().text();
}
//设置用户密码
QString dataOperator::setUserPassword(userInfo user, const QString &newPassword) {
    if (!userExistCheck(user.account)) return "用户不存在";
    user.errorCount = 0; //重置错误计数
    auto query = getNewQuery(user, queryMode::setUserPassword, newPassword);
    return query.exec() ? nullptr : "数据库存储失败，可能信息不正确？：" + query.lastError().text();
}
//验证用户密码
QString dataOperator::validateUserPassword(const QString &account,const QString &password) {
    if (!userExistCheck(account)) return "用户不存在";
    if (userLockCheck(account)) return "账户已锁定";
    userInfo user = queryUserInfo(account);
    if (user.errors != nullptr) return user.errors;
    auto query = getNewQuery(user, queryMode::getUserInfo);
    if (!query.exec()) return "数据库查询失败：" + query.lastError().text();
    if (!query.next()) return "数据库无法查询该用户记录：" + query.lastError().text();
    //验证密码
    if (BCrypt::validatePassword(password.toStdString(), query.value("password_crypted").toString().toStdString())) {
        //验证成功
        user.errorCount = 0;
        updateUserInfo(user);
        return nullptr;
    }
    //验证失败
    user.errorCount++;
    updateUserInfo(user);
    return "密码错误";
}
//获取用户登录的信息
dataOperator::userInfo dataOperator::userLogin(const QString &account, const QString &password) {
    userInfo user;
    //验证密码
    const QString passwordValidationError = validateUserPassword(account, password);
    if (passwordValidationError != nullptr) {
        user.errors = passwordValidationError;
        return user;
    }
    //获取用户信息
    return queryUserInfo(account);
}
dataOperator::userInfo dataOperator::getUserName(const QString &account) {
    if (!userExistCheck(account)) {}
    auto user = queryUserInfo(account);
    return userInfo{
    .account = account,
    //部分隐藏姓名，长度小于2的隐藏左侧字符，长度大于2的隐藏中间字符
    .name = user.name.length() <= 2 ? QString(user.name.length()-1,'*') + user.name.right(1)
        : user.name.left(1)+ QString(user.name.length()-2,'*') + user.name.right(1),
    .id = nullptr,
    .balance = 0,
    .errorCount = 0,
    .errors = user.errors};
}
QString dataOperator::updateBalance(const QString &account,const long long amount)
{
    if (!userExistCheck(account)) {
        //用户不存在，返回错误
        return "用户不存在";
    }
    userInfo user;
    user.account = account;
    auto query = getNewQuery(user, queryMode::updateBalance,nullptr,amount);
    return query.exec() ? nullptr : "数据库存储失败：" + query.lastError().text();
}