#ifndef SCRIPTING_H
#define SCRIPTING_H

#include <QColor>
#include <QObject>
#include <QScriptContext>
#include <QScriptable>
#include <QScriptValue>

class BranchItem;
class VymModel;


void logError(QScriptContext *context, QScriptContext::Error error, const QString &text);

class VymModelWrapper : public QObject, protected QScriptable
{
    Q_OBJECT
public:
    VymModelWrapper (VymModel* m);
private:
    BranchItem* getSelectedBranch();
    QVariant getParameter( bool &ok, const QString &key, const QStringList &parameters);

public slots:
    void addBranch();
    void addBranchBefore();
    void addMapCenter( qreal x, qreal y);
    void addMapInsert( QString filename, int pos, int contentFilter);
    void addMapInsert( const QString &filename, int pos);
    void addMapInsert( const QString &filename);
    void addMapReplace( QString filename);
    void addSlide();
    void addXLink( const QString &begin, const QString &end, int width, const QString &color, const QString &penstyle);
    int branchCount();
    int centerCount();
    void centerOnID( const QString &id);        // tested: ok
    void clearFlags();
    void colorBranch( const QString &color);
    void colorSubtree( const QString &color);
    void copy();
    void cut();
    void cycleTask();
    bool exportMap( const QString &format, const QStringList &parameters);
    QString getDestPath();
    QString getFileDir();
    QString getFileName();
    QString getFrameType();
    QString getHeadingPlainText();
    QString getHeadingXML();
    QString getMapAuthor();
    QString getMapComment();
    QString getMapTitle();
    QString getNotePlainText();
    QString getNoteXML();
    QString getSelectString();
    int getTaskSleepDays();
    QString getURL();
    QString getVymLink();
    QString getXLinkColor();
    int getXLinkWidth();
    QString getXLinkPenStyle();
    QString getXLinkStyleBegin();
    QString getXLinkStyleEnd();
    bool hasActiveFlag ( const QString &flag);
    bool hasNote();
    bool hasRichTextNote();
    bool hasTask();
    void importDir( const QString &path);
    bool isScrolled();
    void loadImage( const QString &filename);
    void loadNote(  const QString &filename);
    void move( qreal x, qreal y);
    void moveRel( qreal x, qreal y);
    void moveDown();
    void moveUp();
    void moveSlideDown( int n) ;
    void moveSlideDown();
    void moveSlideUp( int n );
    void moveSlideUp();
    void nop();
    void note2URLs();
    void parseVymText(const QString &text);
    void paste();
    void redo();
    bool relinkTo( const QString &parent, int num, qreal x, qreal y);
    bool relinkTo( const QString &parent, int num);
    bool relinkTo( const QString &parent);
    void remove();
    void removeChildren();
    void removeKeepChildren();
    void removeSlide(int n);
    void saveImage( const QString &filename, const QString &format); 
    void saveNote( const QString &filename); 
    void scroll();
    bool select( const QString &s );
    bool selectID( const QString &s );
    bool selectFirstBranch();
    bool selectLastBranch();
    bool selectLastImage();
    bool selectParent();
    bool selectLatestAdded();
    void setFlag( const QString &s);
    void setHeadingPlainText( const QString &s );
    void setHideExport( bool b);
    void setHideLinkUnselected( bool b);
    void setIncludeImagesHorizontally( bool b);
    void setIncludeImagesVertically( bool b);
    void setMapAnimCurve( int n );
    void setMapAnimDuration( int n );
    void setMapAuthor( const QString &s);
    void setMapBackgroundColor( const QString &color);
    void setMapComment( const QString &s);
    void setMapDefLinkColor( const QString &color);
    void setMapLinkStyle( const QString &style);
    void setMapRotation( float a);              // tested: ok
    void setMapTitle( const QString &s);
    void setMapZoom ( float z);                 // tested: ok
    void setNotePlainText( const QString &s );
    void setFrameBorderWidth( int border);
    void setFrameBrushColor( const QString &color);
    void setFrameIncludeChildren( bool b );
    void setFramePadding( int padding);
    void setFramePenColor( const QString &color);
    void setFrameType( const QString &type);
    void setScale( qreal x, qreal y );
    void setSelectionColor( const QString &color);
    bool setTaskSleep( const QString &s);
    void setURL( const QString &s);
    void setVymLink( const QString &s );
    void setXLinkColor( const QString &color);
    void setXlinkLineStyle( const QString &style);
    void setXlinkStyleBegin( const QString &style);
    void setXlinkStyleEnd( const QString &style);
    void setXlinkWidth( int w );
    void sleep( int n);
    void sortChildren( bool b);
    void sortChildren();
    void toggleFlag( const QString &s );
    void toggleFrameIncludeChildren();
    void toggleScroll();
    void toggleTarget();
    void toggleTask();
    void undo();
    bool unscroll();
    void unscrollChildren();
    void unselectAll();
    void unsetFlag( const QString &s);

private:
    VymModel *model;
};

///////////////////////////////////////////////////////////////////////////
class VymWrapper : public QObject, protected QScriptable
{
    Q_OBJECT
public:
    VymWrapper ();

public slots:
    void toggleTreeEditor();
    QObject* getCurrentMap();
    void selectMap (uint n);
    bool loadMap( const QString &filename);
};


#endif
