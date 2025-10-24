#include "LibInfo.h"
#include "DStencils.h"

#include "DStencil.h"

DStencils::DStencils( AWObject *pParent, const QString &stringName )
    : AWFolder( pParent, stringName )
{
    mapMeta.insert( "DStencil", ADMeta( QIcon(), "DStencil", "Stencil" ) );

    setName( tr("Stencils") );
}

DStencils::~DStencils()
{
}

QPixmap DStencils::getIcon( AWObject::enumIconRoles )
{
    return getIcon();
}

QPixmap DStencils::getIcon()
{   
    return QPixmap( ":D/Stencils" );
}

QMenu *DStencils::getContextMenu( QWidget *pParent )
{
    QMenu *pMenu = new QMenu( pParent );

    pMenu->addAction( QPixmap( ":D/Stencils" ), tr( "New Stencil..." ), this, SLOT( slotNew() ) );
    pMenu->addAction( tr( "Import Stencil..." ), this, SLOT( slotImport() ) );
    pMenu->addAction( QPixmap( ":AW/Properties" ), tr("Object..."), this, SLOT(slotObjectDialog()) );
    pMenu->addSeparator();
    pMenu->addAction( QPixmap( ":AW/Delete" ), tr("Delete ") + metaObject()->className(), this, SLOT(slotDelete()) );

    return pMenu; // caller invokes and then deletes
}

ADObject *DStencils::getObject( const QString &s, ADObject * )
{
    ADObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DStencil" )
    {
        p = new DStencil( this );
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

void DStencils::slotNew()
{
    DStencil *p = (DStencil*)getObject( "DStencil" );
    p->slotObjectDialog();
}

void DStencils::slotImport()
{
    QString stringFileName = QFileDialog::getOpenFileName( getEditorWidgetParent(), tr( "Import Stencil..." ), QString(), tr("Stencil (*.DStencil)") );
    if ( stringFileName.isEmpty() ) return;

    // open a file...
    QFile file( stringFileName );
    if ( !file.open( QIODevice::ReadWrite ) )
    {
        if ( !file.open( QIODevice::ReadOnly ) )
        {
            doMessageBox( "ERROR", LIB_NAME, tr("Could not open file.\n\n") + file.fileName() );
            return;
        }
        // set read only here
    }

    // read file into XML document...
    QDomDocument domDoc( metaObject()->className() );
    if ( !domDoc.setContent( &file ) )
    {
        doMessageBox( "ERROR", LIB_NAME, tr("Invalid file format.\nCould not create an XML document from file.\n\n") + file.fileName() );
//        doMessage( "ERROR", QString( "%1 Line: %2 Column: %3" ).arg( result.errorMessage ).arg( result.errorLine ).arg( result.errorColumn ) );
        file.close();
        return;
    }
    file.close();

    // validate XML...
    // Root element is only used to check that class in XML matches this class.
    QDomElement domElemRoot = domDoc.documentElement();
    if ( domElemRoot.isNull() )
    {
        doMessageBox( "ERROR", LIB_NAME, tr("Invalid file format.\nCould not find a document element.\n\n") + stringFileName );
        return;
    }
    if ( domElemRoot.tagName() != "DStencil" )
    {
        doMessageBox( "ERROR", LIB_NAME, tr("Invalid file format.\nFile does not appear to be a ") + "DStencil\n\n" + stringFileName );
        return;
    }
    if ( domElemRoot.attribute( "DocVer" ) != SDK_DOC_VER ) 
    {
        doMessageBox( "ERROR", LIB_NAME, tr("Invalid file format.\nFile is not Version ") + SDK_DOC_VER );
        return;
    }

    // Its our data so lets start processing using first child (only child)...
    QDomNode domNode = domElemRoot.firstChild();
    if ( domNode.isNull() ) return;
    if ( domNode.nodeType() != QDomNode::ElementNode ) return;

    QDomElement domElemThis = domNode.toElement();

    DStencil *p = (DStencil*)getObject( "DStencil" );
    p->doLoad( &domElemThis );

    // we added children but stencil is not really modified
    // will also get qt to call getIcon() on each child to get loaded icon
    p->setModified( false );
}

