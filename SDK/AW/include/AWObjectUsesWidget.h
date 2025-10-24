#ifndef H_AWObjectUsesWidget
#define H_AWObjectUsesWidget

#include "AWObject.h"

class AWObjectUsesWidget : public QWidget
{
    Q_OBJECT
public:
    AWObjectUsesWidget( AWObject *pObject, QWidget *pwidgetParent );
};

#endif


