/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "LSLine.h"

#include "SSettings.h"

/****************************************************************
 * LSLine
 ****************************************************************/
LSLine::LSLine()
{
}

void LSLine::setBegin( LineEndStyles n )
{
    if ( n == nBegin ) return;
    nBegin = n;
    emit signalChanged();
}

void LSLine::setEnd( LineEndStyles n )
{
    if ( n == nEnd ) return;
    nEnd = n;
    emit signalChanged();
}

void LSLine::setBeginSize( qreal n )
{
    if ( n == nBeginSize ) return;
    nBeginSize = n;
    emit signalChanged();
}

void LSLine::setEndSize( qreal n )
{
    if ( n == nEndSize ) return;
    nEndSize = n;
    emit signalChanged();
}

void LSLine::setSelected( LineEndStyles nBegin, LineEndStyles nEnd, qreal nBeginSize, qreal nEndSize )
{
    this->nBegin    = nBegin;
    this->nEnd      = nEnd;
    this->nBeginSize= nBeginSize;
    this->nEndSize  = nEndSize;
    emit signalSync();
}

QWidget *LSLine::getButton( QWidget *pParent )
{
    return new LSLineButton( pParent );
}

QWidget *LSLine::getToolBar( QWidget *pParent )
{
    return new LSLineToolBar( pParent );
}

QWidget *LSLine::getPanel( QWidget *pParent )
{
    return new LSLinePanel( pParent );
}

QWidget *LSLine::getSwatch( QWidget *pParent )
{
    return new LSLineSwatch( pParent );
}

void LSLine::doSave( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    Q_UNUSED(pdomElem);

    QDomElement domElem = pdomDoc->createElement( "LSLine" );

    domElem.setAttribute( "Begin", nBegin );
    domElem.setAttribute( "End", nEnd );
    domElem.setAttribute( "BeginSize", nBeginSize );
    domElem.setAttribute( "EndSize", nEndSize );
}

void LSLine::doLoad( QDomElement *pdomElem )
{
    nBegin = LineEndStyles(pdomElem->attribute( "Begin", QString::number( int(nBegin) ) ).toInt());
    nEnd = LineEndStyles(pdomElem->attribute( "End", QString::number( int(nEnd) ) ).toInt());
    nBeginSize = pdomElem->attribute( "BeginSize", "5" ).toDouble(); 
    nEndSize = pdomElem->attribute( "EndSize", "5" ).toDouble(); 
}

/****************************************************************
 * LSLineButton
 ****************************************************************/
LSLineButton::LSLineButton( QWidget *pParent )
    : QToolButton( pParent )
{
    setObjectName( "LSLineButton" );

    pSetting = (LSLine*)g_SSettings->getSetting( "LSLine" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [LSLine].";
        return;
    }

    setIcon( getIcon() );

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
    connect( this, SIGNAL(clicked()), SLOT(slotClicked()) );
}

void LSLineButton::slotRefresh()
{
    setIcon( getIcon() );
}

void LSLineButton::slotClicked()
{
    SDialog dialog( this, "LSLine", "Line" );
    dialog.exec();
}

QPixmap LSLineButton::getIcon()
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
 * LSLineToolBar
 ****************************************************************/
