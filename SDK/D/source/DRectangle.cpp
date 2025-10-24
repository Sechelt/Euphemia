#include "LibInfo.h"
#include "DRectangle.h"

#include <WTextVAlignComboBox.h>
#include <WTextHAlignComboBox.h>

#include <ADTransaction.h>

#include "DDiagram.h"
#include "DLine.h"
#include "DGraphicsProxyItem.h"
#include "DGraphicsItemLine.h"

#include "DSinksWidget.h"
#include "DRectangleWidget.h"

// rotate
#define DRECTANGLE_HANDLE_BASE 0
#define DRECTANGLE_HANDLE_ROTATE 1
#define DRECTANGLE_LINE_ROTATE 0
// shear
#define DRECTANGLE_HANDLE_SHEARH 0
#define DRECTANGLE_HANDLE_SHEARV 1
#define DRECTANGLE_LINE_SHEAR_HL 0
#define DRECTANGLE_LINE_SHEAR_HR 1
#define DRECTANGLE_LINE_SHEAR_VT 2
#define DRECTANGLE_LINE_SHEAR_VB 3

DRectangle::DRectangle( ADObject *pObjectParent, const QString &stringName )
    : DRectangleBase( pObjectParent, stringName )
{
    // \NOTE Sinks can not be created in here as they need proxy. 
    //       Sinks are created once... in setProxy()... which should be called right after adding to scene.
    //       Sink handles are not stored with other handles - they are stored in the sink.
    //       Sinks create their handle when setSelected( true ).
    //       Sinks delete their handle when setSelected( false ).
    //       Sinks are deleted in destructor.
    //      
    //      \sa DSink
    //      \sa setProxy()
    //      \sa setSelected()

    nTextVAlign     = Qt::AlignVCenter;
    nTextHAlign     = Qt::AlignLeft;
    bWordWrap       = true;
    nAngle          = 0;   
    nShearH         = 0;  
    nShearV         = 0;  
    bDynamicSinks   = true;
    bSinksVisible   = false;
    bInitTransform  = false;
    bFixedSize      = false;
}

DRectangle::~DRectangle()
{
    doSinksDelete();
}

void DRectangle::setName( const QString &s )                   
{
    DObject::setName( s );    
    
    if ( getProxy() && getTextDisplay() == TextDisplayName )   
        getProxy()->update();                                  
}                                                              
                                                               
void DRectangle::setCode( const QString &s )                   
{                                                              
    DObject::setCode( s );                                     
    if ( getProxy() && getTextDisplay() == TextDisplayCode )   
        getProxy()->update();                                  
}                                                              
                                                               
void DRectangle::setComment( const QString &s )                
{                                                              
    DObject::setComment( s );                                  
    if ( getProxy() && getTextDisplay() == TextDisplayComment )
        getProxy()->update();                                  
}                                                              

/*!
 * \brief Set size. 
 *  
 * Calling \sa slotSize() is preferrable as it more strictly validates the call. 
 *  
 * This is used when loading and by any layout in use. 
 * 
 * \author pharvey (9/11/20)
 * 
 * \param size 
 */
void DRectangle::setSize( const QSizeF &size )
{
    getProxy()->doPrepareGeometryChange();
    rect.setSize( size );
    if ( isSelected() ) doHandlesSync();
    doSinksSync();
    emit signalChangedShapeSize( getShapeSize() );
    emit signalChangedSize( rect.size() );
}

/*!
 * \brief Set position. 
 *  
 * Calling \sa slotPos() is preferrable as it more strictly validates the call. 
 *  
 * This is used when loading and by any layout in use. 
 * 
 * \author pharvey (9/11/20)
 * 
 * \param pointScene 
 */
void DRectangle::setPos( const QPointF &pointScene )
{
   DObject::setPos( pointScene );
   if ( isSelected() ) doHandlesSync();
   doSinksSync();
   emit signalChangedPos( pointScene );
}

void DRectangle::setPosRaw( const QPointF &pointScene )
{
    DObject::setPosRaw( pointScene );
    if ( isSelected() ) doHandlesSync();
    doSinksSyncRaw(); // we call for a 'Raw' to avoid moving any connected source with us
}

void DRectangle::setProxy( DGraphicsProxyItem *p )
{
    DObject::setProxy( p );
    doSinksCreate();
}

/*!
 * \brief Cycle select type.
 *  
 * Presumably called because of repeated single clicks on this. 
 *  
 * Here we support; Resize, Rotate and Shear - but only if we are not SelectOnly. 
 *  
 * \author pharvey (2/15/20)
 */
void DRectangle::setSelectType()
{
   switch ( getSelectType() )
   {
        case SelectResize:
            DRectangleBase::setSelectType( SelectTransformRotate );
            break;
        case SelectTransformRotate:
            DRectangleBase::setSelectType( SelectTransformShear );
            break;
        case SelectTransformShear:
            DRectangleBase::setSelectType( SelectResize );
            break;
        case SelectOnly:
        case SelectMenu:
            break;
    }
}

