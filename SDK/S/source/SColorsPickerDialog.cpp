/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "SColorsPickerDialog.h"

#include "SColorsPickerWidget.h"

SColorsPickerDialog::SColorsPickerDialog( const QColor &color, QWidget *pParent, bool bShowPaletteColor )
    : QDialog( pParent )
{
    setWindowTitle( tr("Color Picker") );

    this->bShowPaletteColor = bShowPaletteColor;

    QVBoxLayout *pLayout = new QVBoxLayout( this );
    pColorPickerWidget = new SColorsPickerWidget( color, this, bShowPaletteColor );
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

SColorsPickerDialog::~SColorsPickerDialog()
{
    doSaveState();
}

QColor SColorsPickerDialog::getColor()
{
    return pColorPickerWidget->getColor();
}

QColor SColorsPickerDialog::getColor( bool *pOk, const QColor &color, QWidget *pParent, bool bShowPaletteColor )
{
    QColor colorOriginal = color;

    *pOk = true;

    SColorsPickerDialog dialog( color, pParent, bShowPaletteColor );
    if ( dialog.exec() == QDialog::Accepted )
        return dialog.getColor();

    *pOk = false;
    return colorOriginal;
}

void SColorsPickerDialog::doSaveState()
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

void SColorsPickerDialog::slotColorPicker()
{
    bColorPicker = true;
    grabMouse( QCursor( QPixmap( ":W/ColorPicker" ), 0, 0 ) );

    // note: grabMouse() is probably doing the following for us...
    // QGuiApplication::setOverrideCursor()
    // QGuiApplication::restoreOverrideCursor()
}

/*!
 * \brief Set colour to the pixel colour at cursor.
 * 
 * We want to get an image and use the pixel at pos in the image.
 * We can easily get an image of a QScreen but there can be more than one screen.
 * We could get an image for each screen and 'stitch' them together (creating a global screenshot) and use global coordinate but we use a simpler way. 
 * 
 * We get the relevant screen and translate coordinate from global to sceen.
 * 
 * \author pharvey (9/28/25)
 * 
 * \param pEvent 
 */
void SColorsPickerDialog::mousePressEvent( QMouseEvent *pEvent )
{
    if ( bColorPicker  )
    {
        // Use the global pos to determine which screen to use.
        QPoint      pointGlobal         = pEvent->globalPosition().toPoint();
        QScreen *   pScreen             = qApp->screenAt( pointGlobal );
        // Calculate the screen pos.
        QRect       rectScreenGeometry  = pScreen->geometry();
        QPoint      pointScreenPos      = pointGlobal - rectScreenGeometry.topLeft();
        // Grab the screen and get the pixel from it to determine pixel colour.
        pColorPickerWidget->setColor( pScreen->grabWindow().toImage().pixelColor( pointScreenPos ) );
        bColorPicker = false;
        releaseMouse();
        return;
    }

    QDialog::mousePressEvent( pEvent );
}

void SColorsPickerDialog::doLoadState()
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


