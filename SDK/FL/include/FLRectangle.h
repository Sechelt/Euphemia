#ifndef H_FLRectangle
#define H_FLRectangle

#include <DBitmap.h>

class FLDiagram;
class FLFlow;

class FLRectangle : public DBitmap
{
    Q_OBJECT
public:
    FLRectangle( FLDiagram *pDiagram, const QString &stringName = QString() );
    virtual ~FLRectangle();

    // SETTERS
    virtual void setSelectType();
    virtual void setMacro( const QString &s );

    // GETTERS
    virtual FLFlow *        getFlow( CBD::DirectionTypes nDir );
    virtual FLFlow *        getFlow( const QString &stringSink );
    virtual FLRectangle*    getNext( CBD::DirectionTypes nDir = CBD::DirectionUnknown );
    virtual FLRectangle*    getPrevious( CBD::DirectionTypes nDir = CBD::DirectionUnknown );
    virtual FLRectangle*    getPrevious( const QString &stringInherits );
    virtual QPoint          getCell();
    virtual QString         getMacro() { return stringMacro; }
    virtual AWDataWidget *  getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual AWPropWidget *  getPropWidget( QWidget *pWidgetParent = nullptr );

    // DO'RS
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElem );

    virtual void mousePressHandle( DHandle *pHandle, const QPointF &pointScene );  
    virtual void mouseMoveHandle( DHandle *pHandle, const QPointF &pointScene );   
    virtual void mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene );

public slots:
    virtual void slotDelete();

protected:
    QString stringMacro;

    virtual void doSinksCreate();
    virtual void doSinksSync();
    virtual void doSinksSyncRaw();

    virtual void doMenu( DHandle *pHandle, const QPointF &pointScenePos = QPointF() );
    virtual void doInitMenu();
    virtual void doFiniMenu();
    virtual void doSyncMenu();

    virtual void doInitResize();
};

#endif 

