#ifndef H_WFontColorDialog
#define H_WFontColorDialog

#include "WFontColorWidget.h"

class WFontColorDialog : public QDialog
{
    Q_OBJECT
public:
    WFontColorDialog( const CBD::CBDText &font, QWidget *pParent = nullptr );

    CBD::CBDText getValue() { return pWidget->getValue(); }
    static CBD::CBDText getValue( bool *pOk, const CBD::CBDText &font, QWidget *pParent );

protected:
    WFontColorWidget *pWidget;
};

#endif 

