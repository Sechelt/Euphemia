#include "LibInfo.h"
#include "ORGDiagram.h"

#include <AWFactoryWidget.h>

#include <DGraphicsSceneProxy.h>
#include <DGraphicsProxyItem.h>
#include <DGraphicsView.h>

#include "ORGDiagramEditorWidget.h"

#include "ORGPerson.h"
#include "ORGReportsTo.h"

#include "ORGLayout.h"

ORGDiagram::ORGDiagram( ADObject *pParent, const QString &stringName )
    : DDiagram( pParent, stringName )
{
    pLayout = new ORGLayout( this );

    mapMeta.clear();

    mapMeta.insert( "ORGPerson", ADMeta( QIcon(), "ORGPerson", "Person" ) );
    mapMeta.insert( "ORGReportsTo", ADMeta( QIcon(), "ORGReportsTo", "ReportsTo" ) );

    // \sa doClear
    pRoot           = nullptr;
    pDragOver       = nullptr;
    pPasteTo        = nullptr;
    // bGrid           = false;
    // bSnapGrid       = false;
    // sizeCell        = QSize( 200, 100 );
}

ORGDiagram::~ORGDiagram()
{
    delete pLayout;
}

void ORGDiagram::setScene( DGraphicsSceneProxy *p ) 
{
    DDiagram::setScene( p );

    // we always start with a root person
    pRoot = (ORGPerson*)getObject( "ORGPerson" );
    pRoot->setPos( QPointF( p->width() / 2, pRoot->boundingRect().height() / 2 + 10 ) );
}

QPixmap ORGDiagram::getIcon()
{
    return QPixmap( ":ORG/OrgChart" );
}

/*!
 * \brief  Creates a tab widget with a number of tabs for editing/viewing object properties.
 * 
 * The only thing we want to change from inherited is that we DO NOT want the object factory to be able to create/delete objects.
 * We can not create objects unless we know the valid desired pos (not any pos). 
 * We should not delete some objects ie root person. 
 *  
 * \author pharvey (5/8/20)
 * 
 * \param pWidgetParent 
 * 
 * \return AWDataWidget* 
 */
AWDataWidget *ORGDiagram::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = DDiagram::getObjectWidget( pWidgetParent );

    int n = p->indexOf( "AWFactoryWidget" );
    if ( n < 0 ) return p;

    AWFactoryWidget *pFactoryWidget = (AWFactoryWidget*)p->widget( n );
    pFactoryWidget->setAllowCreate( false );
    pFactoryWidget->setAllowDelete( false );

    p->setCurrentIndex( 0 );

    return p;
}

ADObject *ORGDiagram::getObject( const QString &s, ADObject * )
{
    bool        bFolders    = false;
    DObject *   p           = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "ORGPerson" )
        p = new ORGPerson( this );
    else if ( stringClass == "ORGReportsTo" )
    {
        p = new ORGReportsTo( this );
        if ( bFolders ) p->setFolder( tr( "Connectors" ) );
    }

    if ( !p )
    {
        doMessageBox( "WARNING", "Create Object...", "Unknown class requested " + s );
        return nullptr;
    }

    // ensure we are the OID source regardless of parent (avoids messing up OID's when reparenting DObject's)
    p->setOIDSource( this );
    // provide a two-way interface to the scene via a proxy
    DGraphicsProxyItem *pProxy = new DGraphicsProxyObject( p );
    // get the proxy to create a QGraphicsObject based object - in this case DGraphicsObject
    pProxy->doCreateGraphicsObject();
    // add QGraphicsObject to scene
    pProxy->setScene( pScene );
    // set proxy in DObject - may also be used by DObject based objects to init some other things such as create sinks 
    p->setProxy( pProxy );
    // default pos is center of scene
    p->setPos( QPointF( pScene->width() / 2, pScene->height() / 2 ) );
    // announce the birth of a new child
    emit signalCreated( p );    
    emit signalCreated( this, p );
    // set parent modified by default (can be set back if this is a load)           
    setModified();              

    // the caller should doConnect to a model ASAP
    return (ADObject*)p;
}

