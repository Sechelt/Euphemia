#include "LibInfo.h"
#include "ORGDiagramEditorWidget.h"

#include <DGraphicsView.h>

#include "ORGDiagram.h"
#include "ORGDiagramPalWidget.h"
#include "ORGPerson.h"

ORGDiagramEditorWidget::ORGDiagramEditorWidget( ORGDiagram *pDiagram, QWidget *pParent )
    : DDiagramEditorWidget( pDiagram, pParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

ORGDiagramEditorWidget::~ORGDiagramEditorWidget()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

AWPalWidget *ORGDiagramEditorWidget::getPalWidget()
{
    if ( pPalWidget ) return pPalWidget;
    pPalWidget = new ORGDiagramPalWidget( this );
    return pPalWidget;
}

