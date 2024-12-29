#include "mainwindow.h"
#include <algorithm>
#include "about.h"
#include "techused.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QLabel>
#include <QDateTime>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
ui(new Ui::MainWindow),
graph({
     {1, 2, 4}, {1, 12, 2}, {1, 11, 3},
     {2, 1, 4}, {2, 3, 5}, {2, 5, 8},
     {3, 2, 5}, {3, 4, 4},
     {4, 3, 4}, {4, 5, 4}, {4, 6, 8},
     {5, 2, 8}, {5, 4, 5}, {5, 6, 1}, {5, 7, 3},
     {6, 4, 8}, {6, 5, 1}, {6, 7, 2},
     {7, 5, 3}, {7, 6, 2}, {7, 8, 2},{7, 12, 3},
     {8, 7, 2}, {8, 9, 3},
     {9, 8, 3}, {9, 10, 4}, {9, 12, 5}, {9, 13, 8},
     {10, 11, 3}, {10, 9, 4},
     {11, 1, 3}, {11, 10, 3},
     {12, 1, 2}, {12, 7, 3}, {12, 9, 5},
     {13, 9, 8}, {13, 14, 2},
     {14, 13, 2}, {14, 23, 3}, {14, 15, 2}, {14, 23, 3},
     {15, 14, 2}, {15, 23, 2}, {15, 16, 7}, {15, 17, 2},
     {16, 15, 7}, {16, 17, 5},
     {17, 15, 2}, {17, 16, 5}, {17, 18, 1},
     {18, 17, 1}, {18, 19, 6},
     {19, 18, 6}, {19, 20, 4},
     {20, 19, 4}, {20, 21, 5}, {20, 23, 2},
     {21, 22, 1}, {21, 20, 5}, {21, 23, 4},
     {22, 21, 1},
     {23,14,3},{23,15,2},{23,20,2},{23,21,4}
  })
{
    qDebug() << DATA_PATH("data.txt");

    int n = graph.numberOfVertices();
    dist = new int[n];
    path = new int[n];

    ui->setupUi(this);

    tcpServer = new QTcpServer(this);
    tcpSocket = new QTcpSocket(this);
    // tcpSocket->connectToHost(QHostAddress("192.168.10.230"), 8081);
    loginDialog = new login(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &MainWindow::newConnection);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &MainWindow::readTcpData);
    connect(tcpSocket, &QTcpSocket::errorOccurred, this, &MainWindow::handleError);
    connect(tcpSocket, &QTcpSocket::connected, this, &MainWindow::onConnected);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &MainWindow::onDisconnected);
    connect(tcpSocket, &QTcpSocket::stateChanged, this, &MainWindow::onStateChanged);
    connect(loginDialog, &login::userLoggedIn, this, &MainWindow::onUserLoggedIn);
    connect(ui->tableView, &QTableView::doubleClicked, this, &MainWindow::editCurrentRow);
    if (!tcpServer->listen(QHostAddress::Any, 8081)) {
        qDebug() << "Server could not start!";
    } else {
        qDebug() << "Server started!";
    }

    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setStyleSheet("alternate-background-color: #1f1f1f;background-color:#2d2d2d;selection-background-color:#1492E6;selection-color:white;");


    ui->deleteTableView->setAlternatingRowColors(true);
    ui->deleteTableView->setStyleSheet("alternate-background-color: #1f1f1f;background-color:#2d2d2d;selection-background-color:#1492E6;selection-color:white;");


    ui->searchStackedWidget->setCurrentIndex(0); // 设置初始界面为 searchStackedWidget 的第一个页面
    connect(ui->pushButton_9, &QPushButton::clicked, this, &MainWindow::on_pushbtton_9_clicked);

    connect(ui->lineEditSearch, &QLineEdit::textChanged, this, &MainWindow::on_searchTextBox_textChanged);

    connect(ui->chartComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onChartComboBoxIndexChanged(int)));
    setCurrentTime(ui->label_58);
}

MainWindow::~MainWindow()
{
    delete ui;
    tcpSocket->close();

}


void MainWindow::editCurrentRow() {

    DataItem *current = headItem;
    // DataItem *qcurrent = headQueueItem;
    loadDataIntoQueue();

    // 获取当前选中的行和列
    QModelIndex index = ui->tableView->currentIndex();
    if (!index.isValid())
        return; // 如果没有选中的行，直接返回

    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->tableView->model());
    if (!model)
        return; // 如果模型不是QStandardItemModel，直接返回

    int row = index.row();
    QStandardItem *whathappendItem = model->item(row, 4); // 假设what happened在第5列（索引从0开始）
    if (!whathappendItem)
        return; // 如果没有找到对应的项，直接返回

    // 弹出输入框让用户输入
    bool ok;
    QString newWhatHappend = QInputDialog::getText(this, tr("编辑症状"),
                                                   tr("输入内容:"), QLineEdit::Normal,
                                                   whathappendItem->text(), &ok);
    QString ifacceptStr = model->item(row, 6)->text();
    if (ifacceptStr == "急诊") {
        ifacceptStr = "1";
    } else if (ifacceptStr == "门诊") {
        ifacceptStr = "0";
    }
    if (ok && !newWhatHappend.isEmpty()) {
        DataItem cpval = {
            model->item(row, 0)->text(),
            model->item(row, 1)->text(),
            model->item(row, 2)->text(),
            model->item(row, 3)->text(),
            model->item(row, 4)->text(),
            model->item(row, 5)->text(),
            ifacceptStr,
            nullptr,
            nullptr
        };
        DataItem *hitDI = nullptr;
        // DataItem *hitDQI = nullptr;

        while (current != nullptr) {
            // qDebug() << current->toString()<<"---COMP---"<< cpval.toString();
            bool is_same = compareDataItems(*current,cpval);
            // qDebug() << "is_same = "<<is_same;
            if (is_same){
                hitDI = current;
                break;
            }
            current=current->next;
        }
        // while (qcurrent != nullptr) {
        //     qDebug() << qcurrent->toString()<<"---COMPQ---"<< cpval.toString();
        //     bool is_same = compareDataItems(*qcurrent,cpval);
        //     qDebug() << "is_same_queue = "<<is_same;
        //     if (is_same){
        //         hitDQI = qcurrent;
        //         break;
        //     }
        //     qcurrent=qcurrent->next;
        // }
        if(hitDI!=nullptr){
            hitDI->whathappend=newWhatHappend.trimmed();
            hitDI->ifaccept="-1";
            // hitDQI->whathappend=newWhatHappend;
            saveDataToFile(DATA_PATH("data.txt"));
            displayList.clear();
            loadDataIntodeleteTableView();
            loadDataIntoQueue();
            loadDataIntoTableView();
            updateChartView(ui->chartComboBox->currentIndex());
            ui->deptLabel->setText(QString::asprintf("%03d", ui->deptLabel->text().toInt() + 1));
        }
    }
}