QPixmap DRectangle::getIcon()
{
    return QPixmap( ":D/Rectangle" );
}

QMenu *DRectangle::getContextMenu( QWidget *pParent )
{
    QMenu *pMenu = DObject::getContextMenu( pParent );

    if ( !inGroup() && !inLayout() )
    {
        pMenu->addAction( tr("Reset Transformation"), this, SLOT(slotResetTransformation()) );
    }

    return pMenu; // caller invokes and then deletes
}

int DRectangle::getTextFlags() 
{
    if ( bWordWrap ) return (nTextHAlign | nTextVAlign | Qt::TextWordWrap);
    return (nTextHAlign | nTextVAlign);
}

/*!
 * \brief Creates a tab widget with a number of tabs for editing/viewing object properties.
 *  
 * We introduce; 
 *  
 * 1. a list of 'Sinks' (\sa DSink).
 * 2. some text display options 
 *  
 * \author pharvey (5/8/20)
 * 
 * \param pWidgetParent 
 * 
 * \return AWDataWidget* 
 */
AWDataWidget *DRectangle::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = DRectangleBase::getObjectWidget( pWidgetParent );

    p->insertTab( 1, new DSinksWidget( this, p ), tr("Sinks") );
    p->insertTab( 1, new DRectangleWidget( this, p ), tr("Rectangle") );

    p->setCurrentIndex( 0 );

    return p;
}

AWPropWidget *DRectangle::getPropWidget( QWidget *pWidgetParent )
{
    AWPropWidget *pPropWidget = DRectangleBase::getPropWidget( pWidgetParent );

    // text
    pPropWidget->addWidget( tr("Text"), new DRectangleTextWidget( this, pPropWidget ) );

    return pPropWidget;

}

DSink *DRectangle::getSink( const QString &stringName )
{
    DSink *pSink;
    foreach( pSink, vectorSinks )
    {
        if ( pSink->objectName() == stringName ) return pSink;
    }
    return nullptr;
}

/*!
 * \brief Return the first line attached to the given sink. 
 *  
 * Will return nullptr if no ->sink->source->line. 
 *  
 * This is most useful for diagrams that expect 0-1 or 1 line attached to the given sink. 
 * For example; an organization chart. 
 *  
 * \author pharvey (4/15/20)
 * 
 * \param stringName 
 * 
 * \return DLine* 
 */
DLine *DRectangle::getLine( const QString &stringName )
{
    DSink *pSink = getSink( stringName );
    if ( !pSink ) return nullptr;
    QList<DSource*> l = pSink->getSources();
    if ( l.count() < 1 ) return nullptr;

    return l.first()->getLine();
}

int DRectangle::getShapeSize()
{
    if ( rect.width() == 16 && rect.height() == 16 ) return 16;
    if ( rect.width() == 32 && rect.height() == 32 ) return 32;
    if ( rect.width() == 48 && rect.height() == 48 ) return 48;
    if ( rect.width() == 64 && rect.height() == 64 ) return 64;
    if ( rect.width() == 96 && rect.height() == 96 ) return 96;
    if ( rect.width() == 128 && rect.height() == 128 ) return 128;

    return 0;
}

void DRectangle::paintAt( QPainter *pPainter, const QPointF &pointPos )
{
    pPainter->setPen( pen );
    pPainter->setBrush( brush );
    pPainter->setFont( font.font );

    // translate to global 
    QRectF rect;
    rect.setTopLeft( pointPos );
    rect.setSize( this->rect.size() );

    pPainter->drawRect( rect );

    // \note: specifying rect in the following truncates text outside of the rect
    //        May want to make this an option/property
    pPainter->setPen( font.color );
    switch ( nTextDisplay )
    {
        case TextDisplayName:
            pPainter->drawText( rect, getTextFlags(), getName() );
            break;
        case TextDisplayCode:
            pPainter->drawText( rect, getTextFlags(), getCode() );
            break;
        case TextDisplayComment:
            pPainter->drawText( rect, getTextFlags(), getComment() );
            break;
        case TextDisplayNone:
            break;
    }

    return;
    // for debugging...

    // draw center point in boundingRect
    {
        QRectF r( 0, 0, 16, 16 );
        r.moveCenter( boundingRect().center() );
        pPainter->setPen( QPen( Qt::red ) );
        pPainter->setBrush( Qt::NoBrush );
        pPainter->drawEllipse( r );
    }

    // draw pos
    {
        QRectF r( 0, 0, 8, 8 );
        r.moveCenter( getProxy()->mapFromParent( getPos() ) );
        pPainter->setPen( QPen( Qt::darkYellow ) );
        pPainter->setBrush( Qt::NoBrush );
        pPainter->drawEllipse( r );
    }
}

