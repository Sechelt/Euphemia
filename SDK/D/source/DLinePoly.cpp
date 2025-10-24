#include "LibInfo.h"
#include "DLinePoly.h"

#include <math.h>

#include <WLineEndComboBox.h>
#include <AWPropWidget.h>

#include "DDiagram.h"

#include "DRectangle.h"
#include "DGraphicsProxyItem.h"
#include "DGraphicsSceneProxy.h"

#include "DSourcesWidget.h"

DLinePoly::DLinePoly( ADObject *pObjectParent, const QString &stringName )
    : DLine( pObjectParent, stringName )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    pHandleRemoveLineSegment = nullptr;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

DLinePoly::~DLinePoly()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

QPixmap DLinePoly::getIcon()
{
    return QPixmap( ":D/Polyline" );
}

void DLinePoly::mouseMoveHandle( DHandle *pHandle, const QPointF &pointScene )
{
    // is the handle a source?
    if ( pHandle->getConnectType() == DHandle::ConnectTypeSource )
    {
        // find the source
        DSource *pSource;
        foreach( pSource, vectorSources )
        {
            if ( pHandle == pSource->getHandle() )
            {
                doMoveSource( pSource, pointScene );
                return;
            }
        }
        return; 
    }

    // is the handle for creating a new line segment?
    if ( pHandle->getFunction() == DHandle::FunctionNewLineSegment )
    {
        // insert a point into polygon

        // this is the point before the handle (this handle is between two points but we use point before for reference)
        int nPoint = pHandle->getIndex();

        // insert before the point AFTER the handle this will break our line segment into two
        polygon.insert( nPoint + 1, getProxy()->mapFromScene( pHandle->pos() ) );
        
        // our new point  
        int nPointNew = nPoint + 1;

        // bump all of the values above ours by 1 freeing up our index 
        {
            DHandle *p;
            foreach( p, vectorHandles )
            {
                if ( p->getIndex() > nPoint )
                    p->setIndex( p->getIndex() + 1 );
            }
        }

        // remember that the begin point of segment is the reference point for a FunctionNewLineSegment
        // insert a new FunctionNewLineSegment handle before this handle
        {
            int nHandle = vectorHandles.indexOf( pHandle ); 
            QRectF r( polygon.at( nPoint ), polygon.at( nPoint + 1 ));
            DHandle *p = new DHandle( this, DHandle::FunctionNewLineSegment, DHandle::ConnectTypeNone, getProxy()->mapToScene( r.center() ) );
            p->setIndex( nPoint );
            vectorHandles.insert( nHandle, p );
            getProxy()->scene()->addItem( p );
            // p->stackBefore( pHandle );
            p->setVisible( true );
        }

        // insert a new FunctionNewLineSegment handle after this handle
        {
            int nHandle = vectorHandles.indexOf( pHandle );
            QRectF r( polygon.at( nPointNew ), polygon.at( nPointNew + 1 ));
            DHandle *p = new DHandle( this, DHandle::FunctionNewLineSegment, DHandle::ConnectTypeNone, getProxy()->mapToScene( r.center() ) );
            p->setIndex( nPointNew );
            vectorHandles.insert( nHandle + 1, p );
            getProxy()->scene()->addItem( p );
            // p->stackBefore( pHandle );
            p->setVisible( true );
        }

        // change *this* handle from FunctionNewLineSegment to FunctionRemoveLineSegment
        // it now represents an intermediate point
        pHandle->setFunction( DHandle::FunctionRemoveLineSegment );

        // we are the new point
        pHandle->setIndex( nPointNew );

        // move this handle
        doHandleMove( pHandle, pointScene );

        return;
    }

    // must be for moving an intermediate point (DHandle::FunctionRemoveLineSegment)
    doHandleMove( pHandle, pointScene );
}

void DLinePoly::mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene )
{
    // is the handle a source?
    if ( pHandle->getConnectType() == DHandle::ConnectTypeSource )
    {
        // find the source
        DSource *pSource;
        foreach( pSource, vectorSources )
        {
            if ( pHandle == pSource->getHandle() )
            {
                doMoveSource( pSource, pointScene );
                return;
            }
        }
        return; 
    }

    // intermediate point
    if ( pHandle->getFunction() == DHandle::FunctionRemoveLineSegment )
    {
        // move handle
        doHandleMove( pHandle, pointScene );

        // \todo
        // remove segment if it lines up with adjacent segment
        // this can also be done via context menu on handle to request to delete point/handle

        return;
    }
}

