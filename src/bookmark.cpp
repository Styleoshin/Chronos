#include "bookmark.h"

#include <QPushButton>
#include <QMessageBox>
#include <QHeaderView>

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QDebug>

#include <QWebEngineHistory>

#include <QSqlError>
#include <QSqlTableModel>

BookMark::BookMark(QWidget *parent): QDialog(parent)
{
    QPushButton *button_clean_all_bookmark=new QPushButton(tr("Clean all"));
    QPushButton *button_clean_bookmark=new QPushButton(tr("Clean"));
    QSqlTableModel *model=new QSqlTableModel(this);

    QVBoxLayout *layout=new QVBoxLayout;
    QHBoxLayout *layout1=new QHBoxLayout;

    model->setTable("Bookmark");
    model->select();
    m_view->setModel(model);
    m_view->setColumnHidden(0,true);
    layout->addWidget(m_view);
    layout1->addWidget(button_clean_all_bookmark);
    layout1->addWidget(button_clean_bookmark);
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
            QString url=model->data(index.sibling(index.row(),2),Qt::DisplayRole).toString();
            Q_EMIT selectUrlBookmark(url);
        } });

    connect(button_clean_all_bookmark,&QPushButton::pressed,this,[this,model]{QSqlQuery query;
        if(!query.exec("DELETE FROM Bookmark"))
            QMessageBox::critical(this,tr("Error data base"),query.lastError().text());
        model->select();});

    connect(button_clean_bookmark,&QPushButton::pressed,this,[this,model]{removeBookmarkSelected(model);});

    setWindowTitle(tr("Bookmark"));
}

bool BookMark::addBookmark(const QString &url, const QString &description)
{
    QSqlQuery query;

    if(query.exec("SELECT url FROM Bookmark"))
    {
        while(query.next())
            if(query.value(0).toString()==url)
                return false;
    }
    query.prepare("INSERT INTO Bookmark(url,description) VALUES (:url,:description)");
    query.bindValue(":url",url);
    query.bindValue(":description",description);
    if(!query.exec())
        qDebug()<<"BookMark.cpp : fonction add Bookmark, error add to database : "+ query.lastError().text();
    return true ;
}

bool BookMark::pageInBookmark(const QString &url)
{
    QSqlQuery query("SELECT url FROM Bookmark");

    if(query.exec())
    {
        while(query.next())
            if(query.value(0).toString()==url)
                return true;
    }
    else  qDebug()<<"BookMark.cpp : fonction pageInBookmark, error select database : "+ query.lastError().text();
    return false ;
}

QStringList BookMark::showRecentlyBookmark()
{
    QStringList list_bookmark;
    QSqlQuery query;
    if(query.exec("SELECT url FROM Bookmark ORDER BY url DESC"))
        while(query.next() && list_bookmark.size() < 5)
            list_bookmark<<query.value(0).toString();

    else qDebug()<<"BookMark.cpp : fonction pageInBookmark, error select database : "+ query.lastError().text();

    return list_bookmark ;
}

void BookMark::removeBookmarkSelected(QSqlTableModel *model)
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

