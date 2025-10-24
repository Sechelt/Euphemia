/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
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

void LSLine::setSelected( LineEndStyles nBegin, LineEndStyles nEnd )
{
    this->nBegin    = nBegin;
    this->nEnd      = nEnd;
    emit signalSync();
}

QWidget *LSLine::getToolBar( QWidget *pParent )
{
    return new LSLineWidget( pParent, Qt::Horizontal );
}

QWidget *LSLine::getPanel( QWidget *pParent )
{
    return new LSLineWidget( pParent, Qt::Vertical );
}

void LSLine::doSave( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    Q_UNUSED(pdomElem);

    QDomElement domElem = pdomDoc->createElement( "LSLine" );

    domElem.setAttribute( "Begin", nBegin );
    domElem.setAttribute( "End", nEnd );
}

void LSLine::doLoad( QDomElement *pdomElem )
{
    nBegin = LineEndStyles(pdomElem->attribute( "Begin", QString::number( int(nBegin) ) ).toInt());
    nEnd = LineEndStyles(pdomElem->attribute( "End", QString::number( int(nEnd) ) ).toInt());
}

/****************************************************************
 * LSLineWidget
 ****************************************************************/
LSLineWidget::LSLineWidget( QWidget *p, Qt::Orientation n )
    : QWidget( p )
{
    pSetting = (LSLine*)g_SSettings->getSetting( "LSLine" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find [LSLine].";
        return;
    }

    pBegin = new QComboBox( this );
    pBegin->addItem( "None", LSLine::LineEndNone );
    pBegin->addItem( "Arrow", LSLine::LineEndArrow );
    pBegin->addItem( "Ball", LSLine::LineEndBall );
    pBegin->setCurrentIndex( pBegin->findData( (int)pSetting->getBegin() ) );
    pBegin->setToolTip( tr("beginning of line") );
    connect( pBegin, SIGNAL(currentIndexChanged(int)), SLOT(slotBegin(int)) );

    pEnd = new QComboBox( this );
    pEnd->addItem( "None", LSLine::LineEndNone );
    pEnd->addItem( "Arrow", LSLine::LineEndArrow );
    pEnd->addItem( "Ball", LSLine::LineEndBall );
    pEnd->setCurrentIndex( pEnd->findData( (int)pSetting->getEnd() ) );
    pEnd->setToolTip( tr("end of line") );
    connect( pEnd, SIGNAL(currentIndexChanged(int)), SLOT(slotEnd(int)) );

    if ( n == Qt::Horizontal )
    {
        QHBoxLayout *pLayout = new QHBoxLayout( this );
        pLayout->addWidget( pBegin );
        pLayout->addWidget( pEnd );
        pLayout->addStretch( 10 );
    }
    else
    {
        QFormLayout *pLayout = new QFormLayout( this );
        pLayout->addRow( tr("Begin"), pBegin );
        pLayout->addRow( tr("End"), pEnd );
    }

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void LSLineWidget::slotRefresh()
{
    pBegin->setCurrentIndex( pBegin->findData( (int)pSetting->getBegin() ) );
    pEnd->setCurrentIndex( pEnd->findData( (int)pSetting->getEnd() ) );
}

void LSLineWidget::slotBegin( int n )
{
    pSetting->setBegin( (LSLine::LineEndStyles)pBegin->itemData( n ).toInt() );
}

void LSLineWidget::slotEnd( int n )
{
    pSetting->setEnd( (LSLine::LineEndStyles)pEnd->itemData( n ).toInt() );
}


