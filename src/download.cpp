#include "download.h"

#include <QApplication>

#include <QAction>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>

#include <QHBoxLayout>

#include <QProcess>
#include <QClipboard>
#include <QDesktopServices>
#include <QFileIconProvider>
#include <QVariant>

#include <QMouseEvent>
#include <QEvent>

#include <QIcon>

#include <QNetworkReply>
#include <QAbstractNetworkCache>
#include <QUrl>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

Download::Download(QWidget *parent) : QDialog(parent)
{
    m_item_list_download->installEventFilter(this);
    m_item_list_download->setSelectionMode(QAbstractItemView::ExtendedSelection);

    createMenu();
    loadHistoryDownload();

    QPushButton *button_delete_all_file=new QPushButton(QIcon(":icons/download_delete.png"),tr("Delete all"),this);
    QPushButton *button_delete_file=new QPushButton(QIcon(":icons/download_delete.png"),tr("Delete"),this);

    QHBoxLayout *layout_button=new QHBoxLayout;
    layout_button->addWidget(button_delete_all_file);
    layout_button->addWidget(button_delete_file);

    QVBoxLayout *layout=new QVBoxLayout;
    layout->addWidget(m_item_list_download);
    layout->addLayout(layout_button);
    setLayout(layout);

    setWindowTitle(tr("Download"));

    connect(this,&QDialog::rejected,this,[this]{this->hide();});
    connect(this,&QDialog::accepted,this,[this]{this->hide();});
    connect(button_delete_all_file,&QPushButton::pressed,this,&Download::deleteAllDownload);
    connect(button_delete_file,&QPushButton::pressed,this,&Download::deleteDownload);
}

void Download::addDownload(QWebEngineDownloadItem *download_item)
{
    download_item->accept();
    QWidget *widget=new QWidget(m_item_list_download);
    QProgressBar *progress_download=new QProgressBar(this);
    QPushButton *button_download_stop=new QPushButton(QIcon(":icons/file_downloading_stop.png"),"",this);
    QPushButton *button_file_icon=new QPushButton(QIcon(":icons/download_empty.png"),"",this);
    QLabel *name_download=new QLabel(QFileInfo(download_item->path()).fileName(),this); // get name of file
    QLabel *label_progress_download=new QLabel(this);

    button_download_stop->setFlat(true);
    button_file_icon->setFlat(true);

    QVBoxLayout *layout=new QVBoxLayout;
    layout->addWidget(name_download);
    layout->addWidget(progress_download);
    layout->addWidget(label_progress_download);

    QHBoxLayout *layout_all=new QHBoxLayout;
    layout_all->addWidget(button_file_icon);
    layout_all->addLayout(layout);
    layout_all->addWidget(button_download_stop);

    widget->setLayout(layout_all);

    QListWidgetItem *item = new QListWidgetItem;
    m_item_list_download->addItem(item);
    m_item_list_download->setItemWidget(item,widget);
    item->setSizeHint(widget->size());

    QSqlQuery query;
    query.prepare("INSERT INTO History_Download (path,download_link,date) VALUES (:path,:download_link,:date)");
    query.bindValue(":download_link",download_item->url().toString());
    query.bindValue(":path",download_item->path());
    QString date_time_file=QDateTime::currentDateTime().toString("dd.MM.yyyy  hh:mm:ss");
    query.bindValue(":date",date_time_file);
    if(!query.exec())qDebug()<<"Error to download download.cpp : "+query.lastError().text();

    m_list_info_download<<InfoDownload(download_item->url().toString(),download_item->path(),date_time_file);

    QTime time_download;
    time_download.start();

    connect(download_item,&QWebEngineDownloadItem::downloadProgress,this,[this,progress_download,time_download,label_progress_download](quint64 received,quint64 total)
    {
        progress_download->setValue(received*100/total);
        qint64 speed =received * 1000 / time_download.elapsed();
        int time_remaining = (double)(total -received) / speed;
        refreshInfoDownload(label_progress_download,time_remaining,speed);
    });
    //get icon of file
    connect(download_item,&QWebEngineDownloadItem::finished,this,[=]{button_file_icon->setIcon(QFileIconProvider().icon(QFileInfo(download_item->path())));
        delete progress_download;
        delete button_download_stop;
        label_progress_download->setText(date_time_file);});


    connect(button_download_stop,&QPushButton::pressed,this,[button_file_icon,download_item](){button_file_icon->setIcon(QIcon(":icons/file_delete.png"));download_item->cancel();});

    connect(download_item,&QWebEngineDownloadItem::stateChanged,this,&Download::stateDownload);
}

bool Download::eventFilter(QObject *obj, QEvent *event)
{
    QMouseEvent *event_click=static_cast<QMouseEvent*>(event);
    if(event_click->button()==Qt::LeftButton)
        m_item_list_download->setContextMenuPolicy(Qt::ActionsContextMenu);
    return QDialog::eventFilter(obj, event);
}



