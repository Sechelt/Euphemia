#include "LibInfo.h"
#include "DTitle.h"

#include "DDiagram.h"

DTitle::DTitle( ADObject *pObjectParent, const QString &stringName )
    : DRectangle( pObjectParent, stringName )
{
    setObjectName( "%NAME%" );
    this->stringName= "%NAME%";
    stringCode      = "%CODE%";

    stringComment = tr("The following keywords can be used. Just brace them with % and they will be substituted.\n\n");
    stringComment += "AUTHOR,DESC,NAME,CODE,MDATETIME,MDATE,MTIME,CDATETIME,CDATE,CTIME,DATETIME,DATE,TIME";
}

DTitle::~DTitle()
{
}

QPixmap DTitle::getIcon()
{
    return QPixmap( ":D/Title" );
}

void DTitle::paintAt( QPainter *pPainter, const QPointF &pointPos )
{
    pPainter->setPen( pen );
    pPainter->setBrush( brush );
    pPainter->setFont( font.font );

    QRectF rect;
    rect.setTopLeft( pointPos );
    rect.setSize( this->rect.size() );

    QRectF rectTopLeft( rect.left(), rect.top(), rect.width() / 2, rect.height() / 4 );
    QRectF rectTopRight( rectTopLeft.right(), rect.top(), rect.width() / 2, rectTopLeft.height() );
    QRectF rectBottom( rect.left(), rect.top() + rect.height() / 4, rect.width(), rect.height() - rect.height() / 4 );

    pPainter->drawRect( rectTopLeft );
    pPainter->drawRect( rectTopRight );
    pPainter->drawRect( rectBottom );

    // substitute macros
    QString stringTopLeftMod    = getFieldValue( getName() );
    QString stringTopRightMod   = getFieldValue( getCode() );
    QString stringBottomMod     = getFieldValue( getComment() );

    pPainter->setPen( font.color );
    pPainter->drawText( rectTopLeft, stringTopLeftMod );
    pPainter->drawText( rectTopRight, stringTopRightMod );
    pPainter->drawText( rectBottom, getTextFlags(), stringBottomMod );
}

QString DTitle::getFieldValue( const QString &s )
{
    QString stringValue = s;
    if ( s.isEmpty() ) return s;

    DDiagram *pDiagram = getDiagram();
    Q_ASSERT( pDiagram );

    int nIndex = 0;
    nIndex = stringValue.indexOf("%AUTHOR%");
    if ( nIndex >= 0 ) stringValue.replace( nIndex, 8, pDiagram->getAuthor());

    nIndex = stringValue.indexOf("%DESC%");
    if ( nIndex >= 0 ) stringValue.replace( nIndex, 6, pDiagram->getComment());

    nIndex = stringValue.indexOf("%NAME%");
    if ( nIndex >= 0 ) stringValue.replace( nIndex, 6, pDiagram->getName());

    nIndex = stringValue.indexOf("%CODE%");
    if ( nIndex >= 0 ) stringValue.replace( nIndex, 6, pDiagram->getCode());

    nIndex = stringValue.indexOf("%MDATETIME%");
    if ( nIndex >= 0 ) stringValue.replace( nIndex, 11, pDiagram->getModifiedDate().toString("dd.MM.yyyy  hh:mm:ss ap") );

    nIndex = stringValue.indexOf("%MDATE%");
    if ( nIndex >= 0 ) stringValue.replace( nIndex, 7, pDiagram->getModifiedDate().toString("dd.MM.yyyy") );

    nIndex = stringValue.indexOf("%MTIME%");
    if ( nIndex >= 0 ) stringValue.replace( nIndex, 7, pDiagram->getModifiedDate().toString("hh:mm:ss ap") );

    nIndex = stringValue.indexOf("%CDATETIME%");
    if ( nIndex >= 0 ) stringValue.replace( nIndex, 11, pDiagram->getCreatedDate().toString("dd.MM.yyyy  hh:mm:ss ap") );

    nIndex = stringValue.indexOf("%CDATE%");
    if ( nIndex >= 0 ) stringValue.replace( nIndex, 7, pDiagram->getCreatedDate().toString("dd.MM.yyyy") );

    nIndex = stringValue.indexOf("%CTIME%");
    if ( nIndex >= 0 ) stringValue.replace( nIndex, 7, pDiagram->getCreatedDate().toString("hh:mm:ss ap") );

    // if ( nIndex >= 0 ) stringValue.replace( nIndex, 8, Author );

    nIndex = stringValue.indexOf("%DATETIME%");
    if ( nIndex >= 0 ) stringValue.replace( nIndex, 10, QDateTime::currentDateTime().toString("dd.MM.yyyy  hh:mm:ss ap") );

    nIndex = stringValue.indexOf("%DATE%");
    if ( nIndex >= 0 ) stringValue.replace( nIndex, 6, QDate::currentDate().toString("dd.MM.yyyy") );

    nIndex = stringValue.indexOf("%TIME%");
    if ( nIndex >= 0 ) stringValue.replace( nIndex, 6, QTime::currentTime().toString("hh:mm:ss ap") );

    return stringValue;
}


