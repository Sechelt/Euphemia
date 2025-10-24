#ifndef H_PMTableBase
#define H_PMTableBase

#include <AWDataTable.h>
#include "PMRectangle.h"

#include "PMObject.h"
#include "PMModel.h"
#include "PMDDLManager.h"

// stuff we can create
class PMColumn;
class PMKey;

class PMTableBase : public PMRectangle, public PMDDLManager
{
    Q_OBJECT
public:
    PMTableBase( PMModel *pParent );
    virtual ~PMTableBase();

    // SETTERS
    virtual void            setGenerate( bool b )            { bGenerate       = b;}
    virtual void            setRecords( const QString &s)    { stringRecords   = s;}

    // GETTERS              
    virtual QPixmap             getIcon() = 0;
    virtual QList<PMColumn*>    getColumns();
    virtual QList<PMKey*>       getKeys();
    virtual QList<PMKey*>       getAlternateKeys();
    virtual PMKey *             getPrimaryKey();
    virtual QList<PMColumn*>    getForeignKeyColumns();
    virtual PMColumn *          getColumn( const QString &stringName );
    virtual bool                getGenerate()      { return bGenerate;}
    virtual QString             getRecords()       { return stringRecords;}
    virtual AWPropWidget *      getPropWidget( QWidget *pWidgetParent = nullptr );
    virtual QString             getCreateAuto() = 0;
    virtual QString             getDropAuto() = 0;

    // DO'RS
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool        doLoad( QDomElement *pdomElemTable );

    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );

public slots:
    virtual void slotDescription( const QString &s )    { setDescription( s );  }
    virtual void slotAnnotation( const QString &s )     { setAnnotation( s );   }    
    virtual void slotDelete();

protected:
    bool        bGenerate;
    QString     stringRecords;  // estimated number of records expected

    // SETTERS

    // GETTERS
    virtual QSizeF  getOptimalSize();
    virtual void    getColumnWidths( QPainter *pPainter, int *pnName, int *pnDataType, int *pnKey );

    // DO'RS
    virtual void doLoadDefaults();
    virtual void doGoToView( ADObjectPathList listObjectPathItems, uint nDepth );
};

#endif 


