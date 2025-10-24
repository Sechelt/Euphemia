#include "LibInfo.h"
#include "MSAModelEditorWidget.h"

#include "MSAModel.h"
#include "MSAModelPalWidget.h"

MSAModelEditorWidget::MSAModelEditorWidget( MSAModel *pModel, QWidget *pParent )
    : DDiagramEditorWidget( pModel, pParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif


#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

MSAModelEditorWidget::~MSAModelEditorWidget()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

AWPalWidget *MSAModelEditorWidget::getPalWidget()
{
    if ( pPalWidget ) return pPalWidget;
    pPalWidget = new MSAModelPalWidget( this );
    return pPalWidget;
}

