#include "LibInfo.h"
#include "DLineWidget.h"

#include <W.h>
#include <WLineEndComboBox.h>
#include <WText.h>
#include <WBrush.h>
#include <WPen.h>

//
//
//
DLineTextWidget::DLineTextWidget( DLine *p, QWidget *pParent )
    : QWidget( pParent )
{
    pLine = p;

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );
    QFormLayout *pLayout    = new QFormLayout();

    pDisplay = new QComboBox( this );
    pDisplay->addItem( "Name", (int)DLine::TextDisplayName );
    pDisplay->addItem( "Code", (int)DLine::TextDisplayCode );
    pDisplay->addItem( "Comment", (int)DLine::TextDisplayComment );
    pDisplay->addItem( "None", (int)DLine::TextDisplayNone );
    WSetComboBox( pDisplay, pLine->getTextDisplay() );
    pLayout->addRow( tr( "Label:" ), pDisplay );
    connect( pDisplay, SIGNAL(currentIndexChanged(int)), pLine, SLOT(slotTextDisplay(int)) );

    pLayoutTop->addLayout( pLayout );
    pLayoutTop->addStretch( 10 );

    connect( pLine, SIGNAL(signalModified()), SLOT(slotModified()) );
}

void DLineTextWidget::slotModified()
{
    WSetComboBox( pDisplay, pLine->getTextDisplay() );
}

//
//
//
DLineConnectorWidget::DLineConnectorWidget( DLine *p, QWidget *pParent )
    : QWidget( pParent )
{
    pLine = p;
    QFormLayout *pLayoutTop = new QFormLayout( this );

    pComboBoxBegin = new WLineEndComboBox( pLine->getStyle( CBD::EOLBegin ), this );
    pComboBoxEnd = new WLineEndComboBox( pLine->getStyle( CBD::EOLEnd ), this );
    pCheckBoxBegin = new QCheckBox( tr("Truncate"), this );
    pCheckBoxBegin->setCheckState( pLine->isTruncate( CBD::EOLBegin ) ? Qt::Checked : Qt::Unchecked );
    pCheckBoxEnd = new QCheckBox( tr("Truncate"), this );
    pCheckBoxEnd->setCheckState( pLine->isTruncate( CBD::EOLEnd ) ? Qt::Checked : Qt::Unchecked );
    pLayoutTop->addRow( "Begin: ", pComboBoxBegin );
    pLayoutTop->addRow( "", pCheckBoxBegin );
    pLayoutTop->addRow( "End: ", pComboBoxEnd );
    pLayoutTop->addRow( "", pCheckBoxEnd );

    connect( pComboBoxBegin, SIGNAL(signalChanged(W::WLineEndStyles)), pLine, SLOT(slotStyleBegin(W::WLineEndStyles)) );
    connect( pComboBoxEnd, SIGNAL(signalChanged(W::WLineEndStyles)), pLine, SLOT(slotStyleEnd(W::WLineEndStyles)) );
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    connect( pCheckBoxBegin, SIGNAL(checkStateChanged(Qt::CheckState)), pLine, SLOT(slotTruncateBegin(Qt::CheckState)) );
    connect( pCheckBoxEnd, SIGNAL(checkStateChanged(Qt::CheckState)), pLine, SLOT(slotTruncateEnd(Qt::CheckState)) );
#else
    connect( pCheckBoxBegin, SIGNAL(stateChanged(int)), pLine, SLOT(slotTruncateBegin(int)) );
    connect( pCheckBoxEnd, SIGNAL(stateChanged(int)), pLine, SLOT(slotTruncateEnd(int)) );
#endif
    connect( pLine, SIGNAL(signalModified()), SLOT(slotModified()) );
}

void DLineConnectorWidget::slotModified()
{
    pComboBoxBegin->setValue( pLine->getStyle( CBD::EOLBegin ) );
    pCheckBoxBegin->setChecked( pLine->isTruncate( CBD::EOLBegin ) );
    pComboBoxEnd->setValue( pLine->getStyle( CBD::EOLEnd ) );
    pCheckBoxEnd->setChecked( pLine->isTruncate( CBD::EOLEnd ) );
}

//
//
//
DLineWidget::DLineWidget( DLine *pLine, QWidget *pParent )
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

    pLayout01->addWidget( new DLineTextWidget( pLine, pGroup01 ) );
    pLayout02->addWidget( new DLineConnectorWidget( pLine, pGroup02 ) );

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