bool DRectangle::doSourceEnter( DSource *p )
{
    // we accept any source so ignore checking viability of a possible connection
    Q_UNUSED( p );

    if ( inLayout() ) return false;

    // we accept all comers...
    doSinksVisible( true );

    return true;
}

bool DRectangle::doSourceExit( DSource *p )
{
    Q_UNUSED( p );

    if ( inLayout() ) return false;

    doSinksVisible( false );

    return true;
}

bool DRectangle::doSourceDrop( DSource *pSource, const QPointF &pointScene )
{
    Q_UNUSED(pointScene);

    if ( inLayout() ) return false;

    // we accept all comers as long they are line-ends...
    if ( pSource->getType() == CBD::EOLBegin || pSource->getType() == CBD::EOLEnd )
    { 
        // tell the line it can try to connect to us
        pSource->getLine()->doConnect( this,( pSource->getType() == CBD::EOLBegin ? "Begin" : "End" ) );
    }

    // hide the sinks                                                                                               
    doSourceExit( pSource );                                                                                        

    return true;
}

bool DRectangle::doGlue( DSource *pSource, DSink *pSink )
{
    // sanity checks
    if ( !pSource || !pSink ) return false;
    // is it one of our sinks?
    if ( !vectorSinks.contains( pSink ) ) return false;

    pSource->doGlue( pSink );
    pSink->doGlue( pSource );

    // Put the line behind the rectangle to prevent clicking on a truncated section of line when you want the rectangle.
    // pSource->getLine()->doToBefore( this );
    // NOTE: The above does not work when line and rect have different parents. Use a different strategy.

    return true;
}

bool DRectangle::doGlue( DSource *pSource )
{
    // sanity checks
    if ( !pSource ) return false;

    // find a sink at pos...
    DSink *pSink = nullptr;
    {
        DSink *p = nullptr;
        foreach( p, vectorSinks )
        {
           if ( !p->contains( pSource->getPos() ) ) continue;
           pSink = p;
        }
    }

    // did we find a sink?
    if ( !pSink )
    {
        // no sink - but can we create one?
        if ( !getDynamicSinks() ) return false;
        // glue to a new/dynamic sink
        QPointF point = getProxy()->mapFromScene( pSource->getPos() );                                                     
        QPointF pointPercent( boundingRect().width() / point.x(), boundingRect().height() / point.y() );
        pSink = new DSink( this, pointPercent );                                                           
        vectorSinks.append( pSink );                                                                                
    }

    // do some glueing
    return doGlue( pSource, pSink );
}

bool DRectangle::doUnglue( DSource *pSource, DSink *pSink )
{
    // sanity checks
    if ( !pSource || !pSink ) return false;
    // is it one of our sinks?
    if ( !vectorSinks.contains( pSink ) ) return false;

    pSource->doUnglue();
    pSink->doUnglue( pSource );

    // get rid of a dynamic sink when nothing connected to it
    if ( pSink->getType() == DSink::SinkTypeDynamic && !pSink->isGlued() )
    {
        int n = vectorSinks.indexOf( pSink );
        Q_ASSERT( n >= 0 );
        Q_ASSERT( n < vectorSinks.count() );
        vectorSinks.remove( n );
        delete pSink; 
    }

    return true;
}

bool DRectangle::doUnglue( DSink *pSink )
{
    QList<DSource*> listSources = pSink->getSources();
    DSource *pSource;
    foreach( pSource, listSources )
    {
        doUnglue( pSource, pSink );
    }

    return true;
}

bool DRectangle::doUnglue()
{
    DSink *pSink;

    foreach( pSink, vectorSinks )
    {
        doUnglue( pSink );
    }

    return true;
}

bool DRectangle::isGlued( DSink *pSink, DSource *pSource )
{
    return pSink->listSources.contains( pSource );
}

bool DRectangle::isGlued( DSource *pSource )
{
    DSink *pSink;
    foreach( pSink, vectorSinks )
    {
        if ( isGlued( pSink, pSource ) )
            return true;
    }
    return false;
}

void DRectangle::doGroupSync()
{
    if ( isSelected() ) doHandlesSync();
    doSinksSync();
}

