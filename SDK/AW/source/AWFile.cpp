#include "LibInfo.h"
#include "AWFile.h"

#include "AWDocumentNameWidget.h"
#include "AWObjectUsesWidget.h"
#include "AWObjectUsedByWidget.h"
#include "AWObjectChildrenWidget.h"

AWFile::AWFile( ADObject *pParent,  const QString &stringName )
    : AWObject( pParent, stringName ), ADDocument( this )
{
}

AWFile::~AWFile()
{
}

QPixmap AWFile::getIcon()
{
    return QPixmap( ":AW/FileNew" );
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
AWDataWidget *AWFile::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = AWObject::getObjectWidget( pWidgetParent );

    p->removeByClass( "AWObjectNameWidget" );

    p->insertTab( 0, new AWDocumentNameWidget( this, p, getFileName() ), tr("Document") );

    p->setCurrentIndex( 0 );

    return p;
}

QString AWFile::getFileName()
{
    return stringFileName;
}

QString AWFile::getFileSpec( const QString &stringName )
{
    return ( stringName.isEmpty() ? metaObject()->className() : stringName ) + " (*" + getFileExtension() + ")";
}

QString AWFile::getFileExtension()
{
    return "." + QString( metaObject()->className() );
}

/*!
 * \brief Tries to load the given file.
 * 
 * - does NOT prompt to save any changes
 * - usefull when loading from a recent files list but caller probably wants to ensure any changes are saved first
 * 
 * \author pharvey (12/24/24)
 * 
 * \param s      
 */
bool AWFile::doOpen( const QString &stringFileName )
{
    doClear();                                                                                                                                                                                                                        
    return doRead( stringFileName );
}

/*!
 * \brief Open a file.
 * 
 * - attempts to close (save or cancel as needed) to get to a clear state
 * - prompts for a file name
 * - reads in the file
 * 
 * \author pharvey (12/18/24)
 * 
 * \return bool 
 */
bool AWFile::slotOpen()                                                                                                                                                                                                                   
{
    // get us to a clear state
    if ( slotClose() != AWObject::SaveSuccess ) return false;                                                                                                                                                                             

    // get file name                                                                                                                                                                                                                      
    QString stringFileName = QFileDialog::getOpenFileName( 0, QString(), QString(), getFileSpec() );                                                                                                                                      
    if ( stringFileName.isEmpty() )                                                                                                                                                                                                       
        return false;                                                                                                                                                                                                                     
                                                                                                                                                                                                                                          
    // read it in                                                                                                                                                                                                                               
    if ( !doRead( stringFileName ) ) return false;                                                                                                                                                                                        
    setFileName( stringFileName );                                                                                                                                                                                                    
    setModified( false );                                                                                                                                                                                                                 

    return true;
}
                                                                                                                                                                                                                                          
AWObject::SaveResults AWFile::slotSave( bool bRequest )                                                                                                                                                                                   
{                                                                                                                                                                                                                                         
    if ( isNew() ) return slotSaveAs( bRequest );                                                                                                                                                                         
                                                                                                                                                                                                                                          
    if ( !doWrite( getFileName() ) )                                                                                                                                                                                                      
    {                                                                                                                                                                                                                                     
        QMessageBox::StandardButton nButton;                                                                                                                                                                                              
        if ( bRequest )                                                                                                                                                                                                                   
            nButton = QMessageBox::question( getEditorWidgetParent(), getDialogCaption(), tr("Do you want to SaveAs?"),  QMessageBox::Ok | QMessageBox::Cancel | QMessageBox::Discard );                                                                            
        else                                                                                                                                                                                                                              
            nButton = QMessageBox::question( getEditorWidgetParent(), getDialogCaption(), tr("Do you want to SaveAs?"),  QMessageBox::Ok | QMessageBox::Discard );                                                                                                  
                                                                                                                                                                                                                                          
        if ( nButton == QMessageBox::Cancel ) return AWObject::SaveCancel;                                                                                                                                                                
        if ( nButton == QMessageBox::Ok )                                                                                                                                                                                                 
        {                                                                                                                                                                                                                                 
            AWObject::SaveResults n = slotSaveAs( bRequest );                                                                                                                                                                             
            if ( n == AWObject::SaveCancel && bRequest  ) return n;                                                                                                                                                                       
        }                                                                                                                                                                                                                                 
    }                                                                                                                                                                                                                                     
    else                                                                                                                                                                                                                                  
        setModified( false );                                                                                                                                                                                                             
                                                                                                                                                                                                                                          
                                                                                                                                                                                                                                          
    return AWObject::SaveSuccess;                                                                                                                                                                                                         
}                                                                                                                                                                                                                                         
                                                                                                                                                                                                                                          
AWObject::SaveResults AWFile::slotSaveAs( bool bRequest )                                                                                                                                                                                 
{                                                                                                                                                                                                                                         
    // get file name                                                                                                                                                                                                                      
    QString stringFileName;                                                                                                                                                                                                               
                                                                                                                                                                                                                                          
    if ( isNew() )                                                                                                                                                                                                        
    {                                                                                                                                                                                                                                     
        stringFileName = QFileDialog::getSaveFileName( 0, getDialogCaption() + tr( " Save As..." ), QString(), getFileSpec() );                                                                                                                                      
    }                                                                                                                                                                                                                                     
    else               
    {
        stringFileName = QFileDialog::getSaveFileName( 0, getDialogCaption() + tr( " Save As..." ), QFileInfo( getFileName() ).absoluteFilePath(), getFileSpec() );                                                                                                      
    }
                                                                                                                                                                                                                                          
    if ( stringFileName.isEmpty() ) return ( bRequest ? AWObject::SaveCancel : AWObject::SaveSuccess );                                                                                                                                   
                                                                                                                                                                                                                                          
    // append extension                                                                                                                                                                                                                   
    if ( stringFileName.right( getFileExtension().length() ) != getFileExtension() )                                                                                                                                                      
    {
        stringFileName += getFileExtension();                                                                                                                                                                                             
    }
                                                                                                                                                                                                                                          
    QFileInfo fileinfo( stringFileName );                                                                                                                                                                                                 
    stringFileName = fileinfo.absoluteFilePath();                                                                                                                                                                                         
                                                                                                                                                                                                                                          
    // write data                                                                                                                                                                                                                         
    if ( !doWrite( stringFileName ) ) return AWObject::SaveCancel;                                                                                                                                                                        
                                                                                                                                                                                                                                          
    setFileName( stringFileName );                                                                                                                                                                                                        
    setModified( false );                                                                                                                                                                                                                 
                                                                                                                                                                                                                                          
    return AWObject::SaveSuccess;                                                                                                                                                                                                         
}                                                                                                                                                                                                                                         
                                                                                                                                                                                                                                          
