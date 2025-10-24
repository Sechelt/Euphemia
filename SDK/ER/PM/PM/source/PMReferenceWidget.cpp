#include "LibInfo.h"
#include "PMReferenceWidget.h"

#include "PMTableBase.h"
#include "PMKey.h"
#include "PMColumn.h"
#include "PMReferenceColumnsWidget.h"
#include "PMReferenceColumn.h"

//
// PMReferenceWidget
//
// \todo Properly handle Alternate Key(s)
//
PMReferenceWidget::PMReferenceWidget( PMReference *pReference, QWidget *pwidgetParent )
    : QWidget( pwidgetParent )
{
    this->pReference = pReference;

    QVBoxLayout *pLayout = new QVBoxLayout( this );
    QGroupBox *pFrame;
    QVBoxLayout *pLayoutFrame;

    // PK/AK combo box
    {
        QHBoxLayout *p = new QHBoxLayout();
        p->addWidget( new QLabel( tr("Primary Key/Alternate Key"), this ) );
        pKeys = new QComboBox( this );
        pKeys->setToolTip( tr("Primary Key/Alternate Key(s)") );
        p->addWidget( pKeys );
        p->addStretch( 10 );

        pLayout->addLayout( p );

        PMTableBase *pTable = pReference->getParentTableObject();
        if ( pTable )
        { 
            QList<PMKey *> l = pTable->getKeys();       // PK and any AK
            PMKey *pKey;
            foreach( pKey, l )
            {
                QIcon icon;
                if ( pKey->isPrimary() ) icon = QIcon(":PM/PrimaryKey"); else icon = QIcon(":PM/AlternateKey");
                pKeys->addItem( icon, pKey->getName(), qlonglong(pKey) );
            }
            pKeys->addItem( "", QVariant() );           // we start off with no key chosen
            if ( pReference->getKey() ) pKeys->setCurrentIndex( pKeys->findData( qlonglong(pReference->getKey()) ) );
            else pKeys->setCurrentIndex( pKeys->findData( QVariant() ) );
        }
        connect( pKeys, SIGNAL(activated(int)), SLOT(slotKeyChanged(int)) );
    }

    pFrame = new QGroupBox( tr("Rules"), this );
    pLayoutFrame = new QVBoxLayout( pFrame );
    pLayoutFrame->addWidget( pRulesWidget = new PMReferenceRulesWidget( pReference , pFrame ) );
    pLayout->addWidget( pFrame );


    pFrame = new QGroupBox( tr("Columns"), this );
    pLayoutFrame = new QVBoxLayout( pFrame );
    pLayoutFrame->addWidget( pColumnsWidget = new PMReferenceColumnsWidget( pReference , pFrame ) );
    pLayout->addWidget( pFrame );

    pFrame = new QGroupBox( tr("Display"), this );
    pLayoutFrame = new QVBoxLayout( pFrame );
    pLayoutFrame->addWidget( pColumnsDisplayWidget = new PMReferenceColumnsDisplayWidget( pReference , pFrame ) );
    pLayout->addWidget( pFrame );
}

void PMReferenceWidget::setKey( PMKey *pKey )
{
    pReference->setKey( pKey );
    pRulesWidget->doReInit();
    pColumnsWidget->doReInit();
    pColumnsDisplayWidget->doReInit();
}

void PMReferenceWidget::slotKeyChanged( int nIndex )
{
    QVariant v = pKeys->itemData( nIndex );
    if ( v.isNull() ) setKey( nullptr );
    else setKey( (PMKey*)v.toLongLong() );
}

