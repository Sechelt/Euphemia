#include "../source/LibInfo.h"
#include "LMModelPalWidget.h"

#include <WIconLayout.h>
#include <WIconWidget.h>

#include "LMModelEditorWidget.h"

LMModelPalWidget::LMModelPalWidget( LMModelEditorWidget *pEditor, QWidget *pParent )
    : AWPalWidget( pEditor, pParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );
    QToolBox *pToolBox = new QToolBox( this );

    // Shapes
    {
        QWidget *pWidgetTools = new QWidget();
        // pWidgetTools->setStyleSheet( "background-color: lightblue" );
        WIconLayout *pLayout = new WIconLayout;
        pLayout->addWidget( new WIconWidget( tr( "Association Link" ), QIcon( ":LM/AssociationLink16x16" ).pixmap( 16, 16 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Data Item" ), QIcon( ":LM/DataItem16x16" ).pixmap( 16, 16 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Domain" ), QIcon( ":LM/LMDomain16x16" ).pixmap( 16, 16 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Association" ), QIcon( ":LM/Association16x16" ).pixmap( 16, 16 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Entity" ), QIcon( ":LM/Entity16x16" ).pixmap( 16, 16 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Inheritance" ), QIcon( ":LM/Inheritance16x16" ).pixmap( 16, 16 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Inheritance Link" ), QIcon( ":LM/InheritanceLink16x16" ).pixmap( 16, 16 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Relationship" ), QIcon( ":LM/Relationship16x16" ).pixmap( 16, 16 ), pWidgetTools ) );
        pLayout->addWidget( new WIconWidget( tr( "Rule" ), QIcon( ":LM/Rule16x16" ).pixmap( 16, 16 ), pWidgetTools ) );

        pWidgetTools->setLayout( pLayout );
        pToolBox->addItem( pWidgetTools, tr( "ERD: Logical Model" ) );
    }
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

LMModelPalWidget::~LMModelPalWidget()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif


#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}


