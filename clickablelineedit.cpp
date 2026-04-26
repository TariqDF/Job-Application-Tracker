#include "clickablelineedit.h"
#include<QMouseEvent>

ClickableLineEdit::ClickableLineEdit(QWidget *parent):QLineEdit(parent) {




}


void ClickableLineEdit::mousePressEvent(QMouseEvent *event){

    this->clear();
    QLineEdit::mousePressEvent(event);


}