void MainWindow::addDataItem(const DataItem &item) {
    // 遍历链表检查是否存在相同的数据
    QMutexLocker locker(&dataMutex); // 确保线程安全
    DataItem *current = headItem;
    while (current != nullptr) {
        if (current->ID == item.ID && current->name == item.name && current->age == item.age &&
            current->sex == item.sex && current->whathappend == item.whathappend &&
            current->date == item.date ) {
            qDebug() << "Duplicate data found, not adding to the list.";
            tcpSocket->write("AlreadyExist");
            tcpSocket->flush(); // 确保消息被立即发送
            return; // 如果找到相同的数据，则返回
        }
        current = current->next;
    }

    // 如果没有找到相同的数据，则添加到链表头部
    DataItem *newItem = new DataItem(item);
    if (headItem == nullptr) {
        headItem = newItem;
    } else {
        DataItem *current = headItem;
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = newItem;
    }
}

void MainWindow::countIfAcceptNegativeOne() {
    int count = 0;

    // 锁定互斥锁以确保线程安全
    QMutexLocker locker(&dataMutex);
    // 从头节点开始遍历链表
    DataItem* current = headItem;

    while (current != nullptr) {
        if (current->ifaccept == "-1") {
            count++;
        }
        current = current->next;
    }

    // 在 empLabel 上显示结果
    ui->empLabel->setText(QString("%1").arg(count));
}

void MainWindow::newConnection()
{
    tcpSocket = tcpServer->nextPendingConnection();
    connect(tcpSocket, &QTcpSocket::readyRead, this, &MainWindow::readTcpData);
    qDebug() << "New connection from:" << tcpSocket->peerAddress().toString();
}

void MainWindow::saveDataToFile(const QString &filename) {
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text )) {
        QTextStream out(&file);
        DataItem *current = headItem;
        while (current != nullptr) {
            out << current->ID << " " << current->name << " " << current->age << " " << current->sex << " "
                << current->whathappend << " " << current->date << " " << current->ifaccept<<"\n";
            current = current->next;
        }
        file.close();
    } else {
        QMessageBox::warning(this, tr("Write Error"), tr("Cannot open file for writing:\n%1").arg(file.errorString()));
    }
}


// 登录成功时的槽函数
void MainWindow::onUserLoggedIn()
{
    qDebug()<<"Login success!";

    ui->tableView->setDisabled(false);
    loadTotalData(); // 加载数据
    loadDataIntoQueue();
    loadDataIntoTableView();
    updateChartView(ui->chartComboBox->currentIndex());

    root = createDepartmentTree();

    // 获取 treeView 和 workTableView
    QTreeView* treeView = ui->treeView;
    workTableView = ui->workTableView;

    // 加载树数据到 treeView
    populateTreeView(treeView, root);

    // 连接 treeView 点击事件
    connect(treeView, &QTreeView::clicked, this, &MainWindow::onTreeViewClicked);
    autoGenerateDutySchedules(dutySchedules, 32);
    ui->pushButton_9->hide();
    ui->labeltologin->hide();
    ui->labeltologin_2->hide();
    ui->labeltologin_3->hide();
    ui->labeltologin_4->hide();

}


void MainWindow::readTcpData() {
    while (tcpSocket->canReadLine()) {
        QString line = tcpSocket->readLine().trimmed(); // 去掉前后空白字符
        qDebug() << "Received line:" << line; // 打印接收到的具体内容

        QChar frameHeader = line.left(1)[0]; // 获取帧头数据
        QString contentData = line.mid(1);   // 获取内容数据

        if (contentData.isEmpty()) {
            qDebug() << "Empty content data, ignoring.";
            continue; // 忽略空数据
        }


        if (frameHeader == 'a') {
            QStringList fields = contentData.split(" ", Qt::SkipEmptyParts);
            qDebug() << "Fields:" << fields;
            if (fields.size() == 7) {
                DataItem item;

                item.ID = fields[0];
                item.name = fields[1];
                item.age = fields[2];
                item.sex = fields[3];
                item.whathappend = fields[4];
                item.date = fields[5];
                item.ifaccept = fields[6].replace("\n", "");
                item.next = nullptr;

                addDataItem(item); // 添加到链表头部
                qDebug() << "Parsed data:" << item.ID << item.name << item.age << item.sex << item.whathappend << item.date << item.ifaccept;
                saveDataToFile(DATA_PATH("data.txt"));
                loadDataIntodeleteTableView();
                displayList.clear();
                loadDataIntoQueue();
                loadDataIntoTableView();
                updateChartView(ui->chartComboBox->currentIndex());
            } else {
                qDebug() << "Unexpected number of fields for frameHeader 'a'.";
            }
        } else if (frameHeader == 'b') {
            QStringList fields = contentData.split(" ", Qt::SkipEmptyParts);
            qDebug() << "Fields:" << fields;
            if (fields.size() == 2) {
                QMutexLocker locker(&dataMutex); // 确保线程安全
                auto v = graph.getVertexPos(fields[0].toInt());
                auto d = graph.getVertexPos(fields[1].toInt());
                Dijkstra(graph, v, dist, path);
                std::string shortestPath = getShortestPath(graph, v, d, path);
                qDebug() << QString::fromStdString(shortestPath);
                tcpSocket->write(shortestPath.c_str());
                tcpSocket->flush(); // 确保消息被立即发送
            } else {
                qDebug() << "Unexpected number of fields for frameHeader 'b'.";
            }
        } else if (frameHeader == 'c') {
            qDebug() << "Processing frameHeader 'c' data";
        } else {
            qDebug() << "Unknown frameHeader:" << frameHeader;
        }
    }
}

