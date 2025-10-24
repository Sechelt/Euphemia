#ifndef H_DSourcesWidget
#define H_DSourcesWidget

#include "DLine.h"

class DSourcesWidget : public QWidget
{
    Q_OBJECT
public:
    DSourcesWidget( DLine *pLine, QWidget *pParent );
};

#endif


