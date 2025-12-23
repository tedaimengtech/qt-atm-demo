#include "userinfooperator.h"
#include "ui_userinfooperator.h"
#include "validcheck.h"
#include <QRegularExpressionValidator>
#include <QMessageBox>

#include "dataoperator.h"

UserInfoOperator::UserInfoOperator(QWidget *parent,userMode mode,const dataOperator::userInfo& user)
    : QDialog(parent)
    , mode(mode)
    , ui(new Ui::UserInfoOperator)
{
    ui->setupUi(this);
    //设置输入限制
    //数字限制
    ui->account->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9]*00$")));
    ui->password->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9]*00$")));
    ui->balance->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9]*00$")));
    //身份证号限制
    ui->id->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9]{17}[0-9Xx]$")));
    //姓名限制
    ui->name->setValidator(new QRegularExpressionValidator(QRegularExpression("^([a-zA-Z0-9\u4e00-\u9fa5\u00b7]{1,10})$")));
    //如果是修改用户模式，则锁定账号、密码、余额输入框
    if (mode == userMode::modifyUser) {
        ui->account->setText(user.account);
        ui->account->setReadOnly(true);
        ui->password->setReadOnly(true);
        ui->name->setText(user.name);
        ui->id->setText(user.id);
        ui->balance->setText(QString::number(user.balance));
        ui->balance->setReadOnly(true);
    }
}

UserInfoOperator::~UserInfoOperator()
{
    delete ui;
}

bool UserInfoOperator::inputValidCheck() {
    //检查账号是否长度19位且符合Luhn校验
    const QString account = ui->account->text();
    if (!account.isEmpty()) {
        if (account.length() != 19) {
            ui->notice->setText(QString("账号长度不正确,长度：%1").arg(account.length()));
            return false;
        }
        if (!validCheck::isValidLuhn(account,account.length())) {
            ui->notice->setText(QString("账号不符合Luhn校验"));
            return false;
        }
    }
    //如果是创建账户模式，检查密码是否为6位
    if (mode == userMode::createUser) {
        const QString password = ui->password->text();
        if (password.length() != 6 && !password.isEmpty()) {
            ui->notice->setText(QString("密码长度不正确,长度：%1").arg(password.length()));
            return false;
        }
    }
    //检测身份证号是否长度18位且符合身份证校验
    const QString id = ui->id->text();
    if (!id.isEmpty()) {
        if (id.length() != 18) {
            ui->notice->setText(QString("身份证号长度不正确,长度：%1").arg(id.length()));
            return false;
        }
        if (!validCheck::isValidID(id)) {
            ui->notice->setText(QString("身份证号不符合校验"));
            return false;
        }
    }
    //检测余额是否为100的倍数
    const auto balance = ui->balance->text().toULongLong();
    if (balance % 100 != 0) {
        ui->notice->setText(QString("余额不是100的倍数"));
        return false;
    }
    ui->notice->setText(nullptr);
    return true;
}
//输入检测
void UserInfoOperator::on_account_textEdited(const QString &arg1){inputValidCheck();}
void UserInfoOperator::on_id_textEdited(const QString &arg1){
    //将x转为大写
    ui->id->setText(ui->id->text().toUpper());
    inputValidCheck();
}
void UserInfoOperator::on_password_textEdited(const QString &arg1){inputValidCheck();}
void UserInfoOperator::on_balance_textEdited(const QString &arg1){inputValidCheck();}

void UserInfoOperator::on_buttonBox_accepted() {
    //最终检查，确保所有输入合法
    if (!inputValidCheck() || ui->name->text().isEmpty()
        || ui->account->text().isEmpty()
        || ui->id->text().isEmpty()
        //只在创建用户时确定密码是否为空
        || (mode == userMode::createUser && ui->password->text().isEmpty())
        || ui->balance->text().isEmpty()
    ) {
        QMessageBox::warning(this, "警告", "输入信息有误");
        return;
    }
    dataOperator::userInfo newUser {
        .account = ui->account->text(),
        .name = ui->name->text(),
        .id = ui->id->text(),
        .balance = ui->balance->text().toULongLong()
    };
    //写入数据库
    auto instance = dataOperator::getInstance();
    //检查数据库有没有相同账户
    QString errors;
    switch (mode) {
        case userMode::createUser:
            errors = instance.createUser(newUser, ui->password->text());
            break;
        case userMode::modifyUser:
            errors = instance.updateUserInfo(newUser);
            break;
    }
    if (errors != nullptr) {
        QMessageBox::information(this, "失败", "存储数据失败:"+errors);
    } else {
        QMessageBox::warning(this, "成功", "存储数据成功!");
        this->close();
    }
}

