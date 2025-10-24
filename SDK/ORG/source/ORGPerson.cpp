#include "LibInfo.h"
#include "ORGPerson.h"
    
#include <AWDynamicDataWidget.h>

#include <DGraphicsProxyItem.h>

#include "ORGDiagram.h"
#include "ORGObjectNameWidget.h"
#include "ORGObjectProjectWidget.h"
#include "ORGObjectEmploymentWidget.h"

#define ORGSinkTop 0
#define ORGSinkBottom 1

#define ORGMenuPerson 0

ORGPerson::ORGPerson( ORGDiagram *pDiagram, const QString &stringName )
    : DRectangle( pDiagram, stringName )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

    bDynamicSinks   = false;
    bFixedSize      = true;
    pixmapPhoto     = QPixmap( ":ORG/PersonPhoto" ).scaled( 64, 64 );
     
    // default is 200x100     
    rect.setTopLeft( QPointF( -100, -50 ) );
    rect.setBottomRight( QPointF( 100, 50 ) );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

ORGPerson::~ORGPerson()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

void ORGPerson::setSelectType()
{
    switch ( getSelectType() )
    {
        case SelectResize:
            DRectangleBase::setSelectType( SelectMenu );
            break;
        case SelectTransformRotate:
            break;
        case SelectTransformShear:
            break;
        case SelectMenu:
            DRectangleBase::setSelectType( SelectResize );
            break;
        case SelectOnly:
            break;
    }
}

void ORGPerson::setPhoto( const QPixmap &pixmap ) 
{ 
    // if ( pixmapPhoto == pixmap ) return;
    pixmapPhoto = pixmap;
    setModified();
}

void ORGPerson::setTitle( const QString &s ) 
{ 
    if ( stringTitle == s ) return; 
    stringTitle = s; 
    setModified();
}

void ORGPerson::setRole( const QString &s ) 
{ 
    if ( stringRole == s ) return; 
    stringRole = s; 
    setModified();
}

void ORGPerson::setLocation( const QString &s ) 
{ 
    if ( stringLocation == s ) return; 
    stringLocation = s; 
    setModified();
}

void ORGPerson::setEMail( const QString &s ) 
{ 
    if ( stringEMail == s ) return; 
    stringEMail = s; 
    setModified();
}

void ORGPerson::setPhone( const QString &s ) 
{ 
    if ( stringPhone == s ) return; 
    stringPhone = s; 
    setModified();
}

void ORGPerson::setProjectRole( const QString &s ) 
{ 
    if ( stringProjectRole == s ) return; 
    stringProjectRole = s; 
    setModified();
}

QPixmap ORGPerson::getIcon()
{
    return QPixmap( ":ORG/Person" );
}

ORGPerson *ORGPerson::getSuperior()
{
    QList<DSource*> l = getSink( "Top" )->getSources();
    if ( l.count() < 1 ) return nullptr;

    return ((ORGPerson*)l.first()->getLine()->getRectangle( CBD::EOLBegin ));
}

QList<ORGPerson*> ORGPerson::getSubordinates()
{
    QList<ORGPerson*> listResult;

    QList<DSource*> l = getSink( "Bottom" )->getSources();
    DSource *pSource;
    foreach( pSource, l )
    {
        listResult.append( ((ORGPerson*)pSource->getLine()->getRectangle( CBD::EOLEnd )) );
    }

    return listResult;
}

AWDataWidget *ORGPerson::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = DRectangle::getObjectWidget( pWidgetParent );

    p->removeByClass( "AWObjectNameWidget" );
    
    p->insertTab( 0, new ORGObjectProjectWidget( this, p ), tr("Project") );
    p->insertTab( 0, new ORGObjectEmploymentWidget( this, p ), tr("Employment") );
    p->insertTab( 0, new ORGObjectNameWidget( this, p ), tr("Name") );

    p->setCurrentIndex( 0 );

    return p;
}

AWPropWidget *ORGPerson::getPropWidget( QWidget *pWidgetParent )
{
    AWPropWidget *pPropWidget = DRectangle::getPropWidget( pWidgetParent );

    pPropWidget->removeWidget( tr("Name") );                                                                                 
    pPropWidget->removeWidget( tr("Geometry") );                                                                                 
    pPropWidget->addWidget( tr("Employment"), new ORGObjectEmploymentWidget( this, pPropWidget ) );
    pPropWidget->addWidget( tr("Name"), new ORGObjectNameWidget( this, pPropWidget ) );

    return pPropWidget;

}

