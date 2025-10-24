#ifndef H_DTitleDoc
#define H_DTitleDoc

#include "DRectangle.h"

/*!
 * \brief Title box which shows the document's; Name, Code, Comment, Author, Version, Created and Modified. 
 *  
 * This ignores its own Name, Code, and Comment. 
 * 
 * \author pharvey (5/7/20)
 */
class DTitleDoc : public DRectangle
{
    Q_OBJECT
public:
    DTitleDoc( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DTitleDoc();

    virtual QPixmap         getIcon();
    virtual QString         getFriendlyClassName() { return tr( "Title Doc" ); }
    virtual AWPropWidget *  getPropWidget( QWidget *pWidgetParent = nullptr );
    virtual AWDataWidget *  getObjectWidget( QWidget *pWidgetParent );

    virtual void paintAt( QPainter *pPainter, const QPointF &pointPos );
};

#endif 

