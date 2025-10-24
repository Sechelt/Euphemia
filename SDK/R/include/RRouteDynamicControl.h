#include "RRouteDynamic.h"

class RRouteDynamicControl : public QWidget
{
    Q_OBJECT
public:
    RRouteDynamicControl( RRouteDynamic *pRoute, QWidget *pParent );

protected:
    RRouteDynamic *pRoute;

    QCheckBox *pExploreLines;
    QCheckBox *pRouteLines;
    QCheckBox *pDetails;
    QComboBox *pDepart;
    QComboBox *pArrive;
    QSpinBox *pStraightLine;
    QSpinBox *pCrossLine;
    QSpinBox *pLeaveBeginAxis;
    QSpinBox *pLeaveEndAxis;
    QSpinBox *pDelay;

protected slots:
    void slotModified();
};
