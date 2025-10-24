/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "WSvgButton.h"

WSvgButton::WSvgButton( const QByteArray &ba, QWidget *pParent )
    : QWidget( pParent )
{
    ByteArray = ba;
    pRenderer = new QSvgRenderer( this );

    if ( !ba.isEmpty() && !pRenderer->load( ByteArray ) )
    { 
        QMessageBox::critical( this, tr("Load"), tr("Encountered an error while loading vector.") );
    }
}

WSvgButton::~WSvgButton()
{
    delete pRenderer;
}

void WSvgButton::setVector( const QByteArray &ba  )
{
    ByteArray = ba;
    if ( !pRenderer->load( ByteArray ) )
    { 
        QMessageBox::critical( this, tr("Load"), tr("Encountered an error while loading vector.") );
    }
    update();
}

void WSvgButton::paintEvent( QPaintEvent *pEvent )
{
    Q_UNUSED(pEvent);

    QPainter painter( this );

    if ( !ByteArray.isEmpty() ) pRenderer->render( &painter );
    else painter.drawText( QPoint( width() / 2, height() / 2), tr("Double Click\nTo Select Image") );
}

void WSvgButton::mouseDoubleClickEvent( QMouseEvent *pEvent )
{
    Q_UNUSED(pEvent);

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

    setVector( file.readAll() );

    emit signalChanged( ByteArray );
}

