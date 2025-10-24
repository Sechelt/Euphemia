#ifndef H_DPointsWidget
#define H_DPointsWidget

#include "DLine.h"

class DPointsWidget : public QWidget
{
    Q_OBJECT
public:
    DPointsWidget( DLine *pLine, QWidget *pParent );
};

#endif


