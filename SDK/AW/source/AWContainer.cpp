#include "LibInfo.h"
#include "AWContainer.h"

#include "AWFile.h"

#include "AWDocumentNameWidget.h"
#include "AWFactoryWidget.h"
#include "AWObjectChildrenWidget.h"

AWContainer::AWContainer( AWObject *pobjectParent, const QString &stringName )
    : AWObject( pobjectParent, stringName ), ADDocument( this )
{
    bLoaded = false;

    // set parent modified when we change our file name
    if ( pobjectParent )
        connect( this, SIGNAL(signalChangedFileName()), pobjectParent, SLOT(slotModified(true)) );
}

AWContainer::~AWContainer()
{
}

/*!
    setFileName

    Set stringFileName but also update plistviewitem.
*/    
void AWContainer::setFileName( const QString &stringFileName )
{
    this->stringFileName = stringFileName;
    stringActiveFileName = stringFileName;
        
    emit signalChangedFileName();
    emit signalChangedFileName( stringFileName );
}

QPixmap AWContainer::getIcon( AWObject::enumIconRoles nRole )
{
    if ( nRole == AWObject::nIconRoleExpanded )
        return QPixmap( ":AW/FolderOpened" );
    
    return QPixmap( ":AW/FolderClosed" );
}

QPixmap AWContainer::getIcon()
{
    return QPixmap( ":AW/FolderClosed" );
}

/*!
 * \brief Creates a tab widget with a number of tabs for editing/viewing object properties. 
 *  
 * We have introduced \sa ADDocument so we replace \sa AWObjectNameWidget with \sa AWDocumentNameWidget. 
 * 
 * \author pharvey (5/8/20)
 * 
 * \param pWidgetParent 
 * 
 * \return AWDataWidget* 
 */
AWDataWidget *AWContainer::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = AWObject::getObjectWidget( pWidgetParent );

    p->removeByClass( "AWObjectNameWidget" );

    p->insertTab( 0, new AWDocumentNameWidget( this, p, getFileName() ), tr("Document") );

    p->setCurrentIndex( 0 );

    return p;
}

QString AWContainer::getFileName()
{
    return stringFileName;
}

QString AWContainer::getFileSpec()
{
    QString string( metaObject()->className() );

    return string + " (*" + getFileExtension() + ")";
}

ADObject *AWContainer::getObject( const QString &s, ADObject *p )
{
    return AWObject::getObject( s, p );
}

QString AWContainer::getFileExtension()
{
    return "." + QString( metaObject()->className() );
}

/*!
 * \brief Save self to XML.
 *  
 * We override this so that we can handle file based classes differently. 
 *  
 * \author pharvey (5/4/20)
 * 
 * \param pdomDoc 
 * \param pdomElemParent 
 * 
 * \return QDomElement 
 */
