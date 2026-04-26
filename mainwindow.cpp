#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clickablecard.h"
#include <QVector>
#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QComboBox>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QDialog>
#include <QScrollArea>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QRegularExpression>

//class that sets up ui aka constructor... still unsure of this
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    swipeCardWidget* card = qobject_cast<swipeCardWidget*>(ui->CardFrame);
    connect(card,&swipeCardWidget::dragStarted, this, &MainWindow::onCardDragStarted);
    connect(card,&swipeCardWidget::dragMoved, this, &MainWindow::onCardDragMoved);
    connect(card,&swipeCardWidget::dragReleased, this, &MainWindow::onCardDragReleased);

//functions im sure of
    loadFromFile();
    applyFilter();
    ui->SearchInput->setPlaceholderText("search");
    updatecardcounter();
    updatebuttons();
    updatestatuscolor();

}
//destructor
MainWindow::~MainWindow()
{
    delete ui;
}
//simply clears inputs
void MainWindow::clearInputs(){

    ui->CompanyInput->clear();
    ui->DateInput->clear();
    ui->JobLinkInput->clear();
    ui->JobTitleInput->clear();
    ui->LocationInput->clear();
    ui->NotesInput->clear();
    ui->StatusInput->setCurrentIndex(0);
    updatestatuscolor();
}

// loads inputs saved to show card
void MainWindow::loadCard(int index)
{ int statusIndex= ui->StatusInput->findText(cards[index].status);

    ui->CompanyInput->setText(cards[index].company);
    ui->DateInput->setText(cards[index].dateApplied);
    ui->JobLinkInput->setText(cards[index].joblink);
    ui->JobTitleInput->setText(cards[index].title);
    ui->LocationInput->setText(cards[index].location);
    ui->NotesInput->setText(cards[index].notes);
    if(statusIndex!=-1){
    ui->StatusInput->setCurrentIndex(statusIndex);
    }
    updatecardcounter();
    updatebuttons();
    updatestatuscolor();
}
// when new button is pressed, input is saved and gives blank card
void MainWindow::on_NewButton_clicked(){
    if(!inputsAreEmpty()){
        saveCurrentCard();
        ui->FilterInput->setCurrentIndex(0);
        applyFilter();
        clearInputs();
        filteredPosition = filteredIndices.size();
        currentIndex = cards.size();
    }
    else{
    clearInputs();
    currentIndex= -1;
    }
    updatecardcounter();
    updatebuttons();

}
//saves the card
void MainWindow::on_SaveButton_clicked(){

    if(inputsAreEmpty())
        return;

    saveCurrentCard();
    applyFilter();
    filteredPosition=filteredIndices.size()-1;
    currentIndex=filteredPosition;
    loadCard(currentIndex);
    updatecardcounter();
    updatebuttons();

}
// saves changes and goes to the next card
void MainWindow:: on_NextButton_clicked(){


    if(!inputsAreEmpty()){
        saveCurrentCard();}

    if(filteredPosition < filteredIndices.size()-1){
        filteredPosition++;
        currentIndex= filteredIndices[filteredPosition];
        loadCard(currentIndex);
    }
    updatecardcounter();
    updatebuttons();
}
//saves changes and goes to the previous card
void MainWindow::on_PreviousButton_clicked(){

    if(filteredPosition==0)
        return;

    if(!inputsAreEmpty()){
        saveCurrentCard();}

    if(filteredPosition>0){
        filteredPosition--;
        currentIndex=filteredIndices[filteredPosition];
        loadCard(currentIndex);
    }
    updatecardcounter();
    updatebuttons();
}
//deletes card
void MainWindow::on_DeleteButton_clicked(){
    int keepSpot=filteredPosition;
    QMessageBox::StandardButton reply;
    reply= QMessageBox::question(this,"Delete Card","Are you sure you want to delete this card?",QMessageBox::Yes|QMessageBox::No);


    if(reply== QMessageBox::No){return;}

    if(filteredPosition>=filteredIndices.size()-1)
        saveCurrentCard();


    if(filteredIndices.isEmpty()|| filteredPosition<0)
        return;

    cards.removeAt(currentIndex);
    applyFilter();
    if(cards.isEmpty()){
        clearInputs();
        currentIndex=-1;
        filteredPosition=-1;
    }
    else if(currentIndex >= cards.size()){
        if(filteredPosition>=filteredIndices.size()){
            filteredPosition= filteredIndices.size()-1;
        }
        currentIndex=filteredIndices[filteredPosition];
        loadCard(currentIndex);
    }
    else{
        filteredPosition=keepSpot-1;
        currentIndex=filteredIndices[filteredPosition];

        loadCard(currentIndex);
    }
    saveToFile();
    updatecardcounter();
    updatebuttons();
}


