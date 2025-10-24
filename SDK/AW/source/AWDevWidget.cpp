#include "LibInfo.h"
#include "AWDevWidget.h"

#include "AWDevScriptWidget.h"

AWDevWidget::AWDevWidget( QWidget *pParent )
: QTabWidget( pParent )
{
    addTab( pScript = new AWDevScriptWidget( this ), tr("Logic") );
}