QDomElement ORGPerson::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement pdomElem = DRectangle::doSave( pdomDoc, pdomElemParent );

    pdomElem.setAttribute( "ProjectRole", stringProjectRole );
    pdomElem.setAttribute( "Title", stringTitle );
    pdomElem.setAttribute( "Role", stringRole );
    pdomElem.setAttribute( "Location", stringLocation );
    pdomElem.setAttribute( "eMail", stringEMail );
    pdomElem.setAttribute( "Phone", stringPhone );

    ORGDiagram *pDiagram = (ORGDiagram*)getParent( "ORGDiagram" );
    if ( pDiagram->getRoot() == this )
        pdomElem.setAttribute( "isRoot", "1" );

    ulong nBytesCompressed = 0;
    QString stringEncodedImage = CBDPersistUtility::getEncodedImage( pixmapPhoto.toImage(), &nBytesCompressed );
    pdomElem.setAttribute( "ImageSize", QString::number( nBytesCompressed ) );
    pdomElem.setAttribute( "Image", stringEncodedImage );

    return pdomElem;
}

bool ORGPerson::doLoad( QDomElement *pdomElem )
{
    DRectangle::doLoad( pdomElem );

    stringProjectRole   = pdomElem->attribute( "ProjectRole", "" );
    stringTitle         = pdomElem->attribute( "Title", "" );
    stringRole          = pdomElem->attribute( "Role", "" );
    stringLocation      = pdomElem->attribute( "Location", "" );
    stringEMail         = pdomElem->attribute( "eMail", "" );
    stringPhone         = pdomElem->attribute( "Phone", "" );

    ORGDiagram *pDiagram = (ORGDiagram*)getParent( "ORGDiagram" );
    if ( pdomElem->attribute( "isRoot", "0" ).toInt() && !pDiagram->getRoot() )
        pDiagram->setRoot( this );

    QString stringAttribute = pdomElem->attribute( "ImageSize" );
    if ( !stringAttribute.isNull() )
    {
        ulong nBytesCompressed = stringAttribute.toULong();

        stringAttribute = pdomElem->attribute( "Image" );
        if ( !stringAttribute.isNull() )
        {
            pixmapPhoto = QPixmap::fromImage(CBDPersistUtility::getDecodedImage( stringAttribute, nBytesCompressed ) );
        }
    }

    return true;
}

void ORGPerson::mousePressHandle( DHandle *pHandle, const QPointF &pointScene )   
{
    Q_UNUSED(pHandle);
    Q_UNUSED(pointScene);                                                                                          
}                                                                                          
                                                                                           
void ORGPerson::mouseMoveHandle( DHandle *pHandle, const QPointF &pointScene )    
{                                                                                          
    Q_UNUSED(pHandle);
    Q_UNUSED(pointScene);                                                                                          
}                                                                                          
                                                                                           
void ORGPerson::mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene ) 
{                                                                                          
    Q_UNUSED(pointScene);

    if ( pHandle->getFunction() != DHandle::FunctionMenu ) return;

    doMenu( pHandle );
}                                                                                          

void ORGPerson::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{
    Q_UNUSED(nOption);
    Q_UNUSED(pWidget);

    pPainter->setPen( pen );
    pPainter->setBrush( brush );
    pPainter->setFont( font.font );

    pPainter->drawRect( rect );

    // \note: specifying rect in the following truncates text outside of the rect
    //        May want to make this an option/property
    pPainter->setPen( QPen( font.color ) );

    QRectF rectPhoto    = QRectF( rect.topLeft().x(), rect.topLeft().y(), rect.width() / 3, rect.height() );
    QRectF rectInfo     = QRectF( rectPhoto.topRight().x(), rectPhoto.topRight().y(), rect.width() / 3 * 2, rect.height() );

    // Photo
    pPainter->drawPixmap( rectPhoto.topLeft(), pixmapPhoto );

    // Info
    int nLineHeight = pPainter->fontMetrics().height() + 2;
    QRectF rectLine( rectInfo.left() + 2, rectInfo.top(), rectInfo.width() - 2, nLineHeight );
    pPainter->drawText( rectLine, Qt::AlignLeft, getName() );

    rectLine.moveCenter( QPointF( rectLine.center().x(), rectLine.center().y() + nLineHeight ) );
    pPainter->drawText( rectLine, Qt::AlignLeft, getTitle() );

    // avoid further details if person has no role in project 
    if ( getProjectRole().isEmpty() ) return;

    rectLine.moveCenter( QPointF( rectLine.center().x(), rectLine.center().y() + nLineHeight ) );
    pPainter->drawText( rectLine, Qt::AlignLeft, getProjectRole() );

    rectLine.moveCenter( QPointF( rectLine.center().x(), rectLine.center().y() + nLineHeight ) );
    pPainter->drawText( rectLine, Qt::AlignLeft, getEMail() );

    rectLine.moveCenter( QPointF( rectLine.center().x(), rectLine.center().y() + nLineHeight ) );
    pPainter->drawText( rectLine, Qt::AlignLeft, getPhone() );
}
                                                                                          