void MainWindow::addQueueItemAndRefreshTableView() {
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->tableView->model());
    if (!model) {
        model = new QStandardItemModel(this);
        ui->tableView->setModel(model);
    }
    model->clear(); // 清除现有数据

    // 使用 queueData 来填充模型
    for (const DataItem &item : displayList) {
        QList<QStandardItem *> items;
        items << new QStandardItem(item.ID)
              << new QStandardItem(item.name)
              << new QStandardItem(item.age)
              << new QStandardItem(item.sex)
              << new QStandardItem(item.whathappend)
              << new QStandardItem(item.date)
              << new QStandardItem(item.ifaccept);

        QString ifacceptStr = item.ifaccept;
        if (ifacceptStr == "1") {
            ifacceptStr = "急诊";
        } else if (ifacceptStr == "0") {
            ifacceptStr = "门诊";
        }

        items << new QStandardItem(ifacceptStr);

        // 设置每个表项的文本对齐方式和编辑状态
        for (QStandardItem *item : items) {
            item->setTextAlignment(Qt::AlignCenter);
            item->setEditable(false);
        }
        model->appendRow(items);
    }

    ui->tableView->setModel(model);
    // 设置每列的宽度
    ui->tableView->setColumnWidth(0, 105);
    ui->tableView->setColumnWidth(1, 100); // 调整为合适的宽度
    ui->tableView->setColumnWidth(2, 50);
    ui->tableView->setColumnWidth(3, 70);
    ui->tableView->setColumnWidth(4, 180);
    ui->tableView->setColumnWidth(5, 130);
    ui->tableView->setColumnWidth(6, 40);

    ui->tableView->resizeRowsToContents();
    qDebug() << "tabelview sent.";
}

    // 增加元素
void PriorityQueue::push(const DataItem& item) {
        // DataItem* newItem = new DataItem(item);
        // if (headQueueItem == nullptr || *newItem < *headQueueItem) {
        //     newItem->next = headQueueItem;
        //     headQueueItem = newItem;
        // } else {
        //     DataItem* current = headQueueItem;
        //     while (current->next != nullptr && !(*newItem < *(current->next))) {
        //         current = current->next;
        //     }
        //     newItem->next = current->next;
        //     current->next = newItem;
        // }
        heap.push_back(item);
        siftUp(heap.size() - 1);
    }
    //Todo:把库改成自己实现的链队push

    // 删除元素

    // 更新元素

