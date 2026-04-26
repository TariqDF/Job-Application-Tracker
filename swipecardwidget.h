#ifndef SWIPECARDWIDGET_H
#define SWIPECARDWIDGET_H
#include <QFrame>
#include <QMouseEvent>
class swipeCardWidget: public QFrame
{

    Q_OBJECT
public:

    explicit swipeCardWidget(QWidget *parent =nullptr);

signals:
    void dragStarted();
    void dragMoved(QPoint offset);
    void dragReleased(QPoint offset);
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
private:
    bool isDragging=false;
    QPoint dragStartGlobal;
};

#endif // SWIPECARDWIDGET_H