QDomElement AWContainer::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = pdomDoc->createElement( "Class" );
    domElemThis.setAttribute( "ClassName", metaObject()->className() );

    if ( !getName().isEmpty() ) domElemThis.setAttribute( "Name", getName() );
    if ( !getCode().isEmpty() ) domElemThis.setAttribute( "Code", getCode() );
    if ( !getComment().isEmpty() ) domElemThis.setAttribute( "Comment", getComment() );
    domElemThis.setAttribute( "OID", getOID() );
    domElemThis.setAttribute( "NextOID", nNextOID /* DO NOT use getNextOID() for this */ );

    // save the children
    QList<ADObject*> listChildren = getObjects();
    ADObject *pChild;
    foreach( pChild, listChildren )
    {   
        if ( pChild->inherits( "AWFile" ) )
        { 
            AWFile *pFile = (AWFile *)pChild;
            if ( pFile->getFileName().isEmpty() ) continue; // ignore when no file name
            QDomElement domElem = pdomDoc->createElement( "Class" );
            domElem.setAttribute( "ClassName", pFile->metaObject()->className() );
            {
                QString     stringPathDiff;
                QFileInfo   fileinfo( pFile->getFileName() );
                if ( !getRootDir().isEmpty() )
                { 
                    stringPathDiff = AWObject::getPathDiff( getRootDir(), fileinfo.absolutePath() );
                }

                if ( stringPathDiff.isEmpty() )
                    domElem.setAttribute( "Location", fileinfo.fileName() );
                else
                    domElem.setAttribute( "Location", stringPathDiff + "/" + fileinfo.fileName() );
            }
            domElemThis.appendChild( domElem );
        }
        else if ( pChild->inherits( "AWContainer" ) )
        {
            AWContainer *  pcontainer   = (AWContainer *)pChild;
            QDomElement    domElem      = pdomDoc->createElement( "Class" );

            domElem.setAttribute( "ClassName", pcontainer->metaObject()->className() );
            if ( pcontainer->getFileName().isEmpty() )
                domElem.setAttribute( "Location", pcontainer->getFileName() );
            else
            {
                QFileInfo   fileinfo( pcontainer->getFileName() );
                QString     stringPathDiff = AWObject::getPathDiff( getRootDir(), fileinfo.absolutePath() );

                if ( stringPathDiff.isEmpty() )
                    domElem.setAttribute( "Location", fileinfo.fileName() );
                else
                    domElem.setAttribute( "Location", stringPathDiff + "/" + fileinfo.fileName() );
            }
            domElemThis.appendChild( domElem );
        }
        else
        {
            pChild->doSave( pdomDoc, &domElemThis );
        }
    }

    pdomElemParent->appendChild( domElemThis );

    return domElemThis;
}

/*!
 * \brief Load self from XML.
 * 
 * We DO NOT want to call AWObject::doLoad nor ADObject::doLoad as we want to handle file based 
 * children with special consideration... we only want to save a file name for them. 
 *  
 * THIS IS THE SAME (enough) AS ADObject. Consider removing. 
 *  
 * \author pharvey (5/4/20)
 * 
 * \param pdomElemThis 
 * 
 * \return bool 
 */
bool AWContainer::doLoad( QDomElement *pdomElemThis )
{
    QString stringAttribute;

    // where possible - avoid setters
    // some derived classes override setters and map to other objects 
    stringName      = pdomElemThis->attribute( "Name" ); setObjectName( stringName );
    stringCode      = pdomElemThis->attribute( "Code" );
    stringComment   = pdomElemThis->attribute( "Comment" );

    stringAttribute = pdomElemThis->attribute( "NextOID" );
    Q_ASSERT( !stringAttribute.isNull() );
    nNextOID = stringAttribute.toInt();

    // - replace the temp OID with the one being loaded
    stringAttribute = pdomElemThis->attribute( "OID" );
    Q_ASSERT( !stringAttribute.isNull() );
    nOID = stringAttribute.toInt();

    // process child nodes...
    bool        bErrors     = false;
    QDomNode    domNode     = pdomElemThis->firstChild();

    // process all nodes
    while ( !domNode.isNull() )
    {
        if ( domNode.nodeType() != QDomNode::ElementNode )
        {
            domNode = domNode.nextSibling();
            continue;
        }

        QDomElement domElem = domNode.toElement();
        if ( domElem.isNull() )
        {
            domNode = domNode.nextSibling();
            continue;
        }

        if ( domElem.tagName() == "Class" )
        { 
            if ( !doLoadClass( &domElem ) ) bErrors = true;
        }

        domNode = domNode.nextSibling();
    }

    return bErrors;
}

/*!
 * \brief Create an object from a class in the XML.
 *  
 * We override this so that we can handle file based classes differently. 
 *  
 * \author pharvey (5/4/20)
 * 
 * \param pdomElemClass 
 * 
 * \return bool 
 */
