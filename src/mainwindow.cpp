#include "mainwindow.h"
#include "history.h"
#include "setting.h"
#include "download.h"
#include "bookmark.h"

#include <QAction>
#include <QLineEdit>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QInputDialog>
#include <QStatusBar>
#include <QProcess>
#include <QKeyEvent>
#include <QFileDialog>
#include <QStringListModel>
#include <QApplication>


#include <algorithm>
#include <QStringList>

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QWebEngineView>
#include <QWebEngineHistory>
#include <QWebEngineProfile>
#include <QWebEngineDownloadItem>

#include <QSqlError>

MainWindow::MainWindow()
{
    QPushButton *button_new_tab=new QPushButton(QIcon(":icons/menu_new_tab.png"),"",this);

    connexionDataBase();
    createMenuTab();
    createMenuToolBar();

    m_completer_url_field->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer_url_field->setFilterMode(Qt::MatchContains);

    m_download= new Download(this);

    updateRecentlyHistory();
    updateRecentlyBookmark();
    updateCompleterUrlField();

    m_tab->setTabsClosable(true);
    m_tab->installEventFilter(this);

    m_tab->setStyleSheet("QTabBar::tab {width: 200px; }");
    m_tab->setTabShape(QTabWidget::Triangular);
    m_tab->setMovable(true);

    setStyleSheet("QPushButton::pressed{border : none;}");

    m_tab->addTab(createTab("http://google.com"),"Google");

    m_tab->setCornerWidget(button_new_tab,Qt::BottomLeftCorner);
    button_new_tab->setFlat(true);

    setCentralWidget(m_tab);

    connect(button_new_tab,&QPushButton::pressed,this,&MainWindow::newTab);
    connect(m_tab,&QTabWidget::tabCloseRequested,this,&MainWindow::closeTab);

    connect(m_completer_url_field,static_cast<void(QCompleter::*)(const QString &)>(&QCompleter::activated),this,[this](const QString &text){QWebEngineView *view_ref=m_tab->currentWidget()->findChild<QWebEngineView*>();
        view_ref->load(QUrl(text)); });
}

