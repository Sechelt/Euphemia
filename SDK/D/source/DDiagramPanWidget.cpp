#include "LibInfo.h"
#include "DDiagramPanWidget.h"
#include "DDiagramEditorWidget.h"

DDiagramPanWidget::DDiagramPanWidget( AWEditorWidget *pEditor, DGraphicsSceneProxy *pScene, QWidget *pParent )
    : AWPanWidget( pEditor, pParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );
    pPanner = new DPanner( pScene, this );
    pLayoutTop->addWidget( pPanner );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

DDiagramPanWidget::~DDiagramPanWidget()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif


#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}