void ORGDiagram::mouseMoveObjects( QGraphicsSceneMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );
}

void ORGDiagram::mouseReleaseObjects( QGraphicsSceneMouseEvent *pEvent )
{
    Q_UNUSED( pEvent );
}

bool ORGDiagram::slotOpenEditor()
{
    if ( pEditorWidget ) return true;

printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
    // create view
    ORGDiagramEditorWidget *p;
    pEditorWidget = p = new ORGDiagramEditorWidget( this, getEditorWidgetParent() );
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );

    if ( isCrossHairs() ) doCreateCrossHairs();
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );

    emit signalEditorOpened( pEditorWidget );
    emit signalEditorOpened( this, pEditorWidget );

printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
    p->doGoTo( pRoot );
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );

    return true;
}

/*!
 * \brief Copy selected objects to XML. 
 *  
 * A Person may or may/not come with all of its subordinates. 
 *  
 * \todo Currently copys all subordinates as well (logic needed for when delete) but this should change (for copy). 
 *  
 * \sa isSelectionValid 
 *  
 * \author pharvey (3/25/20)
 */
void ORGDiagram::slotSelectionCopy()
{
    // validate selection 
    if ( !canCopy() ) return;

    // call default
    DDiagram::slotSelectionCopy();
}

/*!
 * \brief Delete selected objects.
 *  
 * Delete the selected objects and all of the subordinates and connecting lines.
 *  
 * \author pharvey (3/25/20)
 */
void ORGDiagram::slotSelectionDelete()
{
    // canDelete will alter selection to make complete - or return false
    if ( !canDelete() ) return;

    g_Transaction->initDelete( this, getDeleteMode() );

    pSelectionManager->doDelete();
    pLayout->doLayout();
    doEnsurePages();                                                                                                          

    g_Transaction->fini();                                                                                                    

    emit signalChangedCut( canCut() );                                                                                         
    emit signalChangedCopy( canCopy() );                                                                                       
    emit signalChangedSelection( isSelected() );                                                                              
}

/*!
 * \brief Can we insert a given class.
 *  
 * Used to support drag and drop from toolbox. 
 *  
 * \author pharvey (4/16/20)
 * 
 * \param stringClass 
 * 
 * \return bool 
 */
bool ORGDiagram::canInsert( const QString &stringClass )
{
    if ( stringClass == "Person" ) return true;

    return false;
}

/*!
 * \brief Can we accept a drop. 
 *  
 * If we can insert it we can accept a drop. 
 * 
 * \author pharvey (4/16/20)
 * 
 * \param stringClassDropped 
 * \param pointScene 
 * 
 * \return bool 
 */
bool ORGDiagram::canDrop( const QString &stringClassDropped, const QPointF &pointScene )
{
    return canInsert( stringClassDropped, pointScene );
}

bool ORGDiagram::doDragEnter( const QString &stringClass, const QPointF &pointScene )
{
    Q_UNUSED(pointScene);

    if ( !canInsert( stringClass ) )
    {
        doMessage( "ERROR", stringClass + tr( " can not be added this way." ) );
        return false;
    }
    pSelectionManager->doSelectNone();
    pDragOver = nullptr;
    return true;
}

bool ORGDiagram::doDragMove( const QString &stringClass, const QPointF &pointScene )
{
    if ( pDragOver ) pSelectionManager->setSelected( pDragOver, false );

    if ( !canInsert( stringClass ) ) return false;
    if ( !canDrop( stringClass, pointScene ) ) return false;

    pDragOver = (ORGPerson*)getRectangle( pointScene, "ORGPerson" );
    if ( pDragOver ) pSelectionManager->setSelected( pDragOver );

    return true;
}

bool ORGDiagram::doDragDrop( const QStringList &stringList, const QPointF &pointScene )
{
    QString stringClass = stringList.first();

    if ( !canInsert( stringClass ) ) return false;
    if ( !canDrop( stringClass, pointScene ) ) return false;

    if ( pDragOver ) pSelectionManager->setSelected( pDragOver, false );

    return doInsert( stringClass, pointScene );
}

