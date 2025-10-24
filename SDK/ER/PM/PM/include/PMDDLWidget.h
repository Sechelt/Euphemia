#ifndef H_PMDDLWidget
#define H_PMDDLWidget

#include <CBD.h>
#include <DATASqlWidget.h>

#include "PMDDLManager.h"

class PMDDLWidget : public QTabWidget
{
    Q_OBJECT
public:
    PMDDLWidget( PMDDLManager *pDDLManager, const QString &stringCreate, const QString &stringDrop, QWidget *pParent );

protected:
    PMDDLManager *pDDLManager;
    QString stringCreateAuto;
    QString stringDropAuto;

    QCheckBox *     pManualCreate;
    QCheckBox *     pManualDrop;
    QCheckBox *     pIncludeCreate;
    QCheckBox *     pIncludeDrop;

    DATASqlWidget * pTextEditCreate;
    DATASqlWidget * pTextEditDrop;

protected slots:
    void slotManualCreate( Qt::CheckState b );
    void slotManualDrop( Qt::CheckState b );
    void slotIncludeCreate( Qt::CheckState b );
    void slotIncludeDrop( Qt::CheckState b );
    void slotTextChangedCreate();
    void slotTextChangedDrop();
};

#endif