void PriorityQueue::siftUp(size_t index) {
    while (index > 0) {
        size_t parent = (index - 1) / 2;
        if (heap[index] > heap[parent]) {
            std::swap(heap[index], heap[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

void PriorityQueue::siftDown(size_t index) {
    size_t left, right, smallest;
    while ((left = 2 * index + 1) < heap.size()) {
        right = left + 1;
        smallest = (right < heap.size() && heap[right] >heap[left]) ? right : left;
        if (heap[smallest] > heap[index]) {
            std::swap(heap[index], heap[smallest]);
            index = smallest;
        } else {
            break;
        }
    }
}

DataItem PriorityQueue::pop() {
    if (heap.empty()) {
        throw std::runtime_error("Queue is empty");
    }
    std::swap(heap.front(), heap.back());
    DataItem item = heap.back();
    heap.pop_back();
    siftDown(0);
    return item;
}

bool PriorityQueue::empty() const {
    return heap.empty();
}

void MainWindow::loadDataIntoQueue() {
    QFile file(DATA_PATH("data.txt"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Read Error"), tr("Cannot open file:\n%1").arg(file.errorString()));
        return;
    }
    QTextStream in(&file);
    QString line;
    PriorityQueue pq;

    while (!in.atEnd()) {
        line = in.readLine();
        QStringList fields = line.split(" ", Qt::SkipEmptyParts);
        if (fields.size() == 7) {
            DataItem item;
            item.ID = fields[0];
            item.name = fields[1];
            item.age = fields[2];
            item.sex = fields[3];
            item.whathappend = fields[4];
            item.date = fields[5];
            item.ifaccept = fields[6];
            pq.push(item);
        }
    }
    file.close();

    while (!pq.empty()) {
        DataItem item = pq.pop();
        if (item.ifaccept.toInt() == 0 || item.ifaccept.toInt() == 1) {
            displayList.append(item);
        }
    }

    // loadDataIntoTableView();
}




void MainWindow::loadTotalData() {
    QFile file(DATA_PATH("data.txt"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Read Error"), tr("Cannot open file:\n%1").arg(file.errorString()));
        return;
    }
    QTextStream in(&file);
    QString line;
    PriorityQueue pq;

    while (!in.atEnd()) {
        line = in.readLine();
        QStringList fields = line.split(" ", Qt::SkipEmptyParts);
        if (fields.size() == 7) {
            DataItem item;
            item.ID = fields[0];
            item.name = fields[1];
            item.age = fields[2];
            item.sex = fields[3];
            item.whathappend = fields[4];
            item.date = fields[5];
            item.ifaccept = fields[6];
            item.next = nullptr;

            addDataItem(item); // 使用头插法添加到链表
        }
    }


    file.close();

    // loadDataIntoTableView();
    loadDataIntodeleteTableView();
}
void MainWindow::loadDataIntoTableView() {
    // 创建一个新的模型
    QStandardItemModel *model = new QStandardItemModel(this);
    // 设置列标题
    model->setHorizontalHeaderLabels(QStringList() << "ID" << "Name" << "Age" << "Sex" << "What Happened" << "Date" << "If Accept");

    // 遍历 displayList 填充模型
    for (int row = displayList.size()-1; row >=0; row--) {
        QList<QStandardItem *> items;
        items << new QStandardItem(displayList[row].ID)
              << new QStandardItem(displayList[row].name)
              << new QStandardItem(displayList[row].age)
              << new QStandardItem(displayList[row].sex)
              << new QStandardItem(displayList[row].whathappend)
              << new QStandardItem(displayList[row].date);

        QString ifacceptStr = displayList[row].ifaccept.replace("\n", " ");
        if (ifacceptStr == "1") {
            ifacceptStr = "急诊";
        } else if (ifacceptStr == "0") {
            ifacceptStr = "门诊";
        }

        items << new QStandardItem(ifacceptStr);

        // 设置每个表项的文本对齐方式和编辑状态
        for (QStandardItem *item : items) {
            item->setTextAlignment(Qt::AlignCenter);
            item->setEditable(false);
        }

        model->appendRow(items);
    }

    // 将模型设置到表格视图中
    ui->tableView->setModel(model);
    // 设置每列的宽度
    ui->tableView->setColumnWidth(0, 105);
    ui->tableView->setColumnWidth(1, 100); // 调整为合适的宽度
    ui->tableView->setColumnWidth(2, 50);
    ui->tableView->setColumnWidth(3, 70);
    ui->tableView->setColumnWidth(4, 180);
    ui->tableView->setColumnWidth(5, 130);
    ui->tableView->setColumnWidth(6, 40);

    ui->tableView->resizeRowsToContents();
    qDebug() << "tabelview sent.";
    countIfAcceptNegativeOne();

}


void MainWindow::loadDataIntodeleteTableView() {
    QStandardItemModel *model = new QStandardItemModel(this);
    // 设置列标题
    model->setHorizontalHeaderLabels(QStringList() << "ID" << "Name" << "Age" << "Sex" << "What Happened" << "Date" << "If Accept");

    // 锁定互斥锁以确保线程安全
    // QMutexLocker locker(&dataMutex);
    // 从头节点开始遍历链表
    DataItem *current = headItem;

    while (current != nullptr) {
        // 创建一行新的表项
        QList<QStandardItem *> items;
        items << new QStandardItem(current->ID)
              << new QStandardItem(current->name)
              << new QStandardItem(current->age)
              << new QStandardItem(current->sex)
              << new QStandardItem(current->whathappend)
              << new QStandardItem(current->date);

        QString ifacceptStr = current->ifaccept.replace("\n", " ");
        if (ifacceptStr == "1") {
            ifacceptStr = "急诊";
        } else if (ifacceptStr == "0") {
            ifacceptStr = "门诊";
        } else if (ifacceptStr == "-1") {
            ifacceptStr = "已就诊";
        }
        items << new QStandardItem(ifacceptStr);

        // 设置每个表项的文本对齐方式和编辑状态
        for (QStandardItem *item : items) {
            item->setTextAlignment(Qt::AlignCenter);
            item->setEditable(false);
        }

        // 将这行表项添加到模型中
        model->appendRow(items);
        // 移动到链表的下一个节点
        current = current->next;
    }

    // 将模型设置到表格视图中
    ui->deleteTableView->setModel(model);
    // 设置每列的宽度
    ui->deleteTableView->setColumnWidth(0, 105);
    ui->deleteTableView->setColumnWidth(1, 100); // 调整为合适的宽度
    ui->deleteTableView->setColumnWidth(2, 50);
    ui->deleteTableView->setColumnWidth(3, 70);
    ui->deleteTableView->setColumnWidth(4, 180);
    ui->deleteTableView->setColumnWidth(5, 130);
    ui->deleteTableView->setColumnWidth(6, 102);

    ui->deleteTableView->resizeRowsToContents();

}


// 处理错误
void MainWindow::handleError(QAbstractSocket::SocketError socketError)
{
    qDebug()<<socketError;
    QMessageBox::critical(this, tr("Socket Error"), tcpSocket->errorString());
}

// 连接建立时
void MainWindow::onConnected() {
    qDebug() << "Connected to server.";
}

// 连接断开时
void MainWindow::onDisconnected() {
    qDebug() << "Disconnected from server.";
}

// 状态改变时
void MainWindow::onStateChanged(QAbstractSocket::SocketState socketState) {
    qDebug() << "Socket state changed:" << socketState;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        isMouseDown = true;
        mousePoint = event->globalPosition().toPoint();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug()<<event;
    isMouseDown = false;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    const QPoint delta = event->globalPosition().toPoint() - mousePoint;
    if (isMouseDown == true)
    {
        move(x() + delta.x(), y() + delta.y());
    }
    mousePoint = event->globalPosition().toPoint();
}

void MainWindow::on_closeButton_clicked()
{
    this->close();
}

// 1. 设置 QTableView 为可编辑
void MainWindow::initializeTableView() {
    QStandardItemModel *model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels(QStringList() << "ID" << "Name" << "Age" << "Sex" << "What Happened" << "Date" << "If Accept");
    ui->tableView->setModel(model);
}

void MainWindow::updateChartView(int type){
    DataItem *current = headItem;
    QChart *chart = new QChart();
    QPieSeries *series = new QPieSeries;
    QString title;
    std::vector<QString> labels;
    std::vector<int> counters;
    int len,i;
    switch(type){
    case 0:{
        title = "年龄分布情况";
        labels = {"19","20","21","22"}; // must > 1 Item
        labels.insert(labels.begin(), labels[0]+"-");
        labels.push_back(labels.back()+"+");
        len = labels.size();
        counters = std::vector<int>(len,0);
        while (current != nullptr) {
            int age = current->age.toInt();
            if(age < labels[1].toInt())
                counters[0]++;
            else if(age > labels[len-2].toInt())
                counters[len - 1]++;
            else
                for(i = 1; i < len - 1; i++)
                    if(age == labels[i].toInt())
                        counters[i]++;
            current = current->next;
        }
        break;
    }
    case 1:{
        title = "性别分布情况";
        labels = {"男","女","其他"};
        len = labels.size();
        counters = std::vector<int>(len,0);
        while (current != nullptr) {
            if(current->sex==labels[0])
                counters[0]++;
            else if(current->sex==labels[1])
                counters[1]++;
            else
                counters[2]++;
            current = current->next;
        }
        break;
    }
    case 2:{
        title = "诊断分布情况";
        labels = {"咳嗽","发热","感冒","肺炎","哮喘","晕倒","便秘","胃痛","心悸","过敏"}; // TODO: 加一个医生可以手动添加
        len = labels.size();
        counters = std::vector<int>(len,0);
        while (current != nullptr) {
            for(i = 0; i < len; i++)
                if(current->whathappend.contains(labels[i])) // TODO: 使用KMP实现
                    counters[i]++;
            current = current->next;
        }
        break;
    }
    case 3:{
        title = "高峰时间段分布";
        int hourStep = 4;
        for (i = 0; i < 24; i += hourStep)
            labels.push_back(QString("%1:00-%2:59").arg(i, 2, 10, QChar('0')).arg(i + 3, 2, 10, QChar('0')));
        len = labels.size();
        counters = std::vector<int>(len,0);
        while (current != nullptr) {
            QString dateTimeStr = current->date; // 假设格式为 "xxxx-xx-xx-xx:xx"
            QString timeStr = dateTimeStr.mid(11, 5); // 提取时间部分 "xx:xx"
            int hour = timeStr.left(2).toInt(); // 提取小时部分
            for (i = 0; i < (int)(24/hourStep); i ++)
                if(hour >= i && hour < hourStep*(i+1))
                    counters[i]++;
            current = current->next;
        }
        break;
    }
    }
    qDebug()<<labels;
    qDebug()<<counters;
    for(int i =0;i<len;i++){
        QPieSlice *slice = new QPieSlice(labels[i], counters[i]);
        QColor color = QColor::fromRgb(QRandomGenerator::global()->generate());
        slice->setColor(color); series->append(slice);
    }

    chart->addSeries(series);
    chart->setTitle(title);
    chart->legend()->show();
    ui->chartView->setChart(chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);
}

void MainWindow::onChartComboBoxIndexChanged(int index) {
    updateChartView(index);
}

// 2. 实现删除功能
void MainWindow::on_pushButton_16_clicked() {
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->deleteTableView->model());
    QModelIndexList selected = ui->deleteTableView->selectionModel()->selectedRows();

    if (selected.isEmpty()) {
        return; // 没有选中任何行
    }

    // 弹出确认对话框
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认删除", "您确定彻底删除吗？", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        for (const QModelIndex &index : selected) {
            int row = index.row();
            QString id = model->item(row, 0)->text();

            // 从链表中删除对应节点
            DataItem *current = headItem;
            DataItem *prev = nullptr;
            while (current != nullptr && current->ID != id) {
                prev = current;
                current = current->next;
            }
            if (current != nullptr) {
                if (prev != nullptr) {
                    prev->next = current->next;
                } else {
                    headItem = current->next;
                }
                delete current;  // 释放内存
            }

            // 从 QTableView 中删除选中的行
            model->removeRow(row);
        }

        // 保存更改到文件
        QFile file(DATA_PATH("data.txt"));
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Write Error"), tr("Cannot open file:\n%1").arg(file.errorString()));
            return;
        }
        QTextStream out(&file);
        DataItem *current = headItem;
        while (current != nullptr) {
            out << current->ID << " " << current->name << " " << current->age << " " << current->sex << " "
                << current->whathappend << " " << current->date << " " << current->ifaccept << "\n";
            current = current->next;
        }
        file.close();
    }
}


void MainWindow::on_pushButton_17_clicked() {
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->deleteTableView->model());
    QModelIndexList selected = ui->deleteTableView->selectionModel()->selectedRows();

    if (selected.isEmpty()) {
        return; // 没有选中任何行
    }

    // 弹出确认更改对话框
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认更改", "您确定更改吗？", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        for (const QModelIndex &index : selected) {
            int row = index.row();
            // 获取用户输入的新数据行
            QString line = QInputDialog::getText(this, "输入新的数据行", "Enter new data line (ID Name Age Sex WhatHappened Date IfAccept):");
            if (line.isEmpty()) {
                continue; // 用户取消输入或输入为空，跳过当前行
            }

            QStringList fields = line.split(" ", Qt::SkipEmptyParts);
            if (fields.size() != 7) {
                QMessageBox::warning(this, tr("Input Error"), tr("输入数据不完整或不正确"));
                continue; // 输入数据不完整或不正确，跳过当前行
            }

            // 遍历链表以找到对应的节点
            DataItem *currentItem = headItem;
            bool foundItem = false;
            while (currentItem != nullptr) {
                // 比较链表节点的数据与模型中选中行的数据是否一致
                if (currentItem->ID == model->item(row, 0)->text() &&
                    currentItem->name == model->item(row, 1)->text() &&
                    currentItem->age == model->item(row, 2)->text() &&
                    currentItem->sex == model->item(row, 3)->text() &&
                    currentItem->whathappend == model->item(row, 4)->text() &&
                    currentItem->date == model->item(row, 5)->text() &&
                    currentItem->ifaccept == model->item(row, 6)->text()) {
                    // 更新链表中的节点
                    currentItem->ID = fields[0];
                    currentItem->name = fields[1];
                    currentItem->age = fields[2];
                    currentItem->sex = fields[3];
                    currentItem->whathappend = fields[4];
                    currentItem->date = fields[5];
                    currentItem->ifaccept = fields[6];
                    foundItem = true;
                    break;
                }
                currentItem = currentItem->next;
            }

            // // 遍历队列链表以找到对应的节点
            // DataItem *currentQueueItem = headQueueItem; // 使用新的变量currentQueueItem
            // bool foundQueueItem = false;
            // while (currentQueueItem != nullptr) {
            //     // 比较链表节点的数据与模型中选中行的数据是否一致
            //     if (currentQueueItem->ID == model->item(row, 0)->text() &&
            //         currentQueueItem->name == model->item(row, 1)->text() &&
            //         currentQueueItem->age == model->item(row, 2)->text() &&
            //         currentQueueItem->sex == model->item(row, 3)->text() &&
            //         currentQueueItem->whathappend == model->item(row, 4)->text() &&
            //         currentQueueItem->date == model->item(row, 5)->text() &&
            //         currentQueueItem->ifaccept == model->item(row, 6)->text()) {
            //         // 更新链表中的节点
            //         currentQueueItem->ID = fields[0];
            //         currentQueueItem->name = fields[1];
            //         currentQueueItem->age = fields[2];
            //         currentQueueItem->sex = fields[3];
            //         currentQueueItem->whathappend = fields[4];
            //         currentQueueItem->date = fields[5];
            //         currentQueueItem->ifaccept = fields[6];
            //         foundQueueItem = true;
            //         break;
            //     }
            //     currentQueueItem = currentQueueItem->next;
            // }

            if (!foundItem) {
                QMessageBox::warning(this, tr("Update Error"), tr("未找到匹配的数据项"));
                continue; // 未找到匹配的数据项，跳过当前行
            }

            // 更新模型中的数据
            model->setData(model->index(row, 0), fields[0]);
            model->setData(model->index(row, 1), fields[1]);
            model->setData(model->index(row, 2), fields[2]);
            model->setData(model->index(row, 3), fields[3]);
            model->setData(model->index(row, 4), fields[4]);
            model->setData(model->index(row, 5), fields[5]);
            // 根据ifaccept的值设置对应的字符串
            QString ifAcceptStr = fields[6] == "1" ? "急诊" :
                                      fields[6] == "0" ? "门诊" :
                                      fields[6] == "-1" ? "已接诊" : "";
            model->setData(model->index(row, 6), ifAcceptStr);
        }

        // 保存更改到文件
        saveDataToFile(DATA_PATH("data.txt"));
        loadDataIntodeleteTableView();

        // 刷新显示列表和表格视图
        displayList.clear();
        loadDataIntoQueue();
        loadDataIntoTableView();
    }
}

