#include "LibInfo.h"
#include "DSpacerWidget.h"

#include <WLayoutWidget.h>

DSpacerWidget::DSpacerWidget( DSpacer *pSpacer, QWidget *pParent, bool bToolBar )
    : QWidget( pParent )
{
    this->pSpacer = pSpacer;

    QBoxLayout *pLayoutTop;

    if ( bToolBar )
        pLayoutTop = new QHBoxLayout( this );
    else
        pLayoutTop = new QVBoxLayout( this );

    // spacer type
    {
        QHBoxLayout *p = new QHBoxLayout();
        QLabel *pLabel = new QLabel( tr( "Type:" ), this );
        p->addWidget( pLabel );

        pTypes = new QComboBox( this );
        pTypes->addItem( tr("Strut"), (int)DSpacer::SizeFixed );
        pTypes->addItem( tr("Spring"), (int)DSpacer::SizeExpanding );
        pTypes->setCurrentIndex( pTypes->findData( (int)pSpacer->getSizeType() ) );
        p->addWidget( pTypes );
        p->addStretch( 1 );

        pLayoutTop->addLayout( p );

        connect( pTypes, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangedType(int)) );
        connect( pSpacer, SIGNAL(signalChangedSizeType(DSpacer::SizeTypes)), this, SLOT(slotChangedType(DSpacer::SizeTypes)) );
    }

    // layout
    {
        pLayoutWidget = new WLayoutWidget( this );
        // size hint
        pLayoutWidget->setSizeHint( pSpacer->getSizeHint() );
        connect( pLayoutWidget, SIGNAL(signalChangedSizeHint(const QSizeF &)), pSpacer, SLOT(slotSizeHint(const QSizeF &)) );
        connect( pSpacer, SIGNAL(signalChangedSizeHint(const QSizeF &)), pLayoutWidget, SLOT(slotSizeHint(const QSizeF &)) );
        // margins
        pLayoutWidget->setMargins( pSpacer->getMargins() );
        connect( pLayoutWidget, SIGNAL(signalChangedMargins(const QMargins &)), pSpacer, SLOT(slotMargins(const QMargins &)) );
        connect( pSpacer, SIGNAL(signalChangedMargins(const QMargins &)), pLayoutWidget, SLOT(slotMargins(const QMargins &)) );
        // constraints
        pLayoutWidget->setSizeConstraints( pSpacer->getSizeMinimum(), pSpacer->getSizeMaximum() );
        connect( pLayoutWidget, SIGNAL(signalChangedSizeConstraints(const QSizeF &,const QSizeF &)), pSpacer, SLOT(slotSizeConstraints(const QSizeF &,const QSizeF &)) );
        connect( pSpacer, SIGNAL(signalChangedSizeConstraints(const QSizeF &,const QSizeF &)), pLayoutWidget, SLOT(slotSizeConstraints(const QSizeF &,const QSizeF &)) );
        // stretch
        pLayoutWidget->setStretch( pSpacer->getStretch() );
        connect( pLayoutWidget, SIGNAL(signalChangedStretch(const QSizeF &)), pSpacer, SLOT(slotStretch(const QSizeF &)) );
        connect( pSpacer, SIGNAL(signalChangedStretch(const QSizeF &)), pLayoutWidget, SLOT(slotStretch(const QSizeF &)) );

        pLayoutTop->addWidget( pLayoutWidget );
    }
}

void DSpacerWidget::slotChangedType( int nIndex )
{
    // important to call slotSpacerType() and not setSpacerType()
    pSpacer->slotSizeType( (DSpacer::SizeTypes)pTypes->itemData( nIndex ).toInt() );
}

void DSpacerWidget::slotChangedType( DSpacer::SizeTypes nType )
{
    pTypes->setCurrentIndex( pTypes->findData( (int)nType ) );
}