bool AWContainer::doLoadClass( QDomElement *pdomElemClass )
{
    QString stringClass = pdomElemClass->attribute( "ClassName" );
    if ( stringClass.isEmpty() ) return false;

    ADObject *pObject = nullptr;
    if ( getClasses().contains( stringClass ) )
    { 
        pObject = getObject( stringClass );
    }
    else
    {
        doMessage( "ERROR", tr("Unknown class: ") + stringClass );
        return false;
    }

    if ( !pObject ) return false;

    if ( pObject->inherits( "AWFile" ) )
    {
        AWFile *p = (AWFile*)pObject; 

        QString stringLocation = pdomElemClass->attribute( "Location", "" );
        if ( !stringLocation.isEmpty() ) p->setFileName( getRootDir() + "/" + stringLocation );
        return true;
    }
    else if ( pObject->inherits( "AWContainer" ) )
    { 
        AWContainer *p = (AWContainer*)pObject; 

        QString stringLocation = pdomElemClass->attribute( "Location", "" );
        if ( !stringLocation.isEmpty() ) p->setFileName( getRootDir() + "/" + stringLocation );
        return true;
    }

    return pObject->doLoad( pdomElemClass );
}

void AWContainer::doFindLoaded( ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems, const ADFindCriteria &Criteria )
{
    if ( Criteria.bOther && getAuthor().contains( Criteria.stringValue ) ) eventOutputFind( listObjectPathItems, getIcon(), getAuthor() );
    if ( Criteria.bOther && getVer().contains( Criteria.stringValue ) ) eventOutputFind( listObjectPathItems, getIcon(), getVer() );                              
    AWObject::doFind( nValueType, listObjectPathItems, Criteria );                                                                 

    return;
}

/*!
 * \brief Open given file name. 
 *  
 * Starts by making a request to Close any existing data (including all child objects). 
 * 
 * \author pharvey (4/25/20)
 * 
 * \param stringFileName 
 * 
 * \return bool 
 */
bool AWContainer::slotOpen( const QString &stringFileName )                                                                                            
{                                                                                                                                                      
    // sanity check
    if ( stringFileName.isEmpty() ) return false;                                                                                                      
                                                                                                                                                       
    // make sure we start with no data                                                                                                                 
    if ( slotClose() != AWObject::SaveSuccess ) return false;                                                                                          
                                                                                                                                                       
    // load
    stringActiveFileName = stringFileName;                                                                                                                                            
    if ( !doRead( stringFileName ) )
    {
        stringActiveFileName = getFileName();
        return false;                                                                                                     
    }
    
    // set file name using method to trigger signal
    // but then undo the change to the modify status                                                                                                                                                   
    setFileName( stringFileName );                                                                                                                     
    setModified( false );                                                                                                                              
    bLoaded = true;                                                                                                                                    
                                                                                                                                                       
    return true;                                                                                                                                       
}                                                                                                                                                      
                                                                                                                                                       
/*!
 * \brief Save. 
 *  
 * Save children. 
 * Save self. 
 *  
 * \author pharvey (4/25/20)
 * 
 * \param bRequest 
 * 
 * \return AWObject::SaveResults 
 */
AWObject::SaveResults AWContainer::slotSave( bool bRequest )                                                                                           
{
    // nothing to save
    if ( !isModified() && !isNew() ) return AWObject::SaveSuccess;                                                                    
    // no file name - defer to SaveAs
    if ( isNew() ) return slotSaveAs( bRequest );                                                                                      
    // give child objects a chance to save       
    stringActiveFileName = getFileName();                                                                                                               
    AWObject::SaveResults n = doSaveChildren( bRequest );
    if ( n == AWObject::SaveCancel && bRequest ) return n;                                                                                     
    // save self
    if ( !doWrite( getFileName() ) ) return AWObject::SaveError;                                                                                                                   
                                                                                                                                                       
    return AWObject::SaveSuccess;                                                                                                                      
}                                                                                                                                                      
                                                                                                                                                       
