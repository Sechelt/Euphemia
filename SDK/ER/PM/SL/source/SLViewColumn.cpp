#include "LibInfo.h"
#include "SLViewColumn.h"

#include "PMView.h"

SLViewColumn::SLViewColumn( PMView *pParent )
    : PMViewColumn( pParent )
{ 
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif


#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

SLViewColumn::~SLViewColumn()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}