/*!
 * \brief Is point valid for a new object of given class.
 *  
 * \note This assumes canInsert( stringClass ) would (or has) returned true. 
 * 
 * \author pharvey (3/22/20)
 * 
 * \param stringClass 
 * \param pointPos 
 * 
 * \return bool 
 */
bool ORGDiagram::canInsert( const QString &stringClass, const QPointF &pointPos )
{
    Q_UNUSED(stringClass);

    if ( !getRectangle( pointPos ) ) return false;

    return true;
}

/*!
 * \brief Insert new as a subordinate to person at point.
 *  
 * To support drop/paste. 
 *  
 * \author pharvey (3/23/20)
 * 
 * \param stringClass 
 * \param pointPos 
 * 
 * \return bool 
 */
bool ORGDiagram::doInsert( const QString &stringClass, const QPointF &pointPos )
{
    ORGPerson *pPerson = (ORGPerson*)getRectangle( pointPos, "ORGPerson" );
    if ( !pPerson ) return false;

    return doInsert( stringClass, pPerson );          
}

/*!
 * \brief Insert after a given rectangle.
 *  
 * \author pharvey (3/23/20)
 * 
 * \param stringClass 
 * \param pRectanglePrevious 
 * \param stringSinkPrevious 
 * 
 * \return bool 
 */
bool ORGDiagram::doInsert( const QString &stringClass, ORGPerson *pSuperior )
{
    // create a new ORGPerson 
    // connect it 
    // move it to 2 cells down from pPrevious

    if ( stringClass != "Person" && stringClass != "ORGPerson" ) return false;

    ORGPerson *     pSubordinate= (ORGPerson*)getObject( stringClass );
    ORGReportsTo *  pReportsTo  = (ORGReportsTo*)getObject( "ReportsTo" );

    pSubordinate->setPos( pSuperior->getPos() );
    pReportsTo->setPos( pSuperior->getPos() );                                                                      
    pReportsTo->doConnect( pReportsTo->getSource( CBD::EOLBegin ), pSuperior->getSink( "Bottom" ) );                    
    pReportsTo->doConnect( pReportsTo->getSource( CBD::EOLEnd ), pSubordinate->getSink( "Top" ) );             

    pLayout->doLayout();

    doEnsurePages();                                                                                                                                                                             

    if ( getEditorWidget() ) ((DDiagramEditorWidget*)getEditorWidget())->doGoTo( pSubordinate );
    
    pSelectionManager->setSelected( pSubordinate );
         
    return true;
}

/*!
 * \brief Insert the list of objects under target. 
 *  
 * Presumably; the list of objects have just beeen pasted and we want to wire them in. 
 * To do this we; 
 *  
 * 1. find the object that either has no line connected to the Top or has a line that goes nowhere and then 
 * 2. wire it to the target 
 * 3. call for layout 
 *  
 * This supports \sa doPostLoad. 
 * 
 * \author pharvey (4/15/20)
 * 
 * \param pTarget
 * \param listObjects 
 * 
 * \return bool 
 */
bool ORGDiagram::doInsert( ORGPerson *pTarget, const QList<ORGPerson*> &listObjects )
{
    ORGPerson *p;
    foreach( p, listObjects )
    {
        if ( !p->inherits( "ORGPerson" ) ) continue;
        ORGPerson *pPerson = (ORGPerson*)p;
        if ( !pPerson->getLine( "Top" ) )
        {
            ORGReportsTo *pReportsToNew = (ORGReportsTo *)getObject( "ORGReportsTo" );                                                                        
            pReportsToNew->setPos( pTarget->getPos() );                                                                      
            pReportsToNew->doConnect( pReportsToNew->getSource( CBD::EOLBegin ), pTarget->getSink( "Bottom" ) );                    
            pReportsToNew->doConnect( pReportsToNew->getSource( CBD::EOLEnd ), pPerson->getSink( "Top" ) );             
        }
        else if ( !pPerson->getLine( "Top" )->getRectangle( CBD::EOLBegin ) )
        {
            ORGReportsTo *pReportsTo = (ORGReportsTo *)pPerson->getLine( "Top" );                                                                        
            pReportsTo->doConnect( pReportsTo->getSource( CBD::EOLBegin ), pTarget->getSink( "Bottom" ) );                    
        }
    }

    pLayout->doLayout();
    doEnsurePages();                                                                                                           

    return true;
}

