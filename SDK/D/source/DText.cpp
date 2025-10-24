#include "LibInfo.h"
#include "DText.h"

#include "DDiagram.h"

DText::DText( ADObject *pObjectParent, const QString &stringName )
    : DRectangle( pObjectParent, stringName )
{
    // the difference between us and DRectangle is that we have defaults
    // which make the rectangle invisible and display "text" when no other text
    pen.setStyle( Qt::NoPen );
    brush.setStyle( Qt::NoBrush );
}

DText::~DText()
{
}

QPixmap DText::getIcon()
{
    return QPixmap( ":D/Text" );
}

/*!
 * \brief Text without a box. 
 *  
 * Very similar to DRectangle but here we never draw the rect and we always draw 
 * something for the text. 
 * 
 * \author pharvey (11/24/19)
 * 
 * \param pPainter 
 * \param pOption 
 * \param pWidget 
 */
void DText::paintAt( QPainter *pPainter, const QPointF &pointPos )
{
    pPainter->setPen( pen );
    pPainter->setBrush( brush );
    pPainter->setFont( font.font );

    QRectF rect;
    rect.setTopLeft( pointPos );
    rect.setSize( this->rect.size() );

    pPainter->drawRect( rect );

    pPainter->setPen( font.color );
    switch ( nTextDisplay )
    {
        case TextDisplayName:
            if ( getName().isEmpty() )
                pPainter->drawText( rect, getTextFlags(), tr("text") );
            else
                pPainter->drawText( rect, getTextFlags(), getName() );
            break;
        case TextDisplayCode:
            if ( getCode().isEmpty() )
                pPainter->drawText( rect, getTextFlags(), tr("text") );
            else
                pPainter->drawText( rect, getTextFlags(), getCode() );
            break;
        case TextDisplayComment:
            if ( getComment().isEmpty() )
                pPainter->drawText( rect, getTextFlags(), tr("text") );
            else
                pPainter->drawText( rect, getTextFlags(), getComment() );
            break;
        case TextDisplayNone:
            pPainter->drawText( rect, getTextFlags(), tr("text") );
            break;
    }
}


