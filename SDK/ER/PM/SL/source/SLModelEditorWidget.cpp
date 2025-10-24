#include "LibInfo.h"
#include "SLModelEditorWidget.h"

#include "SLModel.h"
#include "SLModelPalWidget.h"

SLModelEditorWidget::SLModelEditorWidget( SLModel *pModel, QWidget *pParent )
    : DDiagramEditorWidget( pModel, pParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif


#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

SLModelEditorWidget::~SLModelEditorWidget()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

AWPalWidget *SLModelEditorWidget::getPalWidget()
{
    if ( pPalWidget ) return pPalWidget;
    pPalWidget = new SLModelPalWidget( this );
    return pPalWidget;
}

