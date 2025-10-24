#ifndef H_AWObjectChildrenWidget
#define H_AWObjectChildrenWidget

#include "AWObject.h"

class AWObjectChildrenWidget : public QWidget
{
    Q_OBJECT
public:
    AWObjectChildrenWidget( AWObject *pObject, QWidget *pwidgetParent );
};

#endif


