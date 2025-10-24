#include "LibInfo.h"
#include "DATAWPropDialog.h"
#include "DATAWQuery.h"

DATAWPropDialog::DATAWPropDialog( DATAWQuery *pItem, QWidget *pWidgetParent )
    : QDialog( pWidgetParent )
{
    setModal( true );
    setSizeGripEnabled( true );
    setObjectName( pItem->metaObject()->className() );

    setWindowIcon( pItem->getIcon() );
    setWindowTitle( tr( "Properties" ) );

    QVBoxLayout *playoutTop = new QVBoxLayout;
    // top bit (widget)
    playoutTop->addWidget( pItem->getPropWidget( this ) );
    // bottom bit (button)
    QDialogButtonBox *pButtonBox = new QDialogButtonBox( QDialogButtonBox::Close, this );
    playoutTop->addWidget( pButtonBox );

    connect( pButtonBox, SIGNAL(accepted()), this, SLOT(slotOk()) );
    connect( pButtonBox, SIGNAL(rejected()), this, SLOT(slotOk()) );

    setLayout( playoutTop );

    doLoadState();
}

void DATAWPropDialog::doLoadState()
{
    QString s = metaObject()->className();
    if ( !objectName().isEmpty() ) s = s + "/" + objectName();

    QSettings settings;
    int nWidth = settings.value( s + "/width", width() ).toInt();
    int nHeight= settings.value( s + "/height", height() ).toInt();
    resize( nWidth, nHeight );

    return;
    // the following is not good when going on and off of a dual display etc
    // QString s = objectName() + "/";
    // QSettings settings;
    // restoreGeometry( settings.value( s + "geometry" ).toByteArray() );
}

void DATAWPropDialog::doSaveState()
{
    QString s = metaObject()->className();
    if ( !objectName().isEmpty() ) s = s + "/" + objectName();

    QSettings settings;
    settings.setValue( s + "/width", width() );
    settings.setValue( s + "/height", height() );

    return;
    // the following is not good when going on and off of a dual display etc
    // QString s = objectName() + "/";
    // QSettings settings;
    // settings.setValue( s + "geometry", saveGeometry() );
}

void DATAWPropDialog::slotOk()
{
    doSaveState();
    done( QDialog::Accepted );
}