/*!
 * \brief Save self after specifying a file name.
 *  
 * Select a file. 
 * Save children. 
 * Save self. 
 *  
 * \author pharvey (4/25/20)
 * 
 * \param bRequest 
 * 
 * \return AWObject::SaveResults 
 */
AWObject::SaveResults AWContainer::slotSaveAs( bool bRequest )                                                                                         
{                                                                                                                                                      
    // get file name                                                                                                                                   
    QString stringFileName;                                                                                                                            
                                                                                                                                                       
    if ( getFileName().isEmpty() )                                                                                                                     
    {                                                                                                                                                  
        AWContainer *pcontainer = (AWContainer*)getParent( "AWContainer" );                                                                            
        if ( pcontainer && !(pcontainer->getFileName()).isEmpty() )                                                                                    
            stringFileName = QFileDialog::getSaveFileName( 0, getDialogCaption() + tr( " Save As..." ), QFileInfo( pcontainer->getFileName() ).absoluteFilePath(), getFileSpec() );
        else                                                                                                            
        {
            stringFileName = QFileDialog::getSaveFileName( 0, getDialogCaption() + tr( " Save As..." ), QString(), getFileSpec() );                                                
        }
    }                                                                                                                                                  
    else                                                                                                                                               
        stringFileName = QFileDialog::getSaveFileName( 0, getDialogCaption() + tr( " Save As..." ), QFileInfo( getFileName() ).absoluteFilePath(), getFileSpec() );                
                                                                                                                                                       
    if ( stringFileName.isEmpty() ) return ( bRequest ? AWObject::SaveCancel : AWObject::SaveSuccess );                                                
                                                                                                                                                       
    // append extension                                                                                                                                
    if ( stringFileName.right( getFileExtension().length() ) != getFileExtension() )    
    {
        stringFileName += getFileExtension();                                                                                                          
    }
                                                                                                                                                       
    QFileInfo fileinfo( stringFileName );                                                                                                              
    stringActiveFileName = stringFileName = fileinfo.absoluteFilePath();                                                                                                      

    // give child objects a chance to save                                                                                                                       
    AWObject::SaveResults n = doSaveChildren( bRequest );
    if ( n == AWObject::SaveCancel && bRequest ) return n;                                                                                     
                                                                                                                                                               
    // write data
    if ( !doWrite( stringFileName ) ) return AWObject::SaveCancel;                                                                                     
                                                                                                                                                       
    // set file name using method to trigger signal
    // but then undo the change to the modify status                                                                                                                                                   
    setFileName( stringFileName );                                                                                                                     
    setModified( false );                                                                                                                              
    bLoaded = true;                                                                                                                                    
                                                                                                                                                       
    return AWObject::SaveSuccess;                                                                                                                      
}                                                                                                                                                      


/*!
 * \brief Close (reinit) 
 *  
 * Save children (closing editors). 
 * Save self. 
 * Clear self (delete all children etc). 
 *  
 * \author pharvey (12/4/19)
 * 
 * \param bRequest 
 * 
 * \return AWFile::SaveResults 
 */
