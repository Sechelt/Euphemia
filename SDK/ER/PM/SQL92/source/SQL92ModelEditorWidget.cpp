#include "LibInfo.h"
#include "SQL92ModelEditorWidget.h"

#include "SQL92Model.h"
#include "SQL92ModelPalWidget.h"

SQL92ModelEditorWidget::SQL92ModelEditorWidget( SQL92Model *pModel, QWidget *pParent )
    : DDiagramEditorWidget( pModel, pParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif


#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

SQL92ModelEditorWidget::~SQL92ModelEditorWidget()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

AWPalWidget *SQL92ModelEditorWidget::getPalWidget()
{
    if ( pPalWidget ) return pPalWidget;
    pPalWidget = new SQL92ModelPalWidget( this );
    return pPalWidget;
}

