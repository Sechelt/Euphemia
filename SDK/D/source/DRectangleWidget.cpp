#include "LibInfo.h"
#include "DRectangleWidget.h"

//
//
//
DRectangleTextWidget::DRectangleTextWidget( DRectangle *p, QWidget *pParent )
    : QWidget( pParent )
{
    pRectangle = p;

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );
    QFormLayout *pLayout = new QFormLayout();

    pDisplay = new QComboBox( this );
    pDisplay->addItem( "Name", (int)DRectangle::TextDisplayName );
    pDisplay->addItem( "Code", (int)DRectangle::TextDisplayCode );
    pDisplay->addItem( "Comment", (int)DRectangle::TextDisplayComment );
    pDisplay->addItem( "None", (int)DRectangle::TextDisplayNone );
    WSetComboBox( pDisplay, pRectangle->getTextDisplay() );
    pLayout->addRow( tr( "Display:" ), pDisplay );
    connect( pDisplay, SIGNAL(currentIndexChanged(int)), pRectangle, SLOT(slotTextDisplay(int)) );

    pJustify = new WTextHAlignComboBox( pRectangle->getTextHAlign(), this );
    pLayout->addRow( tr( "Justify:" ), pJustify );
    connect( pJustify, SIGNAL(signalChanged(Qt::AlignmentFlag)), pRectangle, SLOT(slotTextHAlign(Qt::AlignmentFlag)) );

    pAlign = new WTextVAlignComboBox( pRectangle->getTextVAlign(), this );
    pLayout->addRow( tr( "Align:" ), pAlign );
    connect( pAlign, SIGNAL(signalChanged(Qt::AlignmentFlag)), pRectangle, SLOT(slotTextVAlign(Qt::AlignmentFlag)) );

    pWordWrap = new QCheckBox( this );
    pWordWrap->setChecked( pRectangle->isWordWrap() );
    pLayout->addRow( tr( "WordWrap:" ), pWordWrap );
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    connect( pWordWrap, SIGNAL(checkStateChanged(Qt::CheckState)), pRectangle, SLOT(slotWordWrap(Qt::CheckState)) );
#else
    connect( pWordWrap, SIGNAL(stateChanged(int)), pRectangle, SLOT(slotWordWrap(int)) );
#endif
    pLayoutTop->addLayout( pLayout );
    pLayoutTop->addStretch( 10 );

    connect( pRectangle, SIGNAL(signalModified()), SLOT(slotModified()) );
}

void DRectangleTextWidget::slotModified()
{
    WSetComboBox( pDisplay, pRectangle->getTextDisplay() );
    pJustify->setValue( pRectangle->getTextHAlign() );
    pAlign->setValue( pRectangle->getTextVAlign() );
    pWordWrap->setChecked( pRectangle->isWordWrap() );
}

//
//
//
DRectangleWidget::DRectangleWidget( DRectangle *pRectangle, QWidget *pParent )
    : QWidget( pParent )
{
    QHBoxLayout *pLayout = new QHBoxLayout( this );
    QGroupBox *pGroup01 = new QGroupBox( tr("Text"), this );
    QGroupBox *pGroup02 = new QGroupBox( tr("Font"), this );
    QGroupBox *pGroup03 = new QGroupBox( tr("Brush"), this );
    QGroupBox *pGroup04 = new QGroupBox( tr("Pen"), this );

    pLayout->addWidget( pGroup01 );
    pLayout->addWidget( pGroup02 );
    pLayout->addWidget( pGroup03 );
    pLayout->addWidget( pGroup04 );

    QHBoxLayout *pLayout01 = new QHBoxLayout( pGroup01 );
    QHBoxLayout *pLayout02 = new QHBoxLayout( pGroup02 );
    QHBoxLayout *pLayout03 = new QHBoxLayout( pGroup03 );
    QHBoxLayout *pLayout04 = new QHBoxLayout( pGroup04 );

    pLayout01->addWidget( new DRectangleTextWidget( pRectangle, pGroup01 ) );

    // font
    {
        WTextPanel *pTextPanel = new WTextPanel( pRectangle->getText(), pGroup02 );
        connect( pTextPanel, SIGNAL(signalChanged(const CBD::CBDText &)), pRectangle, SLOT(slotText(const CBD::CBDText &)) );
        connect( pRectangle, SIGNAL(signalChanged(const CBD::CBDText &)), pTextPanel, SLOT(slotRefresh(const CBD::CBDText &)) );
        pLayout02->addWidget( pTextPanel );
    }
    // brush
    {
        WBrushPanel *pBrushPanel = new WBrushPanel( pRectangle->getBrush(), pGroup03 );
        connect( pBrushPanel, SIGNAL(signalChanged(const QBrush &)), pRectangle, SLOT(slotBrush(const QBrush &)) );
        connect( pRectangle, SIGNAL(signalChanged(const QBrush &)), pBrushPanel, SLOT(slotRefresh(const QBrush &)) );
        pLayout03->addWidget( pBrushPanel );
    }
    // pen
    {
        WPenPanel *pPenPanel = new WPenPanel( pRectangle->getPen(), pGroup04 );
        connect( pPenPanel, SIGNAL(signalChanged(const QPen &)), pRectangle, SLOT(slotPen(const QPen &)) );
        connect( pRectangle, SIGNAL(signalChanged(const QPen &)), pPenPanel, SLOT(slotRefresh(const QPen &)) );
        pLayout04->addWidget( pPenPanel );
    }
}























