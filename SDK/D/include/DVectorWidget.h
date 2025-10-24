#ifndef H_DVectorWidget
#define H_DVectorWidget

#include <WSvgButton.h>

#include "DVector.h"

class DVectorWidget : public WSvgButton
{
    Q_OBJECT
public:
    DVectorWidget( DVector *pVector, QWidget *pParent );

protected:
    DVector *pVector;

protected slots:
    void slotModified();
};

#endif


