#include "ui/pages/recordlistpage.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"
#include "ui/widgets/btnctrl.h"
#include "ui/widgets/msgbox.h"
#include "ui/widgets/pagescroll.h"
#include "utils/hmikeys.h"

#include <QAbstractItemView>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

RecordListPage::RecordListPage(RecordStore::Kind kind, MachineController *c, MainWindow *main,
                               QWidget *parent)
    : FocusPage(parent)
    , m_kind(kind)
    , m_c(c)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *root = new QVBoxLayout(inner);
    root->setContentsMargins(4, 4, 4, 4);
    root->setSpacing(4);

    m_table = new QTableWidget;
    m_table->setColumnCount(2);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setFocusPolicy(Qt::StrongFocus);
    m_table->setShowGrid(true);
    m_table->setWordWrap(false);
    m_table->verticalHeader()->hide();
    m_table->verticalHeader()->setDefaultSectionSize(18);
    m_table->setStyleSheet(QStringLiteral(
        "QTableWidget{outline:0;gridline-color:#ccc;background:#fff;}"
        "QTableWidget:focus{border:2px solid blue;outline:0;}"
        "QHeaderView::section{background:#e8e8e8;padding:2px;border:1px solid #ccc;}"));
#if QT_VERSION >= 0x050000
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->horizontalHeader()->setStretchLastSection(true);
#else
    m_table->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    m_table->horizontalHeader()->setStretchLastSection(true);
#endif
    root->addWidget(m_table, 1);

    auto *btns = new QHBoxLayout;
    btns->addStretch(1);
    m_clear = new BtnCtrl;
    m_clear->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_clear->setMinimumHeight(28);
    btns->addWidget(m_clear, 2);
    btns->addStretch(1);
    root->addLayout(btns);

    installPageScroll(this, inner);
    retranslateUi();

    connect(m_clear, SIGNAL(clicked()), this, SLOT(onClear()));
}

void RecordListPage::initFocusList()
{
    reload();
    xList.append(m_table);
    xList.append(m_clear);
    yList.append(m_table);
    yList.append(m_clear);
}

void RecordListPage::retranslateUi()
{
    m_table->setHorizontalHeaderLabels({tr("Time"), tr("Content")});
    m_clear->setText(tr("Clear"));
}

bool RecordListPage::handleFocusNavKey(int key)
{
    if (focusWidget() != m_table)
        return FocusPage::handleFocusNavKey(key);

    const int n = m_table->rowCount();
    if (n <= 0)
        return false;

    int row = m_table->currentRow();
    if (row < 0)
        row = 0;

    if (key == KEY_UP)
    {
        if (row <= 0)
            return false;
        m_table->setCurrentCell(row - 1, 0);
        return true;
    }
    if (key == KEY_DOWN)
    {
        if (row >= n - 1)
            return false;
        m_table->setCurrentCell(row + 1, 0);
        return true;
    }
    return FocusPage::handleFocusNavKey(key);
}

void RecordListPage::reload()
{
    const QVector<RecordStore::Entry> list = m_c->records()->records(m_kind);
    const int prev = m_table->currentRow();
    m_table->setRowCount(list.size());
    for (int i = 0; i < list.size(); ++i)
    {
        const RecordStore::Entry &e = list.at(i);
        QString time = e.time;
        if (time.size() >= 19)
            time = time.mid(5);
        auto *tItem = new QTableWidgetItem(time);
        tItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        m_table->setItem(i, 0, tItem);

        QString content = e.category;
        if (!e.message.isEmpty())
        {
            if (!content.isEmpty())
                content += QLatin1Char(' ');
            content += e.message;
        }
        auto *cItem = new QTableWidgetItem(content);
        cItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        m_table->setItem(i, 1, cItem);
    }
    if (list.isEmpty())
        return;
    const int row = qBound(0, prev, list.size() - 1);
    m_table->setCurrentCell(row, 0);
}

void RecordListPage::onClear()
{
    if (MsgBox::question(this, tr("Tips"), tr("Clear all records?"))
        != MsgBox::Yes)
        return;
    m_c->records()->clear(m_kind);
    reload();
}
