#include "LibInfo.h"
#include "AWDataDialog.h"
#include "AWObject.h"

AWDataDialog::AWDataDialog( AWObject *pObject, QWidget *pWidgetParent )
    : QDialog( pWidgetParent )
{
    AWDataWidget *pDataWidget = pObject->getObjectWidget( this );

    setModal( true );
    setSizeGripEnabled( true );
    setObjectName( pObject->metaObject()->className() );

    setWindowIcon( pObject->getIcon() );
    setWindowTitle( pObject->getDialogCaption( pObject, tr("Object Properties") ) );

    QVBoxLayout *playoutTop = new QVBoxLayout;
    // top bit (widget)
    playoutTop->addWidget( pDataWidget );
    // bottom bit (button)
    QDialogButtonBox *pButtonBox = new QDialogButtonBox( QDialogButtonBox::Close, this );
    playoutTop->addWidget( pButtonBox );

    connect( pButtonBox, SIGNAL(accepted()), this, SLOT(slotOk()) );
    connect( pButtonBox, SIGNAL(rejected()), this, SLOT(slotRejected()) );

    setLayout( playoutTop );

    doLoadState();
}

void AWDataDialog::doLoadState()
{
    QString s = objectName() + "/";
    QSettings settings;
    restoreGeometry( settings.value( s + "geometry" ).toByteArray() );
}

void AWDataDialog::doSaveState()
{
    QString s = objectName() + "/";
    QSettings settings;
    settings.setValue( s + "geometry", saveGeometry() );
}

void AWDataDialog::slotOk()
{
    doSaveState();
    done( QDialog::Accepted );
}

void AWDataDialog::slotRejected()
{
    doSaveState();
    done( QDialog::Rejected );
}


