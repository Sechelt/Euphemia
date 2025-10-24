#include "LibInfo.h"
#include "LMInheritanceWidget.h"

#include "LMEntityBase.h"

LMInheritanceWidget::LMInheritanceWidget( LMInheritance *p, QWidget *pParent )
    : QWidget( pParent )
{
    pInheritance = p;

    QHBoxLayout *pLayoutTop = new QHBoxLayout( this );
    QFormLayout *pLayout = new QFormLayout();

    // MutuallyExclusive
    pMutuallyExclusive = new QCheckBox( this );
    pMutuallyExclusive->setChecked( pInheritance->isMutuallyExclusive() );
    pLayout->addRow( tr("Mutually Exclusive:"), pMutuallyExclusive );
    connect( pMutuallyExclusive, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotMutuallyExclusive(Qt::CheckState)) );

    // Complete
    pComplete = new QCheckBox( this );
    pComplete->setChecked( pInheritance->isComplete() );
    pLayout->addRow( tr("Complete:"), pComplete );
    connect( pComplete, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotComplete(Qt::CheckState)) );

    // Base Entity
    {
        // load up a combo box with all LMEntityNormal entities attached via a LMInheritanceLink 
        pBaseEntity     = new QComboBox( this );
        vectorEntities  = pInheritance->getEntities();
        for ( int n = 0; n < vectorEntities.count(); n++ )
        {
            pBaseEntity->addItem( vectorEntities.at( n )->getRectangle()->getName() );
        }
        // add blank
        pBaseEntity->addItem( "" );
        // set current
        DSource *p = pInheritance->getBaseEntity();
        if ( p ) pBaseEntity->setCurrentIndex( vectorEntities.indexOf( p ) );
        else pBaseEntity->setCurrentIndex( vectorEntities.count() );

        pLayout->addRow( tr( "Base:" ), pBaseEntity );
        connect( pBaseEntity, SIGNAL(currentIndexChanged(int)), SLOT(slotBaseEntity(int)) );
        connect( pInheritance, SIGNAL(signalChangedBaseEntity(DSource*)), SLOT(slotModified(DSource*)) );
    }

    pLayoutTop->addLayout( pLayout );

    connect( pInheritance, SIGNAL(signalModified()), SLOT(slotModified()) );
}

void LMInheritanceWidget::slotMutuallyExclusive( Qt::CheckState n )
{
    pInheritance->setMutuallyExclusive( n );
}

void LMInheritanceWidget::slotComplete( Qt::CheckState n )
{
    pInheritance->setComplete( n );
}

void LMInheritanceWidget::slotBaseEntity( int n )
{
    if ( n >= vectorEntities.count() ) pInheritance->setBaseEntity( nullptr );
    else pInheritance->setBaseEntity( vectorEntities.at( n ) );
}

void LMInheritanceWidget::slotModified()
{
    pMutuallyExclusive->setChecked( pInheritance->isMutuallyExclusive() );
    pComplete->setChecked( pInheritance->isComplete() );
}

void LMInheritanceWidget::slotModified( DSource *pSource )
{
    if ( pSource ) pBaseEntity->setCurrentIndex( vectorEntities.indexOf( pSource ) );
    else pBaseEntity->setCurrentIndex( vectorEntities.count() );
}


