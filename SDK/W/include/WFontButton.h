#ifndef WFONTBUTTON_H
#define WFONTBUTTON_H

#include <CBD.h>

class CBDTextSwatch : public QLabel
{
public:
    CBDTextSwatch( const CBD::CBDText &font, QWidget *pParent );

    void setValue( const CBD::CBDText &font );

protected:
    void paintEvent( QPaintEvent *pEvent );
    // QSize sizeHint() const;

private:
    CBD::CBDText font;
};

class WFontButton : public QPushButton
{
    Q_OBJECT
public:
    WFontButton( const QFont &font, QWidget *pParent );

    void setValue( const QFont &font );
    QFont getValue() { return font; }

signals:
    void signalChanged( const QFont &font );

protected:
    QFont font;

protected slots:
    void slotClicked();
};

#endif 

