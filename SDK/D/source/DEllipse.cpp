#include "LibInfo.h"
#include "DEllipse.h"
    
#include "DGraphicsProxyItem.h"

#include "DDiagram.h"

DEllipse::DEllipse( ADObject *pObjectParent, const QString &stringName )
    : DRectangle( pObjectParent, stringName )
{
}

DEllipse::~DEllipse()
{
}

QPixmap DEllipse::getIcon()
{
    return QPixmap( ":D/Ellipse" );
}

void DEllipse::paintAt( QPainter *pPainter, const QPointF &pointPos )
{
    pPainter->setPen( pen );
    pPainter->setBrush( brush );
    pPainter->setFont( font.font );

    QRectF r;
    r.setTopLeft( pointPos );
    r.setSize( rect.size() );

    pPainter->drawEllipse( r );

    pPainter->setPen( font.color );
    switch ( nTextDisplay )
    {
        case TextDisplayName:
            pPainter->drawText( r, getTextFlags(), getName() );
            break;
        case TextDisplayCode:
            pPainter->drawText( r, getTextFlags(), getCode() );
            break;
        case TextDisplayComment:
            pPainter->drawText( r, getTextFlags(), getComment() );
            break;
        case TextDisplayNone:
            break;
    }
}


