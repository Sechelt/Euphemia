#include "LibInfo.h"
#include "PMAdditionalChecksWidget.h"

PMAdditionalChecksWidget::PMAdditionalChecksWidget( QWidget *pParent, const QString &stringClient, const QString &stringServer )
    : QWidget( pParent )
{
    QVBoxLayout *playout = new QVBoxLayout( this );            
                                                               
    QTabWidget *pTabWidget = new QTabWidget( this );           
                                                               
    pClient = new QTextEdit( pTabWidget );        
    pClient->setText( stringClient );
    pTabWidget->addTab( pClient, tr("Client") );      
    connect( pClient, SIGNAL(textChanged()), SLOT(slotClient()) );

    pServer = new QTextEdit( pTabWidget );        
    pServer->setText( stringServer );
    pTabWidget->addTab( pServer, tr("Server") );      
    connect( pServer, SIGNAL(textChanged()), SLOT(slotServer()) );

    pTabWidget->setCurrentWidget( pClient );              
                                                               
    playout->addWidget( pTabWidget );                          
}

void PMAdditionalChecksWidget::slotClient()
{
    emit signalClient( pClient->toPlainText() );
}

void PMAdditionalChecksWidget::slotServer()
{
    emit signalServer( pServer->toPlainText() );
}


