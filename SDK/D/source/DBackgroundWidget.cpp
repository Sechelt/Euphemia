#include "LibInfo.h"
#include "DBackgroundWidget.h"

#include <WBrush.h>
#include <WPen.h>
#include <WBitmapWidget.h>
#include <WCanvasSizeWidget.h>

#include "DDiagram.h"

DBackgroundWidget::DBackgroundWidget( DDiagram *pDiagram, QWidget *pParent )
    : QWidget( pParent )
{
    QHBoxLayout *pLayout = new QHBoxLayout( this );

    // brush
    {
        QGroupBox *pGroupBox = new QGroupBox( tr("Brush"), this );
        pLayout->addWidget( pGroupBox );
        QHBoxLayout *pLayoutGroupBox = new QHBoxLayout( pGroupBox );
        WBrushPanel *pWidget = new WBrushPanel( pDiagram->getBrush(), pGroupBox );
        connect( pWidget, SIGNAL(signalChanged(const QBrush &)), pDiagram, SLOT(slotBrushBackground(const QBrush &)) );
        connect( pDiagram, SIGNAL(signalChangedBackgroundBrush(const QBrush &)), pWidget, SLOT(slotRefresh(const QBrush &)) );
        pLayoutGroupBox->addWidget( pWidget );
    }
    // bitmap
    {
        QGroupBox *pGroupBox = new QGroupBox( tr("Bitmap"), this );
        pLayout->addWidget( pGroupBox );
        QHBoxLayout *pLayoutGroupBox = new QHBoxLayout( pGroupBox );
        WBitmapWidget *pWidget = new WBitmapWidget( pDiagram->getBitmapBackgroundUse(), pDiagram->getBitmapBackground(), pDiagram->getBitmapBackgroundPlacement(), pGroupBox );
        connect( pWidget, SIGNAL(signalUse(bool)), pDiagram, SLOT(slotBitmapBackgroundUse(bool)) );
        connect( pWidget, SIGNAL(signalBitmap(const QPixmap &)), pDiagram, SLOT(slotBitmapBackground(const QPixmap &)) );
        connect( pWidget, SIGNAL(signalPlacement(W::WImagePlacements)), pDiagram, SLOT(slotBitmapBackgroundPlacement(W::WImagePlacements)) );
        connect( pDiagram, SIGNAL(signalBitmapBackgroundUse(bool)), pWidget, SLOT(slotUse(bool)) );
        connect( pDiagram, SIGNAL(signalBitmapBackground(const QPixmap &)), pWidget, SLOT(slotBitmap(const QPixmap &)) );
        connect( pDiagram, SIGNAL(signalBitmapBackgroundPlacement(W::WImagePlacements)), pWidget, SLOT(slotPlacement(W::WImagePlacements)) );
        pLayoutGroupBox->addWidget( pWidget );
    }
    // pen
    {
        QGroupBox *pGroupBox = new QGroupBox( tr("Pen"), this );
        pLayout->addWidget( pGroupBox );
        QHBoxLayout *pLayoutGroupBox = new QHBoxLayout( pGroupBox );
        WPenPanel *pWidget = new WPenPanel( pDiagram->getPen(), pGroupBox );
        connect( pWidget, SIGNAL(signalChanged(const QPen &)), pDiagram, SLOT(slotPenBackground(const QPen &)) );
        connect( pDiagram, SIGNAL(signalChangedBackgroundPen(const QPen &)), pWidget, SLOT(slotRefresh(const QPen &)) );
        pLayoutGroupBox->addWidget( pWidget );
    }
    // geometry
    {
        QGroupBox *pGroupBox = new QGroupBox( tr("Geometry"), this );
        pLayout->addWidget( pGroupBox );
        QHBoxLayout *pLayoutGroupBox = new QHBoxLayout( pGroupBox );
        WCanvasSizeWidget *pWidget = new WCanvasSizeWidget( pDiagram->getPage(), pDiagram->isPagePrinter(), pGroupBox );
        connect( pWidget, SIGNAL(signalSize(const QSizeF &)), pDiagram, SLOT(slotPage(const QSizeF &)) );
        connect( pWidget, SIGNAL(signalPrinter(bool)), pDiagram, SLOT(slotPagePrinter(bool)) );
        connect( pDiagram, SIGNAL(signalPage(const QSizeF &)), pWidget, SLOT(slotSize(const QSizeF &)) );
        connect( pDiagram, SIGNAL(signalPagePrinter(bool)), pWidget, SLOT(slotPrinter(bool)) );
        pLayoutGroupBox->addWidget( pWidget );
    }
}























