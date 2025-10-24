#include "LibInfo.h"
#include "WFontButton.h"
#include "W.h"

CBDTextSwatch::CBDTextSwatch( const CBD::CBDText &font, QWidget *pParent )
    : QLabel( pParent )
{
    this->font = font;
}

void CBDTextSwatch::setValue( const CBD::CBDText &font )
{
    this->font = font;
    setFont( font.font );
    update();
}

void CBDTextSwatch::paintEvent( QPaintEvent *pEvent )
{
    Q_UNUSED(pEvent);

    QPainter painter( this );
    painter.setBrush( font.brush );
    painter.setPen( QPen( font.color ) );
    painter.setFont( font.font );
//    painter.drawRect( QRect( 0, 0, width(), height() ) );
    painter.drawText( QRect( 0, 0, width(), height() ), Qt::AlignCenter, tr("Quick Fox") );
}

// ============================================================

WFontButton::WFontButton( const QFont &font, QWidget *pParent )
    : QPushButton( pParent )
{
    this->font = font;
    setText( font.family() );
    QFont f;
    f.setFamily( font.family() );
    f.setBold( font.bold() );
    f.setItalic( font.italic() );
    f.setUnderline( font.underline() );
    setFont( f );
    connect( this, SIGNAL(clicked()), this, SLOT(slotClicked()) );
}

void WFontButton::setValue( const QFont &font )
{
    this->font = font;
    setText( font.family() );
    // display a subset as we do not want the UI controls to look whacky
    QFont f;
    f.setFamily( font.family() );
    f.setBold( font.bold() );
    f.setItalic( font.italic() );
    f.setUnderline( font.underline() );
    setFont( f );
}

void WFontButton::slotClicked()
{
    bool bOk;
    font = QFontDialog::getFont( &bOk, font, this );
    if ( bOk ) 
    {
        QFont f;
        f.setFamily( font.family() );
        f.setBold( font.bold() );
        f.setItalic( font.italic() );
        f.setUnderline( font.underline() );
        setText( font.family() );
        setFont( f );

        emit signalChanged( font );
    } 
}


