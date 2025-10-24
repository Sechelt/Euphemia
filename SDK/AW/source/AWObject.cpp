#include "LibInfo.h"
#include "AWObject.h"

#include "AWClassDialog.h"
#include "AWDataDialog.h"

#include "AWClassNameWidget.h"
#include "AWObjectNameWidget.h"
#include "AWFactoryWidget.h"
#include "AWDynamicDataWidget.h"
#include "AWObjectChildrenWidget.h"
#include "AWObjectUsesWidget.h"
#include "AWObjectUsedByWidget.h"

#include "AWPropObjectWidget.h"

AWObject::AWObject( ADObject *pobjectParent, const QString &stringName )
    : ADObject( pobjectParent, stringName )
{
    // remove ADObject as it is not directly supported in any subsequent object factories 
    mapMeta.clear();

    pEditorWidgetParent = nullptr;
    pEditorWidget       = nullptr;

    // init - if we have parent
    if ( pobjectParent )
    {
        //
        // Echo up to root. 
        //

        // An object has opened/created its editor.
        connect( this, SIGNAL(signalEditorOpened(AWObject*, AWEditorWidget*)), pobjectParent, SIGNAL(signalEditorOpened(AWObject*, AWEditorWidget*)) );
        // An object has closed/deleted its editor.
        connect( this, SIGNAL(signalEditorClosing(AWObject*, AWEditorWidget*)), pobjectParent, SIGNAL(signalEditorClosing(AWObject*, AWEditorWidget*)) );
    }
    else
    {
        // qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " " << metaObject()->className();
    }
}

AWObject::~AWObject()
{
    // remove any editor
    if ( pEditorWidget )
        delete pEditorWidget;
}

void AWObject::setEditorWidgetParent( QWidget *p )
{
    pEditorWidgetParent = p;
}

/*!
 * \brief Make ourself ready for a delete.
 *  
 * We will do what we can to make ourself ready for a delete. This may involve 
 * changing our modified state, selection state, etc. In which case we trust that 
 * the emitted signals are picked up, where needed, to disconnect from UI. 
 *  
 * \author pharvey (12/4/19)
 */
void AWObject::doClear()
{
    // Ensure there are no UI entanglements. 

    // Unselect ourself.
    // A signal will go out when we change from Selected to Unselected. This should be used by the UI
    // to ensure any UI elements we provided are disconnected/deleted from the UI. 
    // For example; \sa AWPropWidget and \sa AWDataWidget. 
    setSelected( false );

    // Close any open editor.
    // Most AWObject's do not have one but slotCloseEditor accounts for this.
    // A signal will go out before the editor is deleted. This should be used by the UI
    // to prep for the impending delete (to disconnect the editor from the UI).
    slotCloseEditor();

    // do underlying data
    // \note This will cause a call to doClear() for each child
    //       so calling this simply picks up the processing where it left off.
    ADObject::doClear();
}

QDomElement AWObject::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    // save common and children...
    QDomElement domElemThis = ADObject::doSave( pdomDoc, pdomElemParent );

    domElemThis.appendChild( doSaveDynamicData( pdomDoc ) );

    return domElemThis;
}

/*
bool AWObject::doLoad( QDomElement *pdomElemThis )                              
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
*/

/*!
 * \brief Find given string. 
 *  
 * Overrides ADObject because we want to provide a meaningful icon. 
 * 
 * \author pharvey (1/3/20)
 * 
 * \param nValueType 
 * \param listObjectPathItems 
 * \param stringFind 
 */
void AWObject::doFind( ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems, const ADFindCriteria &Criteria )
{                                                                                                                                    
    // check ourself for a match                                                                                                     
    if ( Criteria.bName && getName().contains( Criteria.stringValue ) ) eventOutputFind( listObjectPathItems, getIcon(), getName() );                            
    if ( Criteria.bCode && getCode().contains( Criteria.stringValue ) ) eventOutputFind( listObjectPathItems, getIcon(), getCode() );                            
    if ( Criteria.bComment && getComment().contains( Criteria.stringValue ) ) eventOutputFind( listObjectPathItems, getIcon(), getComment() );                      
    if ( Criteria.bDynamicData )
    {
        QList<QByteArray> l = dynamicPropertyNames();
        QByteArray a;
        foreach( a, l )
        {
            QString stringName( a );
            QString stringValue = property( a ).toString();
            if ( stringName.contains( Criteria.stringValue ) ) eventOutputFind( listObjectPathItems, getIcon(), stringName );
            if ( stringValue.contains( Criteria.stringValue ) ) eventOutputFind( listObjectPathItems, getIcon(), stringValue );
        }
    }

    // check our children                                                                                                            
    QList<ADObject*>            listChildren = getObjects( QString(), QString(), QString() );                                        
    ADObject *                  pChild;                                                                                              
    int                         nIndex = -1;                                                                                         
    foreach( pChild, listChildren )                                                                                                  
    {                                                                                                                                
        nIndex++;                                                                                                                    
                                                                                                                                     
        // append child to path (always provide pointer AND index because we happen to have both anyway)                             
        listObjectPathItems += ADObjectPathItem( nValueType, pChild, pChild->metaObject()->className(), pChild->getName(), nIndex ); 
        // do find                                                                                                                   
        pChild->doFind( nValueType, listObjectPathItems, Criteria );                                                               
        // remove child from path                                                                                                    
        listObjectPathItems.pop_back();                                                                                              
    }                                                                                                                                
}                                                                                                                                    

