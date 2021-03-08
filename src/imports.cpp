#include "imports.h"

#include "attributeitem.h"
#include "branchitem.h"
#include "file.h"
#include "linkablemapobj.h"
#include "mainwindow.h"
#include "misc.h"
#include "vymmodel.h"
#include "xsltproc.h"

#include <QMessageBox>

extern Main *mainWindow;
extern QDir vymBaseDir;

ImportBase::ImportBase()
{
    model = NULL;
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
    progressDialog.setCancelButton(NULL);

    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument jsdoc = QJsonDocument::fromJson(file.readAll());
        file.close();

        QJsonObject jsobj = jsdoc.object();

        QJsonArray jsarr = jsobj["children"].toArray();
        foreach (const QJsonValue &value, jsarr) {
            parseJson (value, ParseMode::countBookmarks);
        }

        qDebug() << "Bookmarks found: " << totalBookmarks;
        qDebug() << "Starting to extend map...";

        progressDialog.setRange(0, totalBookmarks);
        progressDialog.setValue(1);
        progressDialog.show();

        model->blockReposition();
        foreach (const QJsonValue &value, jsarr) {
            parseJson (value, ParseMode::buildMap);
        }
        model->unblockReposition();
    }

    return false;
}

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
        if (jsobj.contains("uri") && jsobj["uri"].isString())
            totalBookmarks++;
    } else {
        selbi = model->addNewBranch(selbi);
        selbi->setHeadingPlainText(jsobj["title"].toString());

        if (jsobj.contains("uri") && jsobj["uri"].isString()) {
            currentBookmarks++;
            progressDialog.setValue(currentBookmarks);
            QApplication::processEvents();
            selbi->setURL(jsobj["uri"].toString());

            QList<QVariant> cData;
            cData << "new attribute"
                  << "undef";
            AttributeItem *ai;

            foreach (QString key, jsobj.keys())
            {
                ai = new AttributeItem(cData);  // FIXME-2 remove cdata  
                ai->setKey(key);
                if (jsobj[key].isString())
                    ai->setValue(jsobj[key].toString());
                else if (jsobj[key].isDouble())
                    ai->setValue(QString::number(qint64(jsobj[key].toDouble())));
                else {
                    // Ignore only the "postdata: null" field for now
                    if (key != "postdata")
                    {
                        qDebug() << "FF import, unknown key type: " << jsobj[key].type();
                        qDebug() << "FF bookmark: " << key << jsobj[key].toString();
                        ai->setValue("?");
                    }
                }

                model->addAttribute(selbi, ai); // FIXME-2 deep copy?
            }
        }

        model->emitDataChanged(selbi); // FIXME-2 required, but can reposition in between be blocked?
    }

    if (jsobj.contains("children") && jsobj["children"].isArray()) {

        QJsonArray jsarr = jsobj["children"].toArray();
        foreach (const QJsonValue &val, jsarr) {
            parseJson (val, mode, selbi);
        }

        if (selbi->depth() > 2) selbi->scroll();    // FIXME-2 reposition?
    } 
    return true;
}
   
/////////////////////////////////////////////////
bool ImportMM::transform()
{
    // try to unzip
    if (File::Success == unzipDir(tmpDir, inputFile)) {

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
