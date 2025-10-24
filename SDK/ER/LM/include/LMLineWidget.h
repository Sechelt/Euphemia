#ifndef H_LMLineWidget
#define H_LMLineWidget

#include "LMLine.h"

class LMLineTextWidget : public QWidget
{
    Q_OBJECT
public:
    LMLineTextWidget( LMLine *pLine, QWidget *pParent );

protected:
    LMLine *     pLine;
    QComboBox * pDisplay;

protected slots:
    virtual void slotModified();
};

class LMLineWidget : public QWidget
{
    Q_OBJECT
public:
    LMLineWidget( LMLine *pLine, QWidget *pParent );
};

#endif


