#ifndef H_WFontColorWidget
#define H_WFontColorWidget

#include <CBD.h>

class WColorButton;
class WBrushStyleComboBox;

class WFontColorWidget : public QWidget
{
    Q_OBJECT
public:
    WFontColorWidget( const CBD::CBDText &font, QWidget *pParent );

    void setValue( const CBD::CBDText &font );

    CBD::CBDText getValue() { return font; }

signals:
    void signalChanged( const CBD::CBDText &font );

public slots:
    void slotValue( const CBD::CBDText &font );

protected:
    CBD::CBDText font;

    WColorButton *          pForegroundColor;
    WColorButton *          pBackgroundColor;
    WBrushStyleComboBox *   pBackgroundStyle;

protected slots:
    void slotForegroundColorChanged( QColor color );
    void slotBackgroundColorChanged( QColor color );
    void slotBackgroundStyleChanged( Qt::BrushStyle n );
};

#endif 