//
// PMReferenceRulesWidget
//
PMReferenceRulesWidget::PMReferenceRulesWidget( PMReference *pReference, QWidget *pwidgetParent )
    : QWidget( pwidgetParent )
{
    this->pReference = pReference;                                                                             
    QVBoxLayout *pLayout = new QVBoxLayout( this );

    // OPTIONS
    {
        QHBoxLayout *pLayout01 = new QHBoxLayout();

        pLayout01->addWidget( new QLabel( tr("Match:") ) );
        QComboBox *pMatch = new QComboBox( this );
        pMatch->addItem( "FULL" );
        pMatch->addItem( "PARTIAL" );
        pMatch->addItem( "" );
        pMatch->setCurrentText( pReference->getMatch() );
        pLayout01->addWidget( pMatch );
        connect( pMatch, SIGNAL(currentTextChanged(const QString &)), SLOT(slotMatch(const QString &)) );

        pLayout01->addWidget( new QLabel( tr("On Delete:") ) );
        QComboBox *pOnDelete = new QComboBox( this );
        pOnDelete->addItem( "CASCADE" );
        pOnDelete->addItem( "NO ACTION" );
        pOnDelete->addItem( "SET DEFAULT" );
        pOnDelete->addItem( "SET NULL" );
        pOnDelete->addItem( "" );
        pOnDelete->setCurrentText( pReference->getOnDelete() );
        pLayout01->addWidget( pOnDelete );
        connect( pOnDelete, SIGNAL(currentTextChanged(const QString &)), SLOT(slotOnDelete(const QString &)) );

        pLayout01->addWidget( new QLabel( tr("On Update:") ) );
        QComboBox *pOnUpdate = new QComboBox( this );
        pOnUpdate->addItem( "CASCADE" );
        pOnUpdate->addItem( "NO ACTION" );
        pOnUpdate->addItem( "SET DEFAULT" );
        pOnUpdate->addItem( "SET NULL" );
        pOnUpdate->addItem( "" );
        pOnUpdate->setCurrentText( pReference->getOnUpdate() );
        pLayout01->addWidget( pOnUpdate );
        connect( pOnUpdate, SIGNAL(currentTextChanged(const QString &)), SLOT(slotOnUpdate(const QString &)) );
        pLayout01->addStretch( 10 );
        pLayout->addLayout( pLayout01 );
    }
}

void PMReferenceRulesWidget::doReInit()
{
}

void PMReferenceRulesWidget::slotMatch( const QString &s )
{
    pReference->setMatch( s );
}

void PMReferenceRulesWidget::slotOnDelete( const QString &s )
{
    pReference->setOnDelete( s );
}

void PMReferenceRulesWidget::slotOnUpdate( const QString &s )
{
    pReference->setOnUpdate( s );
}

//
// PMReferenceColumnsDisplayWidget
//
PMReferenceColumnsDisplayWidget::PMReferenceColumnsDisplayWidget( PMReference *pReference, QWidget *pwidgetParent )
    : QWidget( pwidgetParent )
{
    this->pReference = pReference;

    QList<ADObject*> l = pReference->getObjects( QString(), QString(), "PMReferenceColumnDisplay" );
    if ( l.count() ) 
        pColumnDisplay = (PMReferenceColumnDisplay *)l.first(); 
    else
        pColumnDisplay = (PMReferenceColumnDisplay*)pReference->getObject( "PMReferenceColumnDisplay" );

    QHBoxLayout *pLayout = new QHBoxLayout( this );

    {
        QVBoxLayout *p = new QVBoxLayout();
        pColumnsUsed = new QListWidget( this );
        p->addWidget( new QLabel( tr("Used"), this ) );
        p->addWidget( pColumnsUsed );
        pColumnsUsed->setToolTip( tr("Columns to display when viewing/selecting a row in the foreign key table.") );
        pLayout->addLayout( p );

        QVector<PMColumn *> vectorColumns = pColumnDisplay->getColumns();
        for ( int n = 0; n < vectorColumns.count(); n++ )
        {
            PMColumn *pColumn = vectorColumns.at( n );
            pColumnsUsed->addItem( pColumn->getName() );
        }
    }

    {
        QVBoxLayout *pLayoutButtons = new QVBoxLayout();
        pLayoutButtons->addStretch( 10 );

        pToUsed = new QToolButton( this );
        pToUsed->setIcon( QIcon( ":PM/ToUsed" ) );
        pToUsed->setToolTip( tr("Move column from available to used.") );
        pLayoutButtons->addWidget( pToUsed );
        connect( pToUsed, SIGNAL(clicked()), SLOT(slotToUsed()) );

        pLayoutButtons->addStretch( 5 );

        pToAvailable  = new QToolButton( this );
        pToAvailable->setIcon( QIcon( ":PM/ToAvailable" ) );
        pToAvailable->setToolTip( tr("Move column from used to available.") );
        pLayoutButtons->addWidget( pToAvailable );
        connect( pToAvailable, SIGNAL(clicked()), SLOT(slotToAvailable()) );

        pLayoutButtons->addStretch( 10 );

        pLayout->addLayout( pLayoutButtons );
    }

    {
        QVBoxLayout *p = new QVBoxLayout();
        pColumnsAvailable = new QListWidget( this );
        p->addWidget( new QLabel( tr("Available"), this ) );
        p->addWidget( pColumnsAvailable );
        pColumnsAvailable->setToolTip( tr("Columns available in the foreign key table.") );
        pLayout->addLayout( p );
    }

    doInit();
}