AWObject::SaveResults AWContainer::slotClose( bool bRequest )                                                                                           
{             
    AWObject::SaveResults n = doCloseChildren( bRequest );
    if ( n == AWObject::SaveCancel && bRequest ) return n;                                                                                     
                                                          
    // All *file* children have been saved or User has elected to discard any changes.                                                                          
    // No active editors at this point.
    
    // Now deal with ourself. Provide an opportunity to save any changes.                                                                               
    if ( isModified() )                                                                                                                                
    {                                                                                                                                                   
        QMessageBox::StandardButton nButton;                                                                                                            
        if ( bRequest )                                                                                                                                 
            nButton = QMessageBox::question( getEditorWidgetParent(),                                                                                                   
                                             getDialogCaption(),                                                                                                  
                                             tr("Do you want to save changes?") + "\n\n" + getFileName(),
                                             (QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard) );                                        
        else                                                                                                                                            
            nButton = QMessageBox::question( getEditorWidgetParent(),                                                                                                   
                                             getDialogCaption(),                                                                                                  
                                             tr("Do you want to save changes?") + "\n\n" + getFileName(),
                                             QMessageBox::Save | QMessageBox::Discard );                                                                
                                                                                                                                                        
        if ( nButton == QMessageBox::Cancel ) return SaveCancel;                                                                                        
        if ( nButton == QMessageBox::Save )                                                                                                             
        {                                                                                                                                               
            AWObject::SaveResults n = slotSave( bRequest );                                                                                             
            if ( n == AWObject::SaveCancel && bRequest  ) return n;                                                                                     
        }                                                                                                                                               
    }                                                                                                                                                   
                                                                                                                                                        
    // get rid of our data                                                                                                                              
    doClear();                                                                                                                                          
    setModified( false );                                                                                                                               
                                                                                                                                                        
    return AWObject::SaveSuccess;                                                                                                                       
}                                                                                                                                                       

/*!
 * \brief Delete child after giving it a chance to Save any modifications. 
 *  
 * The child can not Cancel so if there are modifications the User can only 
 * Save or Lose changes. 
 *  
 * \note The app should probably catch signalDeleteRequest(ADObject*) for the root but 
 *       it depends upon the general strategy the app has for the root.
 *  
 * \note The objects are untangled from the UI in doClear() and this is called in 
 *       ADObject::slotDelete(ADObject *). 
 *  
 * \author pharvey (12/4/19)
 * 
 * \param pChild 
 * 
 * \return bool 
 */
bool AWContainer::slotDelete( ADObject *pChild )
{
/*
    // chance to Save                              
    if ( pChild->inherits("AWContainer") )         
        ((AWContainer*)pChild)->slotClose( false );
    else if ( pChild->inherits("AWFile") )         
        ((AWFile*)pChild)->slotClose( false );     
*/

    // do it
    ADObject::slotDelete( pChild );

    return true;
}

/*!
 * \brief Save child AWFile/AWContainer objects. 
 *  
 * Gives each a chance to save themselves. 
 * Does NOT close any active editors. 
 * 
 * \author pharvey (4/25/20)
 * 
 * \param bRequest 
 * 
 * \return AWObject::SaveResults 
 */
AWObject::SaveResults AWContainer::doSaveChildren( bool bRequest )
{
    // save file based (AWFile/AWContainer) children                                                                                                                    
    QList<ADObject*> listChildren = getObjects();                                                                                                      
    ADObject *p;                                                                                                                                       
    foreach( p, listChildren )                                                                                                                         
    {                                                                                                                                                  
        if ( p->inherits( "AWContainer" ) )                                                                                                            
        {
            AWContainer *pContainer = (AWContainer *)p;
            if ( pContainer->isNew() || pContainer->isModified() )
            { 
                AWObject::SaveResults n = pContainer->slotSave( bRequest );                                                                                
                if ( n == AWObject::SaveCancel && bRequest ) return n;  
                // containers will ignore objects with empty file name during save so just leave them                                                                                   
                // if ( n == AWObject::SaveLoseChanges && pContainer->isNew() ) slotDelete( p );                                              
                // if ( n == AWObject::SaveLoseChanges ) pContainer->slotClose( false );                                                                   
            }
        }                                                                                                                                              
        else if ( p->inherits( "AWFile" ) )                                                                                                            
        {                                                                                                                                              
            AWFile *pFile = (AWFile*)p;  

            Q_ASSERT( pFile->isLoadWithEditor() );                                                                                                              

            if ( pFile->isLoadWithEditor() && pFile->isEditor() )
            { 
                if ( pFile->isNew() || pFile->isModified() )
                {
                    AWObject::SaveResults n = pFile->slotSave( bRequest );                                                                                     
                    if ( n == AWObject::SaveCancel && bRequest ) return n;                                                                                     
                    // containers will ignore objects with empty file name during save so just leave them                                                                                   
                    // if ( n == AWObject::SaveLoseChanges && pFile->isNew() ) slotDelete( p );                                                   
                    // if ( n == AWObject::SaveLoseChanges ) pFile->slotClose( false );                                                                        
                }
            }
        }                                                                                                                                              
    }                                                                                                                                                  

    return AWObject::SaveSuccess;
}

