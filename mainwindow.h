#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

struct JobCard
{
    QString company;
    QString title;
    QString dateApplied;
    QString location;
    QString status;
    QString joblink;
    QString notes;
};

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    enum class SwipeDirection{
        Left,
        Right,
        Up,
        Down
    };

private slots:
    void on_NewButton_clicked();
    void on_SaveButton_clicked();
    void on_NextButton_clicked();
    void on_PreviousButton_clicked();
    void on_DeleteButton_clicked();
    void on_StatusInput_currentTextChanged(const QString &text);
    void on_SearchButton_clicked();
    void on_SearchClearButton_clicked();
    void on_FilterInput_currentTextChanged(const QString &text);
    void on_SearchInput_textChanged(const QString &text);
    void on_swipeUpButton_clicked();
    void on_swipeDownButton_clicked();
    void on_swipeLeftButton_clicked();
    void on_swipeRightButton_clicked();
    void on_viewAllButton_clicked();
private:
    Ui::MainWindow *ui;
    QVector<JobCard>cards;
    QVector<int> filteredIndices;
    QPoint cardStartPos;
    bool isAnimating=false;
    int filteredPosition =-1;
    int currentIndex=-1;
    void clearInputs();
    void loadCard(int index);
    void saveToFile();
    void loadFromFile();
    bool inputsAreEmpty();
    void saveCurrentCard();
    int getCurrentIndex();
    void updatecardcounter();
    void updatebuttons();
    void updatestatuscolor();
    int findMatchingCard(const QString &searchText);
    void applyFilter(int preferredIndex =-1);
    QString statusFromSwipe(SwipeDirection direction);
    void processSwipe(SwipeDirection direction);
    void triggerSwipe(SwipeDirection direction);
    void animateSwipe(SwipeDirection direction);
    void onCardDragStarted();
    void onCardDragMoved(QPoint offset);
    void onCardDragReleased(QPoint offset);
    void animateCardBack();
    void showAllCardsGrid();
};
#endif // MAINWINDOW_H
