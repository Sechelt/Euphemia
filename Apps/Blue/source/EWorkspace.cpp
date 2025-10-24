#include "AppInfo.h"
#include "EWorkspace.h"

#include <SSettings.h>

#include "EFile.h"

EWorkspace::EWorkspace( const QString &stringName )
    : AWObject( nullptr, stringName )
{
    // The only child object we support is an \sa EFile. 
    mapMeta.clear();
    mapMeta.insert( "EFile", ADMeta( QIcon(), "EFile", "File" ) );
}

EWorkspace::~EWorkspace()
{
}

/*!
 * \brief Create a child object. 
 *  
 * The only child object we support is an \sa EFile. 
 *  
 * A graphics scene is created for the EFile. No view is created.
 *  
 * \author pharvey (4/18/23)
 * 
 * \param s      
 * 
 * \return ADObject* 
 */
ADObject *EWorkspace::getObject( const QString &s, ADObject * )
{
    ADObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "EFile" )
    {
        EFile *pFile;
        p = pFile = new EFile( this );
        // we must always have a scene to calc things even when not displaying   
        // and this must be done right after EFile created and not within it   
        // pFile->doGraphicsCreate( pFile );
    }

    if ( !p )
    {
        doMessageBox( "WARNING", "Create Object...", "Unknown class requested " + s );
        return nullptr;
    }

    // announce the birth of a new child
    emit signalCreated( p );    
    emit signalCreated( this, p );

    // we are never modified because the workspace is never persisted in this app
    // setModified();              

    return p;
}

QMenu *EWorkspace::getContextMenu( QWidget *pParent ) 
{
    // we do not want to give access to the workspace object beyond the following as it is just a place holder
    QMenu *pMenu = new QMenu( pParent );
    pMenu->addAction( QPixmap( ":W/new" ), tr("New file"), this, SLOT(slotNewFile()) );

    return pMenu;
}

/*!
 * \brief Close children. 
 *  
 * Use this in app closeEvent. 
 *  
 * Will assert if a child is NOT AWFile based.
 * 
 * \author pharvey (4/18/23)
 * 
 * \param bRequest True if we want the option to cancel.
 * 
 * \return AWObject::SaveResults 
 */
AWObject::SaveResults EWorkspace::doCloseChildren( bool /* bRequest */ )
{
    // close all file based children                                                                                                                    
    QList<ADObject*> listChildren = getObjects();                                                                                                       
    ADObject *p;                                                                                                                                        
    foreach( p, listChildren )                                                                                                                          
    {                                                                                                                                                   
        Q_ASSERT( p->inherits( "EFile" ) );                                                                                                             
        EFile *pFile = (EFile*)p;      
        AWFile::SaveResults nSaved = pFile->slotClose();

        switch ( nSaved )
        {
            case AWFile::SaveSuccess:
            case AWFile::SaveLoseChanges:
                break;
            case AWFile::SaveCancel:
            case AWFile::SaveError:
                return nSaved;
        }
        if  ( pFile->isEditor() ) pFile->slotCloseEditor();
        slotDelete( pFile );
    }                                                                                                                                                   

    return AWObject::SaveSuccess;
}

void EWorkspace::slotNewFile()
{
    getObject( "EFile" );
}


