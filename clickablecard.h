#ifndef CLICKABLECARD_H
#define CLICKABLECARD_H

#include <QFrame>
#include <QMouseEvent>
class clickableCard: public QFrame
{
    Q_OBJECT

public:

    explicit clickableCard(int cardIndex, QWidget *parent= nullptr);

signals:
    void clicked(int cardIndex);
protected:
    void mousePressEvent(QMouseEvent *event) override;
private:
    int cardIndex;
};

#endif // CLICKABLECARD_H
