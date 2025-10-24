#include "LibInfo.h"
#include "FLDiagramEditorWidget.h"

#include <DGraphicsView.h>

#include "FLDiagram.h"
#include "FLDiagramPalWidget.h"
#include "FLBegin.h"

FLDiagramEditorWidget::FLDiagramEditorWidget( FLDiagram *pDiagram, QWidget *pParent )
    : DDiagramEditorWidget( pDiagram, pParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    // The following works, for about 1 sec, and is then moved again. So just creates a flicker.
    // if ( pView && pDiagram->getBegin() ) pView->centerOn( pDiagram->getBegin()->getProxy() );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

FLDiagramEditorWidget::~FLDiagramEditorWidget()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

AWPalWidget *FLDiagramEditorWidget::getPalWidget()
{
    if ( pPalWidget ) return pPalWidget;
    pPalWidget = new FLDiagramPalWidget( this );
    return pPalWidget;
}

