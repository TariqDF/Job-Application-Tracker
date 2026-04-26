#include "clickablecard.h"

clickableCard:: clickableCard(int cardIndex, QWidget *parent): QFrame(parent), cardIndex(cardIndex) {
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_Hover, true);

    setStyleSheet("clickableCard{"
                  "border:1px solid gray;"
                  "border-radius: 8px;"
                  "background-coulor:white;"
                  "}"
                  "clickableCard:hover{"
                  "border: 2px solid #4A90E2;"
                  "background-color: #F0F7FF;"
                  "}");
}




void clickableCard:: mousePressEvent(QMouseEvent *event){

    if (event->button()== Qt::LeftButton){
        emit clicked(cardIndex);
        event->accept();
        return;
    }


    QFrame::mousePressEvent(event);

}