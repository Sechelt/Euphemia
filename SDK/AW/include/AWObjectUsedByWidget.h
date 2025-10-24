#ifndef H_AWObjectUsedByWidget
#define H_AWObjectUsedByWidget

#include "AWObject.h"

class AWObjectUsedByWidget : public QWidget
{
    Q_OBJECT
public:
    AWObjectUsedByWidget( AWObject *pObject, QWidget *pwidgetParent );
};

#endif


