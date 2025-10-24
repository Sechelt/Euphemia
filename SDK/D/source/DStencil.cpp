#include "LibInfo.h"
#include "DStencil.h"

#include "DStencilItem.h"
#include "DDiagram.h"
#include "DGraphicsSceneProxy.h"

DStencil::DStencil( AWObject *pParent, const QString &stringName )
    : AWFolder( pParent, stringName )
{
    mapMeta.insert( "DStencilItemSvg", ADMeta( QIcon(), "DStencilItemSvg", "Svg" ) );
    mapMeta.insert( "DStencilItemBitmap", ADMeta( QIcon(), "DStencilItemBitmap", "Bitmap" ) );
}

DStencil::~DStencil()
{
}

QPixmap DStencil::getIcon( AWObject::enumIconRoles )
{
    return getIcon();
}

QPixmap DStencil::getIcon()
{   
    return QPixmap( ":D/Stencil" );
}

QMenu *DStencil::getContextMenu( QWidget *pParent )
{
    QMenu *pMenu = new QMenu( pParent );

    pMenu->addAction( tr( "Add Svg..." ), this, SLOT( slotAddSvg() ) );
    pMenu->addAction( tr( "Add Bitmap..." ), this, SLOT( slotAddBitmap() ) );
    pMenu->addSeparator();
    pMenu->addAction( tr( "Export..." ), this, SLOT( slotExport() ) );
    pMenu->addAction( QPixmap( ":AW/Properties" ), tr("Object..."), this, SLOT(slotObjectDialog()) );
    pMenu->addSeparator();
    pMenu->addAction( QPixmap( ":AW/Delete" ), tr("Delete ") + metaObject()->className(), this, SLOT(slotDelete()) );

    return pMenu; // caller invokes and then deletes
}

ADObject *DStencil::getObject( const QString &s, ADObject * )
{
    ADObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DStencilItemSvg" )
    {
        p = new DStencilItemSvg( this );
    }
    else if ( stringClass == "DStencilItemBitmap" )
    {
        p = new DStencilItemBitmap( this );
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

void DStencil::slotAddSvg()
{
    QFileDialog fileDialog( nullptr );
    fileDialog.setAcceptMode( QFileDialog::AcceptOpen );
    fileDialog.setMimeTypeFilters( QStringList() << "image/svg+xml" << "image/svg+xml-compressed" );
    fileDialog.setWindowTitle(tr("Select Vector Image..."));
    // if (m_currentPath.isEmpty())
    //  fileDialog.setDirectory(picturesLocation());

    if ( fileDialog.exec() != QDialog::Accepted ) return;

    QString stringFileName = fileDialog.selectedFiles().constFirst();
    QFile file( stringFileName );
    if ( !file.open( QIODevice::ReadOnly ) )
    {
        return;
    }
    QFileInfo FileInfo( file );

    DStencilItemSvg *p = (DStencilItemSvg*)getObject( "DStencilItemSvg" );
    p->setName( FileInfo.baseName() );
    p->setVector( file.readAll() );
}

void DStencil::slotAddBitmap()
{
    QStringList mimeTypeFilters;
    {
        const QByteArrayList supportedMimeTypes = QImageReader::supportedMimeTypes();
        foreach( const QByteArray& mimeTypeName, supportedMimeTypes ) 
        {
            mimeTypeFilters.append(mimeTypeName);
        }
        mimeTypeFilters.sort();
    }


    QFileDialog fileDialog( nullptr );
    fileDialog.setAcceptMode( QFileDialog::AcceptOpen );
    fileDialog.setMimeTypeFilters( mimeTypeFilters );
    fileDialog.setWindowTitle(tr("Select Bitmap Image..."));

    if ( fileDialog.exec() != QDialog::Accepted ) return;

    QString stringFileName = fileDialog.selectedFiles().constFirst();
    QFile file( stringFileName );
    if ( !file.open( QIODevice::ReadOnly ) )
    {
        return;
    }
    QFileInfo FileInfo( file );

    DStencilItemBitmap *p = (DStencilItemBitmap*)getObject( "DStencilItemBitmap" );
    p->setName( FileInfo.baseName() );
    QPixmap pixmap;
    if ( pixmap.loadFromData( file.readAll() ) )
        p->setPixmap( pixmap );
}

void DStencil::slotExport()
{
    QString stringFileName = QFileDialog::getSaveFileName( 0, tr("Export As..."), getName() + ".DStencil", tr("DStencil (*.DStencil)") );
    if ( stringFileName.isEmpty() ) return;

    // create an XML document with class name and document version...
    QDomDocument domDoc( metaObject()->className() );
    QDomElement domElementRoot = domDoc.createElement( metaObject()->className() );
    domElementRoot.setAttribute( "DocVer", SDK_DOC_VER );
    domDoc.appendChild( domElementRoot );

    // save self to document...
    QDomElement domElemThis = doSave( &domDoc, &domElementRoot );

    // save to file...
    QSaveFile file( stringFileName );
    file.open( QIODevice::WriteOnly );
    file.write( domDoc.toString().toLatin1() );
    file.commit();
}

