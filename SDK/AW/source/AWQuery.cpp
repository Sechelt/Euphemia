#include "LibInfo.h"
#include "AWQuery.h"

AWQuery::AWQuery( ADObject *pParent,  const QString &stringName )
    : AWObject( pParent, stringName )
{
    bQueried = false;
}

AWQuery::~AWQuery()
{
}

QPixmap AWQuery::getIcon()
{
    return QPixmap( ":AW/FileNew" );
}

QMenu *AWQuery::getContextMenu( QWidget *pParent )
{
    QMenu *pMenu = new QMenu( pParent );

    pMenu->addAction( QPixmap( ":AW/Refresh" ), tr("Refresh"), this, SLOT(slotRefresh()) );

    return pMenu;
}

AWDataWidget *AWQuery::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = AWObject::getObjectWidget( pWidgetParent );

//    p->removeByClass( "AWObjectNameWidget" );
//    p->insertTab( 0, new AWDocumentNameWidget( this, p, getFileName() ), tr("Document") );

    p->setCurrentIndex( 0 );

    return p;
}

/*!
 * \brief Save to XML. 
 *  
 * By default we save self and all children. Here we save ourself and NOT our children.  
 * 
 * \author pharvey (1/5/21)
 * 
 * \param pdomDoc 
 * \param pdomElemParent 
 * 
 * \return QDomElement 
 */
QDomElement AWQuery::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = pdomDoc->createElement( "Class" );
    domElemThis.setAttribute( "ClassName", metaObject()->className() );

    if ( !getName().isEmpty() ) domElemThis.setAttribute( "Name", getName() );
    if ( !getCode().isEmpty() ) domElemThis.setAttribute( "Code", getCode() );
    if ( !getComment().isEmpty() ) domElemThis.setAttribute( "Comment", getComment() );
    if ( g_Transaction->canCopy() ) domElemThis.setAttribute( "Path", getPathString( g_Transaction->pRoot ) );
    domElemThis.setAttribute( "OID", getOID() );
    domElemThis.setAttribute( "NextOID", nNextOID /* DO NOT use getNextOID() for this */ );

    pdomElemParent->appendChild( domElemThis );

    return domElemThis;
}

bool AWQuery::doLoad( QDomElement *pdomElemThis )
{
    // load common and children... 
    ADObject::doLoad( pdomElemThis );

    // load additional nodes...
    QDomElement         domElem;
    QDomNode            domNode;
    domNode = pdomElemThis->firstChild();
    while( !domNode.isNull() ) 
    {
        domElem = domNode.toElement();
        if( domElem.isNull() ) 
        {
            domNode = domNode.nextSibling();
            continue;
        }

        if ( domElem.tagName() == "DynamicData" ) doLoadDynamicData( &domElem );

        domNode = domNode.nextSibling();
    }

    return true;
}

void AWQuery::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    bLoadingChildren = true;

    // derived classes will probably want to do same 

    bLoadingChildren = false;
    bQueried = true;
}

/*!
 * \brief Clear 
 *  
 * We replace doClear() because to us doClear means delete our children.
 * 
 * \author pharvey (1/3/21)
 */
void AWQuery::doClear()
{
    setSelected( false );

    // Close any open editor.
    // Most AWObject's do not have one but slotCloseEditor accounts for this.
    // A signal will go out before the editor is deleted. This should be used by the UI
    // to prep for the impending delete (to disconnect the editor from the UI).
    slotCloseEditor();

    // delete our children - giving them a chance to doClear()
    QList<ADObject*> listChildren = getObjects();
    ADObject *p;
    foreach( p, listChildren )
    {
        slotDelete( p );
    }

    // we are never modified - so set it back to false - letting others know
    setModified( false );
    bQueried = false;
}

void AWQuery::slotRefresh()
{
    if ( bQueried ) doClear();
    doQuery();
}

/*!
 * \brief Intercept modified event. 
 *  
 * Stop modified state from propagating (to root). 
 * 
 * \author pharvey (1/3/21)
 * 
 * \param pSource 
 */
void AWQuery::eventModified( ADObject *pSource )
{
    Q_UNUSED( pSource );
}


