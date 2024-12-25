#ifndef login_H
#define login_H

#include <QDialog>

namespace Ui {
class login;
}

class login : public QDialog
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();

    static bool isLoggedIn; // 静态变量跟踪登录状态

Q_SIGNALS:
    void userLoggedIn(); // 确保信号声明在 signals: 部分

private slots:
    void on_loginSureButton_clicked();
    void on_loginSureButton_2_clicked();

private:
    Ui::login *ui;

    bool validateCredentials(const QString &userId, const QString &password);
};

#endif // login_H
