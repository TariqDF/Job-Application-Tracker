#include "swipecardwidget.h"

swipeCardWidget::swipeCardWidget(QWidget *parent):QFrame(parent){}

void swipeCardWidget::mousePressEvent(QMouseEvent *event){


    if(event->button()== Qt::LeftButton)
    {
        isDragging=true;
        dragStartGlobal= event->globalPosition().toPoint();
        emit dragStarted();
    }
    QFrame::mousePressEvent(event);
}


void swipeCardWidget::mouseMoveEvent(QMouseEvent *event){

    if(isDragging)
    {
        QPoint currentGlobal= event->globalPosition().toPoint();
        QPoint offset=currentGlobal-dragStartGlobal;
        emit dragMoved(offset);
    }

    QFrame::mouseMoveEvent(event);
}


void swipeCardWidget::mouseReleaseEvent(QMouseEvent *event){
    if(isDragging&& event->button()==Qt::LeftButton){
        isDragging=false;
        QPoint currentGlobal= event->globalPosition().toPoint();
        QPoint offset= currentGlobal-dragStartGlobal;
        emit dragReleased(offset);
    }

    QFrame::mouseReleaseEvent(event);
}