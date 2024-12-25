#include "techused.h"
#include "ui_techused.h"
#include "mainwindow.h"

TechUsed::TechUsed(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TechUsed)
{
    ui->setupUi(this);
}

TechUsed::~TechUsed()
{
    delete ui;
}

void TechUsed::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        isMouseDown = true;
        mousePoint = event->globalPosition().toPoint();
    }
}

void TechUsed::mouseReleaseEvent(QMouseEvent *event)
{
    isMouseDown = false;
}

void TechUsed::mouseMoveEvent(QMouseEvent *event)
{
    const QPoint delta = event->globalPosition().toPoint() - mousePoint;
    if(isMouseDown == true){
        move(x() + delta.x(), y() + delta.y());}
    else{
        move(x()+delta.x(), y()+delta.y());}
        mousePoint = event->globalPosition().toPoint();
}

void TechUsed::on_pushButton_clicked()
{
    this->close();
}
