#include "history.h"

#include <QPushButton>
#include <QMessageBox>
#include <QHeaderView>

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QWebEngineHistory>
#include <QWebEngineHistoryItem>
#include <QWebEngineView>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>

History::History(QWidget *parent) : QDialog(parent)
{
    QPushButton *button_clean_all_history=new QPushButton(tr("Clean all"));
    QPushButton *button_clean_history=new QPushButton(tr("Clean"));
    QSqlTableModel *model=new QSqlTableModel(this);

    QVBoxLayout *layout=new QVBoxLayout;
    QHBoxLayout *layout1=new QHBoxLayout;

    model->setTable("History_Browser");
    model->select();
    m_view->setModel(model);
    m_view->setColumnHidden(0,true);
    layout->addWidget(m_view);
    layout1->addWidget(button_clean_all_history);
    layout1->addWidget(button_clean_history);
    layout->addLayout(layout1);
    setLayout(layout);

    /*Style for m_view*/
    m_view->verticalHeader()->setVisible(false);
    m_view->setGridStyle(Qt::NoPen);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setFocusPolicy(Qt::NoFocus);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_view->horizontalHeader()->setStretchLastSection(true);

    connect(m_view,&QTableView::doubleClicked,this,[this,model](QModelIndex const& index){if(index.isValid())
        {
            QString url=model->data(index.sibling(index.row(),3),Qt::DisplayRole).toString();
            Q_EMIT selectUrlHistory(url);
        } });

    connect(button_clean_all_history,&QPushButton::pressed,this,[this,model]{QSqlQuery query;
        if(!query.exec("DELETE FROM History_Browser"))
            QMessageBox::critical(this,tr("Error data base"),query.lastError().text());
        model->select();});

    connect(button_clean_history,&QPushButton::pressed,this,[this,model](){removeHistorySelected(model);});


    setStyleSheet("QWidget {"
                  "background-color: #333333;"
                  "color: #fffff8;}");

    setWindowTitle(tr("History"));
}

/*static function call in mainwindow.cpp when laod a new page*/
void History::addHistory(const QWebEngineHistoryItem &item)
{
    QSqlQuery query;
    query.clear();
    query.prepare("INSERT INTO History_Browser (url,date,time) VALUES (:url,:date,:time)");
    query.bindValue(":url",item.url().toString());
    query.bindValue(":date",item.lastVisited().toString("dd.MM.yyyy"));
    query.bindValue(":time",item.lastVisited().toString("hh:mm:ss"));
    if(!query.exec())
        qDebug()<<"add in bdd history.cpp :"+ query.lastError().text();
}

QStringList History::showRecentlyHistory()
{
    QStringList list_history;
    QSqlQuery query("SELECT url FROM History_Browser ORDER BY id DESC");
    if(query.exec())
        while(query.next() && list_history.size() < 5)
            list_history<<query.value(0).toString();

    else qDebug()<<"History.cpp : showReccentlyHistrory() : select url FROM History_Browser : "+ query.lastError().text();

    return list_history ;
}

QStringList History::getUrlAllHistory()
{
    QStringList list_history;
    QSqlQuery query;
    if(query.exec("SELECT url FROM History_Browser"))
        while(query.next())
            list_history<<query.value(0).toString();

    else qDebug()<<"History.cpp : getUrlAllHistory() : select url FROM History_Browser : "+ query.lastError().text();

    return list_history ;
}

/*fonction when clic a  button clean fioir remove a history selected*/
void History::removeHistorySelected(QSqlTableModel *model)
{
    QModelIndexList list_selection = m_view->selectionModel()->selectedRows();
    QSqlQuery query;

    qSort(list_selection.begin(),list_selection.end(),[](auto lhs,auto rhs){return lhs<rhs;});// sort index selected

    for(int register i=list_selection.size()-1;i>=0;--i)
    {
        model->removeRow(list_selection.at(i).row());// auto delete in table
        model->select();
    }
}