void DLinePoly::contextMenu( DHandle *pHandle, QGraphicsSceneContextMenuEvent *pEvent )
{
    if ( !pHandle ) return;
    if ( pHandle->getFunction() != DHandle::FunctionRemoveLineSegment ) return;

    pHandleRemoveLineSegment = pHandle;

    QMenu *pMenu = new QMenu( nullptr );
    pMenu->addAction( QPixmap( ":AW/Delete" ), tr("Delete"), this, SLOT(slotDeleteHandle()) );
    pMenu->exec( pEvent->screenPos() );
    delete pMenu;
    pEvent->setAccepted( true );

    pHandleRemoveLineSegment = nullptr;
}

void DLinePoly::slotDeleteHandle()
{
    Q_ASSERT( pHandleRemoveLineSegment );

    getProxy()->doPrepareGeometryChange();

    // determine index to point
    int nIndex = pHandleRemoveLineSegment->getIndex(); 

    // adjust vectorHandles
    // remember that the begin point of segment is the reference point for a FunctionNewLineSegment
    // dec all of the values above ours by 1 
    // and catch surplus handles on our way past
    {
        DHandle *pHandleBefore = nullptr;
        DHandle *p;
        foreach( p, vectorHandles )
        {
            // is it previous handle?
            if ( p->getIndex() == (nIndex-1) )
            {
                // is it the NewLineSegment handle? if so - store it
                if ( p->getFunction() == DHandle::FunctionNewLineSegment )
                   pHandleBefore = p;
            }
            // adjust index value
            if ( p->getIndex() >= nIndex )
                p->setIndex( p->getIndex() - 1 );
        }

        Q_ASSERT( pHandleBefore );

        // get rid of NewLineSegment before
        vectorHandles.remove( vectorHandles.indexOf( pHandleBefore ) );
        delete pHandleBefore;

        // get rid of RemoveLineSegment
        vectorHandles.remove( vectorHandles.indexOf( pHandleRemoveLineSegment ) );
        delete pHandleRemoveLineSegment;
    }

    // delete point
    polygon.remove( nIndex );

    if ( isSelected() ) doHandlesSync();
}

void DLinePoly::doHandlesCreate()
{
    Q_ASSERT( vectorHandles.count() == 0 );

    // resize so we have;
    // - one for each point in polygon (except first and last) (polygon.count() - 2)
    // - one for each line segment (polygon.count() - 1)
    vectorHandles.resize( polygon.count() * 2 - 1 - 2 );

    int nHandle = 0;
    // process polygon - skip last point
    for ( int nPoint = 0; nPoint < (polygon.count() - 1); nPoint++ )
    {
        // new line segment handle
        { 
            QRectF r( polygon.at( nPoint ), polygon.at( nPoint + 1 ));
            DHandle *pHandle = new DHandle( this, DHandle::FunctionNewLineSegment, DHandle::ConnectTypeNone, getProxy()->mapToScene( r.center() ) );
            pHandle->setIndex( nPoint );
            vectorHandles[nHandle] = pHandle;
            getProxy()->scene()->addItem( pHandle );
            nHandle++;
        }
        
        // begin has a source handle (not in vectorHandles) so skip
        if ( nPoint == 0 )
            continue;

        // we do not process last handle so all else must be RemoveLineSegment (an intermediate point)
        {
            DHandle *pHandle = new DHandle( this, DHandle::FunctionRemoveLineSegment, DHandle::ConnectTypeNone, getProxy()->mapToScene( polygon.at( nPoint ) ) );
            pHandle->setIndex( nPoint );
            vectorHandles[nHandle] = pHandle;
            getProxy()->scene()->addItem( pHandle );
            nHandle++;
        }
    }

    // Center handle for floating text
    { 
        DHandle *pHandle = new DHandle( this, DHandle::FunctionFloatingTextCenter, DHandle::ConnectTypeNone, getFloatingTextCenterPos() + pointDeltaFloatingTextCenter );
        vectorHandles.append( pHandle );
        getProxy()->scene()->addItem( pHandle );
    }
}

void DLinePoly::doHandlesSync()
{
    Q_ASSERT( vectorHandles.count() > 0 );

    DHandle *pHandle;
    foreach( pHandle, vectorHandles )
    {
        int nPoint = pHandle->getIndex();
        if ( pHandle->getFunction() == DHandle::FunctionNewLineSegment )
        {
            QRectF r( polygon.at( nPoint ), polygon.at( nPoint + 1 ));
            pHandle->setPos( getProxy()->mapToScene( r.center() ) );
        }
        else if ( pHandle->getFunction() == DHandle::FunctionRemoveLineSegment )
            pHandle->setPos( getProxy()->mapToScene( polygon.at(nPoint) ) );
        else if ( pHandle->getFunction() == DHandle::FunctionFloatingTextCenter )
        {
            pHandle->setPos( getFloatingTextCenterPos() + pointDeltaFloatingTextCenter ); 
            pFloatingTextCenter->setPos( pHandle->pos() );
        }
    }
}



