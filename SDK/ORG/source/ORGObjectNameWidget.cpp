#include "LibInfo.h"
#include "ORGObjectNameWidget.h"

#include <WImageButton.h>

ORGObjectNameWidget::ORGObjectNameWidget( ORGPerson *pPerson, QWidget *pWidgetParent )
    : QWidget( pWidgetParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    this->pPerson = pPerson;

    QVBoxLayout *pLayout00 = new QVBoxLayout( this );

    QHBoxLayout *pLayout01 = new QHBoxLayout;

    pPhoto = new WImageButton( this, pPerson->getPhoto().toImage() );
    pPhoto->setMinimumSize( 64, 64 );
    pLayout01->addWidget( pPhoto );

    {
        QGridLayout *pLayout02 = new QGridLayout;

        pLabelOID = new QLabel( QString( "%1" ).arg( pPerson->getOID() ), this );
        pLineEditName = new QLineEdit( pPerson->getName(), this );
        pLineEditCode = new QLineEdit( pPerson->getCode(), this );

        pLayout02->addWidget( new QLabel( tr("OID:"), this ), 0, 0 );
        pLayout02->addWidget( pLabelOID, 0, 1 );
        pLayout02->addWidget( new QLabel( tr("Name:"), this ), 1, 0 );
        pLayout02->addWidget( pLineEditName, 1, 1 );
        pLayout02->addWidget( new QLabel( tr("Code:"), this ), 2, 0 );
        pLayout02->addWidget( pLineEditCode, 2, 1 );

        pLayout01->addLayout( pLayout02 );
    }
    pLayout00->addLayout( pLayout01 );

    pTextEditComment = new QTextEdit( pPerson->getComment(), this );
    pLayout00->addWidget( new QLabel( tr("Comment"), this ) );
    pLayout00->addWidget( pTextEditComment );


    connect( pPhoto, SIGNAL(signalClick()), SLOT(slotPhoto()) );
    connect( pLineEditName, SIGNAL(editingFinished()), SLOT(slotName()) );
    connect( pLineEditCode, SIGNAL(editingFinished()), SLOT(slotCode()) );
    connect( pTextEditComment, SIGNAL(textChanged()), SLOT(slotComment()) );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

void ORGObjectNameWidget::slotPhoto()
{
    QString stringFileName = QFileDialog::getOpenFileName( this, tr("Select image..."), QString(), "*.xpm" );
    if ( stringFileName.isEmpty() ) return;
    QPixmap pixmap;
    bool b = pixmap.load( stringFileName );
    if ( !b ) return;

    pPhoto->setImage( pixmap.scaled( 64, 64 ).toImage() );
    pPerson->setPhoto( pixmap.scaled( 64, 64 ) );
}






