QWidget* MainWindow::createTab(QString url)
{
    QWidget *widget=new QWidget(this);
    QWebEngineView *page_view=new QWebEngineView(this);
    m_list_page_view.push_back(page_view);
    page_view->page()->setAudioMuted(true);// for default this fonction return false, it must be true for use in fonction

    QPushButton *button_back_page=new QPushButton(QIcon(":icons/previous_page.png"),"",this);
    QPushButton *button_next_page=new QPushButton(QIcon(":icons/next_page.png"),"",this);
    QPushButton *button_reload_page=new QPushButton(QIcon(":icons/reload_page.png"),"",this);
    QPushButton *button_option_page=new QPushButton(QIcon(":icons/menu_new_option.png"),"",this);
    QPushButton *button_zoom_page=new QPushButton(QIcon(":icons/zoom.png"),"",this);
    QPushButton *button_zoom_out_page=new QPushButton(QIcon(":icons/zoom_out.png"),"",this);

    button_reload_page->setFlat(true);
    button_next_page->setFlat(true);
    button_back_page->setFlat(true);
    button_option_page->setFlat(true);
    button_zoom_page->setFlat(true);
    button_zoom_out_page->setFlat(true);

    QLineEdit *url_field= new QLineEdit(this);
    QAction *action_bookmark=new QAction(QIcon("icons/bookmark_no.png"),"",this);
    url_field->addAction(action_bookmark,QLineEdit::TrailingPosition);
    url_field->setCompleter(m_completer_url_field);
    connect(action_bookmark,&QAction::triggered,this,[this,page_view,action_bookmark]{if(BookMark::addBookmark(page_view->url().toString(),QInputDialog::getText(this,tr("Bookmark"),tr("Description : ")))  || !page_view->url().toString().isEmpty())
            action_bookmark->setIcon(QIcon(":icons/bookmark_yes.png"));
        else QMessageBox::warning(this,tr("Error"),tr("Can't add")); });
    // QLineEdit *search_google= new QLineEdit(this);

    QLineEdit *word_search= new QLineEdit(this);
    word_search->setPlaceholderText(tr("Search word"));
    word_search->addAction(QIcon(":icons/search_word.png"),QLineEdit::LeadingPosition);
    word_search->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    word_search->setStyleSheet("background: #f3f3f3;  background-repeat: no-repeat;background-position: left; color: #252424;font-family: SegoeUI;font-size: 12px;padding: 2 2 2 20;}");

    QProgressBar *page_load_progress =new QProgressBar(this);
    QStatusBar *statubar=new QStatusBar(this);

    QHBoxLayout *layout_navigator_tool=new QHBoxLayout;
    layout_navigator_tool->addWidget(button_back_page);
    layout_navigator_tool->addWidget(button_next_page);
    layout_navigator_tool->addWidget(button_reload_page);
    layout_navigator_tool->addWidget(url_field);
    //layout_navigator_tool->addWidget(search_google);
    layout_navigator_tool->addWidget(word_search);
    layout_navigator_tool->addWidget(button_zoom_page);
    layout_navigator_tool->addWidget(button_zoom_out_page);
    layout_navigator_tool->addWidget(button_option_page);

    QVBoxLayout *layout_navigator=new QVBoxLayout;
    statubar->addWidget(page_load_progress);
    layout_navigator->addLayout(layout_navigator_tool);
    layout_navigator->addWidget(page_view);
    layout_navigator->addWidget(statubar);
    widget->setLayout(layout_navigator);

    createMenuOption(button_option_page);//create menu button
    Setting::loadSettingPage(page_view);// Load option page

    page_view->load(QUrl(url));

    /*Signal/Slot for QWenEngineView*/
    connect(page_view->page()->profile(),&QWebEngineProfile::downloadRequested,this,[this](QWebEngineDownloadItem *download){ m_download->addDownload(download);
                                                                                                                              m_download->show();});

    connect(page_view,&QWebEngineView::iconChanged,this,[this,widget](QIcon const& icon){m_tab->setTabIcon(m_tab->indexOf(widget),icon);});

    connect(page_view,&QWebEngineView::titleChanged,this,[this](QString const title){m_tab->setTabText(m_tab->currentIndex(),title);});

    connect(page_view,&QWebEngineView::urlChanged,this,[this,url_field](QUrl const title){url_field->setText(title.toString());});

    connect(page_view,&QWebEngineView::loadStarted,this,[page_load_progress,statubar]{page_load_progress->show();
        statubar->show();});

    connect(page_view,&QWebEngineView::loadProgress,this,[page_load_progress,action_bookmark,page_view](int progress){if(BookMark::pageInBookmark(page_view->url().toString()))
            action_bookmark->setIcon(QIcon(":icons/bookmark_yes.png"));
        else action_bookmark->setIcon(QIcon(":icons/bookmark_no.png"));
        page_load_progress->setValue(progress);});

    connect(page_view,&QWebEngineView::loadFinished,this,[this,page_view,page_load_progress,statubar]{page_load_progress->hide();
        statubar->hide();
        History::addHistory(page_view->history()->currentItem());
        m_list_history<<page_view->url().toString();
        updateRecentlyHistory();
        updateRecentlyBookmark();
        updateCompleterUrlField();});

    /*Signal/Slot for QLineEdit*/
    //Find text in page view
    connect(word_search,&QLineEdit::textEdited,[page_view](QString const& text){page_view->page()->findText(text,QWebEnginePage::FindBackward); });

    // connect(search_google,&QLineEdit::returnPressed,[page_view](QString const& text){page_view->load(QUrl("http://google.com"+text));});

    connect(url_field,&QLineEdit::returnPressed,this,[page_view,url_field]{if(url_field->text().left(7)!="http://")
            url_field->setText("http://"+url_field->text());
        page_view->load(QUrl(url_field->text()));});


    /*Signal/Slot for QPushButton*/

    connect(button_back_page,&QPushButton::pressed,this,[page_view]{page_view->back();});

    connect(button_next_page,&QPushButton::pressed,this,[page_view]{page_view->forward();});

    connect(button_reload_page,&QPushButton::pressed,this,[page_view]{page_view->reload();});

    connect(button_zoom_page,&QPushButton::pressed,this,[page_view]{page_view->setZoomFactor(page_view->zoomFactor()+0.1);});

    connect(button_zoom_out_page,&QPushButton::pressed,this,[page_view]{page_view->setZoomFactor(page_view->zoomFactor()-0.1);});


    /*Stylesheet*/
    button_option_page->setStyleSheet("QPushButton::menu-indicator {image: url(menu_indicator.png);subcontrol-origin: padding;subcontrol-position: bottom right;}");

    return widget;
}


