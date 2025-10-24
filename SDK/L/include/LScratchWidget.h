/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_LScratchWidget
#define H_LScratchWidget

#include <QtWidgets>

class EScratchSwatch : public QWidget
{
    Q_OBJECT
public:
    EScratchSwatch( QWidget *pParent );

    void setImage( const QImage &image );

protected:
    void paintEvent( QPaintEvent *pEvent );

private:
    QImage image;
};


class LScratchWidget : public QWidget
{
    Q_OBJECT
public:
    LScratchWidget( QWidget *pParent );

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

    EScratchSwatch *    pSwatch;
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

