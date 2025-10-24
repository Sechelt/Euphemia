#include "LibInfo.h"
#include "AWScratchWidget.h"

//
//
//
AWScratchWidget::AWScratchWidget( QWidget *pwidgetParent, const QString &stringXML, QXmlStreamReader *pSvg )
    : QWidget( pwidgetParent )
{
    this->stringXML = stringXML;
    pRenderer = new QSvgRenderer( pSvg, this );
}

AWScratchWidget::~AWScratchWidget()
{
}

//
//
//
AWScratchTabWidget::AWScratchTabWidget( QWidget *pwidgetParent )
    : QTabWidget( pwidgetParent )
{
    setWindowTitle( tr("Scratch Pad") );
}

AWScratchTabWidget::~AWScratchTabWidget()
{
}

void AWScratchTabWidget::contextMenuEvent( QContextMenuEvent *pEvent )
{
    QMenu *pMenu = new QMenu( this );
    pMenu->addAction( QPixmap( ":W/paste" ), tr( "Paste" ), this, SLOT( slotPaste() ) );
//    pMenu->addAction( QPixmap( ":W/remove" ), tr("Delete ") + metaObject()->className(), this, SLOT(slotDelete()) );

    pMenu->exec( mapToGlobal( pEvent->pos() ) );

    delete pMenu;
}


