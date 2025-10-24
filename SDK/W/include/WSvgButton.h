/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_WSvgButton
#define H_WSvgButton

#include <QSvgRenderer>

#include "W.h"

#include "W.h"

class WSvgButton : public QWidget
{
    Q_OBJECT
public:
    explicit WSvgButton( const QByteArray &ba, QWidget *pParent );
    ~WSvgButton();

    void setVector( const QByteArray &ba );
    QByteArray getVector() { return ByteArray; }

signals:
    void signalChanged( const QByteArray & );

protected:
    QByteArray      ByteArray;
    QSvgRenderer *  pRenderer;

    void paintEvent( QPaintEvent *pEvent );
    void mouseDoubleClickEvent( QMouseEvent *pEvent );
};

#endif 

