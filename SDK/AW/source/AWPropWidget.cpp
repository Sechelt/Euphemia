#include "LibInfo.h"
#include "AWPropWidget.h"

#include "AWObject.h"

AWPropWidget::AWPropWidget( AWObject *pObject, QWidget *pwidgetParent )
    : WStackedWidget( pwidgetParent )
{
    this->pObject = pObject;
}

AWPropWidget::~AWPropWidget()
{
}



