#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QtSql/QSqlDatabase>
#include <QString>
#include <QDebug>
#include <QSqlError>
#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QTcpSocket>
#include <QTcpServer>
#include <QMutexLocker>
#include <QDateTime>
#include "login.h"
#include <QDebug>
#include <QList>

namespace Ui {
class MainWindow;
}
struct DataItem {
    QString ID;
    QString name;
    QString age;
    QString sex;
    QString whathappend;
    QString date; // 格式为 "xxxx-xx-xx-xx:xx"
    QString ifaccept;
    DataItem *next;
    DataItem *ptr = nullptr; // 指向链表节点的指针

};

struct DataQueueItem {

    QString ID;
    QString name;
    QString age;
    QString sex;
    QString whathappend;
    QString date;
    QString ifaccept;
    DataQueueItem *next;

    bool operator>(const DataQueueItem& other) const {
        if (ifaccept.toInt() != other.ifaccept.toInt()) {
            return ifaccept.toInt() < other.ifaccept.toInt();
        }

        // 日期时间，格式为 "xxxx-xx-xx-xx:xx"
        QDateTime thisDateTime = QDateTime::fromString(this->date, "yyyy-MM-dd-HH:mm");
        QDateTime otherDateTime = QDateTime::fromString(other.date, "yyyy-MM-dd-HH:mm");
        qDebug() << "Comparing:" << thisDateTime << "with" << otherDateTime << (thisDateTime < otherDateTime);
        return thisDateTime > otherDateTime;
    }
};

class PriorityQueue {

public:
    PriorityQueue() : headQueueItem(nullptr) {}

    void push(const DataQueueItem& item);
    DataQueueItem pop();
    bool empty() const;
    void remove(const QString& id);
    DataQueueItem* find(const QString& id) ;
    void update(const DataQueueItem& newItem);
    QList<DataQueueItem> getAll() const ;
private:
    std::vector<DataQueueItem> heap;
    void siftUp(size_t index);
    void siftDown(size_t index);
    DataQueueItem *headQueueItem = nullptr; // 链表头指针

};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void addDataItem(const DataItem &item);
    DataItem* findDataItem(const QString &ID);
    void saveDataToFile(const QString &filename);
    void loadDataIntoTableView();
    QMutex dataMutex; // 确保线程安全
    // static QDateTime parseDateTime(const QString &dateTimeStr);

 protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    QPoint mousePoint;


    private slots:

    void on_searchTextBox_textChanged(const QString &text);


    void refreshTableView() ;

    void on_closeButton_clicked();

    void loadTotalData();

    void on_minimizeButton_clicked();

    void on_pushButton_clicked();

    void on_attendanceButton_clicked();

    void on_searchButton_clicked();

    void on_paymentButton_clicked();

    void on_attendance1_2_clicked();

    void on_addEmpButton_clicked();

    void on_updateEmpButton_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_2_clicked();

    void on_empDept_currentIndexChanged(const int &index);

    void on_deleteEmpButton_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_pushButton_7_clicked();

    void on_updateTableView_doubleClicked(const QModelIndex &index);

    void on_pushButton_8_clicked();

    void on_deleteTableView_doubleClicked(const QModelIndex &index);

    void on_searchTextBox_returnPressed();

    void on_pushButton_6_clicked();

    void selectedPushButton(QPushButton *button);

    void deselectedPushButton(QPushButton *button);

    void on_techButton_clicked();

    void on_tableView_activated(const QModelIndex &index);

    void on_aboutButton_clicked();

    void on_pushButton_3_clicked();

    void readTcpData();
    void handleError(QAbstractSocket::SocketError socketError);
    void onConnected(); void onDisconnected();
    void onStateChanged(QAbstractSocket::SocketState socketState);
    void newConnection();
    void on_pushbtton_9_clicked();
    void onUserLoggedIn(); // 定义槽函数
    void loadDataIntoQueue();
    void loadDataIntodeleteTableView();
    void countIfAcceptNegativeOne();
    void on_pushButton_15_clicked();
    void on_pushButton_16_clicked();
    void on_pushButton_17_clicked();
    void saveQueueDataToFile(const QString &filename);
    void addQueueItemAndRefreshTableView() ;
    void initializeTableView();

private:
    Ui::MainWindow *ui;
    bool isMouseDown = false;
    QTcpSocket *tcpSocket;
    DataItem *headItem = nullptr; // 链表头指针
    DataQueueItem *headQueueItem = nullptr; // 链表头指针
    QTcpServer *tcpServer;
    login *loginDialog;
    QList<DataQueueItem> displayList;


};

#endif // MAINWINDOW_H
