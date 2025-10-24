#ifndef H_LMEntityBase
#define H_LMEntityBase

#include "LMRectangle.h"
// #include "LMModel.h"

class LMModel;
class LMAttribute;
class LMRelationship;
class LMAssociativeLink;
class LMDataItem;

class LMEntityBase : public LMRectangle
{
    Q_OBJECT
public:
    LMEntityBase( LMModel *pmodel );
    virtual ~LMEntityBase();

    // SETTERS
    virtual void            setGenerate( bool b )            { bGenerate       = b;}
    virtual void            setRecords( const QString &s)    { stringRecords   = s;}

    // GETTERS              
    virtual QPixmap         getIcon() = 0;
    virtual AWDataWidget *  getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual AWPropWidget *  getPropWidget( QWidget *pWidgetParent = nullptr );
    virtual QStringList     getAttributeNames();
    virtual QStringList     getIdentifierNames();
    virtual LMAttribute *   getAttribute( const QString &stringName );
    virtual bool            getGenerate()      { return bGenerate;}
    virtual QString         getRecords()       { return stringRecords;}
    virtual ADObject *      getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    // DO'RS
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool        doLoad( QDomElement *pdomElemTable );

    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );

public slots:
    virtual void slotDescription( const QString &s )    { setDescription( s );  }
    virtual void slotAnnotation( const QString &s )     { setAnnotation( s );   }    

protected:
    bool        bGenerate;
    QString     stringRecords;  // estimated number of records expected

    virtual QSizeF  getOptimalSize();
    virtual void    getAttributeWidths( QPainter *pPainter, int *pnName, int *pnDataType, int *pnKey );

    virtual void doGoToView( ADObjectPathList listObjectPathItems, uint nDepth );
};

#endif 


