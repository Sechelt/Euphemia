#ifndef H_FRLabel
#define H_FRLabel

#include <W.h>
#include <WTextHAlignComboBox.h>
#include <WTextVAlignComboBox.h>

#include "FRObject.h"

class FRForm;

class FRLabel : public FRObject
{
    Q_OBJECT
public:

    explicit FRLabel( FRForm *pForm, const QString &stringName = QString() );
    virtual ~FRLabel();

    virtual void setName( const QString &sringName );      
    virtual void setProxy( DGraphicsProxyItem *p );

    virtual QPixmap         getIcon();
    virtual AWPropWidget *  getPropWidget( QWidget *pWidgetParent = nullptr );
};

class FRLabelPropWidget : public QWidget
{
    Q_OBJECT
public:
    FRLabelPropWidget( FRLabel *pLabel, QWidget *pParent );

protected slots:
    virtual void slotText();

protected:
    FRLabel *               pLabel;
    QLabel *                pWidget;
    QLineEdit *             pLineEditText;
    WTextHAlignComboBox *   pJustify;
    WTextVAlignComboBox *   pAlign;

protected slots:
    virtual void slotModified();
};


#endif


