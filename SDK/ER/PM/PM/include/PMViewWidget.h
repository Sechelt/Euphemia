#ifndef H_PMViewWidget
#define H_PMViewWidget

#include <CBD.h>
#include <DATASqlWidget.h>

#include "PMView.h"

class PMViewWidget : public QWidget
{
    Q_OBJECT
public:
    PMViewWidget( PMView *pView, QWidget *pParent );

protected:
    PMView *        pView;
    DATASqlWidget * pEditor;

protected slots:
    void slotTextChanged();
};

#endif

