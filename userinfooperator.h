#pragma once
#include <QDialog>
#include "dataoperator.h"
namespace Ui {
class UserInfoOperator;
}

class UserInfoOperator : public QDialog
{
    Q_OBJECT
public:
    enum class userMode {createUser,modifyUser};
    explicit UserInfoOperator(QWidget *parent = nullptr,userMode mode=userMode::createUser,const dataOperator::userInfo& user = {});
    ~UserInfoOperator() override;


private slots:
    void on_id_textEdited(const QString &arg1);
    void on_password_textEdited(const QString &arg1);
    void on_balance_textEdited(const QString &arg1);
    void on_account_textEdited(const QString &arg1);
    void on_buttonBox_accepted();
private:
    Ui::UserInfoOperator *ui;

    bool inputValidCheck();
    userMode mode;
};