/*!
    doMessageBox
    
    Use this to create a QMessageBox because this will also emit signalMessageGeneral.
*/
void AWObject::doMessageBox( const QString &stringSeverity, const QString &stringCaption, const QString &stringMessage )
{
    // message box
    if ( stringSeverity.isEmpty() || stringSeverity.left(1) == "I" )
        QMessageBox::information( getEditorWidgetParent(), stringCaption, stringMessage );
    else if ( stringSeverity.left(1) == "E" )
        QMessageBox::critical( getEditorWidgetParent(), stringCaption, stringMessage );
    else if ( stringSeverity.left(1) == "W" )
        QMessageBox::warning( getEditorWidgetParent(), stringCaption, stringMessage );

    doMessage( stringSeverity , stringMessage );
}

/*!
 * \brief Work our way to root calling doGoTo(). Set focus on the way back down. 
 *  
 * This, along with doGoTo( DObject* ) (introduced later), set focus when an object 
 * is clicked in the object browser. 
 *  
 * This differs from \sa slotGoTo and \sa slotGoToView (and their supporting doGoTo and doGoToView) 
 * because in this case we know the object exists - so we can go directly to it - no need for a path. 
 * 
 * \author pharvey (4/27/20)
 */
void AWObject::doGoTo()
{
    // recurse our way to root
    if ( parent() )
    { 
        AWObject *p = (AWObject*)parent();
        p->doGoTo();
    }

    // do what we can on the way back down
}

/*!
 * \brief Open editor (or set focus).
 *  
 * Most AWObjects do not have an 'editor' widget so the default is to simply return false;  
 *  
 * \note Opening/Closing an editor is not the same as Loading/Saving a file - they are done 
 *       independently. 
 *  
 * \author pharvey (10/26/19)
 * 
 * \return bool True if we have either created or set focus in a editor False otherwise 
 */
bool AWObject::slotOpenEditor()
{
    // derived classes should 
    // emit signalEditorOpened( this, pEditorWidget );
    // emit signalEditorOpened( pEditorWidget );

    // do not propogate request to descendants

    return false;
}

/*!
 * \brief Close any editor. 
 *  
 * Here we delete any editor. Caller handles save as needed.
 *  
 * \author pharvey (12/4/19)
 * 
 * \param bRequest 
 * 
 * \return bool 
 */
bool AWObject::slotCloseEditor()
{
    // no editor? nothing to do here
    if ( !pEditorWidget ) return false;
    // let world know we are going to delete the editor so any prep can happen
    emit signalEditorClosing( this, pEditorWidget );    // echo up
    emit signalEditorClosing( pEditorWidget );          // for those that connected directly to this
    // delete the editor
    delete pEditorWidget;
    pEditorWidget = nullptr;

    // do not propogate request to descendants
    return true;
}

bool AWObject::slotClassDialog( QWidget *p )
{
    QWidget *pParent = ( p ? p : getEditorWidgetParent() );
    AWClassDialog dialog( this, pParent );
    if ( dialog.exec() == QDialog::Accepted ) return true;
    return false;
}

bool AWObject::slotObjectDialog( QWidget *p )
{
    QWidget *pParent = ( p ? p : getEditorWidgetParent() );
    AWDataDialog dialog( this, pParent );
    if ( dialog.exec() == QDialog::Accepted ) return true;
    return false;
}

/*!
 * \brief Works up the object hierarchy looking for a parent widget for editor.
 * 
 * \author pharvey (2/5/20)
 * 
 * \return QWidget* parent widget else nullptr
 */
