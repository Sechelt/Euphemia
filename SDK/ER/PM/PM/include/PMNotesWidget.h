#ifndef H_PMNotesWidget
#define H_PMNotesWidget

#include "PMObject.h"

class PMNotesWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit PMNotesWidget( PMObject *pObject, QWidget *pParent );

protected slots:
    void slotDescription();
    void slotAnnotation();

protected:
    PMObject *pObject;
    QPlainTextEdit *pTextEditDescription;
    QPlainTextEdit *pTextEditAnnotation;
};


#endif


