#ifndef H_DLineWidget
#define H_DLineWidget

#include <WLineEndComboBox.h>

#include "DLine.h"

class DLineTextWidget : public QWidget
{
    Q_OBJECT
public:
    DLineTextWidget( DLine *pLine, QWidget *pParent );

protected:
    DLine *     pLine;
    QComboBox * pDisplay;

protected slots:
    virtual void slotModified();
};

class DLineConnectorWidget : public QWidget
{
    Q_OBJECT
public:
    DLineConnectorWidget( DLine *pLine, QWidget *pParent );

protected:
    DLine *             pLine;
    WLineEndComboBox *  pComboBoxBegin;
    WLineEndComboBox *  pComboBoxEnd;
    QCheckBox *         pCheckBoxBegin;
    QCheckBox *         pCheckBoxEnd;

protected slots:
    virtual void slotModified();
};

class DLineWidget : public QWidget
{
    Q_OBJECT
public:
    DLineWidget( DLine *pLine, QWidget *pParent );
};

#endif


