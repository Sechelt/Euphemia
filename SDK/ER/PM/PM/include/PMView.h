#ifndef H_PMView
#define H_PMView

// #include <AWDataTable.h>
#include "PMRectangle.h"

#include "PMObject.h"
#include "PMModel.h"
#include "PMDDLManager.h"

class PMView : public PMRectangle, public PMDDLManager
{
    Q_OBJECT
public:
    PMView( PMModel *pParent );
    virtual ~PMView();

    // SETTERS
    virtual void setDef( const QString &s );

    // GETTERS              
    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "View" ); }
    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual ADObject *          getObject( const QString &s, ADObject *p = nullptr );
    virtual QString             getDef() { return stringDef; }
    virtual QString             getCreateAuto();
    virtual QString             getDropAuto();
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses );

    // DO'RS
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool        doLoad( QDomElement *pdomElemTable );
    virtual void        doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );

    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );

public slots:
    virtual void slotDescription( const QString &s )    { setDescription( s );  }
    virtual void slotAnnotation( const QString &s )     { setAnnotation( s );   }    

protected:
    QString stringDef;

    virtual QSizeF  getOptimalSize();
    virtual void    getColumnWidths( QPainter *pPainter, int *pnName );

    virtual void doParse();
    virtual void doGoToView( ADObjectPathList listObjectPathItems, uint nDepth );
};

#endif 


