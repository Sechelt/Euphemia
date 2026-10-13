/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "SLine.h"

/****************************************************************
 * SLine
 ****************************************************************/
SLine::SLine()
{
}

void SLine::setValue( qreal n )
{
    if ( n == nSize ) return;
    nSize = n;
    emit signalChanged();
}

void SLine::setSelected( qreal n )
{
    if ( n == nSize ) return;
    nSize = n;
    emit signalSync();
}

QWidget *SLine::getButton( QWidget *pParent )
{
    return new SLineButton( pParent );
}

QWidget *SLine::getToolBar( QWidget *pParent )
{
    return new SLineToolBar( pParent );
}

QWidget *SLine::getPanel( QWidget *pParent )
{
    return new SLinePanel( pParent );
}

QWidget *SLine::getSwatch( QWidget *pParent )
{
    return new SLineSwatch( pParent );
}

void SLine::doSave( QDomDocument *pdomDoc, QDomElement * )
{
    QDomElement domElemThis = pdomDoc->createElement( "SLine" );

    domElemThis.setAttribute( "Size", nSize );
}

void SLine::doLoad( QDomElement *pdomElemThis )
{
    nSize = pdomElemThis->attribute( "Size", "0" ).toDouble(); 
}

/****************************************************************
 * SLineButton
 ****************************************************************/
SLineButton::SLineButton( QWidget *pParent )
    : QToolButton( pParent )
{
    setObjectName( "SLineButton" );

    pLine = (SLine*)g_SSettings->getSetting( "SLine" );
    if ( !pLine )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [SLine].";
        return;
    }

    setIcon( getIcon() );

    connect( pLine, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pLine, SIGNAL(signalSync()), SLOT(slotRefresh()) );
    connect( this, SIGNAL(clicked()), SLOT(slotClicked()) );
}

void SLineButton::slotRefresh()
{
    setIcon( getIcon() );
}

void SLineButton::slotClicked()
{
    SDialog dialog( this, "SLine", "Pen" );
    dialog.exec();
}

QPixmap SLineButton::getIcon()
{
    // dynamic

    /*
    QPixmap pixmap( ":W/Pen16" );
    QPainter p( &pixmap );
    p.fillRect( QRect( 0, 13, 16, 5 ), pLine->getValue().color() );
    return pixmap;
    */

    // static
    return QPixmap( ":W/Arrow" );
}

/****************************************************************
 * SLineToolBar
 ****************************************************************/
SLineToolBar::SLineToolBar( QWidget *pParent )
    : QWidget( pParent )
{
    setObjectName( "SLineToolBar" );

    pLine = (SLine*)g_SSettings->getSetting( "SLine" );
    if ( !pLine )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [SLine].";
        return;
    }


    nSize = pLine->getValue();

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

    connect( pLine, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pLine, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void SLineToolBar::slotRefresh()
{
    nSize = pLine->getValue();
    pSize->setValue( nSize );
}

void SLineToolBar::slotSize( qreal n )
{
    nSize = n;
    pLine->setValue( nSize );
}

void SLineToolBar::slotMore()
{
    SDialog dialog( this, "SLine", "Pen" );
    dialog.exec();
}

/****************************************************************
 * SLinePanel
 ****************************************************************/
SLinePanel::SLinePanel( QWidget *pParent )
    : QWidget( pParent )
{
    setWindowTitle( tr("Pen") );

    pLine = (SLine*)g_SSettings->getSetting( "SLine" );
    if ( pLine )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [SLine].";
        return;
    }

    nSize = pLine->getValue();

    pSize = new QDoubleSpinBox( this );
    pSize->setMinimum( 0 );
    pSize->setMaximum( 100 );
    pSize->setValue( nSize );
    connect( pSize, SIGNAL(valueChanged(qreal)), SLOT(slotSize(qreal)) );

    // FORM
    QFormLayout *pLayoutForm = new QFormLayout( this );
    pLayoutForm->addRow( tr("Size:"), pSize );

    connect( pLine, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pLine, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void SLinePanel::slotRefresh()
{
    nSize = pLine->getValue();
    pSize->setValue( nSize );
}

void SLinePanel::slotSize( qreal n )
{
    nSize = n;
    pLine->setValue( nSize );
}

/****************************************************************
 * SLineSwatch
 ****************************************************************/
SLineSwatch::SLineSwatch( QWidget *pParent )
    : QWidget( pParent )
{
    pLine = (SLine*)g_SSettings->getSetting( "SLine" );
    if ( pLine )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [SLine].";
        return;
    }

    nSize = pLine->getValue();
    connect( pLine, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pLine, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void SLineSwatch::slotRefresh()
{
    nSize = pLine->getValue();
    update();
}

void SLineSwatch::paintEvent( QPaintEvent *pEvent )
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

QSize SLineSwatch::sizeHint() const
{
    return QSize( 48, 48 );
}


