#include "about.h"
#include "ui_about.h"
#include "mainwindow.h"

About::About(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
}

About::~About()
{
    delete ui;
}

void About::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        isMouseDown = true;
        mousePoint =event->globalPosition().toPoint();
    }
}

void About::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button()== Qt::AllButtons)
        isMouseDown = false;
}

void About::mouseMoveEvent(QMouseEvent *event)
{
    const QPoint delta = event->globalPosition().toPoint() - mousePoint;
    if(isMouseDown == true)
    {move(x() + delta.x(), y() + delta.y());}
    else{
        move(x()+delta.x(), y()+delta.y());}
    mousePoint = event->globalPosition().toPoint();
}

void About::on_pushButton_clicked()
{
    this->close();

}
