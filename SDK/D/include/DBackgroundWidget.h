#ifndef H_DBackgroundWidget
#define H_DBackgroundWidget

#include <W.h>

class DDiagram;

class DBackgroundWidget : public QWidget
{
    Q_OBJECT
public:
    DBackgroundWidget( DDiagram *pDiagram, QWidget *pParent );
};

#endif


