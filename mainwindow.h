#ifndef MAINWINDOW_H
#define MAINWINDOW_H
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
#include <QList>
#include <QChartView>
#include <QPieSeries>
#include <QPieSlice>
#include <QRandomGenerator>
#include <QTreeView>
#include "login.h"
#include <qtableview>
#include <QVBoxLayout>
#include <bitset>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QStandardItemModel>
#include <QDate>
#include <QString>
#include <QInputDialog>
#include <QAbstractItemView>
#include <QTreeView>

#include "ui_mainwindow.h"
#include "login.h"
#include "graph.h"
#include "dijkstra.h"

#define DATA_PATH(fileName) (QCoreApplication::applicationDirPath() + "/../../../data/" + fileName)

namespace Ui {
class MainWindow;
}

struct TreeNode {
    QString name;
    TreeNode* children[10] = {nullptr};  // Fixed size array of 10 children
    int childCount = 0;  // Keep track of number of children
};

struct DutySchedule {
    std::bitset<32> schedule[3][7]; // 三行七列的二维矩阵
};

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
    QString toString() const { return "ID: " + ID + ";" + "Name: " + name + ";" + "Age: " + age + ";" + "Sex: " + sex + ";" + "What happened: " + whathappend + ";" + "Date: " + date + ";" + "If Accept: " + ifaccept;}
    bool operator>(const DataItem& other) const {
        if (ifaccept.toInt() != other.ifaccept.toInt()) {
            return ifaccept.toInt() < other.ifaccept.toInt();
        }

        // 日期时间，格式为 "xxxx-xx-xx-xx:xx"
        QDateTime thisDateTime = QDateTime::fromString(this->date, "yyyy-MM-dd-HH:mm");
        QDateTime otherDateTime = QDateTime::fromString(other.date, "yyyy-MM-dd-HH:mm");
        // qDebug() << "Comparing:" << thisDateTime << "with" << otherDateTime << (thisDateTime < otherDateTime);
        return thisDateTime > otherDateTime;
    }
};

struct HeapNode {
    DataItem data;
    HeapNode* left;
    HeapNode* right;
    HeapNode* parent;
    
    HeapNode(const DataItem& item) : data(item), left(nullptr), right(nullptr), parent(nullptr) {}
};

class PriorityQueue {

public:
    PriorityQueue() : root(nullptr), size(0) {}
    ~PriorityQueue() { clear(root); root = nullptr; }
    PriorityQueue(const PriorityQueue&) = delete;  // Prevent copying
    PriorityQueue& operator=(const PriorityQueue&) = delete;

    void push(const DataItem& item);
    DataItem pop();
    bool empty() const { return root == nullptr; }
    void remove(const QString& id);
    DataItem* find(const QString& id);
    void update(const DataItem& newItem);
    QList<DataItem> getAll() const;

private:
    HeapNode* root;
    size_t size;
    
    void clear(HeapNode* node);
    HeapNode* findLastParent();
    void siftUp(HeapNode* node);
    void siftDown(HeapNode* node);
    HeapNode* findNode(HeapNode* node, const QString& id);
    void collectItems(HeapNode* node, QList<DataItem>& items) const;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void addDataItem(const DataItem &item);
    void saveDataToFile(const QString &filename);
    void loadDataIntoTableView();
    QMutex dataMutex; // 确保线程安全

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
    void on_deleteEmpButton_clicked();
    void on_searchTextBox_returnPressed();
    void on_pushButton_6_clicked();
    void selectedPushButton(QPushButton *button);
    void deselectedPushButton(QPushButton *button);
    void on_techButton_clicked();
    void on_aboutButton_clicked();
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
    void addQueueItemAndRefreshTableView() ;
    void initializeTableView();
    void updateChartView(int type);
    void onChartComboBoxIndexChanged(int index);
    void onTreeViewClicked(const QModelIndex &index);
    void editCurrentRow();


private:
    TreeNode* getNodeByName(TreeNode* root, const QString& name);
    DutySchedule combineDutySchedules(TreeNode* departmentNode);
    void setCurrentTime(QLabel* label);

    Ui::MainWindow *ui;
    bool isMouseDown = false;
    QTcpSocket *tcpSocket;
    DataItem *headItem = nullptr; // 链表头指针
    // DataQueueItem *headQueueItem = nullptr; // 链表头指针
    QTcpServer *tcpServer;
    login *loginDialog;
    Graphlnk<int, int> graph;
    int *dist;
    int *path;
    DutySchedule createDutySchedule(int personCount, int offset);
    void displayDutySchedule(QTableView* tableView, const DutySchedule& schedule);
    QList<DataItem> displayList;
    void populateTreeView(QTreeView *treeView, TreeNode *root);
    TreeNode* root;
    QTableView* workTableView;
    void autoGenerateDutySchedules(std::map<QString, DutySchedule> &schedules, int personCount);
    std::map<QString, DutySchedule> dutySchedules;
    TreeNode* createDepartmentTree();
    template <class T, class E>
    inline bool compareDataItems(const T &item1, const E &item2) {
        return (item1.ID == item2.ID) &&
               (item1.name == item2.name) &&
               (item1.age == item2.age) &&
               (item1.sex == item2.sex) &&
               (item1.whathappend == item2.whathappend) &&
               (item1.date == item2.date) &&
               (item1.ifaccept == item2.ifaccept);
    }
    template <class T>
    inline void printLinkedList(const T *head) { //debug usage
        const T *current = head;
        while (current != nullptr) {
            qDebug() << current->toString();
            current = current->next;
        }
    }

};

#endif // MAINWINDOW_H