QDomElement DRectangle::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = DRectangleBase::doSave( pdomDoc, pdomElemParent );

    domElemThis.setAttribute( "TextVAlign", nTextVAlign );
    domElemThis.setAttribute( "TextHAlign", nTextHAlign );
    domElemThis.setAttribute( "WordWrap", bWordWrap );
    domElemThis.setAttribute( "DynamicSinks", bDynamicSinks );

    QTransform t = getProxy()->transform();
    if ( t.m11() != 1 ) domElemThis.setAttribute( "m11", t.m11() );
    if ( t.m12() ) domElemThis.setAttribute( "m12", t.m12() );
    if ( t.m13() ) domElemThis.setAttribute( "m13", t.m13() );
    if ( t.m21() ) domElemThis.setAttribute( "m21", t.m21() );
    if ( t.m22() != 1 ) domElemThis.setAttribute( "m22", t.m22() );
    if ( t.m23() ) domElemThis.setAttribute( "m23", t.m23() );
    if ( t.m31() ) domElemThis.setAttribute( "m31", t.m31() );
    if ( t.m32() ) domElemThis.setAttribute( "m32", t.m32() );
    if ( t.m33() != 1 ) domElemThis.setAttribute( "m33", t.m33() );
    if ( t.dx() ) domElemThis.setAttribute( "dx", t.dx() );
    if ( t.dy() ) domElemThis.setAttribute( "dy", t.dy() );

    // save sinks
    // forget dynamic sink if connected object is not coming along
    {
        DSink *pSink;
        for ( int nIndex = 0; nIndex < vectorSinks.count(); nIndex++ )
        {
            pSink = vectorSinks[nIndex];
            QDomElement domElemSink = pdomDoc->createElement( "Sink" );
            domElemSink.setAttribute( "Index", nIndex );
            domElemSink.setAttribute( "Type", pSink->getType() );
            if ( !pSink->objectName().isEmpty() ) domElemSink.setAttribute( "Name", pSink->objectName() );
            DSink::SinkTypes nType = pSink->getType();
            switch ( nType  )
            { 
                case DSink::SinkTypeStatic:
                {
                    domElemSink.setAttribute( "x", pSink->getPos().x() );
                    domElemSink.setAttribute( "y", pSink->getPos().y() );
                    domElemThis.appendChild( domElemSink );
                }
                break;
                case DSink::SinkTypeDynamic:
                {
                    if ( !g_Transaction->canCopy() || pSink->getRectangle()->isSelected() )
                    { 
                        domElemSink.setAttribute( "x", pSink->getPercent().x() );
                        domElemSink.setAttribute( "y", pSink->getPercent().y() );
                        domElemThis.appendChild( domElemSink );
                    }
                }
                break;
            }

            // NOTE
            // 0-n sources for each sink but we do not save this here.
            // Any connected DLine will save connect information.
            // It would be redundant to save it here as well.

        }
    }

    // do this in case child has changed
    domElemThis.setAttribute( "NextOID", nNextOID );

    return domElemThis;
}

bool DRectangle::doLoad( QDomElement *pdomElemObject )
{
    DRectangleBase::doLoad( pdomElemObject );

    QString stringAttribute;

    // transformation
    {
        QTransform t;                                                         
        t.setMatrix(    pdomElemObject->attribute( "m11", "1" ).toDouble(),   
                        pdomElemObject->attribute( "m12", "0" ).toDouble(),   
                        pdomElemObject->attribute( "m13", "0" ).toDouble(),   
                        pdomElemObject->attribute( "m21", "0" ).toDouble(),   
                        pdomElemObject->attribute( "m22", "1" ).toDouble(),   
                        pdomElemObject->attribute( "m23", "0" ).toDouble(),   
                        pdomElemObject->attribute( "m31", "0" ).toDouble(),   
                        pdomElemObject->attribute( "m32", "0" ).toDouble(),   
                        pdomElemObject->attribute( "m33", "1" ).toDouble()  );
        t.translate(    pdomElemObject->attribute( "dx", "0" ).toDouble(),    
                        pdomElemObject->attribute( "dy", "0" ).toDouble()  ); 
        getProxy()->setTransform( t );                                        
    }

    // other
    {
        nTextVAlign     = (Qt::AlignmentFlag)pdomElemObject->attribute( "TextVAlign", QString::number( Qt::AlignVCenter ) ).toInt();
        nTextHAlign     = (Qt::AlignmentFlag)pdomElemObject->attribute( "TextHAlign", QString::number( Qt::AlignLeft ) ).toInt();
        bWordWrap       = pdomElemObject->attribute( "WordWrap", QString::number( bWordWrap ) ).toInt();
        bDynamicSinks   = pdomElemObject->attribute( "DynamicSinks", QString::number( bDynamicSinks ) ).toInt();
    }

    //
    // OUR CHILD NODES
    // - sinks

    // clear sinks
    qDeleteAll( vectorSinks );
    vectorSinks.clear();

    //
    QDomElement domElem;
    QDomNode    domNode;

    domNode = pdomElemObject->firstChild();

    while ( !domNode.isNull() )
    {
        domElem = domNode.toElement();
        if ( !domElem.isNull() )
        {
            if ( domElem.tagName() == "Sink" )
            {
                int nIndex              = domElem.attribute( "Index" ).toInt();
                DSink::SinkTypes nType  = (DSink::SinkTypes)domElem.attribute( "Type" ).toInt();
                QString stringName      = domElem.attribute( "Name" );
                QPointF point( domElem.attribute( "x" ).toDouble(), domElem.attribute( "y" ).toDouble() );
                DSink *pSink = nullptr;
                switch ( nType  )
                { 
                    case DSink::SinkTypeStatic:
                    {
                        pSink = new DSink( this, stringName ); 
                        pSink->setPos( point );
                    }
                    break;
                    case DSink::SinkTypeDynamic:
                    {
                        pSink = new DSink( this, point, stringName ); 
                    }
                    break;
                }
                
                if ( nIndex >= vectorSinks.count() ) vectorSinks.resize( nIndex + 1 );
                vectorSinks[nIndex] = pSink;
            }
        } // if element
        domNode = domNode.nextSibling();
    } // while

    return true;
}

