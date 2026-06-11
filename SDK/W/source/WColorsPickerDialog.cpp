/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "WColorsPickerDialog.h"

#include "WColorsPickerWidget.h"

WColorsPickerDialog::WColorsPickerDialog( const QColor &color, QWidget *pParent, bool bShowPaletteColor )
    : QDialog( pParent )
{
    setWindowTitle( tr("Color Picker") );

    this->bShowPaletteColor = bShowPaletteColor;

    QVBoxLayout *pLayout = new QVBoxLayout( this );
    pColorPickerWidget = new WColorsPickerWidget( color, this, bShowPaletteColor );
    pLayout->addWidget( pColorPickerWidget );

    // buttons
    QHBoxLayout *pLayoutButtons = new QHBoxLayout();

    QToolButton *pColorPicker = new QToolButton( this );
    pColorPicker->setIcon( QIcon( ":W/ColorPicker" ) );
    pColorPicker->setToolTip( tr("pick a color from the screen") );
    pLayoutButtons->addWidget( pColorPicker );

    pLayoutButtons->addStretch( 15 );

    QDialogButtonBox *pButtonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this );
    pLayoutButtons->addWidget( pButtonBox ); 

    pLayout->addLayout( pLayoutButtons );

    connect( pColorPicker, SIGNAL(released()), this, SLOT(slotColorPicker()) );
    connect( pButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );
    connect( pButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject );
    doLoadState();
}

WColorsPickerDialog::~WColorsPickerDialog()
{
    doSaveState();
}

QColor WColorsPickerDialog::getColor()
{
    return pColorPickerWidget->getColor();
}

QColor WColorsPickerDialog::getColor( bool *pOk, const QColor &color, QWidget *pParent, bool bShowPaletteColor )
{
    QColor colorOriginal = color;

    *pOk = true;

    WColorsPickerDialog dialog( color, pParent, bShowPaletteColor );
    if ( dialog.exec() == QDialog::Accepted )
        return dialog.getColor();

    *pOk = false;
    return colorOriginal;
}

void WColorsPickerDialog::doSaveState()
{
    if ( bShowPaletteColor ) 
    {
        QString s = metaObject()->className();
        if ( !objectName().isEmpty() ) s = s + "/" + objectName();

        QSettings settings;
        settings.setValue( s + "/width1", width() );
        settings.setValue( s + "/height1", height() );
    }
    else
    {
        QString s = metaObject()->className();
        if ( !objectName().isEmpty() ) s = s + "/" + objectName();

        QSettings settings;
        settings.setValue( s + "/width2", width() );
        settings.setValue( s + "/height2", height() );
    }
}

void WColorsPickerDialog::slotColorPicker()
{
    bColorPicker = true;
    grabMouse( QCursor( QPixmap( ":W/ColorPicker" ), 0, 0 ) );

    // note: grabMouse() is probably doing the following for us...
    // QGuiApplication::setOverrideCursor()
    // QGuiApplication::restoreOverrideCursor()
}

void WColorsPickerDialog::mousePressEvent( QMouseEvent *pEvent )
{
    if ( bColorPicker  )
    {
        QWidget *p = window();
        Q_ASSERT( p );
#if QT_VERSION < 0x060000
        pColorPickerWidget->setColor( p->grab().toImage().pixelColor( pEvent->screenPos().toPoint() ) );
#else
        pColorPickerWidget->setColor( p->grab().toImage().pixelColor( pEvent->scenePosition().toPoint() ) );
#endif
        bColorPicker = false;
        releaseMouse();
        return;
    }

    QDialog::mousePressEvent( pEvent );
}

void WColorsPickerDialog::doLoadState()
{
    if ( bShowPaletteColor ) 
    {
        QString s = metaObject()->className();
        if ( !objectName().isEmpty() ) s = s + "/" + objectName();

        QSettings settings;
        int nWidth = settings.value( s + "/width1", width() ).toInt();
        int nHeight= settings.value( s + "/height1", height() ).toInt();
        resize( nWidth, nHeight );
    }
    else
    {
        QString s = metaObject()->className();
        if ( !objectName().isEmpty() ) s = s + "/" + objectName();

        QSettings settings;
        int nWidth = settings.value( s + "/width2", width() ).toInt();
        int nHeight= settings.value( s + "/height2", height() ).toInt();
        resize( nWidth, nHeight );
    }
}


