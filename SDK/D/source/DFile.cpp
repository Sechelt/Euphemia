#include "LibInfo.h"
#include "DFile.h"

#include <AWObjectNameWidget.h>
#include <AWFactoryWidget.h>
#include <AWObjectUsesWidget.h>
#include <AWObjectUsedByWidget.h>
#include <AWObjectChildrenWidget.h>
#include <AWDocumentNameWidget.h>

#include "DStencils.h"
#include "DStencilsWidget.h"
#include "DGraphicsSceneProxy.h"
#include "DDiagram.h"

DFile::DFile( const QString &stringName )
    : AWFile( nullptr, stringName )
{
    mapMeta.insert( "DStencils", ADMeta( QIcon(), "DStencils", "Stencils" ) );
    mapMeta.insert( "DDiagram", ADMeta( QIcon(), "DDiagram", "Diagram" ) );

    pStencilsWidget = nullptr;
    nStrictness = ADValidation::High;
}

DFile::~DFile()
{
    if ( pStencilsWidget ) delete pStencilsWidget;
    pStencilsWidget = nullptr;
}

void DFile::setStrictness( ADValidation::Strictness n )
{
    nStrictness = n;
    setModified();
}

ADObject *DFile::getObject( const QString &s, ADObject * )
{
    ADObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DStencils" )
    {
        // caller should handle a nullptr return
        if ( getObjects( QString(), QString(), "DStencils" ).count() )
        {
            doMessageBox( "INFO", tr("New Stencils Folder"), tr("A stencils folder already exists and the maximum is one.") );
            return nullptr; 
        }
        p = new DStencils( this );
    }
    else if ( stringClass == "DDiagram" )
    {
        DDiagram *pDiagram;
        p = pDiagram = new DDiagram( this );
        pDiagram->getValidationConfiguration();
        // we must always have a scene to calc things even when not displaying   
        // and this must be done right after diagram created and not within it   
        pDiagram->setScene( new DGraphicsSceneProxy( pDiagram, QRectF( 0, 0, 1000, 1000 ) ) );
    }

    if ( !p )
    {
        doMessageBox( "WARNING", "Create Object...", "Unknown class requested " + s );
        return nullptr;
    }

    // announce the birth of a new child
    emit signalCreated( p );    
    emit signalCreated( this, p );
    // set parent modified by default (can be set back if this is a load)           
    setModified();              

    return p;
}

DStencilsWidget *DFile::getStencilsWidget( QWidget *p )
{
    if ( pStencilsWidget ) return pStencilsWidget;

    DStencils *         pStencils       = nullptr;
    QList<ADObject*>    listStencils    = getObjects( QString(), QString(), "DStencils" );
    if ( listStencils.count() ) pStencils = (DStencils*)listStencils.first();
    pStencilsWidget = new DStencilsWidget( p, pStencils );
    connect( this, SIGNAL(signalCreated(ADObject*)), pStencilsWidget, SLOT(slotCreated(ADObject*)) );
    connect( this, SIGNAL(signalDeleted(ADObject*)), pStencilsWidget, SLOT(slotDeleted(ADObject*)) );

    return pStencilsWidget;
}

QMenu *DFile::getContextMenu( QWidget *pParent )
{
    QMenu *pMenu = new QMenu( pParent );

    pMenu->addAction( tr( "New Document..." ), this, SLOT( slotNewDiagram() ) );
    pMenu->addAction( tr( "New Stencils Folder" ), this, SLOT( slotNewStencils() ) );
    pMenu->addAction( QPixmap( ":AW/Properties" ), tr("Class..."), this, SLOT(slotClassDialog()) );
    pMenu->addAction( QPixmap( ":AW/Properties" ), tr("Object..."), this, SLOT(slotObjectDialog()) );

    return pMenu; // caller invokes and then deletes
}

QDomElement DFile::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = AWFile::doSave( pdomDoc, pdomElemParent );

    domElemThis.setAttribute( "Strictness", getStrictness() );

    return domElemThis;
}

bool DFile::doLoad( QDomElement *pdomElemThis )
{
    nStrictness = (ADValidation::Strictness)pdomElemThis->attribute( "Strictness", "2" ).toInt();

    return AWFile::doLoad( pdomElemThis );
}

void DFile::doClear()
{
    AWFile::doClear();
}

void DFile::slotValidateFile()
{
    slotValidate( nStrictness );
}

/*!
 * \brief Add a new (empty) diagram to the workspace.
 * 
 * \author pharvey (8/16/21)
 */
void DFile::slotNewDiagram()
{
    DDiagram *p = (DDiagram*)getObject( "DDiagram" );
    if ( !p ) return;
    p->slotObjectDialog();
}

/*!
 * \brief Create an object for holding a collection of DStencil. 
 *  
 * We only allow 0-1 DStencils object for the workspace... it does not make sense to have more. 
 * The way DStencilsWidget is used - assumes 0-1. 
 * 
 * \author pharvey (5/7/20)
 */
void DFile::slotNewStencils()
{
    // will return null if one already exists
    getObject( "DStencils" );
}