QWidget *AWObject::getEditorWidgetParent() 
{
    if ( pEditorWidgetParent ) return pEditorWidgetParent;
    QObject *pObject = parent();
    if ( !pObject ) return nullptr;
    if ( !pObject->inherits( "AWObject" ) ) return nullptr;

    return ((AWObject*)pObject)->getEditorWidgetParent();
}

QMenu *AWObject::getContextMenu( QWidget *pParent )
{
    QMenu *pMenu = new QMenu( pParent );

    pMenu->addAction( QPixmap( ":AW/Properties" ), tr("Class..."), this, SLOT(slotClassDialog()) );
    pMenu->addAction( QPixmap( ":AW/Properties" ), tr("Object..."), this, SLOT(slotObjectDialog()) );
    pMenu->addSeparator();
    pMenu->addAction( QPixmap( ":AW/Delete" ), tr("Delete") /* + getFriendlyClassName() */, this, SLOT(slotDelete()) );

    return pMenu; // caller invokes and then deletes
}

AWClassWidget *AWObject::getClassWidget( QWidget *pWidgetParent )
{
    // caller must delete
    // delete done nicely if pWidgetParent provided

    AWClassWidget *p = new AWClassWidget( this, pWidgetParent );

    p->addTab( new AWClassNameWidget( p ), tr("Name") );

    return p;
}

/*!
 * \brief Creates a tab widget with a number of tabs for editing/viewing object properties.
 *  
 * This, via \sa slotObjectDialog and \sa AWDataDialog is the way an object properties dialog is created. 
 *  
 * Derived classes can completely override this method but most should request the object widget of the 
 * class that they are derived from and then add/remove tabs as needed. This will, hopefully, add some 
 * consistency between properties of various objects. 
 *  
 * \author pharvey (5/8/20)
 * 
 * \param pWidgetParent 
 * 
 * \return AWDataWidget* 
 */
AWDataWidget *AWObject::getObjectWidget( QWidget *pWidgetParent )
{
    // caller must delete 
    // delete done nicely if pWidgetParent provided
    // delete done nicely if used in a AWDataDialog with proper parent (typical case)

    AWDataWidget *p = new AWDataWidget( this, pWidgetParent );

    // Add some basic widgets.
    // Derived classes can call here to get the tab widget and 
    // then add their own tabs (and/or remove the existing tabs).

    p->addTab( new AWObjectNameWidget( this, p ), tr("Name") );
    // this is where derived classes should, typically, insert new tabs
    p->addTab( new AWDynamicDataWidget( this, p ), tr("Dynamic Data") );
    p->addTab( new AWFactoryWidget( this, p ), tr("Factory") );
    p->addTab( new AWObjectChildrenWidget( this, p ), tr("Children") );
    p->addTab( new AWObjectUsesWidget( this, p ), tr("Uses") );
    p->addTab( new AWObjectUsedByWidget( this, p ), tr("Used By") );

    p->setCurrentIndex( 0 );

    return p;
}

AWPropWidget *AWObject::getPropWidget( QWidget *pWidgetParent )
{
    // create the tool box
    AWPropWidget *pPropWidget = new AWPropWidget( this, pWidgetParent );
    
    // add our bits    
    pPropWidget->addWidget( tr("Name"), new AWPropObjectWidget( this, pPropWidget ) );

    // caller will add more (if needed) and/or off to the app
    // caller must delete
    return pPropWidget;
}

AWDataTable *AWObject::getListWidget( const QString &stringClass, QWidget *pWidgetParent )
{
    Q_UNUSED(stringClass);
    Q_UNUSED(pWidgetParent);

    return nullptr; 
}

/*!
 * \brief Get some text that will give User an indication of what object we are talking about.
 * 
 * \author pharvey (4/25/20)
 * 
 * \return QString 
 */
QString AWObject::getDialogCaption( AWObject *p, const QString &stringEvent )
{
    AWObject *pObject = ( p ? p : this );

    return QString( "%3 (%1 (%2))" ).arg( pObject->getFriendlyClassName() ).arg( pObject->getName() ).arg( stringEvent );
}

QDomElement AWObject::doSaveDynamicData( QDomDocument *pdomDoc )
{
    QDomElement domElem = pdomDoc->createElement( "DynamicData" );

    QList<QByteArray> l = dynamicPropertyNames();
    QByteArray a;
    foreach( a, l )
    {
        domElem.setAttribute( QString( a ), property( a ).toString() );
    }

    return domElem;
}

void AWObject::doLoadDynamicData( QDomElement *pdomElem )
{
    auto var = pdomElem->attributes();
    for ( int i=0; i < var.count(); ++i)
    {
        setProperty( var.item(i).nodeName().toLatin1(), var.item(i).nodeValue() );
    }
}


