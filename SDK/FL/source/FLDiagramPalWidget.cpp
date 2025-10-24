#include "LibInfo.h"
#include "FLDiagramPalWidget.h"

#include <WStackedWidget.h>
#include <WIconWidget.h>
#include <WIconLayout.h>

#include "FLDiagramEditorWidget.h"

FLDiagramPalWidget::FLDiagramPalWidget( FLDiagramEditorWidget *pEditor, QWidget *pParent )
    : AWPalWidget( pEditor, pParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    QVBoxLayout *   pLayoutTop      = new QVBoxLayout( this );
    WStackedWidget *pStackedWidget  = new WStackedWidget( this );

    // Shapes & Connectors
    {
        QWidget *pWidgetTools = new QWidget();
        // pWidgetTools->setStyleSheet( "background-color: lightblue" );
        WIconLayout *pLayout = new WIconLayout;
        pLayout->addWidget( new WIconWidget( tr( "Async" ), QIcon( ":FL/Async" ).pixmap( 16, 16 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Condition" ), QIcon( ":FL/Condition" ).pixmap( 16, 16 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Step" ), QIcon( ":FL/Step" ).pixmap( 16, 16 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Loop" ), QIcon( ":FL/Loop" ).pixmap( 16, 16 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Sync" ), QIcon( ":FL/Sync" ).pixmap( 16, 16 ), pWidgetTools ) );

        pWidgetTools->setLayout( pLayout );
        pStackedWidget->addWidget( tr( "Logic" ), pWidgetTools );
    }

    pLayoutTop->addWidget( pStackedWidget );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

FLDiagramPalWidget::~FLDiagramPalWidget()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif


#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}


