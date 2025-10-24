#ifndef H_DRectangleWidget
#define H_DRectangleWidget

#include <W.h>
#include <WTextHAlignComboBox.h>
#include <WTextVAlignComboBox.h>
#include <WText.h>
#include <WBrush.h>
#include <WPen.h>

#include "DRectangle.h"

class DRectangleTextWidget : public QWidget
{
    Q_OBJECT
public:
    DRectangleTextWidget( DRectangle *pRectangle, QWidget *pParent );

protected:
    DRectangle *            pRectangle;
    QComboBox *             pDisplay;
    WTextHAlignComboBox *   pJustify;
    WTextVAlignComboBox *   pAlign;
    QCheckBox *             pWordWrap;

protected slots:
    virtual void slotModified();
};

class DRectangleWidget : public QWidget
{
    Q_OBJECT
public:
    DRectangleWidget( DRectangle *pRectangle, QWidget *pParent );
};

#endif


