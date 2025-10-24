#include "LibInfo.h"
#include "AWClassWidget.h"
#include "AWObject.h"

AWClassWidget::AWClassWidget( AWObject *pObject, QWidget *pwidgetParent )
    : QTabWidget( pwidgetParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    this->pObject = pObject;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}



