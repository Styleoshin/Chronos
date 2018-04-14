#include "setting.h"

#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>

#include <QMessageBox>

#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>

#include <QWebEngineSettings>
#include <QWebEngineView>

#include <QSettings>

Setting::Setting(QList<QWebEngineView*>list_page, QWidget *parent) : QDialog(parent),m_list_view(list_page)
{
    m_tab->addTab(createTabFontOption(),tr("Font"));
    m_tab->addTab(createTabWebAttribute(),tr("Other"));
    QVBoxLayout *layout=new QVBoxLayout;
    layout->addWidget(m_tab);
    setLayout(layout);
}




void Setting::loadSettingPage(QWebEngineView *s)
{
    QSettings settings("Option.ini", QSettings::IniFormat);
    settings.beginGroup("Attribute");
    s->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled,settings.value("js").toBool());

    s->settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard,settings.value("js_access_clipboard").toBool());

    s->settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows,settings.value("js_open_windows").toBool());

    s->settings()->setAttribute(QWebEngineSettings::AutoLoadImages,settings.value("auto_load_images").toBool());

    s->settings()->setAttribute(QWebEngineSettings::ErrorPageEnabled,settings.value("error_page").toBool());

    s->settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled,settings.value("fullscreen_support").toBool());

    s->settings()->setAttribute(QWebEngineSettings::ScreenCaptureEnabled,settings.value("screen_capture").toBool());

    s->settings()->setAttribute(QWebEngineSettings::AutoLoadIconsForPage,settings.value("autoload_icons").toBool());

    s->settings()->setAttribute(QWebEngineSettings::TouchIconsEnabled,settings.value("touch_icons").toBool());

    s->settings()->setAttribute(QWebEngineSettings::LinksIncludedInFocusChain,settings.value("links_included_in_focus_chain").toBool());

    s->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls,settings.value("local_content_access_remote_urls").toBool());

    s->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls,settings.value("local_content_access_file_urls").toBool());

    s->settings()->setAttribute(QWebEngineSettings::HyperlinkAuditingEnabled,settings.value("hyperlink_auditing").toBool());

    s->settings()->setAttribute(QWebEngineSettings::XSSAuditingEnabled,settings.value("xssauditing").toBool());

    s->settings()->setAttribute(QWebEngineSettings::PluginsEnabled,settings.value("plugins").toBool());

    s->settings()->setAttribute(QWebEngineSettings::WebGLEnabled,settings.value("webgl").toBool());

    s->settings()->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled,settings.value("accelerated_2d_canvas").toBool());

    s->settings()->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled,settings.value("scroll_animator").toBool());

    s->settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled,settings.value("local_storage").toBool());

    s->settings()->setAttribute(QWebEngineSettings::SpatialNavigationEnabled,settings.value("spatial_navigation").toBool());

    settings.endGroup();
}




void Setting::validateOption(const QComboBox *const combo_font_size, const QComboBox *const combo_font_family, const qint8 size)
{

    for(auto const s: m_list_view)
    {
        if(combo_font_size->currentIndex()==0)s->settings()->setFontSize(QWebEngineSettings::MinimumFontSize,size);
        else if(combo_font_size->currentIndex()==1)s->settings()->setFontSize(QWebEngineSettings::MinimumLogicalFontSize,size);
        else if(combo_font_size->currentIndex()==2)s->settings()->setFontSize(QWebEngineSettings::DefaultFontSize,size);
        else s->settings()->setFontSize(QWebEngineSettings::DefaultFixedFontSize,size);

        if(combo_font_family->currentIndex()==0)s->settings()->setFontFamily(QWebEngineSettings::StandardFont,"Arial");
        else if(combo_font_family->currentIndex()==1)s->settings()->setFontFamily(QWebEngineSettings::FixedFont,"Arial");
        else if(combo_font_family->currentIndex()==2)s->settings()->setFontFamily(QWebEngineSettings::SerifFont,"Arial");
        else  if(combo_font_family->currentIndex()==3)s->settings()->setFontFamily(QWebEngineSettings::SansSerifFont,"Arial");
        else if(combo_font_family->currentIndex()==4)s->settings()->setFontFamily(QWebEngineSettings::CursiveFont,"Arial");
        else if(combo_font_family->currentIndex()==5)s->settings()->setFontFamily(QWebEngineSettings::FantasyFont,"Arial");
        else s->settings()->setFontFamily(QWebEngineSettings::PictographFont,"Arial");
    }
}