void ORGPerson::slotDelete()
{
qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " ";
}

void ORGPerson::doSinksCreate()
{
    vectorSinks.resize( 2 );
    vectorSinks[ORGSinkTop] = new DSink( this, "Top" );
    vectorSinks[ORGSinkBottom] = new DSink( this, "Bottom" );
    doSinksSync();
}

void ORGPerson::doSinksSync()
{
    if ( !getProxy() ) return;
    if ( vectorSinks.count() == 0 ) return;

    vectorSinks[ORGSinkTop]->setPos( getProxy()->mapToScene( QPointF( 0, rect.top() + 1 ) ) );
    vectorSinks[ORGSinkBottom]->setPos( getProxy()->mapToScene( QPointF( 0, rect.bottom() - 1 ) ) );
}

void ORGPerson::doSinksSyncRaw()
{
    if ( !pProxy ) return;
    if ( vectorSinks.count() == 0 ) return;

    vectorSinks[ORGSinkTop]->setPosRaw( getProxy()->mapToScene( QPointF( 0, rect.top() + 1 ) ) );
    vectorSinks[ORGSinkBottom]->setPosRaw( getProxy()->mapToScene( QPointF( 0, rect.bottom() - 1 ) ) );
}

void ORGPerson::doMenu( DHandle *pHandle, const QPointF &pointScenePos )
{
    Q_UNUSED(pointScenePos);

    if ( pHandle == vectorHandles.at( ORGMenuPerson ) )                                            
    {                                                                                           
        ORGDiagram *pDiagram = (ORGDiagram*)getParent( "ORGDiagram" );                               
        pDiagram->doInsert( "ORGPerson", this );
    }                                                                                           
}

void ORGPerson::doInitMenu()
{
    if ( vectorHandles.count() > 0 ) return;
    if ( !getProxy() ) return;

    vectorHandles.resize( 3 );

    DHandle *pHandle;

    pHandle = new DHandle( this, DHandle::FunctionMenu, DHandle::ConnectTypeNone, getProxy()->mapToScene( rect.topRight() ) );
    pHandle->setPixmap( QIcon( ":ORG/Person" ).pixmap( 16, 16 ) );
    vectorHandles[ORGMenuPerson] = pHandle;
    getProxy()->scene()->addItem( pHandle );
}


void ORGPerson::doFiniMenu()
{
    qDeleteAll( vectorHandles );
    vectorHandles.clear();
}

void ORGPerson::doSyncMenu()
{
    if ( vectorHandles.count() < 1 ) return;
    if ( !getProxy() ) return;

    DHandle *pHandle;

    pHandle = vectorHandles[ORGMenuPerson];
    pHandle->setPos( pProxy->mapToScene( rect.topRight() ) );
}

void ORGPerson::doInitResize()
{
    if ( vectorHandles.count() > 0 ) return;
    if ( !getProxy() ) return;

    vectorHandles.resize( 4 );

    DHandle *pHandle;

    pHandle = new DHandle( this, DHandle::FunctionSelectTopLeft, DHandle::ConnectTypeNone, getProxy()->mapToScene( rect.topLeft() ) );
    vectorHandles[CBD::RectangleTopLeft] = pHandle;
    getProxy()->scene()->addItem( pHandle );

    pHandle = new DHandle( this, DHandle::FunctionSelectTopRight, DHandle::ConnectTypeNone, getProxy()->mapToScene( rect.topRight() ) );
    vectorHandles[CBD::RectangleTopRight] = pHandle;
    getProxy()->scene()->addItem( pHandle );

    pHandle = new DHandle( this, DHandle::FunctionSelectBottomRight, DHandle::ConnectTypeNone, getProxy()->mapToScene( rect.bottomRight() ) );
    vectorHandles[CBD::RectangleBottomRight] = pHandle;
    getProxy()->scene()->addItem( pHandle );

    pHandle = new DHandle( this, DHandle::FunctionSelectBottomLeft, DHandle::ConnectTypeNone, getProxy()->mapToScene( rect.bottomLeft() ) );
    vectorHandles[CBD::RectangleBottomLeft] = pHandle;
    getProxy()->scene()->addItem( pHandle );
}



