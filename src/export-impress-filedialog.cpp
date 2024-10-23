#include "export-impress-filedialog.h"

extern QDir vymBaseDir;

ExportImpressFileDialog::ExportImpressFileDialog() : QFileDialog() { init(); }

ExportImpressFileDialog::ExportImpressFileDialog(QWidget *parent, const QString &caption)
    : QFileDialog(parent, caption)
{
    init();
}

bool ExportImpressFileDialog::foundConfig() { return !filters.isEmpty(); }

QString ExportImpressFileDialog::selectedConfig()
{
    QStringList::Iterator itpath = configPaths.begin();
    QStringList::Iterator itf = filters.begin();
    while (itf != filters.end()) {
        if (*itf == selectedNameFilter())
            return *itpath;
        itpath++;
        itf++;
    }
    qWarning("ExportImpressFileDialog::selectedConfig  No filter found!");
    return "";
}

void ExportImpressFileDialog::newConfigPath(const QString &s) { lastFilter = s; }

void ExportImpressFileDialog::show()
{
    setNameFilters(filters);
    QFileDialog::show();
}

void ExportImpressFileDialog::init()
{
    setFileMode(QFileDialog::AnyFile);
    QDir d = vymBaseDir;
    d.cd("exports");
    scanExportConfigs(d);
    d.setPath(d.homePath() + "/.vym/exports");
    scanExportConfigs(d);
    d.setPath(d.currentPath() + "/exports");
    scanExportConfigs(d);

    setNameFilters(filters);
    connect(this, SIGNAL(filterSelected(const QString &)), this,
            SLOT(newConfigPath(const QString &)));
}

void ExportImpressFileDialog::addFilter(const QString &f)
{
    lastFilter = f;
    filters.append(f);
}

void ExportImpressFileDialog::scanExportConfigs(QDir dir)
{
    // Scan existing export configurations
    SimpleSettings set;
    QFile f;
    if (dir.exists()) {
        // Traverse files
        dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

        QFileInfoList list = dir.entryInfoList();
        for (int i = 0; i < list.size(); ++i) {
            QFileInfo fi = list.at(i);

            if (fi.fileName().endsWith(".conf")) {
                configPaths.append(fi.absoluteFilePath());
                set.clear();
                set.readSettings(fi.absoluteFilePath());
                addFilter(set.value(QString("Name")) + " (*.odp)");
            }
        }
    }
}
