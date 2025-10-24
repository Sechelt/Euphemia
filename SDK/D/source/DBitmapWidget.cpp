#include "LibInfo.h"
#include "DBitmapWidget.h"

#include <WBitmapButton.h>

DBitmapWidget::DBitmapWidget( DBitmap *pBitmap, QWidget *pParent )
    : QWidget( pParent )
{
    this->pBitmap = pBitmap;

    QVBoxLayout *pLayout = new QVBoxLayout( this );

    {
        QHBoxLayout *pLayout01 = new QHBoxLayout();
        pPlacement = new QComboBox( this );
        pPlacement->addItem( "Normal", (int)W::WImagePlacementNormal );
        pPlacement->addItem( "Tiled", (int)W::WImagePlacementTiled );
        pPlacement->addItem( "Scaled", (int)W::WImagePlacementScaled );
        WSetComboBox( pPlacement, pBitmap->getPlacement() );
        connect( pPlacement, SIGNAL(currentIndexChanged(int)), pBitmap, SLOT(slotPlacement(int)) );
        pLayout01->addWidget( pPlacement );
        pLayout01->addStretch( 10 );
        pLayout->addLayout( pLayout01 );
    }
    {
        pBitmapButton = new WBitmapButton( pBitmap->getPixmap(), this );
        connect( pBitmapButton, SIGNAL(signalChanged(const QPixmap &)), pBitmap, SLOT(slotPixmap(const QPixmap &)) );
        pLayout->addWidget( pBitmapButton );
    }
    connect( pBitmap, SIGNAL(signalModified()), SLOT(slotModified()) );
}

void DBitmapWidget::slotModified()
{
    WSetComboBox( pPlacement, pBitmap->getPlacement() );
    pBitmapButton->setPixmap( pBitmap->getPixmap() );
}