QWidget *Setting::createTabFontOption()
{
    QLabel *label_font_size=new QLabel(tr("Type Font size"),this);
    QLabel *label_size_font_size=new QLabel(tr("Font size"),this);
    QLabel *label_font_family=new QLabel(tr("Font family"),this);
    QPushButton *button_validate=new QPushButton(tr("Validate"),this);
    button_validate->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    QComboBox *combo_font_size=new QComboBox(this);
    QComboBox *combo_font_family=new QComboBox(this);

    QSpinBox *spin_font_size=new QSpinBox(this);

    spin_font_size->setRange(0,100);

    combo_font_size->addItem(tr("Minimum Font Size"));
    combo_font_size->addItem(tr("Minimum Logical Font Size"));
    combo_font_size->addItem(tr("Default Font Size"));
    combo_font_size->addItem(tr("Default Fixed Font Size"));

    combo_font_family->addItem(tr("Standard Font"));
    combo_font_family->addItem(tr("Fixed Font"));
    combo_font_family->addItem(tr("Serif Font"));
    combo_font_family->addItem(tr("Sans Serif Font"));
    combo_font_family->addItem(tr("Cursive Font"));
    combo_font_family->addItem(tr("Fantasy Font"));
    combo_font_family->addItem(tr("Pictograph Font"));

    QVBoxLayout *layout_name_option=new QVBoxLayout;
    QVBoxLayout *layout_set_option=new QVBoxLayout;
    QHBoxLayout *layout_option=new QHBoxLayout;
    QVBoxLayout *set_layout_option=new QVBoxLayout;

    layout_name_option->addWidget(label_font_size);
    layout_name_option->addWidget(label_font_family);
    layout_name_option->addWidget(label_size_font_size);

    layout_set_option->addWidget(combo_font_size);
    layout_set_option->addWidget(combo_font_family);
    layout_set_option->addWidget(spin_font_size);

    layout_option->addLayout(layout_name_option);
    layout_option->addLayout(layout_set_option);

    set_layout_option->addLayout(layout_option);
    set_layout_option->addWidget(button_validate);

    QWidget *widget=new QWidget(this);
    widget->setLayout(set_layout_option);

    connect(button_validate,&QPushButton::pressed,this,[this,combo_font_size,combo_font_family,spin_font_size]{validateOption(combo_font_size,combo_font_family,spin_font_size->value());});

    return widget;
}

