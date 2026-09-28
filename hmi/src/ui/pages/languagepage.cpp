#include "ui/pages/languagepage.h"
#include "core/machinecontroller.h"
#include "core/i18nmanager.h"
#include "core/picturemanager.h"
#include "ui/mainwindow.h"
#include "ui/widgets/pagescroll.h"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

namespace {

QPushButton *langBtn(PictureManager::Picture normal, PictureManager::Picture focus)
{
    auto *b = new QPushButton;
    b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    b->setMinimumSize(80, 60);
    b->setMaximumSize(150, 150);
    b->setStyleSheet(PictureManager::instance().iconButtonStyle(normal, focus));
    return b;
}

} // namespace

LanguagePage::LanguagePage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *root = new QHBoxLayout(inner);
    root->setContentsMargins(4, 4, 4, 4);
    root->addStretch(1);

    m_chinese = langBtn(PictureManager::Chinese, PictureManager::ChineseFocus);
    m_english = langBtn(PictureManager::English, PictureManager::EnglishFocus);

    auto wrap = [](QPushButton *btn) {
        auto *col = new QVBoxLayout;
        col->addStretch(1);
        col->addWidget(btn, 3);
        col->addStretch(1);
        return col;
    };
    root->addLayout(wrap(m_chinese), 4);
    root->addStretch(2);
    root->addLayout(wrap(m_english), 6);
    root->addStretch(2);

    installPageScroll(this, inner);
    refreshIcons();

    connect(m_chinese, SIGNAL(clicked()), this, SLOT(onChinese()));
    connect(m_english, SIGNAL(clicked()), this, SLOT(onEnglish()));
}

void LanguagePage::initFocusList()
{
    xList.append(m_chinese);
    xList.append(m_english);
    yList.append(m_chinese);
    yList.append(m_english);
}

void LanguagePage::onChinese()
{
    setLanguage(AppSettings::Chinese);
}

void LanguagePage::onEnglish()
{
    setLanguage(AppSettings::English);
}

void LanguagePage::setLanguage(AppSettings::Language lang)
{
    auto *s = m_c->settings();
    if (s->language == lang)
        return;
    m_c->i18n()->applyLanguage(int(lang));
    refreshIcons();
    if (m_main)
    {
        m_main->retranslateUi();
        QMessageBox::information(this, tr("Tips"),
                                 lang == AppSettings::Chinese ? tr("Language: Chinese")
                                                              : tr("Language: English"));
    }
}

void LanguagePage::refreshIcons()
{
    const bool cn = m_c->settings()->language == AppSettings::Chinese;
    m_chinese->setEnabled(!cn);
    m_english->setEnabled(cn);
}
