/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "WIconWidget.h"

WIconWidget::WIconWidget( const QString &stringText, const QPixmap &pixmapIcon, QWidget *pWidget, const QString &stringTip )
    : QWidget( pWidget )
{
    bSelected = false;

    setAutoFillBackground( true );

    QGridLayout *pLayout = new QGridLayout( this );
    pLabelIcon  = new QLabel( this );
    pLabelText  = new QLabel( this );
    pLabelIcon->setPixmap( pixmapIcon );
    pLabelText->setText( stringText );
    pLayout->addWidget( pLabelIcon, 0, 0, Qt::AlignHCenter );
    pLayout->addWidget( pLabelText, 1, 0, Qt::AlignHCenter );
    setToolTip( stringTip );
}

WIconWidget::~WIconWidget()
{
}

void WIconWidget::setSelected( bool b )
{
    if ( bSelected == b ) return;

    if ( b ) setBackgroundRole( QPalette::Highlight );
    else setBackgroundRole( QPalette::NoRole );
    bSelected = b;
}

void WIconWidget::mousePressEvent( QMouseEvent *pMouseEvent )
{
    if ( pMouseEvent->button() == Qt::LeftButton )
    {
        // so someone could call our setSelected() if they want...
        emit signalClicked( this );
        // initiate drag...
        QDrag *drag = new QDrag( this );
        QMimeData *mimeData = new QMimeData;

        mimeData->setText( pLabelText->text() );
        drag->setMimeData( mimeData );
// #if QT_VERSION < 0x060000
//        drag->setPixmap( *(pLabelIcon->pixmap()) );
// #else
        drag->setPixmap( pLabelIcon->pixmap( (Qt::ReturnByValueConstant)0 ) );
// #endif


        /* Qt::DropAction dropAction = */ drag->exec();
        return;
    }

    QWidget::mousePressEvent( pMouseEvent );
}