/*This function creat a button menu option*/
void MainWindow::createMenuOption(QPushButton *button_menu)
{
    QMenu *menu=new QMenu(this);
    QMenu *menu_history=new QMenu(tr("History"),this);
    QMenu *menu_bookmark=new QMenu(tr("Bookmark"),this);

    QAction *action_history=new QAction(tr("Show all history"),this);
    QAction *action_bookmark=new QAction(tr("Show all Bookmark"),this);

    menu_history->setIcon(QIcon(":icons/menu_new_history.png"));
    menu_bookmark->setIcon(QIcon(":icons/bookmark_yes.png"));

    menu->addAction(QIcon(":icons/menu_new_tab.png"),tr("New Tab"),this,&MainWindow::newTab);
    menu->addAction(QIcon(":icons/menu_new_window.png"),tr("New window"),this,&MainWindow::menuNewWidget);
    menu->addMenu(menu_history);
    menu->addMenu(menu_bookmark);
    menu->addAction(QIcon(":icons/menu_new_download.png"),tr("Download"),this,[this](){m_download->show();});
    menu->addAction(QIcon(":icons/menu_new_option.png"),tr("Option"),this,&MainWindow::showSetting);

    connect(menu_history,&QMenu::aboutToShow,this,[this,menu_history,action_history]{menu_history->addAction(action_history);
        menu_history->addSeparator();
        menu_history->addActions(m_list_action_history);});

    connect(menu_bookmark,&QMenu::aboutToShow,this,[this,menu_bookmark,action_bookmark]{menu_bookmark->addAction(action_bookmark);
        menu_bookmark->addSeparator();
        menu_bookmark->addActions(m_list_action_bookmark);});


    connect(action_history,&QAction::triggered,this,&MainWindow::showHistory);
    connect(action_bookmark,&QAction::triggered,this,&MainWindow::showBookmark);

    button_menu->setMenu(menu);
}

/*Create menu for QTabWidget*/
void MainWindow::createMenuTab()
{
    QAction *action_bar= new QAction(tr("New tab"),this);
    QAction *action_window=new QAction(tr("New window"),this);
    QAction *action_sound= new QAction(tr("Disable sound"),this);
    QAction *action_close_tab= new QAction(tr("Close tab"),this);
    QAction *action_close_all_tab= new QAction(tr("Close all tab"),this);

    m_tab->addActions({action_bar,action_window,action_sound,action_close_tab,action_close_all_tab});

    connect(action_bar,&QAction::triggered,this,&MainWindow::newTab);
    connect(action_window,&QAction::triggered,this,&MainWindow::menuNewWidget);
    connect(action_close_tab,&QAction::triggered,this,&MainWindow::closeTab);
    connect(action_close_all_tab,&QAction::triggered,this,[this]{for(int register index=0;index<m_tab->count();++index)
            if(m_tab->currentWidget()!= m_tab->widget(index))
            {
                m_tab->removeTab(index);
                index=-1;
            }});

    connect(action_sound,&QAction::triggered,this,[this,action_sound]{QWebEngineView *view_ref=m_tab->currentWidget()->findChild<QWebEngineView*>();
        if(view_ref->page()->isAudioMuted())
        {
            view_ref->page()->setAudioMuted(false);
            action_sound->setText(tr("Enable sound"));
        }
        else
        {
            view_ref->page()->setAudioMuted(true);
            action_sound->setText(tr("Disable sound"));
        }
    });
}