/*!
 * \brief Request to set size. 
 *  
 * This should be used, instead of \sa setSize() or \sa setRect(), as much as possible 
 * because it more strictly validates the call. 
 * 
 * \author pharvey (9/11/20)
 * 
 * \param size 
 */
void DRectangle::slotSize( const QSizeF &size )
{
    // nothing to do - short circuit to end any round-trip request
    if ( size == rect.size() ) return;

    // can we change size?
    if ( inLayout() || isFixedSize()  )
    {
        // DLayout will use setSize() so this must be from a control
        // continue the round-trip request by sending the control the unchanged value (request denied)
        emit signalChangedShapeSize( getShapeSize() );
        emit signalChangedSize( rect.size() );
        return;
    }

    // do it
    setSize( size );
    setModified();
}

void DRectangle::slotTextVAlign( Qt::AlignmentFlag n )
{
    if ( nTextVAlign == n ) return;
    nTextVAlign = n;
    if ( pProxy ) pProxy->update();
    setModified();
    emit signalChangedTextVAlign( n );
}

void DRectangle::slotTextHAlign( Qt::AlignmentFlag n )
{
    if ( nTextHAlign == n ) return;
    nTextHAlign = n;
    if ( pProxy ) pProxy->update();
    setModified();
    emit signalChangedTextHAlign( n );
}

void DRectangle::slotWordWrap( Qt::CheckState n )
{
    if ( bWordWrap == n ) return;
    bWordWrap = n;
    if ( pProxy ) pProxy->update();
    setModified();
}

/*!
 * \brief Resize (width & height) to given size. 
 *  
 * \sa slotOptimizeSize 
 * 
 * \author pharvey (5/11/20)
 * 
 * \param n 
 */
void DRectangle::slotShapeSize( int n )
{
    if ( isFixedSize() ) return;
    if ( n == getShapeSize() ) return;

    if ( n == 0 ) return;

    getProxy()->doPrepareGeometryChange();

    rect.setWidth( n );
    rect.setHeight( n );

    // sync handles
    if ( isSelected() ) doHandlesSync();
    doSinksSync();

    emit signalChangedShapeSize( getShapeSize() );
    emit signalChangedSize( rect.size() );
}

void DRectangle::slotResetTransformation()
{
    if ( isSelected() ) getDiagram()->getSelectionManager()->setSelected( this, false );

    getProxy()->doPrepareGeometryChange();
    doFiniTransform();
    getProxy()->setTransform( QTransform() );
}

/*!
 * \brief Resize to optimal size. This typically means sizing for contents such as text. 
 *  
 * \sa slotShapeSize 
 * 
 * \author pharvey (5/11/20)
 */
void DRectangle::slotOptimizeSize()
{
    if ( isFixedSize() ) return;

    getProxy()->doPrepareGeometryChange();

    rect.setSize( getOptimalSize() );

    // sync handles
    if ( isSelected() ) doHandlesSync();
    doSinksSync();

    emit signalChangedShapeSize( getShapeSize() );
    emit signalChangedSize( rect.size() );
}

/*!
 * \brief Calculate the optimal size. Typically for contents such as text. 
 *  
 * The size may be larger, smaller or the same as current. 
 * This supports \sa slotOptimizeSize. The idea is that derived classes probably do not 
 * have to worry about applying the size (done in slotOptimizeSize) but may want to 
 * override this method to provide the size. 
 * 
 * \author pharvey (5/11/20)
 * 
 * \return QSizeF 
 */
QSizeF DRectangle::getOptimalSize()
{
    // do nothing (for now) so just return our current size
    return boundingRect().size();
}

void DRectangle::doSinksCreate()
{
    vectorSinks.resize( 5 );
    vectorSinks[CBD::RectangleStaticSinkTop]    = new DSink( this, "Top" );
    vectorSinks[CBD::RectangleStaticSinkBottom] = new DSink( this, "Bottom" );
    vectorSinks[CBD::RectangleStaticSinkLeft]   = new DSink( this, "Left" );
    vectorSinks[CBD::RectangleStaticSinkRight]  = new DSink( this, "Right" );
    vectorSinks[CBD::RectangleStaticSinkCenter] = new DSink( this, "Center" );
    doSinksSync();
}

