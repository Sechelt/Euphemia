#ifndef H_DBitmapWidget
#define H_DBitmapWidget

#include "DBitmap.h"

class WBitmapButton;

class DBitmapWidget : public QWidget
{
    Q_OBJECT
public:
    DBitmapWidget( DBitmap *pBitmap, QWidget *pParent );

protected:
    DBitmap *       pBitmap;
    QComboBox *     pPlacement;
    WBitmapButton * pBitmapButton;

protected slots:
    void slotModified();
};

#endif