void MainWindow::refreshTableView() {
    // QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->tableView->model());
    // if (!model) {
    //     model = new QStandardItemModel(this);
    //     ui->tableView->setModel(model);
    // }
    // model->clear();
    // model->setHorizontalHeaderLabels(QStringList() << "ID" << "Name" << "Age" << "Sex" << "What Happened" << "Date" << "If Accept");

    // DataItem *current = headItem;
    // while (current != nullptr) {
    //     QList<QStandardItem *> items;
    //     QStandardItem *idItem = new QStandardItem(current->ID);
    //     idItem->setData(QVariant::fromValue<void*>(current), Qt::UserRole); // 存储链表节点指针
    //     items << idItem
    //           << new QStandardItem(current->name)
    //           << new QStandardItem(current->age)
    //           << new QStandardItem(current->sex)
    //           << new QStandardItem(current->whathappend)
    //           << new QStandardItem(current->date);

    //     QString ifacceptStr = QString(current->ifaccept).replace("\n", " ");
    //     if (ifacceptStr == "1") {
    //         ifacceptStr = "急诊";
    //     } else if (ifacceptStr == "0") {
    //         ifacceptStr = "门诊";
    //     } else if (ifacceptStr == "-1") {
    //         ifacceptStr = "已就诊";
    //     }
    //     items << new QStandardItem(ifacceptStr);

    //     for (QStandardItem *subitem : items) {
    //         subitem->setTextAlignment(Qt::AlignCenter);
    //         subitem->setEditable(true);
    //     }
    //     model->appendRow(items);
    //     current = current->next;
    // }

    // ui->tableView->setModel(model);
    // ui->tableView->setColumnWidth(0, 105);
    // ui->tableView->setColumnWidth(1, 100);
    // ui->tableView->setColumnWidth(2, 50);
    // ui->tableView->setColumnWidth(3, 70);
    // ui->tableView->setColumnWidth(4, 180);
    // ui->tableView->setColumnWidth(5, 130);
    // ui->tableView->setColumnWidth(6, 40);
    // ui->tableView->resizeRowsToContents();
    DataItem item;
    DataItem queueitem;
    loadTotalData(); // 加载数据
    loadDataIntoQueue();
    loadDataIntoTableView();
    countIfAcceptNegativeOne();

}