void DRectangle::doSinksVisible( bool b )
{
    if ( b == bSinksVisible ) return;

    DSink *pSink;
    foreach( pSink, vectorSinks )
    {
        pSink->setVisible( b ); // this will create/delete DHandle for the sink
    }

    bSinksVisible = b;
}

void DRectangle::doSinksDelete()
{
    // disconnect from any connected sources
    doDisconnect();
    // they should delete any DHandle they may have on their way out
    qDeleteAll( vectorSinks );
    vectorSinks.clear();
}

void DRectangle::doSinksSync()
{
    if ( !getProxy() ) return;
    if ( vectorSinks.count() == 0 ) return;

    DSink *pSink;
    int n = 0;
    foreach( pSink, vectorSinks )
    {
        if ( n == CBD::RectangleStaticSinkTop )
            pSink->setPos( getProxy()->mapToScene( QPointF( rect.width() / 2, rect.top() + 1 ) ) );
        else if ( n == CBD::RectangleStaticSinkBottom )
            pSink->setPos( getProxy()->mapToScene( QPointF( rect.width() / 2, rect.bottom() - 1 ) ) );
        else if ( n == CBD::RectangleStaticSinkLeft )
            pSink->setPos( getProxy()->mapToScene( QPointF( rect.left() + 1, rect.height() / 2 ) ) );
        else if ( n == CBD::RectangleStaticSinkRight )
            pSink->setPos( getProxy()->mapToScene( QPointF( rect.right() - 1, rect.height() / 2 ) ) );
        else if ( n == CBD::RectangleStaticSinkCenter )
            pSink->setPos( getProxy()->mapToScene( rect.center() ) );
        else
            pSink->setPos();    // dynamic
        n++;
    }
}

void DRectangle::doSinksSyncRaw()
{
    if ( !pProxy ) return;
    if ( vectorSinks.count() == 0 ) return;

    DSink *pSink;
    int n = 0;
    foreach( pSink, vectorSinks )
    {
        if ( n == CBD::RectangleStaticSinkTop )
            pSink->setPosRaw( getProxy()->mapToScene( QPointF( rect.width() / 2, rect.top() + 1 ) ) );
        else if ( n == CBD::RectangleStaticSinkBottom )
            pSink->setPosRaw( getProxy()->mapToScene( QPointF( rect.width() / 2, rect.bottom() - 1 ) ) );
        else if ( n == CBD::RectangleStaticSinkLeft )
            pSink->setPosRaw( getProxy()->mapToScene( QPointF( rect.left() + 1, rect.height() / 2 ) ) );
        else if ( n == CBD::RectangleStaticSinkRight )
            pSink->setPosRaw( getProxy()->mapToScene( QPointF( rect.right() - 1, rect.height() / 2 ) ) );
        else if ( n == CBD::RectangleStaticSinkCenter )
            pSink->setPosRaw( getProxy()->mapToScene( rect.center() ) );
        else
            pSink->setPosRaw(); // dynamic
        n++;
    }
}

void DRectangle::doHandleMove( DHandle *pHandle, const QPointF &pointScenePos )
{
    switch ( getSelectType() )
    {
        case SelectResize:
            doResize( pHandle, pointScenePos );
            break;
        case SelectTransformRotate:
            doRotate( pHandle, pointScenePos );
            break;
        case SelectTransformShear:
            doShear( pHandle, pointScenePos );
            break;
        case SelectMenu:
            doMenu( pHandle, pointScenePos );
            break;
        case SelectOnly:
            break;
    }
}

void DRectangle::doHandlesCreate()
{
    Q_ASSERT( vectorHandles.count() == 0 );

    switch ( getSelectType() )
    {
        case SelectResize:
            doInitResize();
            break;
        case SelectTransformRotate:
            doInitRotate();
            break;
        case SelectTransformShear:
            doInitShear();
            break;
        case SelectMenu:
            doInitMenu();
            break;
        case SelectOnly:
            doInitSelect();
            break;
    }
}

void DRectangle::doHandlesDelete()
{
    switch ( getSelectType() )
    {
        case SelectResize:
            doFiniResize();
            break;
        case SelectTransformRotate:
            doFiniRotate();
            break;
        case SelectTransformShear:
            doFiniShear();
            break;
        case SelectMenu:
            doFiniMenu();
            break;
        case SelectOnly:
            doFiniSelect();
            break;
    }
}

void DRectangle::doHandlesSync()
{
    Q_ASSERT( vectorHandles.count() > 0 );

    switch ( getSelectType() )
    {
        case SelectResize:
            doSyncResize();
            break;
        case SelectTransformRotate:
            doSyncRotate();
            break;
        case SelectTransformShear:
            doSyncShear();
            break;
        case SelectMenu:
            doSyncMenu();
            break;
        case SelectOnly:
            doSyncSelect();
            break;
    }
}

