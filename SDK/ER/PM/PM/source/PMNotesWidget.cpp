#include "LibInfo.h"
#include "PMNotesWidget.h"

PMNotesWidget::PMNotesWidget( PMObject *pObject, QWidget *pParent )
    : QTabWidget( pParent )
{
    this->pObject = pObject;

    pTextEditDescription = new QPlainTextEdit( this );
    pTextEditDescription->setPlainText( pObject->getDescription() );
    connect( pTextEditDescription, SIGNAL(textChanged()), SLOT(slotDescription()) );
    addTab( pTextEditDescription, tr("Description") );
     
    pTextEditAnnotation = new QPlainTextEdit( this );
    pTextEditAnnotation->setPlainText( pObject->getAnnotation() );
    connect( pTextEditAnnotation, SIGNAL(textChanged()), SLOT(slotAnnotation()) );
    addTab( pTextEditAnnotation, tr("Annotation") );
}

void PMNotesWidget::slotDescription()  
{
    pObject->setDescription( pTextEditDescription->toPlainText() );
}

void PMNotesWidget::slotAnnotation()   
{ 
    pObject->setAnnotation( pTextEditAnnotation->toPlainText() );
}

