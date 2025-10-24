#ifndef H_PMProcedureWidget
#define H_PMProcedureWidget

#include <CBD.h>
#include <DATASqlWidget.h>

#include "PMProcedure.h"

class PMProcedureWidget : public QWidget
{
    Q_OBJECT
public:
    PMProcedureWidget( PMProcedure *pProcedure, QWidget *pParent );

protected:
    PMProcedure *   pProcedure;
    DATASqlWidget * pEditor;

protected slots:
    void slotTextChanged();
};

#endif