//zeng
void MainWindow::on_pushButton_15_clicked() {

    QString line = QInputDialog::getText(this, "Input", "Enter all infomation");
    QStringList fields = line.split(" ", Qt::SkipEmptyParts);

    qDebug() << "input OK:"<<line;

    if (fields.size() == 7) {
        DataItem item;

        item.ID = fields[0];
        item.name = fields[1];
        item.age = fields[2];
        item.sex = fields[3];
        item.whathappend = fields[4];
        item.date = fields[5];
        item.ifaccept = fields[6].replace("\n","");
        item.next = nullptr;

        // DataItem queueitem;

        // queueitem.ID = fields[0];
        // queueitem.name = fields[1];
        // queueitem.age = fields[2];
        // queueitem.sex = fields[3];
        // queueitem.whathappend = fields[4];
        // queueitem.date = fields[5];
        // queueitem.ifaccept = fields[6].replace("\n","");
        // queueitem.next = nullptr;


        qDebug() << "Parsed data:" << item.ID << item.name << item.age << item.sex << item.whathappend << item.date << item.ifaccept;
        addDataItem(item); // 添加到链表头部
        qDebug() << "add OK!";
        try {
            saveDataToFile(DATA_PATH("data.txt"));
            qDebug() << "save OK!";
        } catch (const std::exception &e) {
            qDebug() << "捕获到异常:" << e.what();
        } catch (...) {
            qDebug() << "捕获到未知异常";
        }


        try{
            loadDataIntodeleteTableView();
        }catch(...){
            qDebug() << "捕获到未知异常";

        }
        qDebug() << "load OK!";

        // qDebug() << queueitem.ID << queueitem.name << queueitem.age << queueitem.sex << queueitem.whathappend << queueitem.date << queueitem.ifaccept;
        displayList.append(item);

        displayList.clear();
        loadDataIntoQueue();
        loadDataIntoTableView();
    }
}


void MainWindow::on_pushButton_3_clicked() {
    // QString searchText = QInputDialog::getText(this, "Input", "Enter search text:");
    // QStandardItemModel *model = new QStandardItemModel(this);
    // model->setHorizontalHeaderLabels(QStringList() << "ID" << "Name" << "Age" << "Sex" << "What Happened" << "Date" << "If Accept");

    // DataItem *current = headItem;
    // while (current != nullptr) {
    //     if (current->ID.contains(searchText) || current->name.contains(searchText) || current->age.contains(searchText) ||
    //         current->sex.contains(searchText) || current->whathappend.contains(searchText) || current->date.contains(searchText) ||
    //         current->ifaccept.contains(searchText)) {
    //         QList<QStandardItem *> items;
    //         items << new QStandardItem(current->ID)
    //               << new QStandardItem(current->name)
    //               << new QStandardItem(current->age)
    //               << new QStandardItem(current->sex)
    //               << new QStandardItem(current->whathappend)
    //               << new QStandardItem(current->date);

    //         QString ifacceptStr = QString(current->ifaccept).replace("\n", " ");
    //         if (ifacceptStr == "1") {
    //             ifacceptStr = "急诊";
    //         } else if (ifacceptStr == "0") {
    //             ifacceptStr = "门诊";
    //         } else if (ifacceptStr == "-1") {
    //             ifacceptStr = "已就诊";
    //         }
    //         items << new QStandardItem(ifacceptStr);

    //         for (QStandardItem *subitem : items) {
    //             subitem->setTextAlignment(Qt::AlignCenter);
    //             subitem->setEditable(true);
    //         }
    //         model->appendRow(items);
    //     }
    //     current = current->next;
    // }
    // ui->tableView->setModel(model);

}



