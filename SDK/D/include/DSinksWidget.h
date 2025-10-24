#ifndef H_DSinksWidget
#define H_DSinksWidget

#include "DRectangle.h"

class DSinksWidget : public QWidget
{
    Q_OBJECT
public:
    DSinksWidget( DRectangle *pRectangle, QWidget *pParent );
};

#endif


