/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "SArrow.h"

/****************************************************************
 * SArrow
 ****************************************************************/
SArrow::SArrow()
{
}

void SArrow::setValue( qreal n )
{
printf( "[PAH][%s][%s][%d] %f\n", __FILE__, __FUNCTION__, __LINE__, n );
    if ( n == nSize ) return;
    nSize = n;
    emit signalChanged();
}

void SArrow::setSelected( qreal n )
{
    if ( n == nSize ) return;
    nSize = n;
    emit signalSync();
}

QWidget *SArrow::getButton( QWidget *pParent )
{
    return new SArrowButton( pParent );
}

QWidget *SArrow::getToolBar( QWidget *pParent )
{
    return new SArrowToolBar( pParent );
}

QWidget *SArrow::getPanel( QWidget *pParent )
{
    return new SArrowPanel( pParent );
}

QWidget *SArrow::getSwatch( QWidget *pParent )
{
    return new SArrowSwatch( pParent );
}

void SArrow::doSave( QDomDocument *pdomDoc, QDomElement * )
{
    QDomElement domElemThis = pdomDoc->createElement( "SArrow" );

    domElemThis.setAttribute( "Size", nSize );
}

void SArrow::doLoad( QDomElement *pdomElemThis )
{
    nSize = pdomElemThis->attribute( "Size", "0" ).toDouble(); 
}

/****************************************************************
 * SArrowButton
 ****************************************************************/
SArrowButton::SArrowButton( QWidget *pParent )
    : QToolButton( pParent )
{
    setObjectName( "SArrowButton" );

    pArrow = (SArrow*)g_SSettings->getSetting( "SArrow" );
    if ( !pArrow )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [SArrow].";
        return;
    }

    setIcon( getIcon() );

    connect( pArrow, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pArrow, SIGNAL(signalSync()), SLOT(slotRefresh()) );
    connect( this, SIGNAL(clicked()), SLOT(slotClicked()) );
}

void SArrowButton::slotRefresh()
{
    setIcon( getIcon() );
}

void SArrowButton::slotClicked()
{
    SDialog dialog( this, "SArrow", "Pen" );
    dialog.exec();
}

QPixmap SArrowButton::getIcon()
{
    // dynamic

    /*
    QPixmap pixmap( ":W/Pen16" );
    QPainter p( &pixmap );
    p.fillRect( QRect( 0, 13, 16, 5 ), pArrow->getValue().color() );
    return pixmap;
    */

    // static
    return QPixmap( ":W/Arrow" );
}

/****************************************************************
 * SArrowToolBar
 ****************************************************************/
SArrowToolBar::SArrowToolBar( QWidget *pParent )
    : QWidget( pParent )
{
    setObjectName( "SArrowToolBar" );

    pArrow = (SArrow*)g_SSettings->getSetting( "SArrow" );
    if ( !pArrow )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [SArrow].";
        return;
    }


    nSize = pArrow->getValue();

    QHBoxLayout *pLayout = new QHBoxLayout( this );

    pSize = new QDoubleSpinBox( this );
    pSize->setToolTip( tr("size of arrow head") );
    pSize->setMinimum( 0 );
    pSize->setMaximum( 100 );
    pSize->setValue( nSize );
    pLayout->addWidget( pSize );
    connect( pSize, SIGNAL(valueChanged(qreal)), SLOT(slotSize(qreal)) );

    pMore = new QToolButton( this );
    pMore->setToolTip( tr("more") );
    pMore->setText( ":" );
    pLayout->addWidget( pMore );
    connect( pMore, SIGNAL(clicked()), SLOT(slotMore()) );

    pLayout->addStretch( 11 );

    connect( pArrow, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pArrow, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void SArrowToolBar::slotRefresh()
{
    nSize = pArrow->getValue();
    pSize->setValue( nSize );
}

void SArrowToolBar::slotSize( qreal n )
{
    nSize = n;
    pArrow->setValue( nSize );
}

void SArrowToolBar::slotMore()
{
    SDialog dialog( this, "SArrow", "Pen" );
    dialog.exec();
}

/****************************************************************
 * SArrowPanel
 ****************************************************************/
SArrowPanel::SArrowPanel( QWidget *pParent )
    : QWidget( pParent )
{
    setWindowTitle( tr("Pen") );

    pArrow = (SArrow*)g_SSettings->getSetting( "SArrow" );
    if ( pArrow )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [SArrow].";
        return;
    }

    nSize = pArrow->getValue();

    pSize = new QDoubleSpinBox( this );
    pSize->setMinimum( 0 );
    pSize->setMaximum( 100 );
    pSize->setValue( nSize );
    connect( pSize, SIGNAL(valueChanged(qreal)), SLOT(slotSize(qreal)) );

    // FORM
    QFormLayout *pLayoutForm = new QFormLayout( this );
    pLayoutForm->addRow( tr("Size:"), pSize );

    connect( pArrow, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pArrow, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void SArrowPanel::slotRefresh()
{
    nSize = pArrow->getValue();
    pSize->setValue( nSize );
}

void SArrowPanel::slotSize( qreal n )
{
    nSize = n;
    pArrow->setValue( nSize );
}

/****************************************************************
 * SArrowSwatch
 ****************************************************************/
SArrowSwatch::SArrowSwatch( QWidget *pParent )
    : QWidget( pParent )
{
    pArrow = (SArrow*)g_SSettings->getSetting( "SArrow" );
    if ( pArrow )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [SArrow].";
        return;
    }

    nSize = pArrow->getValue();
    connect( pArrow, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pArrow, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void SArrowSwatch::slotRefresh()
{
    nSize = pArrow->getValue();
    update();
}

void SArrowSwatch::paintEvent( QPaintEvent *pEvent )
{
    Q_UNUSED(pEvent);

    // paint swatch here
    /*
    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setRenderHint( QPainter::SmoothPixmapTransform );
    painter.setPen( pen );
    painter.fillRect( 0, 0, width(), height(), pen.color().lighter() );
    painter.drawLine( 0, height()/2, width(), height()/2 );
    */
}

QSize SArrowSwatch::sizeHint() const
{
    return QSize( 48, 48 );
}


