#ifndef CLICKABLELINEEDIT_H
#define CLICKABLELINEEDIT_H

#include <QLineEdit>

class ClickableLineEdit : public QLineEdit
{

    Q_OBJECT
public:


    explicit ClickableLineEdit(QWidget *parent=nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // CLICKABLELINEEDIT_H
