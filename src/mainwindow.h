#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QMenuBar>

#include <QCompleter>

#include <QSqlDatabase>

class QStringList;
class QWebEngineView;
class Download;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow();
    MainWindow(MainWindow&)=delete;
    MainWindow& operator=(MainWindow)=delete;

private :
    QWidget *createTab(QString url);
    void createMenuOption(QPushButton *button_menu);
    void createMenuTab();
    void createMenuToolBar();
    void connexionDataBase();
    void updateRecentlyHistory();
    void updateRecentlyBookmark();
    void updateCompleterUrlField();

private slots :
    void showHistory();
    void showBookmark();
    void showSetting();
    void newTab();
    void menuNewWidget();
    void closeTab(int index=0);

private :
    QTabWidget *m_tab= new QTabWidget(this);

    QList<QWebEngineView*> m_list_page_view;

    QList<QAction *> m_list_action_history;
    QList<QAction *> m_list_action_bookmark;

    QStringList m_list_history;
    QCompleter *m_completer_url_field=new QCompleter(this);

    QMenu *m_menu_file= menuBar()->addMenu(tr("&File"));
    QMenu *m_menu_tool= menuBar()->addMenu(tr("Tool"));
    QMenu *m_menu_display= menuBar()->addMenu(tr("Display"));
    bool m_menu_show=true;

    QSqlDatabase m_db = QSqlDatabase::addDatabase("QSQLITE");

    Download *m_download;

protected :
    bool eventFilter(QObject *obj, QEvent *event);

};


#endif // MAINWINDOW_H
