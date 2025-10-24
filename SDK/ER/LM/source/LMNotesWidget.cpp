#include "LibInfo.h"
#include "LMNotesWidget.h"

LMNotesWidget::LMNotesWidget( QWidget *pParent, const QString &stringDescription, const QString &stringAnnotation )
    : QTabWidget( pParent )
{
    pTextEditDescription = new QTextEdit( this );
    pTextEditDescription->setPlainText( stringDescription );
    connect( pTextEditDescription, SIGNAL(textChanged()), SLOT(slotDescription()) );
    addTab( pTextEditDescription, tr("Description") );
     
    pTextEditAnnotation = new QTextEdit( this );
    pTextEditAnnotation->setPlainText( stringAnnotation );
    connect( pTextEditAnnotation, SIGNAL(textChanged()), SLOT(slotAnnotation()) );
    addTab( pTextEditAnnotation, tr("Annotation") );
}

void LMNotesWidget::slotDescription()  
{
    emit signalDescriptionChanged( pTextEditDescription->toPlainText() ); 
}

void LMNotesWidget::slotAnnotation()   
{ 
    emit signalAnnotationChanged( pTextEditAnnotation->toPlainText() ); 
}

