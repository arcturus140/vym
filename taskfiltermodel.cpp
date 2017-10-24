#include "taskfiltermodel.h"
#include "taskmodel.h"

#include <QSortFilterProxyModel>

#include "branchitem.h"

extern TaskModel *taskModel;

void TaskFilterModel::setFilter (bool b)
{
    useFilter = b;
}

void TaskFilterModel::setMapFilter (const QString &s)	
{
    mapFilter = s;
}

void TaskFilterModel::setFilterNew (bool b)
{
    filterNew = b;
}

void TaskFilterModel::setFilterFlags1 (bool b)
{
    filterFlags1 = b;
}

void TaskFilterModel::setFilterFlags2 (bool b)
{
    filterFlags2 = b;
}

void TaskFilterModel::setFilterFlags3 (bool b)
{
    filterFlags3 = b;
}

bool TaskFilterModel::filterAcceptsRow(int sourceRow, 
         const QModelIndex &sourceParent) const
{
    QModelIndex ix = sourceModel()->index(sourceRow, 0, sourceParent);

    // Filter by mapname
    QString mapname = taskModel->getTask(ix)->getMapName();
    if (mapname.isEmpty()) mapname = "justSomePseudoMapNameForFiltering";
    if ( !mapFilter.isEmpty() && mapname != mapFilter )
        return false;

    // Filter new tasks
    if (filterNew && taskModel->getTask(ix)->getAwake() != Task::Morning)
        return false;

    // Filter active tasks
    if (useFilter && ((taskModel->getTask(ix)->getDaysSleep() > 0) || (taskModel->getTask(ix)->getStatus() == Task::Finished) ) )
        return false;

    // Filter flags  // FIXME-1 currently only "arrow-up"
    if (filterFlags1 && !taskModel->getTask(ix)->getBranch()->hasActiveStandardFlag("arrow-up") )
        return false;

    // Filter flags  // FIXME-1 currently only "2arrow-up"
    if (filterFlags2 && !taskModel->getTask(ix)->getBranch()->hasActiveStandardFlag("2arrow-up") )
        return false;

    // Filter flags  // FIXME-1 currently only "stopsign"
    if (filterFlags3 && !taskModel->getTask(ix)->getBranch()->hasActiveStandardFlag("stopsign") )
        return false;
    return true;
}