void MainWindow::setCurrentTime(QLabel* label) {
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString formattedTime = currentDateTime.toString("yyyy年MM月dd日HH:mm");
    label->setText(formattedTime);
}

void MainWindow::on_searchTextBox_textChanged(const QString &text) {
    if (text.isEmpty()) {
        loadDataIntoQueue();
        loadDataIntodeleteTableView();
        return;
    }
    QStandardItemModel *model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels(QStringList() << "ID" << "Name" << "Age" << "Sex" << "What Happened" << "Date" << "If Accept");

    DataItem *current = headItem;
    while (current != nullptr) {
        if (current->ID.contains(text) || current->name.contains(text) || current->age.contains(text) ||
            current->sex.contains(text) || current->whathappend.contains(text) || current->date.contains(text) ||
            current->ifaccept.contains(text)) {
            QList<QStandardItem *> items;
            items << new QStandardItem(current->ID)
                  << new QStandardItem(current->name)
                  << new QStandardItem(current->age)
                  << new QStandardItem(current->sex)
                  << new QStandardItem(current->whathappend)
                  << new QStandardItem(current->date);

            QString ifacceptStr = QString(current->ifaccept).replace("\n", " ");
            if (ifacceptStr == "1") {
                ifacceptStr = "急诊";
            } else if (ifacceptStr == "0") {
                ifacceptStr = "门诊";
            } else if (ifacceptStr == "-1") {
                ifacceptStr = "已就诊";
            }
            items << new QStandardItem(ifacceptStr);

            for (QStandardItem *subitem : items) {
                subitem->setTextAlignment(Qt::AlignCenter);
                subitem->setEditable(false);
            }
            model->appendRow(items);
        }
        current = current->next;
    }
    ui->deleteTableView->setModel(model);
}

void MainWindow::on_pushbtton_9_clicked(){
    // login *lgn = new login(this);
    loginDialog->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    loginDialog->show();
}
void MainWindow::on_minimizeButton_clicked()
{
    this->setWindowState(Qt::WindowMinimized);
}

void MainWindow::on_pushButton_clicked()
{
    // ui->searchTextBox->setText("");
}

void MainWindow::on_attendanceButton_clicked()
{
    MainWindow::close();
    TechUsed *tech = new TechUsed(this);
    tech->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    tech->show();
}

void MainWindow::on_searchButton_clicked()
{
    // 切换到搜索界面
    selectedPushButton(ui->searchButton);
    deselectedPushButton(ui->deleteEmpButton);
    deselectedPushButton(ui->techButton);
    deselectedPushButton(ui->aboutButton);
    deselectedPushButton(ui->updateEmpButton);
    deselectedPushButton(ui->addEmpButton);
    ui->searchStackedWidget->setCurrentIndex(0);
}

void MainWindow::on_paymentButton_clicked()
{
    MainWindow::close();
    About *abt = new About(this);
    abt->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    abt->show();
}

void MainWindow::on_attendance1_2_clicked()
{
    // 空壳函数，用于处理事件
}

void MainWindow::on_addEmpButton_clicked()
{
    selectedPushButton(ui->addEmpButton);
    deselectedPushButton(ui->deleteEmpButton);
    deselectedPushButton(ui->techButton);
    deselectedPushButton(ui->aboutButton);
    deselectedPushButton(ui->updateEmpButton);
    deselectedPushButton(ui->searchButton);
    ui->searchStackedWidget->setCurrentIndex(5);
}

void MainWindow::on_updateEmpButton_clicked()
{
    selectedPushButton(ui->updateEmpButton);
    deselectedPushButton(ui->deleteEmpButton);
    deselectedPushButton(ui->techButton);
    deselectedPushButton(ui->aboutButton);
    deselectedPushButton(ui->searchButton);
    deselectedPushButton(ui->addEmpButton);
    updateChartView(ui->chartComboBox->currentIndex());
    ui->searchStackedWidget->setCurrentIndex(3);
}


void MainWindow::on_deleteEmpButton_clicked()
{
    selectedPushButton(ui->deleteEmpButton);
    deselectedPushButton(ui->updateEmpButton);
    deselectedPushButton(ui->techButton);
    deselectedPushButton(ui->aboutButton);
    deselectedPushButton(ui->searchButton);
    deselectedPushButton(ui->addEmpButton);
    ui->searchStackedWidget->setCurrentIndex(4);



}

TreeNode* MainWindow::createDepartmentTree() {
    TreeNode* root = new TreeNode{"校医室"};

    TreeNode* fluDept = new TreeNode{"感冒发热科"};
    fluDept->children.append(new TreeNode{"诊室1"});
    fluDept->children.append(new TreeNode{"诊室2"});

    TreeNode* internalDept = new TreeNode{"内科"};
    internalDept->children.append(new TreeNode{"诊室3"});
    internalDept->children.append(new TreeNode{"诊室4"});

    TreeNode* surgeryDept = new TreeNode{"外科"};
    surgeryDept->children.append(new TreeNode{"诊室5"});
    surgeryDept->children.append(new TreeNode{"诊室6"});

    TreeNode* emergencyDept = new TreeNode{"急诊"};
    emergencyDept->children.append(new TreeNode{"诊室7"});
    emergencyDept->children.append(new TreeNode{"诊室8"});

    TreeNode* pharmacyDept = new TreeNode{"药房"};
    TreeNode* treatmentDept = new TreeNode{"处置室"};

    root->children.append(fluDept);
    root->children.append(internalDept);
    root->children.append(surgeryDept);
    root->children.append(emergencyDept);
    root->children.append(pharmacyDept);
    root->children.append(treatmentDept);

    return root;
}

