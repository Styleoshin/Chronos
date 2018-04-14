#ifndef HISTORY_H
#define HISTORY_H

#include <QDialog>
#include <QTableView>

class QWebEngineHistoryItem;
class QSqlTableModel;

class History : public QDialog
{
    Q_OBJECT
public:
    explicit History(QWidget *parent = 0);
    History(History&);
    History& operator=(History&);

    static void addHistory(QWebEngineHistoryItem const& item);
    static QStringList showRecentlyHistory();
    static QStringList getUrlAllHistory();

private :
    void removeHistorySelected(QSqlTableModel *model);

signals:
    void selectUrlHistory(QString const& url);

private :
    QTableView *m_view=new QTableView(this);

};



#endif // HISTORY_H
