#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <QDialog>
#include <QWebEngineDownloadItem>
#include <QListWidget>

class QLabel;

class Download : public QDialog
{
    struct InfoDownload{
        InfoDownload(QString s1=0,QString s2=0,QString s3=0) : m_download_folder(s1), m_download_link(s2),m_date(s3){}
        QString m_download_link;
        QString m_download_folder;
        QString m_date;
    };

    Q_OBJECT
public:
    explicit Download(QWidget *parent = 0);
    Download(Download&)=delete;
    Download& operator=(Download&)=delete;

    void addDownload(QWebEngineDownloadItem *download_item);

private : //fonction
    void stock_history_download();
    void load_show_history_download();
    void createMenu();
    void loadHistoryDownload();
    void refreshInfoDownload(QLabel *label_progress_download, int time_remaining, int speed);

private slots :
    void stateDownload(const QWebEngineDownloadItem::DownloadState &request);
    void deleteAllDownload();
    void deleteDownload();

private :
    QListWidget *m_item_list_download=new QListWidget(this);
    QList<InfoDownload>m_list_info_download;

protected :
    bool eventFilter(QObject *obj, QEvent *event);

};

#endif // DOWNLOAD_H
