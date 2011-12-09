#include "taskeditor.h"

#include <QAbstractTableModel>
#include <QDebug>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

#include "branchitem.h"
#include "mainwindow.h"
#include "task.h"
#include "taskmodel.h"
#include "vymmodel.h"

extern TaskModel* taskModel;
extern Main *mainWindow;

//extern QString iconPath;

TaskEditor::TaskEditor(QWidget *)
{
    // Create Table view
    view = new QTableView; 

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget (view);
    setLayout (mainLayout);

    view->verticalHeader()->hide();
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->horizontalHeader()->setStretchLastSection(true);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    proxyModel = new QSortFilterProxyModel();
    proxyModel->setSourceModel(taskModel);
    proxyModel->setSortCaseSensitivity (Qt::CaseInsensitive);
//    proxyModel->setFilterRegExp(QRegExp("WIP", Qt::CaseInsensitive));
//    proxyModel->setFilterKeyColumn(1);

    proxyModel->setDynamicSortFilter (true);

    view->setModel (proxyModel);
    view->horizontalHeader()->setSortIndicator (0,Qt::AscendingOrder);
    view->setSortingEnabled(true);

    connect (
	view->selectionModel(),SIGNAL (selectionChanged (QItemSelection,QItemSelection)),
	this, SLOT (selectionChanged (QItemSelection,QItemSelection)));
}

void TaskEditor::sort()
{
    taskModel->recalcPriorities();
    proxyModel->sort( proxyModel->sortColumn(), proxyModel->sortOrder() );
}

bool TaskEditor::select (Task *task)	//FIXME-0 select whole row
{
    if (task)
    {
	view->selectionModel()->select (proxyModel->mapFromSource(taskModel->index (task) ), QItemSelectionModel::ClearAndSelect  );
	return true;
    }
    return false;
}

void TaskEditor::selectionChanged ( const QItemSelection & selected, const QItemSelection & )
{
    QModelIndex ix;
    QModelIndex ix_org;
    foreach (ix,selected.indexes() )	// FIXME-3 what, if multiple selection in MapEditor?
    {
	// Also select in other editors
	ix_org= proxyModel->mapToSource(ix);
	Task *t=taskModel->getTask (ix_org);
	if (t) 
	{
	    BranchItem *bi=t->getBranch();
	    if (bi) 
	    {
		VymModel *m=bi->getModel();
		m->select (bi);
		if (m!=mainWindow->currentModel() )
		    mainWindow->gotoModel (m);
		view->setStyleSheet( 
		QString ("selection-color: %1;" 
			 "selection-background-color: %2;").arg(bi->getHeadingColor().name() ).arg(m->getSelectionColor().name() ) );
		view->scrollTo (ix, QAbstractItemView::EnsureVisible);
	    }
	}
    }
    view->resizeColumnsToContents();
    view->horizontalHeader()->setStretchLastSection(true);
}