void PMReferenceColumnsDisplayWidget::doReInit()
{
    // clear
    // - get PMReferenceColumnDisplay (should be exactly one of these)
    PMReferenceColumnDisplay *pReferenceColumnDisplay = nullptr;
    {
        QList<ADObject*> l = pReference->getObjects( QString(), QString(), "PMReferenceColumnDisplay" );
        Q_ASSERT( l.count() == 1 );
        pReferenceColumnDisplay = (PMReferenceColumnDisplay*)l.first();
    }
    // - disconnect any PMColumn's from PMReferenceColumnDisplay
    {
        QList<ADObject*> l = pReferenceColumnDisplay->getUses( QString(), QString(), "PMColumn" );
        ADObject *pObject;
        foreach( pObject, l )
        {
            pReferenceColumnDisplay->doDisconnectColumn( (PMColumn*)pObject );
        }                                                                                                            
    }
    pColumnsUsed->clear();
    pColumnsAvailable->clear();
    doInit();
}

void PMReferenceColumnsDisplayWidget::doInit()
{
    // load pColumnsAvailable
    {
        PMTableBase *pTableParent    = pReference->getParentTableObject();
        if ( !pTableParent ) 
        {
            qWarning( "[PAH][%s][%s][%d] No primary key table. The line-end should be connected to the table with the primary-key.\n", __FILE__, __FUNCTION__, __LINE__ );
            return;
        }

        QList<ADObject*> l = pTableParent->getObjects( QString(), QString(), "PMColumn" );
        ADObject *pObject;
        foreach( pObject, l )
        {
            pColumnsAvailable->addItem( pObject->getName() );
        }                                                                                                            
    }

    // load pColumnsUsed
    {
        PMReferenceColumnDisplay *pReferenceColumnDisplay = nullptr;
        QList<ADObject*> l = pReference->getObjects( QString(), QString(), "PMReferenceColumnDisplay" );
        if ( l.count() < 1 ) 
        {
            qWarning( "[PAH][%s][%s][%d] No PMReferenceColumnDisplay. Fixed.\n", __FILE__, __FUNCTION__, __LINE__ );
            pReferenceColumnDisplay = (PMReferenceColumnDisplay*)pReference->getObject( "PMReferenceColumnDisplay" );
            Q_ASSERT( pReferenceColumnDisplay );
        }
        else 
            pReferenceColumnDisplay = (PMReferenceColumnDisplay*)l.first();

        {
            QList<ADObject*> l = pReferenceColumnDisplay->getUses( QString(), QString(), "PMColumn" );
            ADObject *pObject;
            foreach( pObject, l )
            {
                pColumnsUsed->addItem( pObject->getName() );
            }                                                                                                            
        }
    }
}

void PMReferenceColumnsDisplayWidget::slotToUsed()
{
    PMTableBase *pTableParent = pReference->getParentTableObject();
    if ( !pTableParent ) 
    {
        qWarning( "[PAH][%s][%s][%d] No primary key table. The line-end should be connected to the table with the primary-key.\n", __FILE__, __FUNCTION__, __LINE__ );
        return;
    }

    QList<QListWidgetItem *> l = pColumnsAvailable->selectedItems();
    QListWidgetItem*pItem;
    foreach( pItem, l )
    {
        PMColumn *pColumn = pTableParent->getColumn( pItem->text() );
        if ( !pColumn  ) continue;
        pColumnDisplay->doConnectColumn( pColumn );
        pColumnsUsed->addItem( pItem->text() );
    }
}

void PMReferenceColumnsDisplayWidget::slotToAvailable()
{
    PMTableBase *pTableParent = pReference->getParentTableObject();
    if ( !pTableParent ) 
    {
        qWarning( "[PAH][%s][%s][%d] No primary key table. The line-end should be connected to the table with the primary-key.\n", __FILE__, __FUNCTION__, __LINE__ );
        return;
    }

    QList<QListWidgetItem *> l = pColumnsUsed->selectedItems();
    QListWidgetItem*pItem;
    foreach( pItem, l )
    {

        PMColumn *pColumn = pTableParent->getColumn( pItem->text() );
        if ( !pColumn  ) continue;
        pColumnDisplay->doDisconnectColumn( pColumn );
        delete pColumnsUsed->takeItem( pColumnsUsed->row( pItem ) );
    }
}


