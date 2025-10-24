#include "LibInfo.h"
#include "DDiagramPalWidget.h"

#include <WIconWidget.h>
#include <WIconLayout.h>

#include "DDiagramEditorWidget.h"

DDiagramPalWidget::DDiagramPalWidget( DDiagramEditorWidget *pEditor, QWidget *pParent )
    : AWPalWidget( pEditor, pParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    QVBoxLayout *   pLayoutTop      = new QVBoxLayout( this );

    pStackedWidget  = new WStackedWidget( this );

    // Layouts
    {
        QWidget *pWidgetTools = new QWidget();
        WIconLayout *pLayout = new WIconLayout;
        pLayout->addWidget( new WIconWidget( tr( "Layout Horizontal" ), QIcon( ":D/HLayout" ).pixmap( 32, 32 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Layout Vertical" ), QIcon( ":D/VLayout" ).pixmap( 32, 32 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Layout Grid" ), QIcon( ":D/GLayout" ).pixmap( 32, 32 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Spacer Horizontal" ), QIcon( ":D/HSpacer" ).pixmap( 32, 32 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Spacer Vertical" ), QIcon( ":D/VSpacer" ).pixmap( 32, 32 ), pWidgetTools ) );

        pWidgetTools->setLayout( pLayout );
        pStackedWidget->addWidget( tr( "Layouts" ), pWidgetTools );
    }
    // Shapes
    {
        QWidget *pWidgetTools = new QWidget();
        // pWidgetTools->setStyleSheet( "background-color: lightblue" );
        WIconLayout *pLayout = new WIconLayout;
        pLayout->addWidget( new WIconWidget( tr( "Rectangle" ), QIcon( ":D/Rectangle" ).pixmap( 32, 32 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Ellipse" ), QIcon( ":D/Ellipse" ).pixmap( 32, 32 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Bitmap" ), QIcon( ":D/Image" ).pixmap( 32, 32 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Vector" ), QIcon( ":D/Image" ).pixmap( 32, 32 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Text" ), QIcon( ":D/Text" ).pixmap( 32, 32 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Title" ), QIcon( ":D/Title" ).pixmap( 32, 32 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Title Doc" ), QIcon( ":D/Title" ).pixmap( 32, 32 ), pWidgetTools ) );

        pWidgetTools->setLayout( pLayout );
        pStackedWidget->addWidget( tr( "Shapes" ), pWidgetTools );
    }
    // Connectors
    {
        QWidget *pWidgetTools = new QWidget();
        WIconLayout *pLayout = new WIconLayout;
        pLayout->addWidget( new WIconWidget( tr( "Line" ), QIcon( ":D/Line" ).pixmap( 32, 32 ), pWidgetTools, tr("general purpose connector - single segment, straight line") ) );
        pLayout->addWidget( new WIconWidget( tr( "Polyline" ), QIcon( ":D/Polyline" ).pixmap( 32, 32 ), pWidgetTools, tr("general purpose connector - segmented") ) );
        pLayout->addWidget( new WIconWidget( tr( "Polyline - AutoRoute" ), QIcon( ":D/Polyline" ).pixmap( 32, 32 ), pWidgetTools, tr("general purpose connector - segmented with AutoRouting") ) );
        pLayout->addWidget( new WIconWidget( tr( "90 Horizontal" ), QIcon( ":D/Line90H" ).pixmap( 32, 32 ), pWidgetTools, tr("general purpose connector - right-angle") ) );
        pLayout->addWidget( new WIconWidget( tr( "90 Vertical" ), QIcon( ":D/Line90V" ).pixmap( 32, 32 ), pWidgetTools, tr("general purpose connector - right-angle") ) );
        pLayout->addWidget( new WIconWidget( tr( "90 - AutoRoute" ), QIcon( ":D/Dynamic" ).pixmap( 32, 32 ), pWidgetTools, tr("general purpose connector - right-angles with AutoRouting") ) );
        pLayout->addWidget( new WIconWidget( tr( "Dynamic" ), QIcon( ":D/Dynamic" ).pixmap( 32, 32 ), pWidgetTools, tr("general purpose connector - right-angles with AutoRouting") ) );
        pLayout->addWidget( new WIconWidget( tr( "Arc" ), QIcon( ":D/Arc" ).pixmap( 32, 32 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "CallOut" ), QIcon( ":D/CallOut" ).pixmap( 32, 32 ), pWidgetTools ) );

        pWidgetTools->setLayout( pLayout );
        pStackedWidget->addWidget( tr( "Connectors" ), pWidgetTools );
    }

    pLayoutTop->addWidget( pStackedWidget );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

DDiagramPalWidget::~DDiagramPalWidget()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif


#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}


