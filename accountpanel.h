#ifndef ACCOUNTPANEL_H
#define ACCOUNTPANEL_H
#include "dataoperator.h"
#include <QMainWindow>
#include <ui_inputdialog.h>

namespace Ui {
class AccountPanel;
}

class AccountPanel : public QMainWindow
{
    Q_OBJECT

public:
    explicit AccountPanel(dataOperator::userInfo user, QString password,QWidget *parent = nullptr);

    void displayUserInfo() const;

    ~AccountPanel();

private slots:
    void on_exit_clicked();

    void on_modifyInfo_clicked();

    void on_setPassword_clicked();

    void on_transferBalance_clicked();

    void on_withdrawBalance_clicked();

    void refresh();

private:
    Ui::AccountPanel *ui;
    dataOperator::userInfo user;
    QString password;
    QTimer *timer;
};

#endif // ACCOUNTPANEL_H
