#include "RRouteAStar.h"

class RRouteAStarControl : public QWidget
{
    Q_OBJECT
public:
    RRouteAStarControl( RRouteAStar *pRoute, QWidget *pParent );

protected:
    RRouteAStar *pRoute;

    QCheckBox *pExploreLines;
    QCheckBox *pRouteLines;
    QCheckBox *pDetails;
    QComboBox *pHeuristic;
    QCheckBox *pDiagonal;
    QCheckBox *pStraightLine;
    QDoubleSpinBox *pStraighLineValue;
    QSpinBox *pDelay;

protected slots:
    void slotModified();
};