QWidget *Setting::createTabWebAttribute()
{
    QVBoxLayout *layout_js=new QVBoxLayout;
    QGroupBox *group_javascript=new QGroupBox(tr("Javascript"),this);
    QCheckBox *check_js=new QCheckBox(tr("Javascript"),this);
    QCheckBox *check_js_access_clipboard=new QCheckBox(tr("Javascript can access clipboard"),this);
    QCheckBox *check_js_open_windows=new QCheckBox(tr("Javascript can open windows"),this);

    layout_js->addWidget(check_js);
    layout_js->addWidget(check_js_access_clipboard);
    layout_js->addWidget(check_js_open_windows);
    group_javascript->setLayout(layout_js);

    QGridLayout *layout_page=new QGridLayout;
    QGroupBox *group_page=new QGroupBox(tr("Page"));
    QCheckBox *check_auto_load_images=new QCheckBox(tr("Auto load images"),this);
    QCheckBox *check_error_page=new QCheckBox(tr("Error page"),this);
    QCheckBox *check_fullscreen_support=new QCheckBox(tr("Fullscreen support"),this);
    QCheckBox *check_screen_capture=new QCheckBox(tr("Screen capture"),this);
    QCheckBox *check_autoload_icons=new QCheckBox(tr("Auto load icons"),this);
    QCheckBox *check_touch_icons=new QCheckBox(tr("Touch icons"),this);

    layout_page->addWidget(check_auto_load_images,0,0);
    layout_page->addWidget(check_error_page,0,1);
    layout_page->addWidget(check_fullscreen_support,0,2);
    layout_page->addWidget(check_screen_capture,1,0);
    layout_page->addWidget(check_autoload_icons,1,1);
    layout_page->addWidget(check_touch_icons,1,2);
    group_page->setLayout(layout_page);

    QGridLayout *layout_other=new QGridLayout;
    QGroupBox *group_other=new QGroupBox(tr("Other"));
    QCheckBox *check_links_included_in_focus_chain=new QCheckBox(tr("Links included in focus chain"),this);
    QCheckBox *check_local_content_access_remote_urls=new QCheckBox(tr("Local content can access remote urls"),this);
    QCheckBox *check_local_content_access_file_urls=new QCheckBox(tr("Local content can access file urls"),this);
    QCheckBox *check_hyperlink_auditing=new QCheckBox(tr("Hyperlink auditing"),this);
    QCheckBox *check_xssauditing=new QCheckBox(tr("XSSAuditing"),this);
    QCheckBox *check_spatial_navigation=new QCheckBox(tr("Spatial navigation"),this);
    QCheckBox *check_plugins=new QCheckBox(tr("Plugins"),this);
    QCheckBox *check_webgl=new QCheckBox(tr("WebGL"),this);
    QCheckBox *check_accelerated_2d_canvas=new QCheckBox(tr("Accelerated 2d canvas"),this);
    QCheckBox *check_scroll_animator=new QCheckBox(tr("Scroll animator"),this);
    QCheckBox *check_local_storage=new QCheckBox(tr("Local storage"),this);

    layout_other->addWidget(check_links_included_in_focus_chain,0,0);
    layout_other->addWidget(check_local_content_access_remote_urls,0,1);
    layout_other->addWidget(check_local_content_access_file_urls,0,2);
    layout_other->addWidget(check_hyperlink_auditing,1,0);
    layout_other->addWidget(check_xssauditing,1,1);
    layout_other->addWidget(check_plugins,1,2);
    layout_other->addWidget(check_webgl,2,0);
    layout_other->addWidget(check_accelerated_2d_canvas,2,1);
    layout_other->addWidget(check_scroll_animator,2,2);
    layout_other->addWidget(check_local_storage,3,0);
    layout_other->addWidget(check_spatial_navigation,3,1);
    group_other->setLayout(layout_other);

    QPushButton *button_validate=new QPushButton(tr("Validate"),this);
    button_validate->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    QWidget *widget=new QWidget(this);
    QVBoxLayout *layout_option=new QVBoxLayout;

    layout_option->addWidget(group_javascript);
    layout_option->addWidget(group_page);
    layout_option->addWidget(group_other);
    layout_option->addWidget(button_validate);
    widget->setLayout(layout_option);


    QSettings settings("Option.ini", QSettings::IniFormat);
    settings.beginGroup("Attribute");
    check_js->setChecked(settings.value("js").toBool());
    check_js_access_clipboard->setChecked(settings.value("js_access_clipboard").toBool());
    check_js_open_windows->setChecked(settings.value("js_open_windows").toBool());
    check_auto_load_images->setChecked(settings.value("auto_load_images").toBool());
    check_error_page->setChecked(settings.value("error_page").toBool());
    check_fullscreen_support->setChecked(settings.value("fullscreen_support").toBool());
    check_screen_capture->setChecked(settings.value("screen_capture").toBool());
    check_autoload_icons->setChecked(settings.value("autoload_icons").toBool());
    check_touch_icons->setChecked(settings.value("touch_icons").toBool());
    check_links_included_in_focus_chain->setChecked(settings.value("links_included_in_focus_chain").toBool());
    check_local_content_access_remote_urls->setChecked(settings.value("local_content_access_remote_urls").toBool());
    check_local_content_access_file_urls->setChecked(settings.value("local_content_access_file_urls").toBool());
    check_hyperlink_auditing->setChecked(settings.value("hyperlink_auditing").toBool());
    check_xssauditing->setChecked(settings.value("xssauditing").toBool());
    check_plugins->setChecked(settings.value("plugins").toBool());
    check_webgl->setChecked(settings.value("webgl").toBool());
    check_accelerated_2d_canvas->setChecked(settings.value("accelerated_2d_canvas").toBool());
    check_scroll_animator->setChecked(settings.value("scroll_animator").toBool());
    check_local_storage->setChecked(settings.value("local_storage").toBool());
    check_spatial_navigation->setChecked(settings.value("spatial_navigation").toBool());
    settings.endGroup();

    connect(button_validate,&QPushButton::pressed,this,[=](){

        // Save setting
        QSettings settings("Option.ini", QSettings::IniFormat);
        settings.beginGroup("Attribute");
        settings.setValue("js",check_js->isChecked());
        settings.setValue("js_access_clipboard",check_js_access_clipboard->isChecked());
        settings.setValue("js_open_windows",check_js_open_windows->isChecked());
        settings.setValue("auto_load_images",check_auto_load_images->isChecked());
        settings.setValue("error_page",check_error_page->isChecked());
        settings.setValue("fullscreen_support",check_fullscreen_support->isChecked());
        settings.setValue("screen_capture",check_screen_capture->isChecked());
        settings.setValue("autoload_icons",check_autoload_icons->isChecked());
        settings.setValue("touch_icons",check_touch_icons->isChecked());
        settings.setValue("links_included_in_focus_chain",check_links_included_in_focus_chain->isChecked());
        settings.setValue("local_content_access_remote_urls",check_local_content_access_remote_urls->isChecked());
        settings.setValue("local_content_access_file_urls",check_local_content_access_file_urls->isChecked());
        settings.setValue("hyperlink_auditing",check_hyperlink_auditing->isChecked());
        settings.setValue("xssauditing",check_xssauditing->isChecked());
        settings.setValue("plugins",check_plugins->isChecked());
        settings.setValue("webgl",check_webgl->isChecked());
        settings.setValue("accelerated_2d_canvas",check_accelerated_2d_canvas->isChecked());
        settings.setValue("scroll_animator",check_scroll_animator->isChecked());
        settings.setValue("local_storage",check_local_storage->isChecked());
        settings.setValue("spatial_navigation",check_spatial_navigation->isChecked());
        settings.endGroup();
    });

    connect(button_validate,&QPushButton::pressed,this,[this]{QMessageBox::information(this,tr("Option"),tr("Restart program for apply the change"));});

    return widget;
}

