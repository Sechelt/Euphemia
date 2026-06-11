/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#ifndef H_WScratchTool
#define H_WScratchTool

#include "W.h"

class WScratchSwatch : public QWidget
{
    Q_OBJECT
public:
    WScratchSwatch( QWidget *pParent );

    void setImage( const QImage &image );

protected:
    void paintEvent( QPaintEvent *pEvent );

private:
    QImage image;
};


class WScratchTool : public QWidget
{
    Q_OBJECT
public:
    WScratchTool( QWidget *pParent );

    void setMax( int );

    QImage getImage();

    void doAppend( const QImage & );
    void doClear();
    void doEnableAdd( bool b = true );

signals:
    void signalAdd();
    void signalPaste( const QImage & );

protected:
    QVector<QImage> vectorImages;

    WScratchSwatch *    pSwatch;
    QSlider *           pSlider;
    QToolButton *pAdd;
    QToolButton *pRemove;
    QToolButton *pPaste;

    int nMax = 10;

protected slots:
    void slotSlider( int );
    void slotRemove();
    void slotPaste();
};

#endif 