bool ORGDiagram::doLoad( QDomElement *pdomElemModel )                               
{
    slotSelectAll();
    pSelectionManager->doDelete();
    pRoot = nullptr;
    return DDiagram::doLoad( pdomElemModel );
}

/*!
 * \brief Extended - we want to 'wire in' any pasted objects.
 *  
 * Catch case where objects have just been pasted into the diagram. In this case we must 
 * 'wire' them into the diagram and perform an auto-layout. 
 *  
 * \author pharvey (4/16/20)
 */
void ORGDiagram::doPostLoad()                                                          
{
    // we only care about paste...
    if ( !g_Transaction->canPaste() ) return DDiagram::doPostLoad();

    // canPaste should have been called and it would have set the target for paste
    Q_ASSERT(pPasteTo);

    // do the default...
    DDiagram::doPostLoad();

    // Pasted objects on diagram. Now insert them into OrgChart...
    QList<ORGPerson*> listPastedObjects;

    QList<QString> listPastedReferences = g_Transaction->mapXRef.values();                 
    QString stringPath;                                                          
    foreach( stringPath, listPastedReferences )                                            
    {                                                                            
        ADObject *p = g_Transaction->pRoot->getObject( stringPath.split( ',' ) );
        Q_ASSERT(p);

        if ( p->inherits("ORGPerson") )                                            
        {
            listPastedObjects.append( (ORGPerson*)p );
        }                                                                        
    }                                                                            

    doInsert( pPasteTo, listPastedObjects );

    pPasteTo = nullptr;
}                                                                                    

/*!
 * \brief Selects the given person and all of its subordinates. 
 *  
 * This also selects the lines. 
 *  
 * This is called to prepare for a delete. 
 *  
 * \sa slotSelectionDelete. 
 * \sa isSelectionValid 
 *  
 * \author pharvey (4/15/20)
 * 
 * \param p 
 */
void ORGDiagram::doSelect( ORGPerson *p )
{
printf( "[PAH][%s][%s][%d] %p\n", __FILE__, __FUNCTION__, __LINE__, p->getLine( "Top" ) );
    if ( p->getLine( "Top" ) ) pSelectionManager->setSelected( p->getLine( "Top" ) );
    pSelectionManager->setSelected( p );

    QList<ORGPerson*> l = p->getSubordinates();
    ORGPerson *pPerson;
    foreach( pPerson, l )
    {
printf( "[PAH][%s][%s][%d] %p\n", __FILE__, __FUNCTION__, __LINE__, pPerson );
printf( "[PAH][%s][%s][%d] %s\n", __FILE__, __FUNCTION__, __LINE__, pPerson->metaObject()->className() );
        doSelect( pPerson );
    }
}

void ORGDiagram::doClear()
{
    pRoot           = nullptr;
    pDragOver       = nullptr;
    pPasteTo        = nullptr;
//    bGrid           = false;
//    bSnapGrid       = false;

    DDiagram::doClear();
}

/*!
 * \brief Is selection valid for a paste.
 * 
 * \author pharvey (4/16/20)
 * 
 * \return bool 
 */
