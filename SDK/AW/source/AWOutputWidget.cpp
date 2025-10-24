#include "LibInfo.h"
#include "AWOutputWidget.h"

#include "AWOutputMessagesWidget.h"
#include "AWOutputSearchWidget.h"
#include "AWOutputValidationWidget.h"

AWOutputWidget::AWOutputWidget( QWidget *pParent )
: QTabWidget( pParent )
{
    nMessagesCount      = 0;
    nSearchCount        = 0;
    nValidationCount    = 0;

    QToolButton *pButton = new QToolButton( this );
    pButton->setIcon( QPixmap( ":W/minus" ) );
    setCornerWidget( pButton, Qt::TopLeftCorner );
    pButton->setToolTip( tr("clear output in current output tab") );
    connect( pButton, SIGNAL(clicked()), SLOT(slotCurrentClear()) );

    addTab( pMessages = new AWOutputMessagesWidget( this ), tr("Messages") );

    addTab( pSearch = new AWOutputSearchWidget( this ), tr("Search") );
    connect( pSearch, SIGNAL(signalGoTo(ADObjectPathList)), SIGNAL(signalGoTo(ADObjectPathList)) );
    connect( pSearch, SIGNAL(signalGoToView(ADObjectPathList)), SIGNAL(signalGoToView(ADObjectPathList)) );
    connect( pSearch, SIGNAL(signalFind(const ADFindCriteria &)), SIGNAL(signalFind(const ADFindCriteria &)) );

    addTab( pValidation = new AWOutputValidationWidget( this ), tr("Validation") );
    connect( pValidation, SIGNAL(signalGoTo(ADObjectPathList)), SIGNAL(signalGoTo(ADObjectPathList)) );
    connect( pValidation, SIGNAL(signalGoToView(ADObjectPathList)), SIGNAL(signalGoToView(ADObjectPathList)) );

    // catch case where current tab changes so we can reset message counter 
    connect( this, SIGNAL(currentChanged(int)), SLOT(slotCurrentChanged(int)) );
}

void AWOutputWidget::setFocusFind()
{
    setCurrentWidget( pSearch );
    pSearch->setFocusFind();
}

void AWOutputWidget::setFocusReplace()
{
    setCurrentWidget( pSearch );
    pSearch->setFocusReplace();
}

void AWOutputWidget::slotOutput( ADEventOutput *pEvent )
{
#if QT_VERSION < 0x060000
    if ( pEvent->metaObject()->className() == QStringLiteral( "ADEventValidation" ) )
#else
    if ( pEvent->metaObject()->className() == QString( "ADEventValidation" ) )
#endif
    { 
        nValidationCount++;
        setTabText( indexOf( pValidation ), tr("Validation") + " " + QString::number( nValidationCount ) );
        pValidation->slotMessage( (ADEventValidation*)pEvent );
    }
#if QT_VERSION < 0x060000
    else if ( pEvent->metaObject()->className() == QStringLiteral( "ADEventGeneral" ) )
#else
    else if ( pEvent->metaObject()->className() == QString( "ADEventGeneral" ) )
#endif
    { 
        nMessagesCount++;
        setTabText( indexOf( pMessages ), tr("Messages") + " " + QString::number( nMessagesCount ) );
        pMessages->slotMessage(  (ADEventGeneral*)pEvent );
    }
    else if ( pEvent->metaObject()->className() == QString( "ADEventSearch" ) )
    { 
        nSearchCount++;
        setTabText( indexOf( pSearch ), tr("Search") + " " + QString::number( nSearchCount ) );
        pSearch->slotMessage( (ADEventSearch*)pEvent );
    }
    else if ( pEvent->metaObject()->className() == QString( "ADDataEventGeneral" ) )
    { 
    }
    else if ( pEvent->metaObject()->className() == QString( "ADDataEventDiagnostic" ) )
    { 
    }
    else if ( pEvent->metaObject()->className() == QString( "ADDataEventResult" ) )
    { 
    }
    else if ( pEvent->metaObject()->className() == QString( "ADEventRequest" ) )
    { 
        ADEventRequest *p = (ADEventRequest*)pEvent;
        if ( p->stringCategory == "Validation" )
        {
            if ( p->nRequest == ADEventRequest::RequestClear )
            {
                pValidation->slotClear(); 
                nValidationCount = 0;
                setTabText( indexOf( pValidation ), tr("Validation") ); 
                return;
            }
            else if ( p->nRequest == ADEventRequest::RequestFocus )
            {
                setCurrentWidget( pValidation );
                return;
            }
        }
        else if ( p->stringCategory == "Messages" )
        {
            if ( p->nRequest == ADEventRequest::RequestClear )
            {
                pMessages->slotClear();   
                nMessagesCount = 0;
                setTabText( indexOf( pMessages ), tr("Messages") ); 
                return;
            }
            else if ( p->nRequest == ADEventRequest::RequestFocus )
            {
                setCurrentWidget( pMessages );
                return;
            }
        }
        else if ( p->stringCategory == "Search" )
        {
            if ( p->nRequest == ADEventRequest::RequestClear )
            {
                pSearch->slotClear();     
                nSearchCount = 0;
                setTabText( indexOf( pSearch ), tr("Search") ); 
                return;
            }
            else if ( p->nRequest == ADEventRequest::RequestFocus )
            {
                setCurrentWidget( pSearch );
                return;
            }
        }
        qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " Unhandled request category: " << p->stringCategory;
    }
}

// reset message counter when tab changed
void AWOutputWidget::slotCurrentChanged( int nIndex )
{
    QWidget *pWidget = currentWidget();

    if ( pWidget == pMessages )
    {
        nMessagesCount = 0;
        setTabText( nIndex, tr("Messages") ); 
    }
    else if ( pWidget == pSearch )
    { 
        nSearchCount = 0;
        setTabText( nIndex, tr("Search") ); 
    }
    else if ( pWidget == pValidation )
    { 
        nValidationCount = 0;
        setTabText( nIndex, tr("Validation") ); 
    }
}

void AWOutputWidget::slotCurrentClear()
{
    QWidget *pWidget = currentWidget();

    if ( pWidget == pMessages ) 
    {
        setTabText( indexOf( pMessages ), tr("Messages") ); 
        pMessages->slotClear();
    }
    else if ( pWidget == pSearch )
    {
        setTabText( indexOf( pSearch ), tr("Search") ); 
        pSearch->slotClear();
    }
    else if ( pWidget == pValidation )
    {
        setTabText( indexOf( pValidation ), tr("Validation") ); 
        pValidation->slotClear();
    }
}


