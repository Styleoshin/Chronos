#ifndef BOOKMARK_H
#define BOOKMARK_H

#include <QDialog>
#include <QTableView>

#include <QSqlQuery>
class QSqlTableModel;

class BookMark : public QDialog
{
    Q_OBJECT

public:
    explicit BookMark(QWidget *parent=0);
    //BookMark()=default;
    BookMark(BookMark&)=delete;
    BookMark& operator=(BookMark&)=delete;

    static bool addBookmark(QString const& url,QString const& description);
    static bool pageInBookmark(QString const& url);
    static QStringList showRecentlyBookmark();

signals :
    void selectUrlBookmark(QString const& url);

private : //fonction
    void removeBookmarkSelected(QSqlTableModel *model);

private : //member
    QTableView *m_view=new QTableView(this);
};

#endif // BOOKMARK_H
