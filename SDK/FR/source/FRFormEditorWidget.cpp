#include "LibInfo.h"
#include "FRFormEditorWidget.h"

#include "FRForm.h"
#include "FRFormPalWidget.h"

FRFormEditorWidget::FRFormEditorWidget( FRForm *pForm, QWidget *pParent )
    : DDiagramEditorWidget( pForm, pParent )
{
}

FRFormEditorWidget::~FRFormEditorWidget()
{
}

AWPalWidget *FRFormEditorWidget::getPalWidget()
{
    if ( pPalWidget ) return pPalWidget;
    pPalWidget = new FRFormPalWidget( this );
    return pPalWidget;
}