void MainWindow::on_FilterInput_currentTextChanged(const QString &text){

    Q_UNUSED(text);
    applyFilter();
}

// changes card to json format
QJsonObject jobToJson(const JobCard &job)
{
    QJsonObject obj;

    obj["company"]= job.company;
    obj["title"]= job.title;
    obj["date"]=job.dateApplied;
    obj["location"]=job.location;
    obj["status"]=job.status;
    obj["jobLink"]=job.joblink;
    obj["notes"]=job.notes;


    return obj;

}

//from json to inputs
JobCard jsonToJob(const QJsonObject &obj)
{
    JobCard job;

    job.company=obj["company"].toString();
    job.title=obj["title"].toString();
    job.dateApplied=obj["date"].toString();
    job.joblink=obj["jobLink"].toString();
    job.location=obj["location"].toString();
    job.status=obj["status"].toString();
    job.notes=obj["notes"].toString();


    return job;
}
//saves to json file
void MainWindow::saveToFile(){
    QJsonArray array;

    for(const JobCard &job:cards){

        array.append(jobToJson(job));
    }

    QJsonDocument doc(array);

    QFile file("jobs.json");
    if(file.open(QIODevice::WriteOnly)){
        file.write(doc.toJson());
        file.close();
    }
}

//loads from json file
void MainWindow::loadFromFile(){
   QFile file("jobs.json");


    if(!file.open(QIODevice::ReadOnly))
       return;

    QByteArray data=file.readAll();
    file.close();

    QJsonDocument doc= QJsonDocument::fromJson(data);
    QJsonArray array= doc.array();

    cards.clear();

    for (const QJsonValue &value :array) {
        if(!value.isObject())
            continue;
        cards.append(jsonToJob(value.toObject()));
    }

    if(!cards.isEmpty()){
        currentIndex=0;
        loadCard(currentIndex);
    }
}


// checks if inputs are empty
bool MainWindow::inputsAreEmpty(){
    return ui->CompanyInput->text().trimmed().isEmpty()&&
           ui->DateInput->text().trimmed().isEmpty()&&
           ui->JobLinkInput->text().trimmed().isEmpty()&&
           ui->JobTitleInput->text().trimmed().isEmpty()&&
           ui->LocationInput->text().trimmed().isEmpty()&&
           !ui->StatusInput->currentText().trimmed().isEmpty()&&
           ui->NotesInput->toPlainText().trimmed().isEmpty();
}
//saves the current card, can be called in many places
void MainWindow::saveCurrentCard(){
    JobCard card;
    card.company=ui->CompanyInput->text();
    card.dateApplied=ui->DateInput->text();
    card.joblink=ui->JobLinkInput->text();
    card.location=ui->LocationInput->text();
    card.notes=ui->NotesInput->toPlainText();
    card.status= ui->StatusInput->currentText();
    card.title=ui->JobTitleInput->text();

    if(currentIndex== -1)
    {
        cards.append(card);
        currentIndex= cards.size() - 1;
    }
    else if(currentIndex>cards.size()-1)
    {
      cards.append(card);
    }
    else{
        cards[currentIndex]=card;
    }
    saveToFile();
}
//returns current index
int MainWindow:: getCurrentIndex(){

    return currentIndex;


}

// keeps track of how many cards are in the array
void MainWindow:: updatecardcounter(){
    int total=filteredIndices.size();

    if(cards.size()==0|| currentIndex==-1){
        ui->cardCounterLabel->setText("0 of 0");
    }
    else if(filteredPosition>=total){

        ui->cardCounterLabel->setText("New card( Total: " + QString::number(total) + " )");

    }
    else{
        ui->cardCounterLabel->setText(QString::number(filteredPosition+1)+ " of "+ QString::number(total));

    }

}
// updates button access
void MainWindow::updatebuttons(){

    bool hasCards= !filteredIndices.isEmpty();

    ui->DeleteButton->setEnabled(hasCards);
    ui->NextButton->setEnabled(hasCards&&filteredPosition<filteredIndices.size()-1);
    ui->PreviousButton->setEnabled(hasCards&&filteredPosition>0);
}

