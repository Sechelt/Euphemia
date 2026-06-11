/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "LLayer.h"

#include <SSettings.h>

#include "LGraphicsLayer.h"
#include "LGraphicsScene.h"
#include "LFile.h"
#include "LObject.h"

LLayer::LLayer( LFile *pParent, const QString &stringName )
    : AWObject( pParent, stringName )
{
    pSettingGrid = (LSGrid*)g_SSettings->getSetting( "LSGrid" );
    if ( !pSettingGrid )
    {
        qInfo() << "Could not find [LSGrid]";
        return;
    }

    slotRefreshGrid();
    connect( pSettingGrid, SIGNAL(signalChanged()), SLOT(slotRefreshGrid()) );
}

LLayer::~LLayer()
{
    if ( pGraphics ) doGraphicsDelete();
}

QMenu *LLayer::getContextMenu( QWidget *pParent )
{
    // must NOT provide a delete because we must have, at least one, object
    QMenu *pMenu = new QMenu( pParent );
    pMenu->addAction( QPixmap( ":AW/Properties" ), tr("Object..."), this, SLOT(slotObjectDialog()) );

    return pMenu; // caller invokes and then deletes
}

/*!
 * \brief Creates a graphics object. 
 *  
 * This creates a LGraphicsObject by default.
 * Could, if needed, override to return something derived from LGraphicsObject.
 * ie to introduce QWidget to the scene (can not use QGraphicsWidget because it is not based upon LGraphicsObject).
 * 
 * \author pharvey (4/7/23)
 * 
 * \param pParent 
 */
void LLayer::doGraphicsCreate( LGraphicsScene *pScene )
{
    Q_ASSERT( pScene );
    Q_ASSERT( !pGraphics );

    pGraphics = new LGraphicsLayer( this );
    LFile *pLayers = (LFile*)getParent( "LFile" );
    connect( pLayers->getGraphics(), SIGNAL(sceneRectChanged(const QRectF &)), SLOT(slotSceneRectChanged(const QRectF &)) );

    QList<LObject*> l = findChildren<LObject*>( QString(), Qt::FindDirectChildrenOnly );
    foreach( LObject *pObject, l )
    {
        pObject->doGraphicsCreate( pGraphics );
    }

    pScene->addItem( pGraphics );

    pGraphics->setVisible( bVisible );
}

void LLayer::doGraphicsDelete()
{
    Q_ASSERT( pGraphics );
    QList<LObject*> l = findChildren<LObject*>( QString(), Qt::FindDirectChildrenOnly );
    foreach( LObject *pObject, l )
    {
        pObject->doGraphicsDelete();
    }
    delete pGraphics; // will be auto-magically removed from scene
    pGraphics = nullptr;
}

QRectF LLayer::boundingRect() const
{
    if ( pGraphics && pGraphics->scene() ) return pGraphics->scene()->sceneRect();
    return QRectF();
}

QPainterPath LLayer::shape() const
{
    QPainterPath path;
    path.addRect( boundingRect() );
    return path;
}

void LLayer::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *pOption, QWidget *pWidget )
{
    Q_UNUSED( pPainter );
    Q_UNUSED( pOption );
    Q_UNUSED( pWidget );
}

QDomElement LLayer::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = AWObject::doSave( pdomDoc, pdomElemParent );
    return domElemThis;
}

bool LLayer::doLoad( QDomElement *pdomElemThis )
{
    AWObject::doLoad( pdomElemThis );
    return true;
}

void LLayer::setModified( bool b )
{
    AWObject::setModified( b );     // modified as in - need to save
    emit signalChanged();           // changed as in - modified or some other state change    
}

void LLayer::setBackground( const QColor &c )
{
    if ( c == colorBackground ) return;
    colorBackground = c;
}

void LLayer::setVisible( bool b )
{
    if ( bVisible == b ) return;
    bVisible = b;
    if ( !pGraphics ) return;
    pGraphics->setVisible( b );
    emit signalChanged();
}

void LLayer::doToBack()
{
    AWObject::doToBack();                  
//    if ( pGraphics ) pGraphics->doToBack();
    setModified();
}

void LLayer::doToFront()
{
    AWObject::doToFront();                  
//    if ( pGraphics ) pGraphics->doToFront();
    setModified();
}

void LLayer::doToPrev( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass )
{
    AWObject::doToPrev( stringName, stringClassName, stringInheritsClass );                  
//    if ( pGraphics ) pGraphics->doToPrev( stringName, stringClassName, stringInheritsClass );
    setModified();
}

void LLayer::doToNext( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass )
{
    AWObject::doToNext( stringName, stringClassName, stringInheritsClass );                  
//    if ( pGraphics ) pGraphics->doToNext( stringName, stringClassName, stringInheritsClass );
    setModified();
}

void LLayer::doToBefore( ADObject *pObject )
{
    Q_ASSERT( pObject );                                       
    Q_ASSERT( pObject->inherits( "LLayer" ) );                 
                                                               
    AWObject::doToBefore( pObject );                           
//    if ( pGraphics ) pGraphics->doToBefore( (LLayer*)pObject );
    setModified();
}

void LLayer::doToAfter( ADObject *pObject )
{
    Q_ASSERT( pObject );                                      
    Q_ASSERT( pObject->inherits( "LLayer" ) );                
                                                              
    AWObject::doToAfter( pObject );                           
//    if ( pGraphics ) pGraphics->doToAfter( (LLayer*)pObject );
    setModified();
}

void LLayer::doClear()
{
    AWObject::doClear();
}

void LLayer::doZoomChanged( int nZoom )
{
    Q_UNUSED( nZoom );
}

void LLayer::doScrollChanged( int nX, int nY )
{
    Q_UNUSED( nX );
    Q_UNUSED( nY );
}

void LLayer::slotRefreshGrid()
{
    nGridType    = pSettingGrid->getType();
    nGridSnap    = pSettingGrid->getSnap();
    sizeGridCell = pSettingGrid->getCellSize();
}



