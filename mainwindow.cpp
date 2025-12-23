#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QRegularExpressionValidator>
#include "validcheck.h"
#include "dataoperator.h"
#include <QMessageBox>

#include "accountpanel.h"
#include "ui_accountpanel.h"
#include "userinfooperator.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //设置输入限制
    ui->account->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9]*$")));
    ui->password->setValidator(new QRegularExpressionValidator(QRegularExpression("^[0-9]*$")));
}

MainWindow::~MainWindow()
{
    delete ui;
}
//账号输入检测
void MainWindow::on_account_editingFinished() const {
    inputValidCheck();
}

//密码输入检测
void MainWindow::on_password_editingFinished() const {
    inputValidCheck();
}
void MainWindow::inputValidCheck() const {
    ui->notice->show();

    //检查账号是否长度19位且符合Luhn校验
    const QString account = ui->account->text();
    if (account.length() != 19 && !account.isEmpty()) {
        ui->notice->setText(QString("账号长度不正确,长度：%1").arg(account.length()));
        return;
    }
    if (!validCheck::isValidLuhn(account,account.length())) {
        ui->notice->setText(QString("账号不符合Luhn校验"));
        return;
    }
    //检查密码是否为19位
    QString password = ui->password->text();
    if (password.length() != 6 && !password.isEmpty()) {
        ui->notice->setText(QString("密码长度不正确,长度：%1").arg(password.length()));
        return;
    }
    ui->notice->setText(nullptr);
}
//无卡登录
void MainWindow::on_normalLogin_clicked()
{
    //获取用户信息
    const auto user = dataOperator::getInstance().userLogin(ui->account->text(),ui->password->text());
    if (user.errors != nullptr) {
        QMessageBox::warning(this,"登录错误",user.errors);
    } else {
        //登录成功
        QMessageBox::information(this,"登录成功",QString("欢迎您，%1！").arg(user.name));
        // 打开账户面板窗口
        auto *a = new AccountPanel(user,ui->password->text());
        a->setAttribute(Qt::WA_DeleteOnClose);
        a->setWindowTitle("账户面板");
        //删除输入的内容
        ui->account->clear();
        ui->password->clear();
        // 连接第二个窗口的关闭信号到当前窗口的显示槽
        connect(a, &AccountPanel::destroyed, this, &MainWindow::show);
        a->show();
    }
}

void MainWindow::on_openAccount_clicked()
{
    UserInfoOperator o(nullptr,UserInfoOperator::userMode::createUser);
    o.setWindowTitle("创建用户");
    o.exec();
}