/*!
 * \brief Close child AWFile/AWContainer objects. 
 *  
 * Similar to \sa doSaveChildren() but in this case we are doing a save because we want to 
 * close the workspace. So we close any editors as well. 
 * 
 * \author pharvey (4/25/20)
 * 
 * \param bRequest 
 * 
 * \return AWObject::SaveResults 
 */
AWObject::SaveResults AWContainer::doCloseChildren( bool bRequest )
{
    // close all file based children                                                                                                                    
    QList<ADObject*> listChildren = getObjects();                                                                                                       
    ADObject *p;                                                                                                                                        
    foreach( p, listChildren )                                                                                                                          
    {                                                                                                                                                   
        if ( p->inherits( "AWContainer" ) )                                                                                                             
        {                                                                                                                                               
            AWContainer *pContainer = (AWContainer*)p;                                                                                                                 
            AWObject::SaveResults n = pContainer->slotClose( bRequest );                                                                         
            if ( n == AWObject::SaveCancel && bRequest ) return n;                                                                                      
        }                                                                                                                                               
        else if ( p->inherits( "AWFile" ) )                                                                                                             
        {                                                                                                                                               
            AWFile *pFile = (AWFile*)p;      

            Q_ASSERT( pFile->isLoadWithEditor() );                                                                                                              

            if ( pFile->isLoadWithEditor() && pFile->isEditor() )
            {
                AWObject::SaveResults n = pFile->slotCloseEditor( bRequest );
                if ( n == AWObject::SaveCancel && bRequest ) return n;                                                                                      
            }
        }                                                                                                                                               
    }                                                                                                                                                   

    return AWObject::SaveSuccess;
}

/*!
    doRead
    
    This method should be fine for derived classes because it calls doLoad() which is implemented by
    the derived class to handle tags this method finds. However; a derived class may want more control
    over the read - in which case it can implement a new version of this method.
*/    
bool AWContainer::doRead( const QString &stringFileName )
{
    // Open File
    QFile file( stringFileName );
    if ( !file.open( QIODevice::ReadOnly ) )
    {
        doMessageBox( "WARNING", LIB_NAME, "Could not open file.\n" + file.fileName() );
        return false;
    }

    // read file into XML document...
    QDomDocument domDoc( metaObject()->className() );
    QDomDocument::ParseResult result = domDoc.setContent( &file );
    if ( !result )
    {
        doMessageBox( "ERROR", LIB_NAME, tr("Invalid file format.\nCould not create an XML document from file.\n\n") + file.fileName() );
        doMessage( "ERROR", QString( "%1 Line: %2 Column: %3" ).arg( result.errorMessage ).arg( result.errorLine ).arg( result.errorColumn ) );
        file.close();
        return false;
    }
    file.close();

    // validate XML...
    // Root element is only used to check that class in XML matches this class.
    QDomElement domElemRoot = domDoc.documentElement();
    if ( domElemRoot.isNull() )
    {
        doMessageBox( "ERROR", LIB_NAME, tr("Invalid file format.\nCould not find a document element.\n\n") + stringFileName );
        return false;
    }
    if ( domElemRoot.tagName() != metaObject()->className() )
    {
        doMessageBox( "ERROR", LIB_NAME, tr("Invalid file format.\nFile does not appear to be a ") + QString( metaObject()->className() ) + "\n\n" + stringFileName );
        return false;
    }
    if ( domElemRoot.attribute( "DocVer" ) != SDK_DOC_VER ) 
    {
        doMessageBox( "ERROR", LIB_NAME, tr("Invalid file format.\nFile is not Version ") + SDK_DOC_VER );
        return false;
    }

    // Its our data so lets start processing using first child (only child)...
    QDomNode domNode = domElemRoot.firstChild();
    if ( domNode.isNull() ) return false;
    if ( domNode.nodeType() != QDomNode::ElementNode ) return false;

    QDomElement domElemThis = domNode.toElement();

    // ini transaction
    g_Transaction->initLoad( this );

    doLoad( &domElemThis );
    doLoadADDocument( &domElemThis );

    bLoaded = true;

    // nothing for post-load but perhaps in the future so call it
    doPostLoad();

    // fini transaction
    g_Transaction->fini();
    setModified( false );

    return true;
}