/*!
 * \brief Resize. 
 *  
 * We do the default but we also; 
 *  
 * - sync ShapeSize  - not supported in DRectangleBase but is supported here
 * - sync sinks      - not needed in DRectangleBase but needed here
 * 
 * \author pharvey (10/6/20)
 * 
 * \param pHandle 
 * \param pointScenePos 
 */
void DRectangle::doResize( DHandle *pHandle, const QPointF &pointScenePos )
{
    int nShapeSize = getShapeSize();
    DRectangleBase::doResize( pHandle, pointScenePos );
    doSinksSync();
    if ( nShapeSize != getShapeSize() ) emit signalChangedShapeSize( getShapeSize() );
}

void DRectangle::doRotate( DHandle *pHandle, const QPointF &pointScenePos )
{
    if ( pHandle != vectorHandles[DRECTANGLE_HANDLE_ROTATE] ) return;

    getProxy()->doPrepareGeometryChange();

    vectorLines[DRECTANGLE_LINE_ROTATE]->setEnd( vectorLines[DRECTANGLE_LINE_ROTATE]->mapFromScene( pointScenePos ) );
    vectorHandles[DRECTANGLE_HANDLE_ROTATE]->setPos( pointScenePos );

    pointRotateDelta = getProxy()->pos() - pHandle->pos();

    nAngle = vectorLines[DRECTANGLE_LINE_ROTATE]->getLine().angle() - 90;

    doTransform();
}

void DRectangle::doInitRotate()
{
    if ( vectorLines.count() ) return;
    if ( !bInitTransform ) doInitTransform();

    QPointF pointBase = getProxy()->pos();

    vectorLines.resize( 1 );
    vectorHandles.resize( 2 );

    vectorLines[DRECTANGLE_LINE_ROTATE] = new DGraphicsItemLine( QLineF( pointBase, pointBase ) );
    getProxy()->scene()->addItem( vectorLines[DRECTANGLE_LINE_ROTATE] );

    vectorHandles[DRECTANGLE_HANDLE_BASE] = new DHandle( this, DHandle::FunctionBaseTransform, DHandle::ConnectTypeNone, pointBase );
    getProxy()->scene()->addItem( vectorHandles[DRECTANGLE_HANDLE_BASE] );

    vectorHandles[DRECTANGLE_HANDLE_ROTATE] = new DHandle( this, DHandle::FunctionRotateIndicator, DHandle::ConnectTypeNone, pointBase );
    getProxy()->scene()->addItem( vectorHandles[DRECTANGLE_HANDLE_ROTATE] );

    doSyncRotate();
}

void DRectangle::doFiniRotate()
{
    qDeleteAll( vectorHandles );
    vectorHandles.clear();

    qDeleteAll( vectorLines );
    vectorLines.clear();
}

void DRectangle::doSyncRotate()
{
    QPointF pointBase       = getProxy()->pos();
    QPointF pointIndicator  = pointBase - pointRotateDelta;

    vectorLines[DRECTANGLE_LINE_ROTATE]->setLine( QLineF( pointBase, pointIndicator ) );
    vectorHandles[DRECTANGLE_HANDLE_BASE]->setPos( pointBase );
    vectorHandles[DRECTANGLE_HANDLE_ROTATE]->setPos( pointIndicator );
}

void DRectangle::doShear( DHandle *pHandle, const QPointF &pointScenePos )
{
    getProxy()->doPrepareGeometryChange();

    QPointF pointBase = getProxy()->pos();

    bool bFixUnusedAxis = false; // not sure which way is best when resize, rotate, and shear all at play

    if ( !bFixUnusedAxis ) pHandle->setPos( pointScenePos );

    if ( pHandle == vectorHandles[DRECTANGLE_HANDLE_SHEARH] )
    {
        if ( bFixUnusedAxis ) pHandle->setPos( QPointF( pointScenePos.x(), pHandle->pos().y() ) );
        pointShearHDelta = pointBase - pHandle->pos();
    }
    else if ( pHandle == vectorHandles[DRECTANGLE_HANDLE_SHEARV] )
    {
        if ( bFixUnusedAxis ) pHandle->setPos( QPointF( pHandle->pos().x(), pointScenePos.y() ) );
        pointShearVDelta = pointBase - pHandle->pos();
    }

    //
    QPointF pointShearH  = vectorHandles[DRECTANGLE_HANDLE_SHEARH]->pos();
    QPointF pointShearV  = vectorHandles[DRECTANGLE_HANDLE_SHEARV]->pos();

    //
    nShearH = pointShearH.x() - pointBase.x();
    nShearV = pointShearV.y() - pointBase.y();

    doTransform();

    // update lines
    // - after transform because corners are going to move
    vectorLines[DRECTANGLE_LINE_SHEAR_HL]->setLine( QLineF( getProxy()->mapToScene( rect.topLeft() ), pointShearH ) );
    vectorLines[DRECTANGLE_LINE_SHEAR_HR]->setLine( QLineF( getProxy()->mapToScene( rect.topRight() ), pointShearH ) );
    vectorLines[DRECTANGLE_LINE_SHEAR_VT]->setLine( QLineF( getProxy()->mapToScene( rect.topRight() ), pointShearV ) );
    vectorLines[DRECTANGLE_LINE_SHEAR_VB]->setLine( QLineF( getProxy()->mapToScene( rect.bottomRight() ), pointShearV ) );
}

