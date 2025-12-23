#pragma once
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:

    void on_normalLogin_clicked();

    static void on_openAccount_clicked();

    void on_account_editingFinished() const;

    void on_password_editingFinished() const;

private:
    Ui::MainWindow *ui;
    void inputValidCheck() const;
};
