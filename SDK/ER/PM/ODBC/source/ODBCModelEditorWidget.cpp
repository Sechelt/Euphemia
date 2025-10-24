#include "LibInfo.h"
#include "ODBCModelEditorWidget.h"

#include "ODBCModel.h"
#include "ODBCModelPalWidget.h"

ODBCModelEditorWidget::ODBCModelEditorWidget( ODBCModel *pModel, QWidget *pParent )
    : DDiagramEditorWidget( pModel, pParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif


#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

ODBCModelEditorWidget::~ODBCModelEditorWidget()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

AWPalWidget *ODBCModelEditorWidget::getPalWidget()
{
    if ( pPalWidget ) return pPalWidget;
    pPalWidget = new ODBCModelPalWidget( this );
    return pPalWidget;
}

