#include "LibInfo.h"
#include "LMLineWidget.h"

#include <W.h>
#include <WText.h>
#include <WBrush.h>
#include <WPen.h>

//
//
//
LMLineTextWidget::LMLineTextWidget( LMLine *p, QWidget *pParent )
    : QWidget( pParent )
{
    pLine = p;

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );
    QFormLayout *pLayout    = new QFormLayout();

    pDisplay = new QComboBox( this );
    pDisplay->addItem( "Name", (int)LMLine::TextDisplayName );
    pDisplay->addItem( "Code", (int)LMLine::TextDisplayCode );
    pDisplay->addItem( "Comment", (int)LMLine::TextDisplayComment );
    pDisplay->addItem( "None", (int)LMLine::TextDisplayNone );
    WSetComboBox( pDisplay, pLine->getTextDisplay() );
    pLayout->addRow( tr( "Label:" ), pDisplay );
    connect( pDisplay, SIGNAL(currentIndexChanged(int)), pLine, SLOT(slotTextDisplay(int)) );

    pLayoutTop->addLayout( pLayout );
    pLayoutTop->addStretch( 10 );

    connect( pLine, SIGNAL(signalModified()), SLOT(slotModified()) );
}

void LMLineTextWidget::slotModified()
{
    WSetComboBox( pDisplay, pLine->getTextDisplay() );
}

//
//
//
LMLineWidget::LMLineWidget( LMLine *pLine, QWidget *pParent )
    : QWidget( pParent )
{
    QHBoxLayout *pLayout = new QHBoxLayout( this );
    QGroupBox *pGroup01 = new QGroupBox( tr("Text"), this );
    QGroupBox *pGroup03 = new QGroupBox( tr("Font"), this );
    QGroupBox *pGroup04 = new QGroupBox( tr("Brush"), this );
    QGroupBox *pGroup05 = new QGroupBox( tr("Pen"), this );

    pLayout->addWidget( pGroup01 );
    pLayout->addWidget( pGroup03 );
    pLayout->addWidget( pGroup04 );
    pLayout->addWidget( pGroup05 );

    QHBoxLayout *pLayout01 = new QHBoxLayout( pGroup01 );
    QHBoxLayout *pLayout03 = new QHBoxLayout( pGroup03 );
    QHBoxLayout *pLayout04 = new QHBoxLayout( pGroup04 );
    QHBoxLayout *pLayout05 = new QHBoxLayout( pGroup05 );

    pLayout01->addWidget( new LMLineTextWidget( pLine, pGroup01 ) );

    // font
    {
        WTextPanel *pTextPanel = new WTextPanel( pLine->getText(), pGroup03 );
        connect( pTextPanel, SIGNAL(signalChanged(const CBD::CBDText &)), pLine, SLOT(slotText(const CBD::CBDText &)) );
        connect( pLine, SIGNAL(signalChanged(const CBD::CBDText &)), pTextPanel, SLOT(slotRefresh(const CBD::CBDText &)) );
        pLayout03->addWidget( pTextPanel );
    }
    // brush
    {
        WBrushPanel *pBrushPanel = new WBrushPanel( pLine->getBrush(), pGroup04 );
        connect( pBrushPanel, SIGNAL(signalChanged(const QBrush &)), pLine, SLOT(slotBrush(const QBrush &)) );
        connect( pLine, SIGNAL(signalChanged(const QBrush &)), pBrushPanel, SLOT(slotRefresh(const QBrush &)) );
        pLayout04->addWidget( pBrushPanel );
    }
    // pen
    {
        WPenPanel *pPenPanel = new WPenPanel( pLine->getPen(), pGroup05 );
        connect( pPenPanel, SIGNAL(signalChanged(const QPen &)), pLine, SLOT(slotPen(const QPen &)) );
        connect( pLine, SIGNAL(signalChanged(const QPen &)), pPenPanel, SLOT(slotRefresh(const QPen &)) );
        pLayout05->addWidget( pPenPanel );
    }
}