/*!
    doWrite
    
    This method should be fine for derived classes because it can handle saving references
    to any classes derived from AWFile and AWContainer. The className() is used to
    identify the actual type of class being referenced.
    
    A class derived from AWContainer may want to replace this method if it wants greater
    control over the save - for example if it wants to save more properties or wants to use
    different tag naming convention.
*/
bool AWContainer::doWrite( const QString &stringFileName )
{
    // create
    QFile file( stringFileName );
    if ( !file.open( QIODevice::WriteOnly ) )
    {
        doMessageBox( "WARNING", LIB_NAME, "Could not open " + file.fileName() );
        return false;
    }

    // ini transaction
    g_Transaction->initSave( this );

    // We may need to switch some temp OID's to persist OID's and other things.
    // This will work its way down the hierarchy covering everything to be saved in this file.
    // !!! There is NO need to call doPreSave again ie for objects to be saved in this file. 
    doPreSave();

    // create an XML document with class name and document version...
    QDomDocument domDoc( metaObject()->className() );
    QDomElement domElementRoot = domDoc.createElement( metaObject()->className() );
    domElementRoot.setAttribute( "DocVer", SDK_DOC_VER );

    domDoc.appendChild( domElementRoot );

    // save ourself (incl. all descendants)
    QDomElement domElemThis = doSave( &domDoc, &domElementRoot );
    doSaveADDocument( &domDoc, &domElemThis );

    // write
    QString stringXML = domDoc.toString();
    file.write( stringXML.toLatin1(), stringXML.length() );

    // close
    file.close();

    // fini transaction
    g_Transaction->fini();

    setModified( false );

    return true;
}

void AWContainer::doClear()
{
    bLoaded         = false;

    // File name has to be done explicitly when parent is an AWContainer because we may just be unloading.
    QObject *pParent = parent();
    if ( !pParent || !pParent->inherits( "AWContainer" ) )
        stringFileName = QString();

    AWObject::doClear();
    ADDocument::doClear();
}

/*!
 * \brief Handle case where a descendant has changed. 
 *  
 * We do NOT change our modified status when an AWFile content is changed.
 * We, instead, change our modified status when; 
 *  
 * 1. a new child object is created 
 * 2. a child objected is deleted 
 * 3. an AWFile file name has changed 
 *  
 * We get the above events via other means. 
 *  
 * \author pharvey (12/3/19)
 * 
 * \param pSource 
 */
void AWContainer::eventModified( ADObject *pSource )
{
    Q_UNUSED( pSource );
}

// =========================================================

