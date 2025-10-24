#include "LibInfo.h"
#include "DATAInfoWidget.h"

#include "DATAProfiles.h"
#include "DATAProfile.h"

DATAInfoWidget::DATAInfoWidget( DATAProfile *pProfile, QWidget *pParent )
    : QTreeWidget( pParent )
{
    this->pProfile = pProfile;
                                                                                                                                        
    QStringList l;
    l << tr( "Feature" ) << tr( "Value" ) << tr( "Description" );

    setColumnCount( 3 );

    setHeaderLabels( l );
    setSelectionBehavior( QAbstractItemView::SelectRows );
    setSelectionMode( QAbstractItemView::NoSelection );    

    if ( pProfile )
    {
        QMapIterator<QString,DATAInfo*> i( pProfile->mapInfo );
        while ( i.hasNext() ) 
        {
            i.next();

            QTreeWidgetItem *pItem = new QTreeWidgetItem( this );
            pItem->setText( 0, i.key() );
            pItem->setText( 1, i.value()->vValue.toString() );
            pItem->setText( 2, i.value()->stringDescription );
            DATAInfo *pInfoChild;
            foreach( pInfoChild, i.value()->mapValues )
            {
                QTreeWidgetItem *pItem2 = new QTreeWidgetItem( pItem );
                pItem2->setText( 0, pInfoChild->stringInfo );
                pItem2->setText( 1, pInfoChild->vValue.toString() );
                pItem2->setText( 2, pInfoChild->stringDescription );
            }
        }
    }
    doLoadState();
}

DATAInfoWidget::~DATAInfoWidget()
{
    doSaveState();
}

void DATAInfoWidget::doSaveState()
{
    QString s = metaObject()->className();
    if ( !objectName().isEmpty() ) s = s + "/" + objectName();
    s += "/column";

    QSettings settings;
    settings.beginWriteArray( s );
    for ( int n = 0; n < columnCount(); n++ )
    {
        settings.setArrayIndex( n );
        settings.setValue( "w", columnWidth( n ) );
    }
    settings.endArray();
}

void DATAInfoWidget::doLoadState()
{
    QString s = metaObject()->className();
    if ( !objectName().isEmpty() ) s = s + "/" + objectName();
    s += "/column";

    QSettings settings;
    int nSize = settings.beginReadArray( s );
    for ( int n = 0; n < nSize; n++ ) 
    {
        settings.setArrayIndex( n );
        setColumnWidth( n, settings.value( "w", QString::number( columnWidth( n ) ) ).toInt() );
    }
    settings.endArray();
}

