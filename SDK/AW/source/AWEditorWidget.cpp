#include "LibInfo.h"
#include "AWEditorWidget.h"

#include "AWObject.h"
#include "AWPalWidget.h"

AWEditorWidget::AWEditorWidget( AWObject *pObject, QWidget *pParent )
    : QMainWindow( pParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    this->pObject = pObject;
    pPalWidget = nullptr;
    pPanWidget = nullptr;

    setWindowIcon( pObject->getIcon() );
    setWindowTitle( pObject->getName() );

    connect( pObject, SIGNAL(signalModified()), SLOT(slotModified()) );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

AWEditorWidget::~AWEditorWidget()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    // we can go out before AWObject so...
    pObject->pEditorWidget = nullptr;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

void AWEditorWidget::doGoTo()
{
   emit signalGoTo( this );
}

bool AWEditorWidget::doFindFirst( const QString &, bool, bool, bool, bool, bool, int, int, bool, bool, bool )
{
    return false;
}

bool AWEditorWidget::doFindFirstInSelection( const QString &, bool, bool, bool, bool, bool, bool, bool )
{
    return false;
}

void AWEditorWidget::doFindNext()
{
}

void AWEditorWidget::doReplace( const QString & )
{
}

void AWEditorWidget::doReplaceAll( const QString & )
{
}

void AWEditorWidget::doCancelFind()
{
}

void AWEditorWidget::slotModified()
{
    setWindowIcon( pObject->getIcon() );
    setWindowTitle( pObject->getName() );
    emit signalModified( this );
}