AWPrefContainerWidget::AWPrefContainerWidget( QWidget *pwidgetParent, const QString &stringPath )
    : QWidget( pwidgetParent )
{
    this->stringPath = stringPath;

    QFormLayout *pLayout  = new QFormLayout( this );

    gPreferences->beginGroup( stringPath );

    // CUSTOM
    pCustom = new QCheckBox( this );
    pCustom->setCheckState( (Qt::CheckState)(gPreferences->value( "Custom", "0" ).toInt()) );
    connect( pCustom, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotCustom(Qt::CheckState)) );
    pLayout->addRow( tr( "Custom" ), pCustom );

    // NAME
    plineeditName = new QLineEdit( this );
    plineeditName->setText( gPreferences->value( "Name", "" ).toString() );
    plineeditName->setToolTip( tr("a short, simple, and meaningful name to give this model") );
    connect( plineeditName, SIGNAL(textChanged(const QString &)), SLOT(slotName(const QString &)) );
    pLayout->addRow( tr( "Name" ), plineeditName );

    // CODE
    pCode = new QLineEdit( this );
    pCode->setText(  gPreferences->value( "Code", "" ).toString()  );
    pCode->setToolTip( tr("a system code") );
    connect( pCode, SIGNAL(textChanged(const QString &)), SLOT(slotCode(const QString &)) );
    pLayout->addRow( tr( "Code" ), pCode );

    // COMMENT
    ptexteditComment = new QTextEdit( this );
    ptexteditComment->setText(  gPreferences->value( "Comment", "" ).toString()  );
    connect( ptexteditComment, SIGNAL(textChanged()), SLOT(slotComment()) );
    pLayout->addRow( tr( "Comment" ), ptexteditComment );

    // AUTHOR
    plineeditAuthor = new QLineEdit( this );
    plineeditAuthor->setText(  gPreferences->value( "Author", "" ).toString()  );
    plineeditAuthor->setToolTip( tr("the author of this model") );
    connect( plineeditAuthor, SIGNAL(textChanged(const QString &)), SLOT(slotAuthor(const QString &)) );
    pLayout->addRow( tr( "Author" ), plineeditAuthor );

    // VERSION
    plineeditVersion = new QLineEdit( this );
    plineeditVersion->setText(  gPreferences->value( "Version", "" ).toString()  );
    plineeditVersion->setToolTip( tr("model revision identifier") );
    connect( plineeditVersion, SIGNAL(textChanged(const QString &)), SLOT(slotVersion(const QString &)) );
    pLayout->addRow( tr( "Version" ), plineeditVersion );

    gPreferences->endGroup();

    doCustom( pCustom->checkState() );

    pCustom->setFocus();
}

AWPrefContainerWidget::~AWPrefContainerWidget()
{
}

void AWPrefContainerWidget::slotCustom( Qt::CheckState nState )
{
    gPreferences->setValue( stringPath + "/Custom", int(nState) );
    doCustom( nState );
}                                                                          
                                                                           
void AWPrefContainerWidget::slotName( const QString &s )                   
{
    gPreferences->setValue( stringPath + "/Name", s.simplified() );
}                                                                          
                                                                           
void AWPrefContainerWidget::slotCode( const QString &s )                   
{                                                                          
    gPreferences->setValue( stringPath + "/Code", s.simplified() );
}                                                                          
                                                                           
void AWPrefContainerWidget::slotComment()                                  
{                                                                          
    gPreferences->setValue( stringPath + "/Comment", ptexteditComment->toPlainText().simplified() );
}                                                                          
                                                                           
void AWPrefContainerWidget::slotAuthor( const QString &s )                 
{                                                                          
    gPreferences->setValue( stringPath + "/Author", s.simplified() );
}                                                                          
                                                                           
void AWPrefContainerWidget::slotVersion( const QString &s )                
{                                                                          
    gPreferences->setValue( stringPath + "/Version", s.simplified() );
}                                                                          
                                                                           
void AWPrefContainerWidget::doCustom( Qt::CheckState nState )                
{
    if ( nState == Qt::Checked )
    {
        plineeditName->setEnabled( true );
        pCode->setEnabled( true );
        ptexteditComment->setEnabled( true );
        plineeditAuthor->setEnabled( true );
        plineeditVersion->setEnabled( true );
    }
    else
    {
        plineeditName->setEnabled( false );
        pCode->setEnabled( false );
        ptexteditComment->setEnabled( false );
        plineeditAuthor->setEnabled( false );
        plineeditVersion->setEnabled( false );
    }
}                                                                          
                                                                           

