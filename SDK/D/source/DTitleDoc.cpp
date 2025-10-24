#include "LibInfo.h"
#include "DTitleDoc.h"

#include <WText.h>
#include <WBrush.h>
#include <WPen.h>

#include <AWPropObjectWidget.h>
#include <AWDocumentNameWidget.h>
#include <AWObjectChildrenWidget.h>
#include <AWObjectUsesWidget.h>
#include <AWObjectUsedByWidget.h>

#include "DDiagram.h"

DTitleDoc::DTitleDoc( ADObject *pObjectParent, const QString &stringName )
    : DRectangle( pObjectParent, stringName )
{
}

DTitleDoc::~DTitleDoc()
{
}

QPixmap DTitleDoc::getIcon()
{
    return QPixmap( ":D/Title" );
}

/*!
 * \brief Provides a Property Widget.
 * 
 * The unusual thing that happens here is that we provide a AWPropObjectWidget for the diagram. 
 * This is because we use the document/diagram name fields - not our own. 
 *  
 * \author pharvey (5/7/20)
 * 
 * \param pWidgetParent 
 * 
 * \return AWPropWidget* 
 */
AWPropWidget *DTitleDoc::getPropWidget( QWidget *pWidgetParent )
{
    // create the tool box
    AWPropWidget *pPropWidget = DRectangle::getPropWidget( pWidgetParent );

    pPropWidget->removeWidget( tr("Name") );    // we do not want to use DObject for this 
    pPropWidget->removeWidget( tr("Text") );    // we do not care about text align etc

    // lets use diagram for this
    pPropWidget->addWidget( tr("Name"), new AWPropObjectWidget( getDiagram(), pPropWidget ) );

    return pPropWidget;
}

/*!
 * \brief Provides an Object Widget. 
 *  
 * The unusual thing that happens here is that we provide a AWDocumentNameWidget for the diagram. 
 * This is because we use our DDiagram fields - not our own. 
 *  
 * \author pharvey (5/7/20)
 * 
 * \param pWidgetParent 
 * 
 * \return AWDataWidget* 
 */
AWDataWidget *DTitleDoc::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = AWObject::getObjectWidget( pWidgetParent );

    // p->removeByClass( "AWObjectNameWidget" );

    // p->insertTab( 0, new AWDocumentNameWidget( getDiagram(), p, getDiagram()->getFileName() ), tr("Document") );

    // p->setCurrentIndex( 0 );

    return p;
}

/*!
 * \brief Paints a rectangle with 7 fields. 
 *  
 * The fields are layed out as; 
 *  
 * +-----------------------+--------------------+ 
 * | Name (1)              | Code (2)           |
 * +-----------------------+--------------------+ 
 * |                                            |
 * | Comment (3)                                |
 * |                                            |
 * +-----------------------+--------------------+ 
 * | Author (4)            | Version (5)        |
 * +-----------------------+--------------------+ 
 * | Created (6)           | Modified (7)       |
 * +-----------------------+--------------------+ 
 * 
 * \author pharvey (5/7/20)
 * 
 * \param pPainter 
 * \param nOption 
 * \param pWidget 
 */
void DTitleDoc::paintAt( QPainter *pPainter, const QPointF &pointPos )
{
    DDiagram *pDiagram = getDiagram();
    Q_ASSERT( pDiagram );

    pPainter->setPen( pen );
    pPainter->setBrush( brush );
    pPainter->setFont( font.font );

    int nLineHeight = pPainter->fontMetrics().height() + 2;

    QRectF rect;
    rect.setTopLeft( pointPos );
    rect.setSize( this->rect.size() );

    QRectF rect1;
    QRectF rect2;
    QRectF rect3;
    QRectF rect4;
    QRectF rect5;
    QRectF rect6;
    QRectF rect7;

/*
    +----------------------+--------------------------+
    | 1                    | 2                        |
    +----------------------+--------------------------+
    |                                                 |
    | 3                                               |
    |                                                 |
    +----------------------+--------------------------+
    | 4                    | 5                        |
    +----------------------+--------------------------+
    | 6                    | 7                        |
    +----------------------+--------------------------+
*/

    rect1 = QRectF( rect.left(), rect.top(), rect.width() / 2, nLineHeight );
    rect2 = QRectF( rect1.right(), rect.top(), rect.width() / 2, nLineHeight );
    rect4 = QRectF( rect.left(), rect.bottom() - nLineHeight * 2, rect.width() / 2, nLineHeight );
    rect5 = QRectF( rect4.right(), rect.bottom() - nLineHeight * 2, rect.width() / 2, nLineHeight );
    rect6 = QRectF( rect.left(), rect.bottom() - nLineHeight, rect.width() / 2, nLineHeight );
    rect7 = QRectF( rect6.right(), rect.bottom() - nLineHeight, rect.width() / 2, nLineHeight );
    // remaining space (if any) is for 3
    rect3 = QRectF( rect.left(), rect1.bottom(), rect.width(), rect4.top() - rect1.bottom() );

    pPainter->drawRect( rect1 );
    pPainter->drawRect( rect2 );
    pPainter->drawRect( rect3 );
    pPainter->drawRect( rect4 );
    pPainter->drawRect( rect5 );
    pPainter->drawRect( rect6 );
    pPainter->drawRect( rect7 );

    pPainter->setPen( font.color );
    pPainter->drawText( rect1, pDiagram->getName() );
    pPainter->drawText( rect2, pDiagram->getCode() );
    pPainter->drawText( rect3, getTextFlags(), pDiagram->getComment() );
    pPainter->drawText( rect4, pDiagram->getAuthor() );
    pPainter->drawText( rect5, pDiagram->getVer() );
    pPainter->drawText( rect6, pDiagram->getCreatedDate().toString() );
    pPainter->drawText( rect7, pDiagram->getModifiedDate().toString() );
}


