#include "AppInfo.h"
#include "EPreferencesDialog.h"

#include <PDrawFreeHand.h>
#include <PDrawErase.h>
#include <PDrawSpray.h>
#include <PDrawLine.h>
#include <PDrawPolygon.h>
#include <PDrawPolygonFilled.h>
#include <PDrawPolyline.h>
#include <PDrawRectangle.h>
#include <PDrawRectangleFilled.h>
#include <PDrawEllipse.h>
#include <PDrawEllipseFilled.h>
#include <PDrawText.h>
#include <PFillFlood.h>
#include <PPasteRectangle.h>

EPreferencesDialog::EPreferencesDialog( QWidget *pParent )
    : QDialog( pParent )
{
    setWindowTitle( tr("Preferences") );

    QVBoxLayout *pLayout = new QVBoxLayout( this );
    QWidget *p = new QWidget( this );

    {
        QVBoxLayout *pLayoutTop = new QVBoxLayout( p );

        // add tool preferences
        {
            QFormLayout *pLayout = new QFormLayout();

            pLayout->addRow( tr("General"), new PGeneralConfig( p ) );
            pLayout->addRow( tr("Free Hand"), new PFreeHandToolBar( p ) );
            pLayout->addRow( tr("Spray"), new PSprayToolBar( p ) );
            pLayout->addRow( tr("Text"), new PTextToolBar( p ) );
            pLayout->addRow( tr("Flood Fill"), new PFillFloodToolBar( p ) );
            pLayout->addRow( tr("Paste"), new PPasteToolBar( p ) );

            pLayoutTop->addLayout( pLayout );
        }
        pLayoutTop->addStretch( 10 );
    }

    pLayout->addWidget( p ); 

    QDialogButtonBox *pButtonBox = new QDialogButtonBox( QDialogButtonBox::Ok, this );
    pLayout->addWidget( pButtonBox ); 

    connect( pButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );
}

void EPreferencesDialog::doPreferences( QWidget *pParent )
{
    EPreferencesDialog dialog( pParent );
    dialog.exec();
    g_Context->doSave();
}


//
//
//
PGeneralConfig::PGeneralConfig( QWidget *pParent )
    : QWidget( pParent )
{
    setObjectName( "PGeneralConfig" );

    QHBoxLayout *pLayout = new QHBoxLayout( this );

    PContextGeneral general = g_Context->getGeneral();

    pColor = new WColorButton( general.brushTransparency.color(), this, WColorButton::Fill );
    pLayout->addWidget( pColor );
    connect( pColor, SIGNAL(signalChanged(const QColor &)), SLOT(slotColor(const QColor &)) );

    pStyle = new WBrushStyleComboBox( this, general.brushTransparency.style() );
    pStyle->setToolTip( tr("brush style") );
    pLayout->addWidget( pStyle );
    connect( pStyle, SIGNAL(signalChanged(Qt::BrushStyle)), SLOT(slotStyle(Qt::BrushStyle)) );

    pImage = new WImageButton( general.brushTransparency.textureImage(), this );
    pLayout->addWidget( pImage, 10 );
    connect( pImage, SIGNAL(signalClick()), SLOT(slotImage()) );

    pRestoreState = new QCheckBox( tr("Restore Window State"), this );
    pRestoreState->setChecked( general.bRestoreState );
    pLayout->addWidget( pRestoreState );
    connect( pRestoreState, SIGNAL(stateChanged(int)), SLOT(slotRestoreState(int)) );

    pLayout->addStretch( 10 );

    connect( g_Context, SIGNAL(signalModified(const PContextGeneral &)), SLOT(slotRefresh(const PContextGeneral &)) );
}

void PGeneralConfig::slotRefresh( const PContextGeneral &t )
{
    pColor->setValue( t.brushTransparency.color() );
    pStyle->setValue( t.brushTransparency.style() );
    pImage->setImage( t.brushTransparency.textureImage() );
    pRestoreState->setChecked( t.bRestoreState );
}

void PGeneralConfig::slotColor( const QColor &color )
{
    PContextGeneral t = g_Context->getGeneral();
    t.brushTransparency.setColor( color );
    g_Context->setGeneral( t );
}

void PGeneralConfig::slotStyle( Qt::BrushStyle n )
{
    PContextGeneral t = g_Context->getGeneral();
    t.brushTransparency.setStyle( n );
    g_Context->setGeneral( t );
}

void PGeneralConfig::slotImage()
{
    QImage image;

    QString stringFileName = QFileDialog::getOpenFileName( this, tr("Select image..."), QString(), "Image files (*.png *.xpm *.jpg)" );
    if ( !stringFileName.isEmpty() )
    {
        image.load( stringFileName );
    }

    // accept null or loaded image
    PContextGeneral t = g_Context->getGeneral();
    t.brushTransparency.setTextureImage( image );
    g_Context->setGeneral( t );
}

void PGeneralConfig::slotRestoreState( int n )
{
    PContextGeneral t = g_Context->getGeneral();
    t.bRestoreState = n;
    g_Context->setGeneral( t );
}


