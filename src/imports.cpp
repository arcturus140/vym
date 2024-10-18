#include "imports.h"

#include "attributeitem.h"
#include "branchitem.h"
#include "file.h"
#include "mainwindow.h"
#include "misc.h"
#include "vymmodel.h"
#include "xsltproc.h"
#include "zip-agent.h"

#include <QMessageBox>

extern Main *mainWindow;
extern QDir vymBaseDir;

ImportBase::ImportBase()
{
    model = nullptr;
    init();
}

ImportBase::ImportBase(VymModel *m)
{
    model = m;
    init();
}

ImportBase::~ImportBase()
{
    // Remove tmpdir
    removeDir(tmpDir);
}

void ImportBase::init()
{
    bool ok;
    tmpDir.setPath(makeTmpDir(ok, "vym-import"));
    if (!tmpDir.exists() || !ok)
        QMessageBox::critical(
            0, QObject::tr("Error"),
            QObject::tr("Couldn't access temporary directory\n"));
}

void ImportBase::setDir(const QString &p) { inputDir = p; }

void ImportBase::setFile(const QString &p) { inputFile = p; }

bool ImportBase::transform() { return true; }

QString ImportBase::getTransformedFile() { return transformedFile; }

/////////////////////////////////////////////////
ImportFirefoxBookmarks::ImportFirefoxBookmarks(VymModel *m) : ImportBase(m) 
{
    totalBookmarks = currentBookmarks = 0;
}

bool ImportFirefoxBookmarks::transform()
{
    QStringList lines;
    QFile file(inputFile);

    progressDialog.setAutoReset(false);
    progressDialog.setAutoClose(false);
    progressDialog.setMinimumWidth(600);
    progressDialog.setCancelButton(nullptr);
    progressDialog.setWindowTitle(QObject::tr("Import Firefox bookmarks","Import dialog"));
    progressDialog.setLabelText(
        QObject::tr("Loading bookmarks:", "Progress dialog while importing bookmarks"));

    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument jsdoc = QJsonDocument::fromJson(file.readAll());
        file.close();

        QJsonObject jsobj = jsdoc.object();

        QJsonArray jsarr = jsobj["children"].toArray();
        foreach (const QJsonValue &value, jsarr) {
            parseJson (value, ParseMode::countBookmarks);
        }

        progressDialog.setRange(0, totalBookmarks);
        progressDialog.setValue(1);
        progressDialog.setWindowModality(Qt::WindowModal);
        progressDialog.show();
        QApplication::processEvents();

        model->blockReposition();

        parseJson (jsobj, ParseMode::buildMap);
        
        model->unblockReposition();

        mainWindow->statusMessage(
                QObject::tr("Imported %1 bookmarks", "Import dialog").arg(totalBookmarks));
    }

    return false;
}

//FIXME-4 Check importing Firefox bookmarks. Switch from JSON to HTML?
//        Or include lz4 to decompress FF json bookmarks?
//        https://unix.stackexchange.com/questions/326897/how-to-decompress-jsonlz4-files-firefox-bookmark-backups-using-the-command-lin
//        HTML is not valid XML :-(

bool ImportFirefoxBookmarks::parseJson(QJsonValue jsval, ParseMode mode, BranchItem *selbi)
{
    if (!selbi) {
        selbi = model->getSelectedBranch();
        if (!selbi) {
            qWarning() << "ImportFirefoxBookmarks: No branch selected!";
            return false;
        }
    }

    QJsonObject jsobj = jsval.toObject();

    if (mode == countBookmarks) {
        // Count bookmarks 
        if (jsobj.contains("uri") && jsobj["uri"].isString())
            totalBookmarks++;
    } else {
        // Build map
        selbi = model->addNewBranch(selbi);
        selbi->setHeadingPlainText(jsobj["title"].toString());

        if (jsobj.contains("uri") && jsobj["uri"].isString()) {
            currentBookmarks++;
            progressDialog.setValue(currentBookmarks);
            selbi->setUrl(jsobj["uri"].toString());
        }

        foreach (QString key, jsobj.keys())
        {
            QVariant v;
            if (key != "children") {
                // Integer types: dateAdded, id, index, lastModified, typeCode
                // Special: postData
                if (key == "dateAdded" || key == "lastModified") {
                    qlonglong l = jsobj[key].toVariant().toLongLong();
                    QDateTime dt;
                    dt.setMSecsSinceEpoch(l / 1000);
                    v = dt;
                } else if (key == "id" || key == "index" || 
                        key == "lastModified" || key == "typeCode" ) {
                    v = jsobj[key].toInt();
                } else if (key == "postData") 
                    v = QString("null");
                else if (jsobj[key].isString())     // FIXME-5 type checks no longer needed qith QVariant
                    v = jsobj[key].toString();
                else {
                // Ignore only the "postdata: null" field for now
                    qWarning() << "Firefox import, unknown key type: " << jsobj[key].type();
                    qDebug() << "                Firefox bookmark: " << key << jsobj[key].toString();
                    v =QString("unknown type.");
                }

                model->setAttribute(selbi, key, v);
            }
        }

        model->emitDataChanged(selbi); // FIXME-5 required, but can reposition in between be blocked?
    } // build bookmakrs

    if (jsobj.contains("children") && jsobj["children"].isArray()) {

        QJsonArray jsarr = jsobj["children"].toArray();
        foreach (const QJsonValue &val, jsarr) {
            parseJson (val, mode, selbi);
        }

        if (selbi->depth() > 2) {
            selbi->scroll();
            model->emitDataChanged(selbi); // FIXME-5 required, but can reposition in between be blocked?
        }
    } 

    return true;
}
   
/////////////////////////////////////////////////
bool ImportMM::transform()
{
    // try to unzip
    ZipAgent zipAgent(tmpDir, inputFile);
    zipAgent.setBackgroundProcess(false);
    zipAgent.startUnzip();
    if (zipAgent.exitStatus() == QProcess::NormalExit) {
        // Set short name, too. Search from behind:
        transformedFile = inputFile;
        int i = transformedFile.lastIndexOf("/");
        if (i >= 0)
            transformedFile = transformedFile.remove(0, i + 1);
        transformedFile.replace(".mmap", ".xml");
        transformedFile = tmpDir.path() + "/" + transformedFile;

        XSLTProc p;
        p.setInputFile(tmpDir.path() + "/Document.xml");
        p.setOutputFile(transformedFile);
        p.setXSLFile(vymBaseDir.path() + "/styles/mmap2vym.xsl");
        p.process();

        return true;
    }
    else
        return false;
}
