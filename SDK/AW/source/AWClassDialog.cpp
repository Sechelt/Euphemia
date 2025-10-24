#include "LibInfo.h"
#include "AWClassDialog.h"
#include "AWObject.h"

AWClassDialog::AWClassDialog( AWObject *pObject, QWidget *pWidgetParent )
    : QDialog( pWidgetParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    AWClassWidget *pClassWidget = pObject->getClassWidget( this );

    setModal( true );
    setSizeGripEnabled( true );
    setObjectName( pObject->metaObject()->className() );

    setWindowIcon( pObject->getIcon() );
    setWindowTitle( pObject->getDialogCaption( pObject, tr("Class Properties") ) );

    QVBoxLayout *playoutTop = new QVBoxLayout;
    // top bit (widget)
    playoutTop->addWidget( pClassWidget );
    // bottom bit (button)
    QDialogButtonBox *pButtonBox = new QDialogButtonBox( QDialogButtonBox::Close, this );
    playoutTop->addWidget( pButtonBox );

    connect( pButtonBox, SIGNAL(accepted()), this, SLOT(slotOk()) );
    connect( pButtonBox, SIGNAL(rejected()), this, SLOT(slotRejected()) );

    setLayout( playoutTop );

    doLoadState();

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

void AWClassDialog::doLoadState()
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

void AWClassDialog::doSaveState()
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

void AWClassDialog::slotOk()
{
    doSaveState();
    done( QDialog::Accepted );
}

void AWClassDialog::slotRejected()
{
    doSaveState();
    done( QDialog::Rejected );
}