void MainWindow::updatestatuscolor(){

    QString status =ui->StatusInput->currentText();


    if(status=="Apply Later"){
        ui->StatusInput->setStyleSheet("QComboBox {color: white;}");
    }
    else if(status=="Applied"){
        ui->StatusInput->setStyleSheet("QComboBox {color: blue;}");
    }
    else if(status=="Interviewing"){
        ui->StatusInput->setStyleSheet("QComboBox {color: lightblue;}");
    }
    else if(status== "Offer"){
        ui->StatusInput->setStyleSheet("QComboBox {color: green;}");
    }
    else if(status=="Rejected"){
        ui->StatusInput->setStyleSheet("QComboBox {color: red;}");
    }
    else if(status=="Follow Up"){
        ui->StatusInput->setStyleSheet("QComboBox {color: orange;}");
    }
    else if(status=="Archived"){
        ui->StatusInput->setStyleSheet("QComboBox {color: gray;}");
    }
    else if(status=="Ghosted"){
        ui->StatusInput->setStyleSheet("QComboBox {color: darkred;}");
    }
    else {
        ui->StatusInput->setStyleSheet("");
    }
}

void MainWindow:: on_StatusInput_currentTextChanged(const QString &text){
    Q_UNUSED(text)
    saveCurrentCard();
    updatestatuscolor();
}





int MainWindow::findMatchingCard(const QString &searchText){

    Q_UNUSED(searchText);
    applyFilter();

    if(filteredIndices.isEmpty())
        return -1;
    else {
        return 1;
    }


}



void MainWindow::on_SearchButton_clicked(){

    QString searchText=ui->SearchInput->text();
    int matchIndex=findMatchingCard(searchText);

    if(matchIndex==-1){
        QMessageBox::information(this,"Search","No Matching card found");
    }

}



void MainWindow::on_SearchClearButton_clicked(){
    ui->SearchInput->clear();
}



void MainWindow::applyFilter(int preferredIndex){

    filteredIndices.clear();

    QString filter= ui->FilterInput->currentText();
    QString trimmedSearch= ui->SearchInput->text().trimmed();
    bool statusbool,searchbool;



    for(int i=0; i<cards.size(); i++){
        statusbool=(filter=="All"|| cards[i].status==filter);
        searchbool=(trimmedSearch.isEmpty()|| cards[i].company.contains(trimmedSearch, Qt::CaseInsensitive) || cards[i].title.contains(trimmedSearch,Qt::CaseInsensitive));

        if(statusbool&&searchbool){
            filteredIndices.append(i);
        }
    }

    if(filteredIndices.isEmpty()){
        currentIndex=-1;
        filteredPosition=-1;
        clearInputs();
        updatecardcounter();
        updatebuttons();
        return;
    }
    else{
        currentIndex=0;
        filteredPosition=0;



    }

    int targetPosition=-1;

    if(preferredIndex!=-1){
        targetPosition=filteredIndices.indexOf(preferredIndex);
    }

    if(targetPosition==-1){

        if(filteredPosition>=0 && filteredPosition<filteredIndices.size())
            targetPosition=filteredPosition;

    }
    filteredPosition=targetPosition;
    currentIndex= filteredIndices[filteredPosition];

    loadCard(currentIndex);
    updatecardcounter();
    updatebuttons();
}



void MainWindow::on_SearchInput_textChanged(const QString &text){
    Q_UNUSED(text);
    applyFilter();
}






QString MainWindow::statusFromSwipe(SwipeDirection direction){

    switch(direction){
    case SwipeDirection::Right: return "Applied";
    case SwipeDirection::Left: return "Rejected";
    case SwipeDirection::Down: return "Archived";
    case SwipeDirection::Up:   return "Interviewing";
    }
    return "";
}






void MainWindow::processSwipe(SwipeDirection direction){

    if(cards.isEmpty()||currentIndex<0|| currentIndex >=cards.size())
        return;

    saveCurrentCard();

    QString newStatus= statusFromSwipe(direction);
    if(newStatus.isEmpty())
        return;

    int swipeIndex=currentIndex;
    cards[currentIndex].status=newStatus;
    applyFilter(swipeIndex);
}


void MainWindow:: on_swipeUpButton_clicked(){
    cardStartPos=ui->CardFrame->pos();
    triggerSwipe(SwipeDirection::Up);
}

void MainWindow:: on_swipeDownButton_clicked(){
    cardStartPos=ui->CardFrame->pos();
    triggerSwipe(SwipeDirection::Down);
}

void MainWindow:: on_swipeLeftButton_clicked(){
    cardStartPos=ui->CardFrame->pos();
    triggerSwipe(SwipeDirection::Left);
}

