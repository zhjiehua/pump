#include "ui/pages/languagepage.h"
#include "core/machinecontroller.h"
#include "core/i18nmanager.h"
#include "core/picturemanager.h"
#include "ui/mainwindow.h"
#include "ui/widgets/btnctrl.h"
#include "ui/widgets/imgbutton.h"
#include "ui/widgets/pagescroll.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

LanguagePage::LanguagePage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *root = new QHBoxLayout(inner);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);
    root->addStretch(1);

    auto *globeCol = new QVBoxLayout;
    globeCol->setSpacing(0);
    globeCol->addStretch(1);
    m_globe = new ImgButton;
    m_globe->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_globe->setMaximumSize(150, 150);
    m_globe->setFocusPolicy(Qt::NoFocus);
    m_globe->setBkImage(PictureManager::Global);
    globeCol->addWidget(m_globe, 3);
    globeCol->addStretch(1);
    root->addLayout(globeCol, 4);
    root->addStretch(2);

    auto *langCol = new QVBoxLayout;
    langCol->setSpacing(12);
    langCol->addStretch(1);

    m_chinese = new BtnCtrl;
    m_chinese->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_chinese->setMaximumSize(120, 80);
    m_chinese->setStyleSheet(PictureManager::instance().iconButtonStyle(
        PictureManager::Chinese, PictureManager::ChineseFocus));
    langCol->addWidget(m_chinese, 6);

    m_chineseLabel = new QLabel;
    m_chineseLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_chineseLabel->setAlignment(Qt::AlignCenter);
    langCol->addWidget(m_chineseLabel, 1);
    langCol->addStretch(1);

    m_english = new BtnCtrl;
    m_english->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_english->setMaximumSize(120, 80);
    m_english->setStyleSheet(PictureManager::instance().iconButtonStyle(
        PictureManager::English, PictureManager::EnglishFocus));
    langCol->addWidget(m_english, 6);

    m_englishLabel = new QLabel;
    m_englishLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_englishLabel->setAlignment(Qt::AlignCenter);
    langCol->addWidget(m_englishLabel, 1);
    langCol->addStretch(1);

    root->addLayout(langCol, 6);
    root->addStretch(2);

    installPageScroll(this, inner);
    retranslateUi();

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

void LanguagePage::retranslateUi()
{
    refreshLabels();
}

void LanguagePage::setLanguage(AppSettings::Language lang)
{
    if (!m_c->i18n()->applyLanguage(int(lang)))
        return;
    if (m_main)
        m_main->retranslateUi();
    QMessageBox::information(this, tr("Tips"), tr("Change Language Success"));
}

void LanguagePage::refreshLabels()
{
    m_chineseLabel->setText(tr("Chinese"));
    m_englishLabel->setText(tr("English"));
}