/*Menu toolbar*/
void MainWindow::createMenuToolBar()
{
    /* action for menu File*/
    QAction *action_new_tab=new QAction(tr("New m_tab"),this);
    QAction *action_new_window=new QAction(tr("New window"),this);
    QAction *action_close_tab=new QAction(tr("Close current tab"),this);
    QAction *action_print_pdf=new QAction(tr("Print to pdf"),this);
    QAction *action_quit=new QAction(tr("Quit"),this);

    /*action for menu Option*/
    QAction *action_history=new QAction(tr("History"),this);
    QAction *action_bookmark=new QAction(tr("Bookmark"),this);
    QAction *action_option=new QAction(tr("Setting"),this);

    /*action for menu Display*/
    QAction *action_fullscreen=new QAction(tr("Fullscreen"),this);
    QAction *action_zoom_page=new QAction(tr("Zoom"),this);
    QAction *action_zoom_out_page=new QAction(tr("Zoom out"),this);
    QAction *action_reset_zoom_page=new QAction(tr("Reset zoom"),this);

    m_menu_file->addActions({action_new_tab,action_new_window,action_print_pdf});
    m_menu_file->addSeparator();
    m_menu_file->addAction(action_close_tab);
    m_menu_file->addSeparator();
    m_menu_file->addAction(action_quit);

    m_menu_tool->addActions({action_history,action_bookmark});
    m_menu_tool->addSeparator();
    m_menu_tool->addAction(action_option);

    m_menu_display->addAction(action_fullscreen);
    m_menu_display->addSeparator();
    m_menu_display->addActions({action_zoom_page,action_zoom_out_page,action_reset_zoom_page});

    /*signal/slot for menu File*/
    connect(action_new_tab,&QAction::triggered,this,&MainWindow::newTab);
    connect(action_new_window,&QAction::triggered,this,&MainWindow::menuNewWidget);
    connect(action_history,&QAction::triggered,this,&MainWindow::showHistory);
    connect(action_bookmark,&QAction::triggered,this,&MainWindow::showBookmark);
    connect(action_close_tab,&QAction::triggered,this,&MainWindow::closeTab);
    connect(action_quit,&QAction::triggered,qApp, QCoreApplication::quit);


    connect(action_print_pdf,&QAction::triggered,this,[this]{QWebEngineView *view_ref=m_tab->currentWidget()->findChild<QWebEngineView*>();
        QString target = QFileDialog::getSaveFileName(this,tr("Save file"), QString(),tr("File *.pdf"));
        view_ref->page()->printToPdf(target);
    });


    /* signal/slot for menu Display*/
    connect(action_fullscreen,&QAction::triggered,this,[this]{ showFullScreen();
        m_menu_file->menuAction()->setVisible(false);
        m_menu_tool->menuAction()->setVisible(false);
        m_menu_display->menuAction()->setVisible(false);
        m_menu_show=true;});

    connect(action_option,&QAction::triggered,this,&MainWindow::showSetting);

    connect(action_zoom_page,&QAction::triggered,this,[this]{QWebEngineView *view_ref=m_tab->currentWidget()->findChild<QWebEngineView*>();
        view_ref->setZoomFactor(view_ref->zoomFactor()+0.1);});

    connect(action_zoom_out_page,&QAction::triggered,this,[this]{QWebEngineView *view_ref=m_tab->currentWidget()->findChild<QWebEngineView*>();
        view_ref->setZoomFactor(view_ref->zoomFactor()-0.1);});

    connect(action_reset_zoom_page,&QAction::triggered,this,[this]{QWebEngineView *view_ref=m_tab->currentWidget()->findChild<QWebEngineView*>();
        view_ref->setZoomFactor(1);});


}

void MainWindow::connexionDataBase()
{
    m_db.setDatabaseName("Chronos.db");

    if(!m_db.open())
    {
        QMessageBox::critical(this,tr("Error data base"),m_db.lastError().text());
    }

    QSqlQuery query;

    if(!query.exec("CREATE TABLE IF NOT EXISTS History_Browser(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,Date VARCHAR(10),Time VARCHAR(7),Url VARCHAR(255))"))
        qDebug()<<"Error to create table History_browser mainwindow.cpp :"+ query.lastError().text();
    if(!query.exec("CREATE TABLE IF NOT EXISTS History_Download(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,path VARCHAR(255),download_link VARCHAR(255),date VARCHAR(20))"))
        qDebug()<<"Error to create table History_Download mainwindow.cpp :"+ query.lastError().text();
    if(!query.exec("CREATE TABLE IF NOT EXISTS Bookmark(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,Description text, Url VARCHAR(255))"))
        qDebug()<<"Error to create table Bookmark mainwindow.cpp :"+ query.lastError().text();
}

