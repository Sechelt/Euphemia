#include "LibInfo.h"
#include "DCallOutWidget.h"

//
//
//
DCallOutTextWidget::DCallOutTextWidget( DCallOut *p, QWidget *pParent )
    : QWidget( pParent )
{
    pCallOut = p;

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );
    QFormLayout *pLayout = new QFormLayout();

    pDisplay = new QComboBox( this );
    pDisplay->addItem( "Name", (int)DCallOut::TextDisplayName );
    pDisplay->addItem( "Code", (int)DCallOut::TextDisplayCode );
    pDisplay->addItem( "Comment", (int)DCallOut::TextDisplayComment );
    pDisplay->addItem( "None", (int)DCallOut::TextDisplayNone );
    WSetComboBox( pDisplay, pCallOut->getTextDisplay() );
    pLayout->addRow( tr( "Display:" ), pDisplay );
    connect( pDisplay, SIGNAL(currentIndexChanged(int)), pCallOut, SLOT(slotTextDisplay(int)) );

    pJustify = new WTextHAlignComboBox( pCallOut->getTextHAlign(), this );
    pLayout->addRow( tr( "Justify:" ), pJustify );
    connect( pJustify, SIGNAL(signalChanged(Qt::AlignmentFlag)), pCallOut, SLOT(slotTextHAlign(Qt::AlignmentFlag)) );

    pAlign = new WTextVAlignComboBox( pCallOut->getTextVAlign(), this );
    pLayout->addRow( tr( "Align:" ), pAlign );
    connect( pAlign, SIGNAL(signalChanged(Qt::AlignmentFlag)), pCallOut, SLOT(slotTextVAlign(Qt::AlignmentFlag)) );

    pWordWrap = new QCheckBox( this );
    pWordWrap->setChecked( pCallOut->isWordWrap() );
    pLayout->addRow( tr( "WordWrap:" ), pWordWrap );
    connect( pWordWrap, SIGNAL(checkStateChanged(Qt::CheckState)), pCallOut, SLOT(slotWordWrap(Qt::CheckState)) );

    pLayoutTop->addLayout( pLayout );
    pLayoutTop->addStretch( 10 );

    connect( pCallOut, SIGNAL(signalModified()), SLOT(slotModified()) );
}

void DCallOutTextWidget::slotModified()
{
    WSetComboBox( pDisplay, pCallOut->getTextDisplay() );
    pJustify->setValue( pCallOut->getTextHAlign() );
    pAlign->setValue( pCallOut->getTextVAlign() );
    pWordWrap->setChecked( pCallOut->isWordWrap() );
}

//
//
//
DCallOutConnectorWidget::DCallOutConnectorWidget( DCallOut *p, QWidget *pParent )
    : QWidget( pParent )
{
    pCallOut = p;

    QFormLayout *pLayoutTop = new QFormLayout( this );

    pComboBoxEnd = new WLineEndComboBox( pCallOut->getStyle( CBD::EOLEnd ), this );
    connect( pComboBoxEnd, SIGNAL(signalChanged(W::WLineEndStyles)), pCallOut, SLOT(slotStyleEnd(W::WLineEndStyles)) );

    pCheckBoxEnd = new QCheckBox( tr("Truncate"), this );
    pCheckBoxEnd->setChecked( pCallOut->isTruncate( CBD::EOLEnd ) );
    connect( pCheckBoxEnd, SIGNAL(checkStateChanged(Qt::CheckState)), pCallOut, SLOT(slotTruncateEnd(Qt::CheckState)) );

    pLayoutTop->addRow( "End: ", pComboBoxEnd );
    pLayoutTop->addRow( "", pCheckBoxEnd );

    connect( pCallOut, SIGNAL(signalModified()), SLOT(slotModified()) );
}

void DCallOutConnectorWidget::slotModified()
{
    pComboBoxEnd->setValue( pCallOut->getStyle( CBD::EOLEnd ) );
    pCheckBoxEnd->setChecked( pCallOut->isTruncate( CBD::EOLEnd ) );
}

//
//
//
DCallOutWidget::DCallOutWidget( DCallOut *pCallOut, QWidget *pParent )
    : QWidget( pParent )
{
    QHBoxLayout *pLayout = new QHBoxLayout( this );
    QGroupBox *pGroup01 = new QGroupBox( tr("Text"), this );
    QGroupBox *pGroup02 = new QGroupBox( tr("Connector"), this );
    QGroupBox *pGroup03 = new QGroupBox( tr("Font"), this );
    QGroupBox *pGroup04 = new QGroupBox( tr("Brush"), this );
    QGroupBox *pGroup05 = new QGroupBox( tr("Pen"), this );

    pLayout->addWidget( pGroup01 );
    pLayout->addWidget( pGroup02 );
    pLayout->addWidget( pGroup03 );
    pLayout->addWidget( pGroup04 );
    pLayout->addWidget( pGroup05 );

    QHBoxLayout *pLayout01 = new QHBoxLayout( pGroup01 );
    QHBoxLayout *pLayout02 = new QHBoxLayout( pGroup02 );
    QHBoxLayout *pLayout03 = new QHBoxLayout( pGroup03 );
    QHBoxLayout *pLayout04 = new QHBoxLayout( pGroup04 );
    QHBoxLayout *pLayout05 = new QHBoxLayout( pGroup05 );

    pLayout01->addWidget( new DCallOutTextWidget( pCallOut, pGroup01 ) );
    pLayout02->addWidget( new DCallOutConnectorWidget( pCallOut, pGroup02 ) );

    // font
    {
        WTextPanel *pTextPanel = new WTextPanel( pCallOut->getText(), pGroup03 );
        connect( pTextPanel, SIGNAL(signalChanged(const CBD::CBDText &)), pCallOut, SLOT(slotText(const CBD::CBDText &)) );
        connect( pCallOut, SIGNAL(signalChanged(const CBD::CBDText &)), pTextPanel, SLOT(slotRefresh(const CBD::CBDText &)) );
        pLayout03->addWidget( pTextPanel );
    }
    // brush
    {
        WBrushPanel *pBrushPanel = new WBrushPanel( pCallOut->getBrush(), pGroup04 );
        connect( pBrushPanel, SIGNAL(signalChanged(const QBrush &)), pCallOut, SLOT(slotBrush(const QBrush &)) );
        connect( pCallOut, SIGNAL(signalChanged(const QBrush &)), pBrushPanel, SLOT(slotRefresh(const QBrush &)) );
        pLayout04->addWidget( pBrushPanel );
    }
    // pen
    {
        WPenPanel *pPenPanel = new WPenPanel( pCallOut->getPen(), pGroup05 );
        connect( pPenPanel, SIGNAL(signalChanged(const QPen &)), pCallOut, SLOT(slotPen(const QPen &)) );
        connect( pCallOut, SIGNAL(signalChanged(const QPen &)), pPenPanel, SLOT(slotRefresh(const QPen &)) );
        pLayout05->addWidget( pPenPanel );
    }
}























