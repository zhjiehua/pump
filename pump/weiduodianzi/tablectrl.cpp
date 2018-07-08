#include "tablectrl.h"
#include <QDebug>
#include <QKeyEvent>
#include <editctrl.h>
#include "mytablemodel.h"
#include <QShortcut>
#include "baseMainPage.h"

TableCtrl::TableCtrl(QWidget *parent)
	: QTableView(parent)
{
	setStyleSheet("QTableView:focus{border:2px solid rgb(155,200,33);}");
    //selection-background-color: qlineargradient(x1: 0, y1: 0, x2: 0.5, y2: 0.5,stop: 0 #FF92BB, stop: 1 white); gridline-color: blue;
   /* setStyleSheet("QTableView {  selection-background-color: qlineargradient(x1: 0, y1: 0, x2: 0.5, y2: 0.5,stop: 0 #FF92BB, stop: 1 white); gridline-color: blue; }");
    setShowGrid(false);
    setFocusPolicy(Qt::NoFocus);*/

}


TableCtrl::~TableCtrl()
{

}


//处理上下左右return、backspace按键;
void TableCtrl::keyPressEvent( QKeyEvent *event )
{

    if(event->key() == KEY_UP)
    {
        QModelIndex index = currentIndex();
        
        int row = index.row();
        if(row > 0)
        {
            row--;
            int col = index.column();
            QModelIndex indexA = model()->index(row, col, QModelIndex());
            m_currentIndex = indexA;
            setCurrentIndex(indexA);
        }
        else
        {
            emit(getOutFocus(0));
            if(selectionModel())
                selectionModel()->clear();
        }

    }
    else if(event->key() == KEY_DOWN)
    {
        QModelIndex index = currentIndex();
        
        int row = index.row();
        if(row < model()->rowCount() -1)
        {
            row++;
            int col = index.column();
            QModelIndex indexA = model()->index(row, col, QModelIndex());
            m_currentIndex = indexA;
            setCurrentIndex(indexA);
        }
        else
        {
            emit(getOutFocus(1));
            if(selectionModel())
                selectionModel()->clear();
        }
    }
    else if(event->key() == KEY_LEFT)
    {
        QModelIndex index = currentIndex();
        
        int col = index.column();
        if(col > 0)
        {
            col--;
            QModelIndex indexA = model()->index(currentIndex().row(), col, QModelIndex());
            m_currentIndex = indexA;
            setCurrentIndex(indexA);
        }
        else
        {
            if(currentIndex().row()>0)
            {
                int row = currentIndex().row()-1;
                col = model()->columnCount()-1;
                QModelIndex indexA = model()->index(row, col, QModelIndex());
                m_currentIndex = indexA;
                setCurrentIndex(indexA);

            }
            else
            {
                emit(getOutFocus(2));
                if(selectionModel())
                    selectionModel()->clear();
            }
        }
    }
    else if(event->key() == KEY_RIGHT)
    {
        QModelIndex index = currentIndex();
        
        int col = index.column();
        if(col <model()->columnCount()-1 && col >= 0)
        {
            col++;
            int row = index.row();
            QModelIndex indexA = model()->index(row, col, QModelIndex());
            m_currentIndex = indexA;
            setCurrentIndex(indexA);
        }
        else
        {
            emit(getOutFocus(3));
            if(selectionModel())
                selectionModel()->clear();
        }
    }
    else if(event->key() == KEY_BACKSPACE)
    {
        qDebug()<<"backspace";
        int row = currentIndex().row();
        if(row >= 0)
            model()->removeRow(row);
    }
    else if(event->key() == KEY_RETURN)
 	{

        if(currentIndex().row() == model()->rowCount()-1)
  		{
  			//新增一行;
			MyTableModel *myModel = qobject_cast<MyTableModel *>(model());
  			myModel->insertRow(model()->rowCount());
  		}
        return;

 	}
	else
		QTableView::keyPressEvent(event);

}


void TableCtrl::focusInEvent(QFocusEvent * event)
{
	QAbstractScrollArea::focusInEvent(event);

	const QItemSelectionModel* model = selectionModel();
	//const bool currentIndexValid = currentIndex().isValid();
	const bool currentIndexValid = m_currentIndex.isValid();


	if (model
		&& !currentIndexValid) {
			QModelIndex index = moveCursor(MoveNext, Qt::NoModifier); // first visible index
			if (index.isValid() && event->reason() != Qt::MouseFocusReason)
				selectionModel()->setCurrentIndex(index, QItemSelectionModel::NoUpdate);
            setCurrentIndex(index);
	}

	if (model && currentIndexValid) {
		if (currentIndex().flags() != Qt::ItemIsEditable)
		{
			//setAttribute(Qt::WA_InputMethodEnabled, false);
			selectionModel()->setCurrentIndex(m_currentIndex, QItemSelectionModel::NoUpdate);
            setCurrentIndex(m_currentIndex);
		}
		else
			setAttribute(Qt::WA_InputMethodEnabled);
	}

	if (!currentIndexValid)
		setAttribute(Qt::WA_InputMethodEnabled, false);

	g_pMainWindow->setShortCutDisable(true);

}

void TableCtrl::initIndex()
{
	m_currentIndex = moveCursor(MoveNext, Qt::NoModifier); // first visible index
}



void TableCtrl::focusOutEvent(QFocusEvent * event)
{
	g_pMainWindow->setShortCutDisable(false);
	QTableView::focusOutEvent(event);
}