LSLineToolBar::LSLineToolBar( QWidget *pParent )
    : QWidget( pParent )
{
    setObjectName( "LSLineToolBar" );

    pSetting = (LSLine*)g_SSettings->getSetting( "LSLine" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [LSLine].";
        return;
    }

    QHBoxLayout *pLayout = new QHBoxLayout( this );

    pBegin = new QComboBox( this );
    pBegin->addItem( "None", LSLine::LineEndNone );
    pBegin->addItem( "Arrow", LSLine::LineEndArrow );
    pBegin->addItem( "Ball", LSLine::LineEndBall );
    pBegin->setCurrentIndex( pBegin->findData( (int)pSetting->getBegin() ) );
    pBegin->setToolTip( tr("beginning of line") );
    pLayout->addWidget( pBegin );
    connect( pBegin, SIGNAL(currentIndexChanged(int)), SLOT(slotBegin(int)) );

    pBeginSize = new QDoubleSpinBox( this );
    pBeginSize->setToolTip( tr("size of begin") );
    pBeginSize->setMinimum( 5 );
    pBeginSize->setMaximum( 100 );
    pBeginSize->setValue( pSetting->getBeginSize() );
    pLayout->addWidget( pBeginSize );
    connect( pBeginSize, SIGNAL(valueChanged(qreal)), SLOT(slotBeginSize(qreal)) );

    pEnd = new QComboBox( this );
    pEnd->addItem( "None", LSLine::LineEndNone );
    pEnd->addItem( "Arrow", LSLine::LineEndArrow );
    pEnd->addItem( "Ball", LSLine::LineEndBall );
    pEnd->setCurrentIndex( pEnd->findData( (int)pSetting->getEnd() ) );
    pEnd->setToolTip( tr("end of line") );
    pLayout->addWidget( pEnd );
    connect( pEnd, SIGNAL(currentIndexChanged(int)), SLOT(slotEnd(int)) );

    pEndSize = new QDoubleSpinBox( this );
    pEndSize->setToolTip( tr("size of end") );
    pEndSize->setMinimum( 5 );
    pEndSize->setMaximum( 100 );
    pEndSize->setValue( pSetting->getEndSize() );
    pLayout->addWidget( pEndSize );
    connect( pEndSize, SIGNAL(valueChanged(qreal)), SLOT(slotEndSize(qreal)) );

    pMore = new QToolButton( this );
    pMore->setToolTip( tr("more") );
    pMore->setText( ":" );
    pLayout->addWidget( pMore );
    connect( pMore, SIGNAL(clicked()), SLOT(slotMore()) );

    pLayout->addStretch( 11 );

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void LSLineToolBar::slotRefresh()
{
    pBegin->setCurrentIndex( pBegin->findData( (int)pSetting->getBegin() ) );
    pBeginSize->setValue( pSetting->getBeginSize() );

    pEnd->setCurrentIndex( pEnd->findData( (int)pSetting->getEnd() ) );
    pEndSize->setValue( pSetting->getEndSize() );
}

void LSLineToolBar::slotBegin( int n )
{
    pSetting->setBegin( (LSLine::LineEndStyles)pBegin->itemData( n ).toInt() );
}

void LSLineToolBar::slotBeginSize( qreal n )
{
    pSetting->setBeginSize( n );
}

void LSLineToolBar::slotEnd( int n )
{
    pSetting->setEnd( (LSLine::LineEndStyles)pEnd->itemData( n ).toInt() );
}

void LSLineToolBar::slotEndSize( qreal n )
{
    pSetting->setEndSize( n );
}

void LSLineToolBar::slotMore()
{
    SDialog dialog( this, "LSLine", "Pen" );
    dialog.exec();
}

/****************************************************************
 * LSLinePanel
 ****************************************************************/
LSLinePanel::LSLinePanel( QWidget *pParent )
    : QWidget( pParent )
{
    setWindowTitle( tr("Line") );

    pSetting = (LSLine*)g_SSettings->getSetting( "LSLine" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [LSLine].";
        return;
    }

    pBegin = new QComboBox( this );
    pBegin->addItem( "None", LSLine::LineEndNone );
    pBegin->addItem( "Arrow", LSLine::LineEndArrow );
    pBegin->addItem( "Ball", LSLine::LineEndBall );
    pBegin->setCurrentIndex( pBegin->findData( (int)pSetting->getBegin() ) );
    pBegin->setToolTip( tr("beginning of line") );
    connect( pBegin, SIGNAL(currentIndexChanged(int)), SLOT(slotBegin(int)) );

    pBeginSize = new QDoubleSpinBox( this );
    pBeginSize->setToolTip( tr("size of begin") );
    pBeginSize->setMinimum( 5 );
    pBeginSize->setMaximum( 100 );
    pBeginSize->setValue( pSetting->getBeginSize() );
    connect( pBeginSize, SIGNAL(valueChanged(qreal)), SLOT(slotBeginSize(qreal)) );

    pEnd = new QComboBox( this );
    pEnd->addItem( "None", LSLine::LineEndNone );
    pEnd->addItem( "Arrow", LSLine::LineEndArrow );
    pEnd->addItem( "Ball", LSLine::LineEndBall );
    pEnd->setCurrentIndex( pEnd->findData( (int)pSetting->getEnd() ) );
    pEnd->setToolTip( tr("end of line") );
    connect( pEnd, SIGNAL(currentIndexChanged(int)), SLOT(slotEnd(int)) );

    pEndSize = new QDoubleSpinBox( this );
    pEndSize->setToolTip( tr("size of end") );
    pEndSize->setMinimum( 5 );
    pEndSize->setMaximum( 100 );
    pEndSize->setValue( pSetting->getEndSize() );
    connect( pEndSize, SIGNAL(valueChanged(qreal)), SLOT(slotEndSize(qreal)) );

    // FORM
    QHBoxLayout *pLayoutStretch = new QHBoxLayout( this );
    QFormLayout *pLayoutForm = new QFormLayout();
    pLayoutForm->addRow( tr("Begin:"), pBegin );
    pLayoutForm->addRow( tr("Size:"), pBeginSize );
    pLayoutForm->addRow( tr("End:"), pEnd );
    pLayoutForm->addRow( tr("Size:"), pEndSize );
    pLayoutStretch->addLayout( pLayoutForm );
    pLayoutStretch->addStretch( 10 );

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void LSLinePanel::slotRefresh()
{
    pBegin->setCurrentIndex( pBegin->findData( (int)pSetting->getBegin() ) );
    pBeginSize->setValue( pSetting->getBeginSize() );

    pEnd->setCurrentIndex( pEnd->findData( (int)pSetting->getEnd() ) );
    pEndSize->setValue( pSetting->getEndSize() );
}

void LSLinePanel::slotBegin( int n )
{
    pSetting->setBegin( (LSLine::LineEndStyles)pBegin->itemData( n ).toInt() );
}

void LSLinePanel::slotBeginSize( qreal n )
{
    pSetting->setBeginSize( n );
}

void LSLinePanel::slotEnd( int n )
{
    pSetting->setEnd( (LSLine::LineEndStyles)pEnd->itemData( n ).toInt() );
}

void LSLinePanel::slotEndSize( qreal n )
{
    pSetting->setEndSize( n );
}

/****************************************************************
 * LSLineSwatch
 ****************************************************************/
LSLineSwatch::LSLineSwatch( QWidget *pParent )
    : QWidget( pParent )
{
    pSetting = (LSLine*)g_SSettings->getSetting( "LSLine" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [LSLine].";
        return;
    }

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void LSLineSwatch::slotRefresh()
{
    update();
}

void LSLineSwatch::paintEvent( QPaintEvent *pEvent )
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

QSize LSLineSwatch::sizeHint() const
{
    return QSize( 48, 48 );
}