void DRectangle::doInitShear()
{
    if ( vectorLines.count() ) return;
    if ( !bInitTransform ) doInitTransform();

    QPointF pointBase = getProxy()->pos();

    vectorLines.resize( 4 );
    vectorHandles.resize( 2 );

    vectorLines[DRECTANGLE_LINE_SHEAR_HL] = new DGraphicsItemLine( QLineF( pointBase, pointBase ) );
    getProxy()->scene()->addItem( vectorLines[DRECTANGLE_LINE_SHEAR_HL] );

    vectorLines[DRECTANGLE_LINE_SHEAR_HR] = new DGraphicsItemLine( QLineF( pointBase, pointBase ) );
    getProxy()->scene()->addItem( vectorLines[DRECTANGLE_LINE_SHEAR_HR] );

    vectorLines[DRECTANGLE_LINE_SHEAR_VT] = new DGraphicsItemLine( QLineF( pointBase, pointBase ) );
    getProxy()->scene()->addItem( vectorLines[DRECTANGLE_LINE_SHEAR_VT] );

    vectorLines[DRECTANGLE_LINE_SHEAR_VB] = new DGraphicsItemLine( QLineF( pointBase, pointBase ) );
    getProxy()->scene()->addItem( vectorLines[DRECTANGLE_LINE_SHEAR_VB] );

    // horizontal
    vectorHandles[DRECTANGLE_HANDLE_SHEARH] = new DHandle( this, DHandle::FunctionShearHIndicator, DHandle::ConnectTypeNone, pointBase );
    getProxy()->scene()->addItem( vectorHandles[DRECTANGLE_HANDLE_BASE] );

    // vertical
    vectorHandles[DRECTANGLE_HANDLE_SHEARV] = new DHandle( this, DHandle::FunctionShearVIndicator, DHandle::ConnectTypeNone, pointBase );
    getProxy()->scene()->addItem( vectorHandles[DRECTANGLE_HANDLE_ROTATE] );

    doSyncShear();
}

void DRectangle::doFiniShear()
{
    qDeleteAll( vectorHandles );
    vectorHandles.clear();
    qDeleteAll( vectorLines );
    vectorLines.clear();
}

void DRectangle::doSyncShear()
{
    QPointF pointBase    = getProxy()->pos();
    QPointF pointShearH  = pointBase - pointShearHDelta;
    QPointF pointShearV  = pointBase - pointShearVDelta;

    vectorLines[DRECTANGLE_LINE_SHEAR_HL]->setLine( QLineF( getProxy()->mapToScene( rect.topLeft() ), pointShearH ) );
    vectorLines[DRECTANGLE_LINE_SHEAR_HR]->setLine( QLineF( getProxy()->mapToScene( rect.topRight() ), pointShearH ) );
    vectorLines[DRECTANGLE_LINE_SHEAR_VT]->setLine( QLineF( getProxy()->mapToScene( rect.topRight() ), pointShearV ) );
    vectorLines[DRECTANGLE_LINE_SHEAR_VB]->setLine( QLineF( getProxy()->mapToScene( rect.bottomRight() ), pointShearV ) );

    vectorHandles[DRECTANGLE_HANDLE_SHEARH]->setPos( pointShearH );
    vectorHandles[DRECTANGLE_HANDLE_SHEARV]->setPos( pointShearV );
}

void DRectangle::doTransform()
{
    QTransform t;
    t.rotate( -nAngle );
    t.shear( -(nShearH / 75), nShearV / 75 );

    getProxy()->setTransform( t, false );
}

void DRectangle::doInitTransform()
{
    pointRotateDelta = getProxy()->pos() - getProxy()->mapToScene( QPointF( rect.center().x(), rect.top() - 20 ) );
    pointShearHDelta = getProxy()->pos() - getProxy()->mapToScene( QPointF( rect.center().x(), rect.top() - 20 ) );
    pointShearVDelta = getProxy()->pos() - getProxy()->mapToScene( QPointF( rect.right() + 20, rect.center().y() ) );
    nAngle  = 0;
    nShearH = 0;
    nShearV = 0;
    bInitTransform = true;
}

void DRectangle::doFiniTransform()
{
    pointRotateDelta = QPointF();
    pointShearHDelta = QPointF();
    pointShearVDelta = QPointF();
    nAngle  = 0;
    nShearH = 0;
    nShearV = 0;
    bInitTransform = false;
}


