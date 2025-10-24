#include "AppInfo.h"
#include "SAOutputWidget.h"

#include <DATAWOutputMessagesWidget.h>
#include <DATAWOutputDiagnosticsWidget.h>

SAOutputWidget::SAOutputWidget( QWidget *pParent )
: AWOutputWidget( pParent )
{
    nDataMessagesCount      = 0;
    nDataDiagnosticsCount   = 0;

    addTab( pDataMessages = new DATAWOutputMessagesWidget( this ), tr("Data Messages") );
    addTab( pDataDiagnostics = new DATAWOutputDiagnosticsWidget( this ), tr("Data Diagnostics") );
}

void SAOutputWidget::slotOutput( ADEventOutput *pEvent )
{
    if ( pEvent->metaObject()->className() == QString( "ADDataEventGeneral" ) )
    { 
        nDataMessagesCount++;
        setTabText( indexOf( pDataMessages ), tr("Data Messages") + " " + QString::number( nDataMessagesCount ) );
        pDataMessages->slotMessage(  (ADDataEventGeneral*)pEvent );
        return;
    }
    else if ( pEvent->metaObject()->className() == QString( "ADDataEventDiagnostic" ) )
    { 
        nDataDiagnosticsCount++;
        setTabText( indexOf( pDataDiagnostics ), tr("Data Diagnostics") + " " + QString::number( nDataDiagnosticsCount ) );
        pDataDiagnostics->slotMessage(  (ADDataEventDiagnostic*)pEvent );
        return;
    }
    else if ( pEvent->metaObject()->className() == QString( "ADEventRequest" ) )
    { 
        ADEventRequest *p = (ADEventRequest*)pEvent;
        if ( p->stringCategory == "Data Messages" )
        {
            if ( p->nRequest == ADEventRequest::RequestClear )
            {
                pDataMessages->slotClear(); 
                nDataMessagesCount = 0;
                setTabText( indexOf( pDataMessages ), tr("Data Messages") ); 
                return;
            }
            else if ( p->nRequest == ADEventRequest::RequestFocus )
            {
                setCurrentWidget( pDataMessages );
                return;
            }
        }
        else if ( p->stringCategory == "Data Diagnostics" )
        {
            if ( p->nRequest == ADEventRequest::RequestClear )
            {
                pDataDiagnostics->slotClear();   
                nDataDiagnosticsCount = 0;
                setTabText( indexOf( pDataDiagnostics ), tr("Data Diagnostics") ); 
                return;
            }
            else if ( p->nRequest == ADEventRequest::RequestFocus )
            {
                setCurrentWidget( pDataDiagnostics );
                return;
            }
        }
    }

    AWOutputWidget::slotOutput( pEvent );
}

void SAOutputWidget::slotCurrentChanged( int nIndex )
{
    QWidget *pWidget = currentWidget();

    if ( pWidget == pDataMessages )
    {
        nDataMessagesCount = 0;
        setTabText( nIndex, tr("Data Messages") ); 
        return;
    }
    else if ( pWidget == pDataDiagnostics )
    { 
        nDataDiagnosticsCount = 0;
        setTabText( nIndex, tr("Data Diagnostics") ); 
        return;
    }
    AWOutputWidget::slotCurrentChanged( nIndex );
}

void SAOutputWidget::slotCurrentClear()
{
    QWidget *pWidget = currentWidget();

    if ( pWidget == pDataMessages ) 
    {
        setTabText( indexOf( pDataMessages ), tr("Data Messages") ); 
        pDataMessages->slotClear();
        return;
    }
    else if ( pWidget == pDataDiagnostics )
    {
        setTabText( indexOf( pDataDiagnostics ), tr("Data Diagnostics") ); 
        pDataDiagnostics->slotClear();
        return;
    }
    AWOutputWidget::slotCurrentClear();
}