void MainWindow::populateTreeView(QTreeView* treeView, TreeNode* root) {
    QStandardItemModel* model = new QStandardItemModel;

    // 清除所有列
    model->removeColumns(0, model->columnCount());

    // 设置根节点
    QStandardItem* rootItem = new QStandardItem(root->name);
    model->appendRow(rootItem);

    // 递归添加子节点
    std::function<void(TreeNode*, QStandardItem*)> addChildren = [&](TreeNode* node, QStandardItem* parentItem) {
        for (TreeNode* child : node->children) {
            QStandardItem* childItem = new QStandardItem(child->name);
            parentItem->appendRow(childItem);
            addChildren(child, childItem);
        }
    };

    addChildren(root, rootItem);

    // 禁用header
    treeView->header()->hide();

    treeView->setModel(model);
}


void MainWindow::onTreeViewClicked(const QModelIndex& index) {
    QString departmentName = index.data().toString();

    if (dutySchedules.find(departmentName) != dutySchedules.end()) {
        // 显示诊室的排班表
        displayDutySchedule(workTableView, dutySchedules[departmentName]);
    } else {
        // 获取科室节点的所有子节点的排班表
        TreeNode* departmentNode = getNodeByName(root, departmentName);
        if (departmentNode) {
            DutySchedule combinedSchedule = combineDutySchedules(departmentNode);
            displayDutySchedule(workTableView, combinedSchedule);
        }
    }
}
TreeNode* MainWindow::getNodeByName(TreeNode* root, const QString& name) {
    if (root->name == name) {
        return root;
    }

    for (TreeNode* child : root->children) {
        TreeNode* result = getNodeByName(child, name);
        if (result) {
            return result;
        }
    }

    return nullptr;
}

DutySchedule MainWindow::combineDutySchedules(TreeNode* departmentNode) {
    DutySchedule combinedSchedule;

    // 遍历所有子节点
    for (TreeNode* child : departmentNode->children) {
        if (dutySchedules.find(child->name) != dutySchedules.end()) {
            DutySchedule childSchedule = dutySchedules[child->name];

            // 将子节点的排班表加到综合排班表上
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 7; ++j) {
                    combinedSchedule.schedule[i][j] |= childSchedule.schedule[i][j];
                }
            }
        }
    }

    return combinedSchedule;
}

void MainWindow::on_searchTextBox_returnPressed()
{
    qDebug() << "Search";
}

void MainWindow::selectedPushButton(QPushButton *button)
{
    button->setStyleSheet("QPushButton{background:#333333;border: none;border-left:6px solid #1E90FF;margin: 0px;padding: 0px;color:white;text-align: left;padding-left:24px;}");
}

void MainWindow::deselectedPushButton(QPushButton *button)
{
    button->setStyleSheet("QPushButton{ background:#1F1F1F; border: none; margin: 0px; padding: 0px; border-left:6px solid #1F1F1F; color:white; text-align: left;padding-left:24px;} QPushButton:hover{background:#333333;border: none;border-left:6px solid #333333;margin: 0px;padding: 0px;color:white;text-align: left;padding-left:24px;}");
}

DutySchedule MainWindow::createDutySchedule(int personCount, int offset) {
    DutySchedule schedule;
    std::vector<int> persons(personCount);
    for (int i = 0; i < personCount; ++i) {
        persons[i] = 1 << i;
    }

    // 遍历每个时间段和每个星期几
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 7; ++j) {
            int person1 = persons[(i * 7 + j + offset) % personCount];
            int person2 = persons[(i * 7 + j + offset + 1) % personCount];
            schedule.schedule[i][j] = person1 | person2; // 每个时间段两人一起值班
        }
    }

    return schedule;
}



void MainWindow::autoGenerateDutySchedules(std::map<QString, DutySchedule> &schedules, int personCount) {
    int offset = 0;
    schedules["诊室1"] = createDutySchedule(personCount, offset+=2);
    schedules["诊室2"] = createDutySchedule(personCount, offset+=2);
    schedules["诊室3"] = createDutySchedule(personCount, offset+=2);
    schedules["诊室4"] = createDutySchedule(personCount, offset+=2);
    schedules["诊室5"] = createDutySchedule(personCount, offset+=2);
    schedules["诊室6"] = createDutySchedule(personCount, offset+=2);
    schedules["诊室7"] = createDutySchedule(personCount, offset+=2);
    schedules["诊室8"] = createDutySchedule(personCount, offset+=2);
    schedules["药房"] = createDutySchedule(personCount, offset+=2);
    schedules["处置室"] = createDutySchedule(personCount, offset+=2);
}

void MainWindow::displayDutySchedule(QTableView* tableView, const DutySchedule& schedule) {
    QStandardItemModel* model = new QStandardItemModel;

    model->setHorizontalHeaderLabels({"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"});
    model->setVerticalHeaderLabels({"9:00-13:00", "13:00-17:00", "17:00-21:00"});

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 7; ++j) {
            std::bitset<32> duty = schedule.schedule[i][j];
            QStringList people;
            for (int k = 0; k < 32; ++k) {
                if (duty.test(k)) {
                    people << QString("人员%1").arg(k+1);
                }
            }
            QStandardItem* item = new QStandardItem(people.join(", "));
            model->setItem(i, j, item);
        }
    }

    tableView->setModel(model);

    // 获取总高度并计算每行高度
    int totalHeight = tableView->height();
    int rowCount = model->rowCount();
    int rowHeight = totalHeight / rowCount;

    // 设置每行高度
    for (int i = 0; i < rowCount; ++i) {
        tableView->setRowHeight(i, rowHeight);
    }
}

void MainWindow::on_pushButton_6_clicked()
{
    // 空壳函数，用于处理事件
}

void MainWindow::on_techButton_clicked()
{
    TechUsed *tech = new TechUsed(this);
    tech->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    tech->show();
}

void MainWindow::on_aboutButton_clicked()
{
    About *abt = new About(this);
    abt->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    abt->show();
}

