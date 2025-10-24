#include "LibInfo.h"
#include "ORGDiagramPalWidget.h"

#include <WStackedWidget.h>
#include <WIconWidget.h>
#include <WIconLayout.h>

#include "ORGDiagramEditorWidget.h"

ORGDiagramPalWidget::ORGDiagramPalWidget( ORGDiagramEditorWidget *pEditor, QWidget *pParent )
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
        pLayout->addWidget( new WIconWidget( tr( "Person" ), QIcon( ":ORG/Person" ).pixmap( 16, 16 ), pWidgetTools ) );

        pWidgetTools->setLayout( pLayout );
        pStackedWidget->addWidget( tr( "Organization" ), pWidgetTools );
    }

    pLayoutTop->addWidget( pStackedWidget );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

ORGDiagramPalWidget::~ORGDiagramPalWidget()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif


#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}