void MainWindow:: on_swipeRightButton_clicked(){
    cardStartPos=ui->CardFrame->pos();
    triggerSwipe(SwipeDirection::Right);
}

void MainWindow:: on_viewAllButton_clicked(){
    showAllCardsGrid();
}

void MainWindow::triggerSwipe(SwipeDirection direction){
    if(isAnimating){return;}
    animateSwipe(direction);
}



void MainWindow::animateSwipe(SwipeDirection direction){

    QFrame* cardAni= ui->CardFrame;
    QPoint startPos=cardAni->pos();
    QPoint endPos =startPos;

    int distance=300;


    switch(direction){
    case SwipeDirection::Up:
        endPos.setY(startPos.y()-distance);
        break;
    case SwipeDirection::Down:
        endPos.setY(startPos.y()+distance);
        break;
    case SwipeDirection::Right:
        endPos.setX(startPos.x()+distance);
        break;
    case SwipeDirection::Left:
        endPos.setX(startPos.x()-distance);
        break;
    }

    QPropertyAnimation* animation = new QPropertyAnimation(cardAni,"pos");
    animation->setDuration(200);
    animation->setStartValue(startPos);
    animation->setEndValue(endPos);


    connect(animation,&QPropertyAnimation::finished,this,[=](){
    processSwipe(direction);
    cardAni->move(cardStartPos);
    animation->deleteLater();
    });
    animation->start();
}

void MainWindow::onCardDragStarted(){
    if(isAnimating){return;}

    cardStartPos=ui->CardFrame->pos();
}


void MainWindow::onCardDragMoved(QPoint offset){
    if(isAnimating){return;}

    ui->CardFrame->move(cardStartPos+offset);
}


void MainWindow::onCardDragReleased(QPoint offset){
    if(isAnimating){return;}


    const int SwipeThreshold =100;

    int dx= offset.x();
    int dy= offset.y();


    if(qAbs(dx)>qAbs(dy)){


        if(dx>=SwipeThreshold){
            triggerSwipe(SwipeDirection::Right);
            return;
        }
        else if(dx<= -SwipeThreshold){

            triggerSwipe(SwipeDirection::Left);
            return;
        }
    }
    else{
        if(dy>=SwipeThreshold){
            triggerSwipe(SwipeDirection::Down);
            return;
        }
        else if(dy<= -SwipeThreshold){
            triggerSwipe(SwipeDirection::Up);
            return;
        }
    }

    animateCardBack();
}



void MainWindow::animateCardBack(){
    QFrame* card= ui->CardFrame;

    QPropertyAnimation* animation =new QPropertyAnimation(card,"pos");
    animation->setDuration(200);
    animation->setStartValue(card->pos());
    animation->setEndValue(cardStartPos);

    connect(animation,&QPropertyAnimation::finished,this,[=](){
        animation->deleteLater();
    });
    animation->start();
}




void MainWindow::showAllCardsGrid(){

    QDialog dialog(this);
    dialog.setWindowTitle("All Job Cards");

    QVBoxLayout *mainLayout= new QVBoxLayout(&dialog);

    QScrollArea *scrollArea= new QScrollArea(&dialog);
    scrollArea->setWidgetResizable(true);

    QWidget *container= new QWidget(scrollArea);
    QGridLayout *gridLayout= new QGridLayout(container);

    int column =3;
    int row = 0;
    int col= 0;

    for(int i=0;i<cards.size(); i++){
        const JobCard &card =cards[i];

        clickableCard *miniCard = new clickableCard(i,container);


        miniCard->setFrameShape(QFrame::Box);
        miniCard->setFrameShadow(QFrame::Raised);
        miniCard->setMinimumSize(180,120);
        connect(miniCard,&clickableCard::clicked,this,[&](int index){
            currentIndex=index;
            applyFilter(index);
            dialog.accept();
        });
        QVBoxLayout *cardLayout= new QVBoxLayout(miniCard);

        QLabel *companyLabel= new QLabel(card.company);
        QLabel *titleLabel= new QLabel(card.title);
        QLabel *statusLabel= new QLabel (card.status);

        companyLabel->setStyleSheet("font-weight: bold");

        cardLayout->addWidget(companyLabel);
        cardLayout->addWidget(titleLabel);
        cardLayout->addWidget(statusLabel);

        gridLayout->addWidget(miniCard,row,col);

        col++;

        if(col>=column){
            col=0;
            row++;
        }
    }

    scrollArea->setWidget(container);
    mainLayout->addWidget(scrollArea);

    dialog.resize(700,500);
    dialog.exec();
}
