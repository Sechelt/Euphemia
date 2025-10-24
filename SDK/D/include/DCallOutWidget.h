#ifndef H_DCallOutWidget
#define H_DCallOutWidget

#include <W.h>
#include <WLineEndComboBox.h>
#include <WTextHAlignComboBox.h>
#include <WTextVAlignComboBox.h>
#include <WText.h>
#include <WBrush.h>
#include <WPen.h>

#include "DCallOut.h"

class DCallOutTextWidget : public QWidget
{
    Q_OBJECT
public:
    DCallOutTextWidget( DCallOut *pCallOut, QWidget *pParent );

protected:
    DCallOut *            pCallOut;
    QComboBox *           pDisplay;
    WTextHAlignComboBox * pJustify;
    WTextVAlignComboBox * pAlign;
    QCheckBox *           pWordWrap;

protected slots:
    virtual void slotModified();
};

class DCallOutConnectorWidget : public QWidget
{
    Q_OBJECT
public:
    DCallOutConnectorWidget( DCallOut *pCallOut, QWidget *pParent );

protected:
    DCallOut *          pCallOut;
    WLineEndComboBox *  pComboBoxEnd;
    QCheckBox *         pCheckBoxEnd;

protected slots:
    virtual void slotModified();
};

class DCallOutWidget : public QWidget
{
    Q_OBJECT
public:
    DCallOutWidget( DCallOut *pCallOut, QWidget *pParent );
};

#endif


