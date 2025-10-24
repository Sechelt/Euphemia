#ifndef H_LMInheritanceWidget
#define H_LMInheritanceWidget

#include "LMInheritance.h"

class LMInheritanceWidget : public QWidget
{
    Q_OBJECT
public:
    LMInheritanceWidget( LMInheritance *pInheritance, QWidget *pParent );

protected slots:
    void slotMutuallyExclusive( Qt::CheckState n );
    void slotComplete( Qt::CheckState n );
    void slotBaseEntity( int n );

    void slotModified();
    void slotModified( DSource * );

protected:
    LMInheritance *     pInheritance;
    QVector<DSource*>   vectorEntities;

    QCheckBox *pMutuallyExclusive; 
    QCheckBox *pComplete;
    QComboBox *pBaseEntity;
};

#endif




