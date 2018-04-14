#ifndef SETTING_H
#define SETTING_H

#include <QDialog>
#include <QTabWidget>

class QWebEngineView;
class QComboBox;

class Setting : public QDialog
{
    Q_OBJECT
public:
    explicit Setting(QList<QWebEngineView*>list_page, QWidget *parent = 0);
    Setting(Setting&)=delete;
    Setting& operator=(Setting&)=delete;

    static void loadSettingPage(QWebEngineView *s); //when start a page

private :
    void validateOption(const QComboBox * const combo_font_size, const QComboBox * const combo_font_family, qint8 const size);    // when click validate
    QWidget *createTabFontOption();
    QWidget *createTabWebAttribute();

private :
    QList<QWebEngineView*>m_list_view;
    QTabWidget *m_tab=new QTabWidget(this);

public slots:
};


#endif // SETTING_H
