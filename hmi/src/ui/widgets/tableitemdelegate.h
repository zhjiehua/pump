#ifndef TABLEITEMDELEGATE_H
#define TABLEITEMDELEGATE_H

#include <QItemDelegate>

class TableItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit TableItemDelegate(const QString &inputMask, QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    QString m_inputMask;
};

#endif
