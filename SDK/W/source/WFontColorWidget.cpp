#include "LibInfo.h"
#include "WFontColorWidget.h"

#include "WColorButton.h"
#include "WBrushStyleComboBox.h"

WFontColorWidget::WFontColorWidget( const CBD::CBDText &font, QWidget *pParent )
    : QWidget( pParent )
{
    this->font = font;

    QFormLayout *pLayout = new QFormLayout( this );

    pForegroundColor = new WColorButton( font.color, this, WColorButton::Pen );
    pLayout->addRow( tr("Foreground Color"), pForegroundColor );
    connect( pForegroundColor, SIGNAL(signalChanged(QColor)), SLOT(slotForegroundColorChanged(QColor)) );

    pBackgroundStyle = new WBrushStyleComboBox( this, font.brush.style() );
    pBackgroundStyle->setValue( font.brush.style() );
    pLayout->addRow( tr("Background Style"), pBackgroundStyle );
    connect( pBackgroundStyle, SIGNAL(signalChanged(Qt::BrushStyle)), SLOT(slotBackgroundStyleChanged(Qt::BrushStyle)) );

    pBackgroundColor = new WColorButton( font.brush.color(), this, WColorButton::Brush );
    pLayout->addRow( tr("Background Color"), pBackgroundColor );
    connect( pBackgroundColor, SIGNAL(signalChanged(QColor)), SLOT(slotBackgroundColorChanged(QColor)) );
}

void WFontColorWidget::setValue( const CBD::CBDText &font )
{
    this->font = font;
    pForegroundColor->setValue( font.color );
    pBackgroundColor->setValue( font.brush.color() );
    pBackgroundStyle->setValue( font.brush.style() );
}

void WFontColorWidget::slotValue( const CBD::CBDText &font )
{
    setValue( font );
}

void WFontColorWidget::slotForegroundColorChanged( QColor color )
{
    font.color = color;
    emit signalChanged( font );
}

void WFontColorWidget::slotBackgroundStyleChanged( Qt::BrushStyle n )
{
    font.brush.setStyle( n );
    emit signalChanged( font );
}

void WFontColorWidget::slotBackgroundColorChanged( QColor color )
{
    font.brush.setColor( color );
    emit signalChanged( font );
}


