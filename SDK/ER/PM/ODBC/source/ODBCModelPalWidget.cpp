#include "../source/LibInfo.h"
#include "ODBCModelPalWidget.h"

#include <WIconLayout.h>
#include <WIconWidget.h>

#include "ODBCModel.h"
#include "ODBCModelEditorWidget.h"

ODBCModelPalWidget::ODBCModelPalWidget( ODBCModelEditorWidget *pEditor, QWidget *pParent )
    : AWPalWidget( pEditor, pParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    ODBCModel *pModel = (ODBCModel*)pEditor->getDiagram();                                                                          
    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );
    QToolBox *pToolBox = new QToolBox( this );

    // shapes
    {
        QWidget *pWidgetTools = new QWidget();
        // pWidgetTools->setStyleSheet( "background-color: lightblue" );
        WIconLayout *pLayout = new WIconLayout;

        if ( !pModel->getClass( "Table" ).isEmpty() )
            pLayout->addWidget( new WIconWidget( tr( "Table" ), QIcon( ":PM/Table" ).pixmap( 16, 16 ), pWidgetTools ) );
        if ( !pModel->getClass( "View" ).isEmpty() )
            pLayout->addWidget( new WIconWidget( tr( "View" ), QIcon( ":PM/View" ).pixmap( 16, 16 ), pWidgetTools ) );
        if ( !pModel->getClass( "Sequence" ).isEmpty() )
            pLayout->addWidget( new WIconWidget( tr( "Sequence" ), QIcon( ":PM/Sequence" ).pixmap( 16, 16 ), pWidgetTools ) );
        if ( !pModel->getClass( "Reference" ).isEmpty() )
            pLayout->addWidget( new WIconWidget( tr( "Reference" ), QIcon( ":PM/Reference" ).pixmap( 16, 16 ), pWidgetTools ) );
        if ( !pModel->getClass( "Procedure" ).isEmpty() )
            pLayout->addWidget( new WIconWidget( tr( "Procedure" ), QIcon( ":PM/Procedure" ).pixmap( 16, 16 ), pWidgetTools ) );
        if ( !pModel->getClass( "Domain" ).isEmpty() )
            pLayout->addWidget( new WIconWidget( tr( "Domain" ), QIcon( ":PM/Domain" ).pixmap( 16, 16 ), pWidgetTools ) );

        pWidgetTools->setLayout( pLayout );
        if ( pModel->inherits( "ODBCExtModel" ) ) pToolBox->addItem( pWidgetTools, tr( "ERD: Physical Model\nODBC v3.8\nExtended" ) );
        else if ( pModel->inherits( "ODBCCorModel" ) ) pToolBox->addItem( pWidgetTools, tr( "ERD: Physical Model\nODBC v3.8\nCore" ) );
        else if ( pModel->inherits( "ODBCMinModel" ) ) pToolBox->addItem( pWidgetTools, tr( "ERD: Physical Model\nODBC v3.8\nMinimum" ) );
        else if ( pModel->inherits( "ODBCDrvModel" ) ) pToolBox->addItem( pWidgetTools, tr( "ERD: Physical Model\nODBC v3.8\nDriver" ) );
        else pToolBox->addItem( pWidgetTools, tr( "ERD: Physical Model\nODBC v3.8" ) ); // should not happen
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

ODBCModelPalWidget::~ODBCModelPalWidget()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif


#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}