/*!                                                                                                                                                                                                                                       
 * \brief Provide an opportunity to save then Close.                                                                                                                                                                                      
 *                                                                                                                                                                                                                                        
 * A Close, in this case, means having no children but retaining the file name.                                                                                                                                                           
 *                                                                                                                                                                                                                                        
 * A User can Cancel if bRequest otherwise we are headed for a complete re-init.                                                                                                                                                          
 *                                                                                                                                                                                                                                        
 * This is not deleted here but baring any Cancel this will be re-init.                                                                                                                                                                   
 *                                                                                                                                                                                                                                        
 * \author pharvey (12/4/19)                                                                                                                                                                                                              
 *                                                                                                                                                                                                                                        
 * \param bRequest                                                                                                                                                                                                                        
 *                                                                                                                                                                                                                                        
 * \return AWFile::SaveResults                                                                                                                                                                                                            
 */                                                                                                                                                                                                                                       
AWObject::SaveResults AWFile::slotClose( bool bRequest )                                                                                                                                                                                  
{                                                                                                                                                                                                                                         
    // provide an opportunity to save any changes                                                                                                                                                                                         
    if ( isModified() )                                                                                                                                                                                                                  
    {                                                                                                                                                                                                                                     
        QMessageBox::StandardButton nButton;                                                                                                                                                                                              
        if ( bRequest )                                                                                                                                                                                                                   
            nButton = QMessageBox::question( getEditorWidgetParent(), getDialogCaption(), tr("Do you want to save changes?") + "\n\n" + getFileName(),  QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard );
        else                                                                                                                                                                                                                              
            nButton = QMessageBox::question( getEditorWidgetParent(), getDialogCaption(), tr("Do you want to save changes?") + "\n\n" + getFileName(), QMessageBox::Save | QMessageBox::Discard );                       
                                                                                                                                                                                                                                          
        if ( nButton == QMessageBox::Cancel ) return AWObject::SaveCancel;                                                                                                                                                                
        if ( nButton == QMessageBox::Save )                                                                                                                                                                                               
        {                                                                                                                                                                                                                                 
            AWObject::SaveResults n = slotSave( bRequest );                                                                                                                                                                               
            if ( n == AWObject::SaveCancel && bRequest  ) return n;                                                                                                                                                                       
        }                                                                                                                                                                                                                                 
        setModified( false );                                                                                                                                                                                                                 
    }                                                                                                                                                                                                                                     
                                                                                                                                                                                                                                          
    doClear();                                                                                                                                                                                                                        
                                                                                                                                                                                                                                          
    return SaveSuccess;                                                                                                                                                                                                                   
}                                                                                                                                                                                                                                         

void AWFile::setFileName( const QString &s )
{
    if (  s == stringFileName ) return;

    stringFileName = s;

    // tell those that care
    emit signalChangedFileName();
    emit signalChangedFileName( stringFileName );
}

    // QProgressBar progress( getEditorWidgetParent() );
    // QProgressDialog progress;
    // progress.setWindowModality( Qt::WindowModal );
    // progress.setCancelButton( nullptr );
    // progress.setLabelText( tr("Loading") );
    // progress.setMinimum( 1 );
    // progress.setMinimum( 2 );
    // progress.setValue( 1 );
    // QApplication::processEvents();


bool AWFile::doRead( const QString &stringFileName )
{
    // open a file...
    QFile file( stringFileName );
    if ( !file.open( QIODevice::ReadWrite ) )
    {
        if ( !file.open( QIODevice::ReadOnly ) )
        {
            doMessageBox( "ERROR", LIB_NAME, tr("Could not open file.\n\n") + file.fileName() );
            return false;
        }
        // set read only here
    }

    // read file into XML document...
    QDomDocument domDoc( metaObject()->className() );
    if ( !domDoc.setContent( &file ) )
    {
        doMessageBox( "ERROR", LIB_NAME, tr("Invalid file format.\nCould not create an XML document from file.\n\n") + file.fileName() );
        // doMessage( "ERROR", QString( "%1 Line: %2 Column: %3" ).arg( result.errorMessage ).arg( result.errorLine ).arg( result.errorColumn ) );
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
    {
        doLoad( &domElemThis );
        doLoadADDocument( &domElemThis );

        // Object hierarchy, parent/child relationships, have been created.
        // Now we can resolve references. This call will do the entire object hierarchy.
        doPostLoad();
    }
    // fini transaction
    g_Transaction->fini();

    setFileName( stringFileName );
    setModified( false );

    return true;
}

bool AWFile::doWrite( const QString &stringFileName )
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

    setFileName( stringFileName );
    setModified( false );

    return true;
}

void AWFile::doClear()
{
    setFileName( QString() );
    AWObject::doClear();
    ADDocument::doClear();
}


