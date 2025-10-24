#include "LibInfo.h"
#include "LMAssociativeLink.h"

#include "LMAssociativeLinkWidget.h"

#include "LMModel.h" 

LMAssociativeLink::LMAssociativeLink( LMModel *pmodelParent )
    : LMRelationship( pmodelParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    ComposedFlagEnd.setFlag( ComposedPerpendicular );
    stringEndCardinality = "1,1";

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

LMAssociativeLink::~LMAssociativeLink()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

/*!
    getIcon
    
*/
QPixmap LMAssociativeLink::getIcon()
{
    return QPixmap( ":LM/AssociationLink16x16" );
}

/*!
 * \brief Creates a tab widget with a number of tabs for editing/viewing object properties. 
 *  
 * We replace LMRelationshipWidget with LMAssociativeLinkWidget.
 *  
 * \author pharvey (5/12/20)
 * 
 * \param pWidgetParent 
 * 
 * \return AWDataWidget* 
 */
AWDataWidget *LMAssociativeLink::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = LMRelationship::getObjectWidget( pWidgetParent );

    // replace LMRelationshipWidget with LMAssociativeLinkWidget
    p->removeByClass( "LMRelationshipWidget" );
    p->insertTab( 1, new LMAssociativeLinkWidget( this, Qt::Horizontal, p ), tr("Relationship") );

    p->setCurrentIndex( 0 );

    return p;
}

AWPropWidget *LMAssociativeLink::getPropWidget( QWidget *pWidgetParent )
{
    AWPropWidget *pPropWidget = LMRelationship::getPropWidget( pWidgetParent );

    // replace 'Relationship'
    pPropWidget->removeWidget( tr("Relationship") );                                                                                 
    {                       
        LMAssociativeLinkWidget *p = new LMAssociativeLinkWidget( this, Qt::Vertical, pPropWidget );
        pPropWidget->addWidget( tr("Relationship"), p );                                                            
    }                                                                                                                

    return pPropWidget;

}

LMEntityNormal * LMAssociativeLink::getEntityNormal()
{
    LMEntityBase *pentitybase = getChildEntityObject();
    if ( pentitybase && pentitybase->metaObject()->className() == QString( "LMEntityNormal" ) )
        return(LMEntityNormal*)pentitybase;

    pentitybase = getParentEntityObject();
    if ( pentitybase && pentitybase->metaObject()->className() == QString( "LMEntityNormal" ) )
        return(LMEntityNormal*)pentitybase;

    return 0;
}

LMEntityAssociative * LMAssociativeLink::getEntityAssociative()
{
    LMEntityBase *pentitybase = getChildEntityObject();
    if ( pentitybase && pentitybase->metaObject()->className() == QString( "LMEntityAssociative" ) )
        return(LMEntityAssociative*)pentitybase;

    pentitybase = getParentEntityObject();
    if ( pentitybase && pentitybase->metaObject()->className() == QString( "LMEntityAssociative" ) )
        return(LMEntityAssociative*)pentitybase;

    return 0;
}

void LMAssociativeLink::doLoadDefaults()
{
/*
    CBDSettings         settings;                                                 
    QString             stringPath = "/CodeByDesign/LM/Defaults/AssociativeLink/";
    QPen                pen;                                                      
    QBrush              brush;                                                    
                                                                                  
    pen.setColor( "#0000ff" );                                                    
    brush.setColor( "#9932cc" );                                                  
    brush.setStyle( Qt::SolidPattern );                                           
    pen = settings.readLinePrefEntry( stringPath, pen );                          
    brush = settings.readFillPrefEntry( stringPath, brush);                       
                                                                                  
    setPen(pen);                                                                  
    setBrush( brush );                                                            
    setDefaultsLoaded( true );                                                    
*/
}