void MainWindow::updateRecentlyHistory()
{
    /*   m_list_action_history.clear();
    QStringList &list_string_action_history=History::showRecentlyHistory();
    for(auto const i : list_string_action_history )
      {
       QAction *action=new QAction(i,this);
       m_list_action_history.append(std::move(action));
       connect(action,&QAction::triggered,this,[this,i]{m_tab->addTab(this->createTab(i),"");
                                                        m_tab->setCurrentIndex(m_tab->count()-1);});
    }*/
}

void MainWindow::updateRecentlyBookmark()
{
    m_list_action_bookmark.clear();
    QStringList list_string_action_bookmark=BookMark::showRecentlyBookmark();
    for(auto const i : list_string_action_bookmark)
    {
        QAction *action=new QAction(i,this);
        m_list_action_bookmark.append(std::move(action));
        connect(action,&QAction::triggered,this,[this,i]{m_tab->addTab(this->createTab(i),"");
            m_tab->setCurrentIndex(m_tab->count()-1); });
    }
}

void MainWindow::updateCompleterUrlField()
{
    QStringListModel *model;
    model = (QStringListModel*)(m_completer_url_field->model());
    if(model==NULL)
        model = new QStringListModel();

    model->setStringList(m_list_history);
    m_completer_url_field->setModel(model);
}

/*Close currently m_tab*/
void MainWindow::closeTab(int index)
{
    if(m_tab->widget(index)!=nullptr)
        m_tab->removeTab(index);
    else m_tab->removeTab(m_tab->currentIndex());
    if(m_tab->count()==0)
        qApp->quit();
}

/*this function create a new m_tab*/
void MainWindow::newTab()
{
    m_tab->addTab(createTab("http://www.google.com"),"Google");;
    m_tab->setCurrentIndex(m_tab->count()-1);
}

/*this slot create a new widget*/
void MainWindow::menuNewWidget()
{
    QProcess *new_widget=new QProcess;
    new_widget->start("Chronos.exe");
}

/*Show history*/
void MainWindow::showHistory()
{
    History history(this);
    connect(&history,&History::selectUrlHistory,this,[this](QString const& url){m_tab->addTab(createTab(url),"");
                                                                                m_tab->setCurrentIndex(m_tab->count()-1); });
    history.exec();
}

void MainWindow::showBookmark()
{
    BookMark bookmark(this);
    connect(&bookmark,&BookMark::selectUrlBookmark,this,[this](QString const& url){m_tab->addTab(createTab(url),"");
                                                                                   m_tab->setCurrentIndex(m_tab->count()-1); });
    bookmark.exec();
}

void MainWindow::showSetting()
{
    Setting setting(m_list_page_view,this);
    setting.exec();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    /*When click left mouse for menu m_tab*/
    QTabWidget *tab_cast=qobject_cast<QTabWidget*>(obj);
    if(m_tab->currentWidget()==tab_cast->currentWidget())
    {
        QMouseEvent *event_tab=static_cast<QMouseEvent*>(event);
        if(event_tab->button()==Qt::RightButton)
            m_tab->setContextMenuPolicy(Qt::ActionsContextMenu);
    }
    /*Key press for menu bar*/
    QKeyEvent *event_press=static_cast<QKeyEvent*>(event);
    if(event_press->key()==Qt::Key_Alt && !this->isFullScreen())
    {
        if(m_menu_show)
        {
            m_menu_file->menuAction()->setVisible(m_menu_show);
            m_menu_tool->menuAction()->setVisible(m_menu_show);
            m_menu_display->menuAction()->setVisible(m_menu_show);
            m_menu_show=false;
        }
        else
        {
            m_menu_file->menuAction()->setVisible(m_menu_show);
            m_menu_tool->menuAction()->setVisible(m_menu_show);
            m_menu_display->menuAction()->setVisible(m_menu_show);
            m_menu_show=true;
        }
    }
    else  if(event_press->key()==Qt::Key_Escape && this->isFullScreen())
        this->showMaximized();

    return QMainWindow::eventFilter(obj, event);
}

