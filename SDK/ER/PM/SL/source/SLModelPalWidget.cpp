#include "../source/LibInfo.h"
#include "SLModelPalWidget.h"

#include <WIconLayout.h>
#include <WIconWidget.h>

#include "SLModel.h"
#include "SLModelEditorWidget.h"

SLModelPalWidget::SLModelPalWidget( SLModelEditorWidget *pEditor, QWidget *pParent )
    : AWPalWidget( pEditor, pParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );
    QToolBox *pToolBox = new QToolBox( this );

    // shapes
    {
        QWidget *pWidgetTools = new QWidget();
        // pWidgetTools->setStyleSheet( "background-color: lightblue" );
        WIconLayout *pLayout = new WIconLayout;

        pLayout->addWidget( new WIconWidget( tr( "Table" ), QIcon( ":PM/Table" ).pixmap( 16, 16 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "View" ), QIcon( ":PM/View" ).pixmap( 16, 16 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Sequence" ), QIcon( ":PM/Sequence" ).pixmap( 16, 16 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Reference" ), QIcon( ":PM/Reference" ).pixmap( 16, 16 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Procedure" ), QIcon( ":PM/Procedure" ).pixmap( 16, 16 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Domain" ), QIcon( ":PM/Domain" ).pixmap( 16, 16 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Rule" ), QIcon( ":PM/Rule" ).pixmap( 16, 16 ), pWidgetTools ) );

        pWidgetTools->setLayout( pLayout );
        pToolBox->addItem( pWidgetTools, tr( "ERD: Physical Model SQLite" ) );
    }
    // markup classes (supported by PMModel)...
    {
        QWidget *pWidgetTools = new QWidget();
        // pWidgetTools->setStyleSheet( "background-color: lightblue" );
        WIconLayout *pLayout = new WIconLayout;
        pLayout->addWidget( new WIconWidget( tr( "Title" ), QIcon( ":DR/Title" ).pixmap( 16, 16 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Title Doc" ), QIcon( ":DR/Title" ).pixmap( 16, 16 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Call Out" ), QIcon( ":DR/CallOut" ).pixmap( 16, 16 ), pWidgetTools ) );

        pWidgetTools->setLayout( pLayout );
        pToolBox->addItem( pWidgetTools, tr( "MarkUp" ) );
    }
    pLayoutTop->addWidget( pToolBox );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

SLModelPalWidget::~SLModelPalWidget()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif


#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}


