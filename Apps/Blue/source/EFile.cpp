/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "AppInfo.h"
#include "EFile.h"

#include <ADObject.h>
#include <LLayer.h>
#include <LEditorWidget.h>
#include <PAPaint.h>
#include <DRDraw.h>

#include "EExportDialog.h"

EFile::EFile( ADObject *pParent, const QString &stringName )
   : LFile( pParent, stringName )
{
    // here is what getObject() can create 
    mapMeta.clear();
    mapMeta.insert( "PAPaint", ADMeta( QIcon( ":PA/Paint" ), "PAPaint", "Paint" ) );
    mapMeta.insert( "DRDraw", ADMeta( QIcon( ":DR/Draw" ), "DRDraw", "Draw" ) );
}

EFile::~EFile()
{
}

QMenu *EFile::getContextMenu( QWidget *pParent )
{
    QMenu *     pMenu   = LFile::getContextMenu( pParent );
    QAction *   pAction;

    // add our stuff at the top of the menu = reverse order
    pAction = pMenu->insertSeparator( pMenu->actions().first() );
    pAction = new QAction( QIcon( ":PA/Paint" ), tr("New paint layer"), this );
    connect( pAction, SIGNAL(triggered()), SLOT(slotNewBitmap()) );
    pMenu->insertAction( pMenu->actions().first(), pAction );
    pAction = new QAction( QIcon( ":DR/Draw" ), tr("New draw layer"), this );
    connect( pAction, SIGNAL(triggered()), SLOT(slotNewVector()) );
    pMenu->insertAction( pMenu->actions().first(), pAction );

    pAction = pMenu->insertSeparator( pMenu->actions().first() );
    if ( isEditor() )
    {
        pAction = new QAction( QPixmap(), tr("Close Editor"), this );
        connect( pAction, SIGNAL(triggered()), SLOT(slotCloseEditor()) );
        pMenu->insertAction( pMenu->actions().first(), pAction );
    }
    else
    {
        pAction = new QAction( QPixmap(), tr("Open Editor"), this );
        connect( pAction, SIGNAL(triggered()), SLOT(slotOpenEditor()) );
        pMenu->insertAction( pMenu->actions().first(), pAction );
    }

    return pMenu; // caller invokes and then deletes
}

bool EFile::doImport()
{
    // just support paint layers for now
    QString stringSpec = "Image (*.png *.bmp *.gif *.jpg *.jpeg *.pbm *.pgm *.ppm *.xbm *.xpm)";

    QString stringFileName = QFileDialog::getOpenFileName( 0, QString(), QString(), stringSpec );                                                                                                                                      
    if ( stringFileName.isEmpty() ) return false;

    // load the file
    QImage image;
    QImageReader reader( stringFileName );
    if ( reader.read( &image ) ) 
    {
        image = image.convertToFormat( QImage::Format_ARGB32 );
        doLayerNew( "PAPaint" );    // should become current layer
        setImage( image );          // passed to current layer
        // setModified( false );
        return true;
    }

    // error
    doMessageBox( "ERROR", "Open...", "Failed to read " + stringFileName + "\n\n" + reader.errorString() );

    return false;
}

/*!
 * \brief Export the current layer or all layers in either Image or XML.
 * 
 * Does not update recent file list.
 * 
 * The LLayer::getImage() and LLayer::doSave() need to work in all layer classes (ie PAPaint and DRDraw).
 * 
 * \note In some cases Export will do the same as default for Save but let User do it anyway.
 * 
 * \author pharv (2/17/2025)
 * 
 * \return bool 
 */
