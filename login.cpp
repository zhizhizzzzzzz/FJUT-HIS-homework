#include "login.h"
#include "mainwindow.h"
#include "ui_login.h"  // 确保包含生成的 UI 头文件
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

// 初始化静态变量
bool login::isLoggedIn = false;


// 构造函数
login::login(QWidget *parent) : QDialog(parent), ui(new Ui::login) {
    ui->setupUi(this);
}

// 析构函数
login::~login() {
    delete ui;
}

// 登录按钮槽函数
void login::on_loginSureButton_clicked() {
    QString userId = ui->loginIdEdit->text();
    QString password = ui->loginSecretEdit->text();

    if (validateCredentials(userId, password)) {
        emit userLoggedIn(); // 发射信号
        isLoggedIn = true; // 更新登录状态为 true
        QMessageBox::information(this, "Login Success", "登录成功");
        qDebug() << "Emitting userLoggedIn signal";
        this->accept(); // 关闭对话框
    } else {
        QMessageBox::warning(this, "Login Failed", "登录失败，请检查信息填写是否正确");
        isLoggedIn = false; // 更新登录状态为 false

    }
}

// 取消按钮槽函数
void login::on_loginSureButton_2_clicked() {
    this->reject(); // 关闭对话框
}

// 验证凭据函数
bool login::validateCredentials(const QString &userId, const QString &password) {
    QFile file(DATA_PATH("doctor.txt"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Read Error", "Cannot open file:\n" + file.errorString());
        return false;
    }
    QTextStream in(&file);
    QString line;
    while (!in.atEnd()) {
        line = in.readLine();
        QStringList fields = line.split(" ", Qt::SkipEmptyParts);
        if (fields.size() == 2 && fields[0] == userId && fields[1] == password) {
            file.close();
            return true;
        }
    }
    file.close();
    return false;
}