bool ORGDiagram::canPaste()
{
    if ( !DDiagram::canPaste() ) return false;

    QList<ADObject*> l = pSelectionManager->getSelected();                                                                          
                                                                                                                 
    if ( l.count() < 1 )                                                                                         
    {                                                                                                            
        doMessageBox( "WARNING", tr("Paste"), tr("Please select a Person.") );                                     
        return false;                                                                                            
    }                                                                                                            
                                                                                                                 
    if ( l.count() > 1 )                                                                                         
    {                                                                                                            
        doMessageBox( "WARNING", tr("Paste"), tr("Too many targets. Please select exactly one Person.") );
        return false;                                                                                            
    }                                                                                                            
                                                                                                                 
    ADObject *p = l.last();                                                                                      
    if ( !p->inherits( "ORGPerson" ) )                                                                              
    {                                                                                                            
        doMessageBox( "WARNING", tr("Paste"), tr("Please select exactly one Person.") );                           
        return false;                                                                                            
    }                                                                                                            
                                                                                                                 
     pPasteTo = (ORGPerson*)p;                                                                                    
                                                                                                                 
    return true;                                                                                                 
}

/*!
 * \brief Is selection valid for a delete.
 * 
 * \author pharvey (4/16/20)
 * 
 * \return bool 
 */
bool ORGDiagram::canDelete()  
{ 
    QList<ADObject*> l = pSelectionManager->getSelected();
    if ( l.count() < 1 )
    { 
//        doMessageBox( "WARNING", tr("Selection Validate"), tr("No selection.") );                            
        return false;                                                                                               
    }

    if ( l.first() == getRoot() )
    { 
//        doMessageBox( "WARNING", tr("Selection Validate"), tr("Invalid selection. Can not delete root person.") );                            
        return false;                                                                                               
    }

    return isSelectionValid(); 
}

/*!
 * \brief Check if selection is valid (for cut/delete). 
 *  
 * 1. all selected persons must be siblings 
 * 2. IF delete THEN select all subordinates for them as well 
 *  
 * \author pharvey (4/1/20)
 * 
 * \return bool 
 */
bool ORGDiagram::isSelectionValid()
{
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );
    ORGPerson *pSuperior = nullptr;

//    QList<ORGPerson*> listSelection;                                                                                
    {                                                                                                               
        QList<ADObject*> l = pSelectionManager->getSelected();                                                                         
        ADObject *p;                                                                                                
        foreach( p, l )                                                                                             
        {                                                                                                           
printf( "[PAH][%s][%s][%d] %p\n", __FILE__, __FUNCTION__, __LINE__, p );
printf( "[PAH][%s][%s][%d] %s\n", __FILE__, __FUNCTION__, __LINE__, p->metaObject()->className() );
            if ( p->inherits( "ORGPerson" ) )
            {
                ORGPerson *pSubordinate = (ORGPerson*)p;
                if ( pSuperior && pSuperior != pSubordinate->getSuperior() )
                { 
// message needs to go elsewhere as we call canDelete (which calls here) just to enable/disable controls
//                    doMessageBox( "WARNING", tr("Selection Validate"), tr("Invalid selection. Can not copy/delete multiple people unless they all report to the same person.") );                            
                    return false;                                                                                               
                }
                else 
                    pSuperior = pSubordinate->getSuperior();
                pSelectionManager->setSelected( pSubordinate );
            }
            else                                                                                                    
                pSelectionManager->setSelected( p, false );                                                                            
        }                                                                                                           
    }                                                                                                               
                                                                                                                    
    if ( !pSelectionManager->hasSelection()  )                                                                                
    {
// message needs to go elsewhere as we call canDelete (which calls here) just to enable/disable controls
//        doMessageBox( "WARNING", tr("Selection Validate"), tr("No viable selection.") );                            
        return false;                                                                                               
    }                                                                                                               

    // select all subordinates
    // - definately want to do this in prep for a cut/delete
    // - may, or may not, want to do this for a simple copy
    QList<ADObject*> l = pSelectionManager->getSelected(); 
printf( "[PAH][%s][%s][%d] num selected %lld\n", __FILE__, __FUNCTION__, __LINE__, l.count() );
    ADObject *p;
    foreach( p, l )
    {
printf( "[PAH][%s][%s][%d] %p\n", __FILE__, __FUNCTION__, __LINE__, p );
printf( "[PAH][%s][%s][%d] %s\n", __FILE__, __FUNCTION__, __LINE__, p->metaObject()->className() );
        doSelect( (ORGPerson*)p );
    }
printf( "[PAH][%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__ );

    return true;
}