bool EFile::doExport()
{
    if ( !getLayerCount() )
    {
        doMessageBox( "ERROR", APP_NAME, tr("There are no layers. Nothing to save. Save cancelled.\n\n") );
        return false;
    }

    EExportDialog dialog( nullptr );
    if ( dialog.exec() != QDialog::Accepted ) return false;

    // get file spec
    QString stringFileSpec;
    if ( dialog.isImage() ) stringFileSpec = getFileSpecImage();
    else stringFileSpec = getFileSpecXML();

    // get file name
    QString stringFileName = QFileDialog::getSaveFileName( 0, getDialogCaption() + tr( "Export..." ), QString(), stringFileSpec );
    if ( stringFileName.isEmpty() ) return false;

    QFileInfo fileinfo( stringFileName );                                                                                                                                                                                                 
    stringFileName = fileinfo.absoluteFilePath();                                                                                                                                                                                         

    QImage imageExport( pGraphics->sceneRect().size().toSize(), QImage::Format_ARGB32 );
    QString stringXML;
    QFile file( stringFileName );

    if ( dialog.isAll() )
    {
        if ( dialog.isImage() )
        {
            QPainter painter( &imageExport );
            int nLayers = getLayerCount();
            for ( int nLayer = 0; nLayer < nLayers; nLayer++ )
            {
                LLayer *pLayer = getLayer( nLayer );
                painter.drawImage( QPoint( 0, 0 ), pLayer->getImage() );
            }
        }
        else
        {
            if ( !file.open( QIODevice::WriteOnly ) )
            {
                doMessageBox( "WARNING", APP_NAME, "Could not open " + file.fileName() );
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
            domElementRoot.setAttribute( "DocVer", CBD_DOC_VER );

            domDoc.appendChild( domElementRoot );

            // save ourself (incl. all descendants)
            QDomElement domElemThis = doSave( &domDoc, &domElementRoot );
            doSaveADDocument( &domDoc, &domElemThis );
            stringXML = domDoc.toString();
        }
    }
    else
    {
        if ( dialog.isImage() )
        {
            LLayer *pLayer = getLayerCurrent();
            imageExport = pLayer->getImage(); // from current layer - its up to DRDraw to return image when asked
        }
        else
        {
            LLayer *pLayer = getLayerCurrent();
            if ( !file.open( QIODevice::WriteOnly ) )
            {
                doMessageBox( "WARNING", APP_NAME, "Could not open " + file.fileName() );
                return false;
            }

            // ini transaction
            g_Transaction->initSave( this );

            // We may need to switch some temp OID's to persist OID's and other things.
            // This will work its way down the hierarchy covering everything to be saved in this file.
            // !!! There is NO need to call doPreSave again ie for objects to be saved in this file. 
            doPreSave();

            // create an XML document with class name and document version...
            QDomDocument domDoc( pLayer->metaObject()->className() );
            QDomElement domElementRoot = domDoc.createElement( pLayer->metaObject()->className() );
            domElementRoot.setAttribute( "DocVer", CBD_DOC_VER );

            domDoc.appendChild( domElementRoot );

            // save layer (incl. all descendants)
            QDomElement domElemThis = pLayer->doSave( &domDoc, &domElementRoot );
            stringXML = domDoc.toString();
        }
    }

    if ( dialog.isImage() )
    {
        QImageWriter writer( stringFileName );
        if ( writer.write( imageExport ) ) return true;
        doMessageBox( "ERROR", "Save...", "Failed to save " + stringFileName + "\n\n" + writer.errorString() );
    }
    else
    {
        if ( file.write( stringXML.toLatin1(), stringXML.length() ) != stringXML.length() )
        {
            doMessageBox( "ERROR", "Save...", "Error during save " + stringFileName + "\n\n" + file.errorString() );
        }
        file.close();
        g_Transaction->fini();
    }

    return false;
}

bool EFile::doPrint()
{
    QMessageBox::information( 0, tr("Print..."), tr("Not implemented yet.") );
    return false;
}

bool EFile::slotOpenEditor()
{
    if ( pEditorWidget ) return true;

    if ( !pGraphics ) doGraphicsCreate( this );

    // create view
    pEditorWidget = new LEditorWidget( this, getEditorWidgetParent() );

    if ( bCrossHairs ) doCreateCrossHairs();

    emit signalEditorOpened( pEditorWidget );
    emit signalEditorOpened( this, pEditorWidget );

    return true;
}

void EFile::slotNewBitmap()
{
    doLayerNew( "PAPaint" );
}

void EFile::slotNewVector()
{
    doLayerNew( "DRDraw" );
}

bool EFile::doRead( const QString &stringFileName )
{
    // This could take awhile.
    // We could throw up a progress dialog but that would require another thread.
    // We could throw up a borderless modal box asking User to be patient.
    // We could change cursor to 'WaitCursor'. This is what is done here.  
#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
printf( "[PAH][%s][%s][%d] Set cursor to WaitCursor\n", __FILE__, __FUNCTION__, __LINE__ );
#endif

    QFileInfo fileInfo( stringFileName );
    bool b;

    if ( QString( fileInfo.suffix() ).toLower() == "blue" )
        b = doReadXML( stringFileName );
    else
        b = doReadImage( stringFileName );

#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif
    return b;
}

bool EFile::doWrite( const QString &stringFileName )
{
    QFileInfo fileInfo( stringFileName );
    bool b;

    if ( QString( fileInfo.suffix() ).toLower() == "blue" )
        b = doWriteXML( stringFileName );
    else 
        b = doWriteImage( stringFileName );

    return b;
}

bool EFile::doReadXML( const QString &stringFileName )
{
    // open a file...
    QFile file( stringFileName );
    if ( !file.open( QIODevice::ReadWrite ) )
    {
        if ( !file.open( QIODevice::ReadOnly ) )
        {
            doMessageBox( "ERROR", APP_NAME, tr("Could not open file.\n\n") + file.fileName() );
            return false;
        }
        // set read only here
    }

    // read file into XML document...
    QDomDocument domDoc( metaObject()->className() );
    if ( !domDoc.setContent( &file ) )
    {
        doMessageBox( "ERROR", APP_NAME, tr("Invalid file format.\nCould not create an XML document from file.\n\n") + file.fileName() );
//        doMessage( "ERROR", QString( "%1 Line: %2 Column: %3" ).arg( result.errorMessage ).arg( result.errorLine ).arg( result.errorColumn ) );
        file.close();
        return false;
    }
    file.close();

    // validate XML...
    // Root element is only used to check that class in XML matches this class.
    QDomElement domElemRoot = domDoc.documentElement();
    if ( domElemRoot.isNull() )
    {
        doMessageBox( "ERROR", APP_NAME, tr("Invalid file format.\nCould not find a document element.\n\n") + stringFileName );
        return false;
    }
    if ( domElemRoot.tagName() != metaObject()->className() )
    {
        doMessageBox( "ERROR", APP_NAME, tr("Invalid file format.\nFile does not appear to be a ") + QString( metaObject()->className() ) + "\n\n" + stringFileName );
        return false;
    }
    if ( domElemRoot.attribute( "DocVer" ) != CBD_DOC_VER ) 
    {
        doMessageBox( "ERROR", APP_NAME, tr("Invalid file format.\nFile is not Version ") + CBD_DOC_VER );
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

bool EFile::doReadImage( const QString &stringFileName )
{
    QImage image;

    // load the file
    QImageReader reader( stringFileName );
    if ( reader.read( &image ) ) 
    {
        image = image.convertToFormat( QImage::Format_ARGB32 );

        doLayerNew( "PAPaint" );    // should become current layer
        setImage( image );          // passed to current layer

        setFileName( stringFileName );
        setModified( false );

        return true;
    }

    // error
    doMessageBox( "ERROR", "Open...", "Failed to read " + stringFileName + "\n\n" + reader.errorString() );

    return false;
}

bool EFile::doWriteXML( const QString &stringFileName )
{
    // create
    QFile file( stringFileName );
    if ( !file.open( QIODevice::WriteOnly ) )
    {
        doMessageBox( "WARNING", APP_NAME, "Could not open " + file.fileName() );
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
    domElementRoot.setAttribute( "DocVer", CBD_DOC_VER );

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

bool EFile::doWriteImage( const QString &stringFileName )
{
    QFileInfo fileInfo( stringFileName );
    if ( getLayerCount() > 1 )
    {
        doMessageBox( "ERROR", tr("Save..."), tr("Failed to save as an image because there are multiple layers.\nFlatten/merge layers to save in a standard image file or save in the blue (xml) file format.") );
        return false;
    }

    LLayer *pLayer = getLayerCurrent();
    QImage image = pLayer->getImage(); // from current layer

    // do it
    QImageWriter writer( stringFileName );
    if ( writer.write( image ) ) 
    {
        setFileName( stringFileName );
        setModified( false );
        return true;
    }

    // error
    doMessageBox( "ERROR", "Save...", "Failed to save " + stringFileName + "\n\n" + writer.errorString() );

    return false;
}

// create layer
// NOTE: Consider using; doLayerNew( stringClass );
ADObject *EFile::getObject( const QString &s, ADObject * )
{
    LLayer *                pObject         = nullptr;
    QString                 stringClass     = getClass( s );

    if ( stringClass == "PAPaint" )
    {
        pObject = new PAPaint( this );
    }
    else if ( stringClass == "DRDraw" )
    {
        pObject = new DRDraw( this );
    }

    if ( !pObject )
    {
        QMessageBox::warning( nullptr, tr("Create Object..."), tr("Unknown class requested ") + s );
        return nullptr;
    }

    // ensure we are the OID source regardless of parent (avoids messing up OID's when reparenting object's)
    pObject->setOIDSource( this );
    // announce the birth of a new child
    emit signalCreated( pObject );
    // set parent modified by default (can be set back if this is a load)               
    setModified();              

    // the caller should doConnect to a model ASAP
    return pObject;
}

AWObject::SaveResults EFile::slotSaveAs( bool bRequest )                                                                                                                                                                                 
{
    if ( !getLayerCount() )
    {
        doMessageBox( "ERROR", APP_NAME, tr("There are no layers. Nothing to save. Save cancelled.\n\n") );
        return AWObject::SaveCancel;
    }

    // get file name
    QString stringFileName;                                                                                                                                                                                                               
                                                                                                                                                                                                                                          
    if ( isNew() )                                                                                                                                                                                                        
    {                                                                                                                                                                                                                                     
        stringFileName = QFileDialog::getSaveFileName( 0, getDialogCaption() + tr( " Save As..." ), QString(), getFileSpec() );                                                                                                                                      
    }                                                                                                                                                                                                                                     
    else               
    {
        // setDefaultSuffix( "png" )
        stringFileName = QFileDialog::getSaveFileName( 0, getDialogCaption() + tr( " Save As..." ), QFileInfo( getFileName() ).absoluteFilePath(), getFileSpec() );                                                                                                      
    }
                                                                                                                                                                                                                                          
    if ( stringFileName.isEmpty() ) return ( bRequest ? AWObject::SaveCancel : AWObject::SaveSuccess );                                                                                                                                   
                                                                                                                                                                                                                                          
    QFileInfo fileinfo( stringFileName );                                                                                                                                                                                                 
    stringFileName = fileinfo.absoluteFilePath();                                                                                                                                                                                         
                                                                                                                                                                                                                                          
    // write data                                                                                                                                                                                                                         
    if ( !doWrite( stringFileName ) ) return AWObject::SaveCancel;                                                                                                                                                                        
                                                                                                                                                                                                                                          
    setFileName( stringFileName );                                                                                                                                                                                                        
    setModified( false );                                                                                                                                                                                                                 

    return AWObject::SaveSuccess;                                                                                                                                                                                                         
}                                                                                                                                                                                                                                         
                                                                                                                                                                                                                                          
QString EFile::getFileSpec( const QString &stringName )
{
    Q_UNUSED( stringName );

    if ( pLayer )
    {
        // more than one layer or layer is other than a paint/raster then we must save in xml
        if ( getLayerCount() > 1 || !pLayer->inherits( "PAPaint" ) ) return getFileSpecXML();
    }

    // a single layer and its paint/raster so save in image format or, optionally, xml
    return getFileSpecImage() + ";;" + getFileSpecXML();
}

QString EFile::getFileSpecImage()
{
    return "Image (*.png *.bmp *.gif *.jpg *.jpeg *.pbm *.pgm *.ppm *.xbm *.xpm)";
}

QString EFile::getFileSpecXML()
{
    return "Blue (*.blue)";
}

QString EFile::getFileExtension()
{
    return ".blue"; // not used because qt will put one on automagically \sa getFileSpec
}