void Download::createMenu()
{
    QAction *action_open_file= new QAction(tr("Open file"),this);
    QAction *action_copy_link= new QAction(tr("Copy link"),this);
    QAction *action_open_folder= new QAction(tr("Open folder"),this);
    QAction *action_delete_file= new QAction(tr("Delete"),this);

    m_item_list_download->addActions({action_open_file,action_copy_link,action_open_folder,action_delete_file});

    connect(action_open_folder,&QAction::triggered,this,[this]{QString path= m_list_info_download.value(m_item_list_download->currentRow()).m_download_folder;
        QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(QFile(path)).path())); });

    connect(action_copy_link,&QAction::triggered,this,[this]{QClipboard *p_Clipboard = QApplication::clipboard();
        p_Clipboard->setText(m_list_info_download.at(m_item_list_download->currentRow()).m_download_link); });

    connect(action_delete_file,&QAction::triggered,this,[this]{deleteDownload();});

    connect(action_open_file,&QAction::triggered,this,[this]{QProcess *process = new QProcess;
        process->start(m_list_info_download.at(m_item_list_download->currentRow()).m_download_folder) ;});

}

void Download::loadHistoryDownload()
{
    QSqlQuery query("SELECT *FROM History_Download");

    if(query.exec())
    {
        while(query.next())
        {
            QWidget *widget=new QWidget(m_item_list_download);
            m_list_info_download<<InfoDownload(query.value(1).toString(),query.value(2).toString(),query.value(3).toString());

            QPushButton *button_file_icon=new QPushButton(this);
            button_file_icon->setFlat(true);
            if(QFileInfo(query.value(1).toString()).exists())
                button_file_icon->setIcon(QIcon(QFileIconProvider().icon(QFileInfo(query.value(1).toString()))));
            else button_file_icon->setIcon(QIcon(":icons/file_delete.png"));

            QLabel *label_name_download=new QLabel(QFileInfo(query.value(1).toString()).fileName(),this); // get name of file
            QLabel *label_date_file=new QLabel(query.value(3).toString(),this);

            QVBoxLayout *layout=new QVBoxLayout;
            layout->addWidget(label_name_download);
            layout->addWidget(label_date_file);

            QHBoxLayout *layout_all=new QHBoxLayout;
            layout_all->addWidget(button_file_icon);
            layout_all->addLayout(layout);

            widget->setLayout(layout_all);

            QListWidgetItem *item = new QListWidgetItem;
            m_item_list_download->addItem(item);
            m_item_list_download->setItemWidget(item,widget);
            item->setSizeHint(widget->size());
        }
    }
    else qDebug()<<"Error to open History_Download download .cpp : "+query.lastError().text();
}


void Download::refreshInfoDownload(QLabel *label_progress_download, int time_remaining, int speed)
{
    QString time_remaining_string=tr("Time remaining : ");
    if(qint32 hour=time_remaining/3600)
    {
        time_remaining_string+=QString::number(hour)+"h";
        time_remaining%=3600;
    }
    if(qint32 minute=time_remaining/60)
        time_remaining_string+=QString::number(minute)+"mn";

    time_remaining_string+=QString::number(time_remaining%60)+"sec";

    if(speed< 1024){
        label_progress_download->setText("Progress : "+QString::number(speed)+" bytes/secs : "+time_remaining_string);
    }
    else if(speed < 1024*1024)
        label_progress_download->setText("Progress : "+QString::number(speed/1024)+" Kb/s : "+time_remaining_string);

    else label_progress_download->setText("Progress : "+QString::number(speed/1024*1024)+" Mb/s : "+time_remaining_string);
}

void Download::stateDownload(QWebEngineDownloadItem::DownloadState const& request)
{
    if(request==QWebEngineDownloadItem::DownloadRequested)
        QMessageBox::warning(this,tr("Error download"),tr("Download has been requested, but has not been accepted yet."));
    else if(request==QWebEngineDownloadItem::DownloadInterrupted)
        QMessageBox::warning(this,tr("Error download"),tr("Download has been interrupted (by the server or because of lost connectivity)"));

}

/*Slot delete download selected*/
void Download::deleteDownload()
{
    QModelIndexList list_selection=m_item_list_download->selectionModel()->selectedRows()  ;
    qSort(list_selection.begin(),list_selection.end(),[](auto lhs,auto rhs){return lhs<rhs;});// sort index select

    for(int register i=list_selection.size()-1;i>=0;--i)
    {
        delete m_item_list_download->item(list_selection.at(i).row());
        m_list_info_download.removeAt(list_selection.at(i).row());
    }

    QSqlQuery query("DELETE FROM History_Download");
    if(!query.exec())
        qDebug()<<"Download.cpp : error to delete data download in function deleteAllDowload(), error : "+query.lastError().text();

    query.prepare("INSERT INTO History_Download (path,download_link) VALUES (:path,:download_link)");
    foreach(InfoDownload info,m_list_info_download)
    {
        query.bindValue(":download_link",info.m_download_link);
        query.bindValue(":path",info.m_download_folder);
        if(!query.exec())
            qDebug()<<"Download.cpp : error to add download fonction deleteDownload() : "+query.lastError().text();
    }
}

void Download::deleteAllDownload()
{
    m_list_info_download.clear();
    m_item_list_download->clear();

    QSqlQuery query("DELETE FROM History_Download");
    if(!query.exec())
        qDebug()<<"Download.cpp : error to delete data download in function deleteAllDowload(), error : "+query.lastError().text();
}
